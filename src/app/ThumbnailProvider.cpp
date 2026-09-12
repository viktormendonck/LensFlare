#include "ThumbnailProvider.h"

#include "ImageCollectionModel.h"
#include "helpers.h"

#include <QBuffer>
#include <QByteArray>
#include <QImage>
#include <QImageReader>
#include <QMetaObject>
#include <QPointer>
#include <QQuickTextureFactory>
#include <QRunnable>
#include <QThreadPool>

#include <filesystem>

using lensFlare::image::Thumbnail;

namespace
{
    QImage loadThumbnailImage(
        const std::filesystem::path& path,
        const QSize& requestedSize
    )
    {
        const Thumbnail thumbnail =
            lensFlare::image::LoadThumbnail(path);

        if (thumbnail.data.empty())
            return {};

        QImage image;

        if (thumbnail.isJpeg)
        {
            QByteArray bytes(
                reinterpret_cast<const char*>(thumbnail.data.data()),
                static_cast<int>(thumbnail.data.size())
            );

            QBuffer buffer(&bytes);
            if (!buffer.open(QIODevice::ReadOnly))
                return {};

            QImageReader reader(&buffer, "JPEG");
            reader.setAutoTransform(true);

            // Let the decoder do the scaling when possible instead of
            // decoding the full embedded JPEG and shrinking it afterwards.
            if (requestedSize.isValid() &&
                requestedSize.width() > 0 &&
                requestedSize.height() > 0)
            {
                reader.setScaledSize(requestedSize);
            }

            image = reader.read();
        }
        else
        {
            if (thumbnail.width <= 0 ||
                thumbnail.height <= 0 ||
                thumbnail.channels != 3)
            {
                return {};
            }

            image = QImage(
                thumbnail.data.data(),
                thumbnail.width,
                thumbnail.height,
                thumbnail.width * thumbnail.channels,
                QImage::Format_RGB888
            ).copy();

            if (requestedSize.isValid())
            {
                image = image.scaled(
                    requestedSize,
                    Qt::KeepAspectRatio,
                    Qt::SmoothTransformation
                );
            }
        }

        return image;
    }

    class ThumbnailResponse final : public QQuickImageResponse
    {
    public:
        ThumbnailResponse(
            std::filesystem::path path,
            QSize requestedSize
        )
        {
            QPointer<ThumbnailResponse> self(this);

            QThreadPool::globalInstance()->start(
                QRunnable::create(
                    [self,
                     path = std::move(path),
                     requestedSize]() mutable
                    {
                        QImage image = loadThumbnailImage(
                            path,
                            requestedSize
                        );

                        if (!self)
                            return;

                        // Publish the result back on the response object's
                        // thread, then tell QML that the request is complete.
                        QMetaObject::invokeMethod(
                            self,
                            [self, image = std::move(image)]() mutable
                            {
                                if (!self)
                                    return;

                                self->Image = std::move(image);
                                emit self->finished();
                            },
                            Qt::QueuedConnection
                        );
                    }
                )
            );
        }

        QQuickTextureFactory* textureFactory() const override
        {
            if (Image.isNull())
                return nullptr;

            return QQuickTextureFactory::textureFactoryForImage(Image);
        }

    private:
        QImage Image;
    };
}

ThumbnailProvider::ThumbnailProvider(ImageCollectionModel* collection)
    : Collection(collection)
{
}

QQuickImageResponse* ThumbnailProvider::requestImageResponse(
    const QString& id,
    const QSize& requestedSize
)
{
    bool ok = false;
    const int index = id.toInt(&ok);

    if (!ok || Collection == nullptr)
        return new ThumbnailResponse({}, requestedSize);

    const int count = Collection->rowCount();
    if (index < 0 || index >= count)
        return new ThumbnailResponse({}, requestedSize);

    // Resolve the model index to a path here, before the worker starts.
    // The background thread only touches the filesystem/core loader, not the
    // QAbstractListModel itself.
    const std::filesystem::path path =
        Collection->GetEntryAt(index).filePath;

    return new ThumbnailResponse(path, requestedSize);
}
