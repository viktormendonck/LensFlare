#include "ImageProvider.h"

#include <utility>

ImageProvider::ImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Image)
{
}

void ImageProvider::setImage(QImage image)
{
    image_ = std::move(image);
}

QImage ImageProvider::requestImage(
    const QString&,
    QSize* size,
    const QSize&
)
{
    if (size)
        *size = image_.size();

    return image_;
}