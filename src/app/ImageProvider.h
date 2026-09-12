#pragma once

#include <QImage>
#include <QQuickImageProvider>

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
    QImage image_;
};