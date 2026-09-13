#include "ImageProvider.h"

#include <QReadLocker>
#include <QWriteLocker>

#include <utility>

ImageProvider::ImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Image)
{
}

void ImageProvider::setImage(QImage image)
{
    QWriteLocker lock(&ImageLock);
    Image = std::move(image);
}

QImage ImageProvider::requestImage(
    const QString&,
    QSize* size,
    const QSize&)
{
    QReadLocker lock(&ImageLock);

    if (size)
        *size = Image.size();

    return Image;
}
