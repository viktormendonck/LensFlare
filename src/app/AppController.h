#pragma once

#include <atomic>

#include <QObject>
#include <QUrl>

#include "ImageCollectionModel.h"
#include "ImageProvider.h"
#include "editor/EditState.h"
#include "editor/EditableImage.h"
#include "editor/PreviewImageGenerator.h"
#include "raw/RawDecoder.h"

class ThumbnailProvider;

class AppController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString statusText READ GetStatusText WRITE SetStatusText NOTIFY StatusTextChanged)
    Q_PROPERTY(int imageRevision READ GetImageRevision NOTIFY ImageRevisionChanged)
    Q_PROPERTY(QUrl previewSource READ GetPreviewSource NOTIFY ImageRevisionChanged)
    Q_PROPERTY(bool previewReady READ GetPreviewReady NOTIFY PreviewReadyChanged)
    Q_PROPERTY(ImageCollectionModel* imageCollection READ GetImageCollection CONSTANT)
    Q_PROPERTY(int currentImageIndex READ GetCurrentImageIndex NOTIFY CurrentImageIndexChanged)
    Q_PROPERTY(float exposure READ GetExposure WRITE SetExposure NOTIFY ExposureChanged)
    Q_PROPERTY(QString loadingThumbnailSource READ GetLoadingThumbnailSource NOTIFY LoadingThumbnailSourceChanged)

public:
    AppController(
        ImageProvider& imageProvider,
        ImageCollectionModel* imageCollection,
        ThumbnailProvider& thumbnailProvider
    );
    ~AppController() override;

    Q_INVOKABLE void OpenFile(const QUrl& url);
    Q_INVOKABLE void OpenImage(int index);
    Q_INVOKABLE void SaveButton();
    Q_INVOKABLE void UndoButton();
    Q_INVOKABLE void RedoButton();

    QString GetStatusText() const { return StatusText; }
    void SetStatusText(const QString& in)
    {
        StatusText = in;
        emit StatusTextChanged();
    }

    int GetImageRevision() const { return ImageRevision; }
    int GetCurrentImageIndex() const { return CurrentImageIndex; }
    float GetExposure() const { return CurrentEditState.exposure; }
    bool GetPreviewReady() const { return PreviewReady; }
    QString GetLoadingThumbnailSource() const { return LoadingThumbnailSource; }

    void SetPreviewReady(bool in)
    {
        if (PreviewReady == in) return;

        PreviewReady = in;
        emit PreviewReadyChanged();
    }

    QUrl GetPreviewSource() const
    {
        return QUrl(QString("image://lensflare/current?v=%1").arg(ImageRevision));
    }

    void SetCurrentImageIndex(int in)
    {
        if (CurrentImageIndex == in) return;
        CurrentImageIndex = in;
        emit CurrentImageIndexChanged();
    }

    void SetImageRevision(int in)
    {
        ImageRevision = in;
        emit ImageRevisionChanged();
    }

    void SetExposure(float exposure);

    ImageCollectionModel* GetImageCollection() const { return ImageCollection; }

signals:
    void StatusTextChanged();
    void ImageRevisionChanged();
    void PreviewReadyChanged();
    void CurrentImageIndexChanged();
    void ExposureChanged();
    void LoadingThumbnailSourceChanged();

private:
    void ResetEdits();
    void PublishPreview(QImage preview);

    QString StatusText{"idle"};
    int ImageRevision{0};
    int CurrentImageIndex{0};
    bool PreviewReady{false};
    std::atomic<int> ImageLoadRequest{0};

    RawDecoder AppRawDecoder{};
    lensflare::image::PreviewImageGenerator AppPreviewGenerator{};
    lensflare::image::EditableImage CurrentSourceImage{};
    EditState CurrentEditState{};

    ImageProvider& AppImageProvider;
    ThumbnailProvider& AppThumbnailProvider;
    ImageCollectionModel* ImageCollection{nullptr};
    QString LoadingThumbnailSource;
};
