#pragma once

#include <QImage>
#include <QQuickImageProvider>
#include <QReadWriteLock>

class ImageProvider : public QQuickImageProvider
{
public:
    ImageProvider();

    void setImage(QImage image);

    QImage requestImage(
        const QString& id,
        QSize* size,
        const QSize& requestedSize
    ) override;

private:
    mutable QReadWriteLock ImageLock;
    QImage Image;
};
