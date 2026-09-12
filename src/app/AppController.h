#pragma once
#include <qobject.h>

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
public:
    AppController(ImageProvider& imageProvider);

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
    void SetImageRevision(const int in)
    {
        ImageRevision = in;
        ImageRevisionChanged();
    }

signals:
    void StatusTextChanged();
    void ImageRevisionChanged();
private:
    QString StatusText{"idle"};
    int ImageRevision{0};

    RawDecoder AppRawDecoder{};
    ImageProvider& AppImageProvider;
};


