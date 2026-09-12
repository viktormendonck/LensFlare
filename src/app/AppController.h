#pragma once
#include <qobject.h>

#include "ImageCollectionModel.h"
#include "ImageProvider.h"
#include "raw/RawDecoder.h"


class AppController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(
        QString statusText
        READ GetStatusText
        WRITE SetStatusText
        NOTIFY StatusTextChanged
    )
    Q_PROPERTY(
        int imageRevision
        READ GetImageRevision
        NOTIFY ImageRevisionChanged
    )
    Q_PROPERTY(
        ImageCollectionModel* imageCollection
        READ GetImageCollection
        CONSTANT
    )
    Q_PROPERTY(
        int currentImageIndex
        READ GetCurrentImageIndex
        NOTIFY CurrentImageIndexChanged
    )
public:
    AppController(ImageProvider& imageProvider, ImageCollectionModel* imageCollection);
    ~AppController() override;

    Q_INVOKABLE void OpenFile(const QUrl& url);
    Q_INVOKABLE void SaveButton();
    Q_INVOKABLE void UndoButton();
    Q_INVOKABLE void RedoButton();

    QString GetStatusText() const{return StatusText;};
    void SetStatusText(const QString& in)
    {
        StatusText = in;
        StatusTextChanged();
    };
    int GetImageRevision() const{return ImageRevision;}
    int GetCurrentImageIndex() const{return CurrentImageIndex;}
    void SetCurrentImageIndex(const int in)
    {
        if (CurrentImageIndex == in)return;
        CurrentImageIndex = in;
        emit CurrentImageIndexChanged();
    }
    void SetImageRevision(const int in)
    {
        ImageRevision = in;
        ImageRevisionChanged();
    }
    ImageCollectionModel* GetImageCollection() const {return ImageCollection;};


signals:
    void StatusTextChanged();
    void ImageRevisionChanged();
    void CurrentImageIndexChanged();
private:
    QString StatusText{"idle"};
    int ImageRevision{0};
    int CurrentImageIndex{0};

    RawDecoder AppRawDecoder{};
    ImageProvider& AppImageProvider;
    ImageCollectionModel* ImageCollection{nullptr};
};


