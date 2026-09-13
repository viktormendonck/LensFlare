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
        const Image image = AppRawDecoder.decode(path);

        ImageCollection->LoadSiblingsFromFile(path);
        SetCurrentImageIndex(ImageCollection->FindIndexForPath(path));

        QImage qtImage(
            image.pixels.data(),
            image.width,
            image.height,
            image.width * image.channels,
            QImage::Format_RGB888
        );

        AppImageProvider.setImage(qtImage.copy());
        SetImageRevision(ImageRevision + 1);

        SetStatusText(
            QString("Opened %1").arg(localPath)
        );
    }
    catch (const std::exception& error)
    {
        SetStatusText(
            QString("Failed to open RAW: %1")
                .arg(error.what())
        );
    }
}

void AppController::OpenImage(int index)
{
    if (!ImageCollection ||
        index < 0 ||
        index >= ImageCollection->rowCount())
        return;

    const auto path =
        ImageCollection->GetEntryAt(index).filePath;

    SetCurrentImageIndex(index);

    const QImage thumbnail =
        AppThumbnailProvider.GetCachedThumbnail(path);

    if (!thumbnail.isNull())
    {
        AppImageProvider.setImage(thumbnail);
        SetImageRevision(ImageRevision + 1);
    }

    const int request = ++ImageLoadRequest;

    QThreadPool::globalInstance()->start(
        QRunnable::create(
            [this, path, request]()
            {
                try
                {
                    const Image image = AppRawDecoder.decode(
                        path,
                        [this, request]()
                        {
                            return request != ImageLoadRequest.load();
                        }
                    );

                    if (request != ImageLoadRequest.load() ||
                        image.pixels.empty())
                        return;

                    QImage qtImage(
                        image.pixels.data(),
                        image.width,
                        image.height,
                        image.width * image.channels,
                        QImage::Format_RGB888
                    );

                    qtImage = qtImage.copy();

                    if (request != ImageLoadRequest.load())
                        return;

                    QMetaObject::invokeMethod(
                        this,
                        [this,
                         request,
                         path,
                         qtImage = std::move(qtImage)]() mutable
                        {
                            if (request != ImageLoadRequest.load())
                                return;

                            AppImageProvider.setImage(
                                std::move(qtImage)
                            );

                            SetImageRevision(ImageRevision + 1);

                            SetStatusText(
                                QString("Opened %1")
                                    .arg(QString::fromStdString(
                                        path.string()
                                    ))
                            );
                        },
                        Qt::QueuedConnection
                    );
                }
                catch (const std::exception& error)
                {
                    const QString message =
                        QString::fromUtf8(error.what());

                    QMetaObject::invokeMethod(
                        this,
                        [this, request, message]()
                        {
                            if (request != ImageLoadRequest.load())
                                return;

                            SetStatusText(
                                QString("Failed to open RAW: %1")
                                    .arg(message)
                            );
                        },
                        Qt::QueuedConnection
                    );
                }
            }
        )
    );
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
