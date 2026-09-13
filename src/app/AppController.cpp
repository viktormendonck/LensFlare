#include "AppController.h"

#include <QMetaObject>
#include <QRunnable>
#include <QThreadPool>

#include "ImageCollectionModel.h"
#include "ImageProvider.h"
#include "ThumbnailProvider.h"

AppController::AppController(
    ImageProvider& imageProvider,
    ImageCollectionModel* imageCollection,
    ThumbnailProvider& thumbnailProvider)
    : AppImageProvider(imageProvider),
      AppThumbnailProvider(thumbnailProvider),
      ImageCollection(imageCollection)
{
}

AppController::~AppController()
{
    ++ImageLoadRequest;
    delete ImageCollection;
}

void AppController::OpenFile(const QUrl& url)
{
    const QString localPath = url.toLocalFile();

    if (localPath.isEmpty())
    {
        SetStatusText("Invalid file path");
        return;
    }

    try
    {
        ++ImageLoadRequest;

        const std::filesystem::path path = localPath.toStdString();
        CurrentSourceImage = AppRawDecoder.decode(path);

        ImageCollection->LoadSiblingsFromFile(path);
        SetCurrentImageIndex(ImageCollection->FindIndexForPath(path));

        ResetEdits();
        PublishPreview(AppPreviewGenerator.Create(CurrentSourceImage));

        SetStatusText(QString("Opened %1").arg(localPath));
    }
    catch (const std::exception& error)
    {
        SetStatusText(QString("Failed to open RAW: %1").arg(error.what()));
    }
}

void AppController::OpenImage(int index)
{
    if (!ImageCollection || index < 0 || index >= ImageCollection->rowCount())
        return;

    SetPreviewReady(false);

    const auto path = ImageCollection->GetEntryAt(index).filePath;
    SetCurrentImageIndex(index);

    LoadingThumbnailSource =
    QString("image://lensflare-thumbnail/%1").arg(index);

    emit LoadingThumbnailSourceChanged();

    const int request = ++ImageLoadRequest;

    QThreadPool::globalInstance()->start(
        QRunnable::create(
            [this, path, request]()
            {
                try
                {
                    auto image = AppRawDecoder.decode(
                        path,
                        [this, request]()
                        {
                            return request != ImageLoadRequest.load();
                        }
                    );

                    if (request != ImageLoadRequest.load() || image.IsNull())
                        return;

                    QImage preview = AppPreviewGenerator.Create(image);

                    if (request != ImageLoadRequest.load() || preview.isNull())
                        return;

                    QMetaObject::invokeMethod(
                        this,
                        [
                            this,
                            request,
                            path,
                            image = std::move(image),
                            preview = std::move(preview)
                        ]() mutable
                        {
                            if (request != ImageLoadRequest.load())
                                return;

                            CurrentSourceImage = std::move(image);
                            ResetEdits();
                            PublishPreview(std::move(preview));

                            SetStatusText(
                                QString("Opened %1").arg(
                                    QString::fromStdString(path.string())
                                )
                            );
                        },
                        Qt::QueuedConnection
                    );
                }
                catch (const std::exception& error)
                {
                    const QString message = QString::fromUtf8(error.what());

                    QMetaObject::invokeMethod(
                        this,
                        [this, request, message]()
                        {
                            if (request != ImageLoadRequest.load())
                                return;

                            SetStatusText(
                                QString("Failed to open RAW: %1").arg(message)
                            );
                        },
                        Qt::QueuedConnection
                    );
                }
            }
        )
    );
}

void AppController::SetExposure(float exposure)
{
    if (CurrentEditState.exposure == exposure)
        return;

    CurrentEditState.exposure = exposure;
    emit ExposureChanged();
}

void AppController::ResetEdits()
{
    CurrentEditState = {};
    emit ExposureChanged();
}

void AppController::PublishPreview(QImage preview)
{
    if (preview.isNull())
        return;

    AppImageProvider.setImage(std::move(preview));

    SetPreviewReady(true);

    SetImageRevision(ImageRevision + 1);
}

void AppController::SaveButton()
{
    SetStatusText("Save Pressed");
}

void AppController::UndoButton()
{
    SetStatusText("Undo Pressed");
}

void AppController::RedoButton()
{
    SetStatusText("Redo Pressed");
}
