#include "ThumbnailProvider.h"

#include "ImageCollectionModel.h"
#include "helpers.h"

#include <QBuffer>
#include <QByteArray>
#include <QImageReader>
#include <QMetaObject>
#include <QMutexLocker>
#include <QPointer>
#include <QQuickTextureFactory>
#include <QRunnable>
#include <QThreadPool>

#include <filesystem>

#include "raw/RawDecoder.h"



namespace
{
    QImage loadThumbnailImage(
        const std::filesystem::path& path,
        const QSize&
    )
    {
        constexpr int ThumbnailHeight = 920;

        const Thumbnail thumbnail = RawDecoder::LoadThumbnail(path);
        if (thumbnail.data.empty()) return {};

        QImage image;

        if (thumbnail.isJpeg)
        {
            QByteArray bytes(
                reinterpret_cast<const char*>(thumbnail.data.data()),
                static_cast<int>(thumbnail.data.size())
            );

            QBuffer buffer(&bytes);
            if (!buffer.open(QIODevice::ReadOnly)) return {};

            QImageReader reader(&buffer, "JPEG");
            reader.setAutoTransform(true);

            const QSize sourceSize = reader.size();
            if (sourceSize.isValid() && sourceSize.height() > ThumbnailHeight)
            {
                const int width = qMax(
                    1,
                    qRound(
                        static_cast<double>(sourceSize.width()) /
                        sourceSize.height() * ThumbnailHeight
                    )
                );
                reader.setScaledSize(QSize(width, ThumbnailHeight));
            }

            image = reader.read();
        }
        else
        {
            if (thumbnail.width <= 0 ||
                thumbnail.height <= 0 ||
                thumbnail.channels != 3)
                return {};

            image = QImage(
                thumbnail.data.data(),
                thumbnail.width,
                thumbnail.height,
                thumbnail.width * thumbnail.channels,
                QImage::Format_RGB888
            ).copy();
        }

        if (image.isNull()) return {};

        if (image.height() > ThumbnailHeight)
            image = image.scaledToHeight(
                ThumbnailHeight,
                Qt::SmoothTransformation
            );

        return image;
    }

    class ThumbnailResponse final : public QQuickImageResponse
    {
    public:
        ThumbnailResponse(
            std::filesystem::path path,
            QSize requestedSize,
            QString cacheKey,
            QString folder,
            QHash<QString, QImage>* cache,
            QMutex* cacheMutex,
            QString* cachedFolder
        )
        {
            QPointer<ThumbnailResponse> self(this);

            QThreadPool::globalInstance()->start(QRunnable::create(
                [self,
                 path = std::move(path),
                 requestedSize,
                 cacheKey = std::move(cacheKey),
                 folder = std::move(folder),
                 cache,
                 cacheMutex,
                 cachedFolder]() mutable
                {
                    QImage image;
                    {
                        QMutexLocker lock(cacheMutex);
                        const auto it = cache->constFind(cacheKey);
                        if (it != cache->cend()) image = it.value();
                    }
                    if (image.isNull())
                    {
                        image = loadThumbnailImage(path, requestedSize);

                        if (!image.isNull())
                        {
                            QMutexLocker lock(cacheMutex);
                            if (*cachedFolder == folder)
                                cache->insert(cacheKey, image);
                        }
                    }

                    if (!self) return;

                    QMetaObject::invokeMethod(
                        self,
                        [self, image = std::move(image)]() mutable
                        {
                            if (!self) return;
                            self->Image = std::move(image);
                            emit self->finished();
                        },
                        Qt::QueuedConnection
                    );
                }
            ));
        }

        QQuickTextureFactory* textureFactory() const override
        {
            return Image.isNull()
                ? nullptr
                : QQuickTextureFactory::textureFactoryForImage(Image);
        }

    private:
        QImage Image;
    };
}

ThumbnailProvider::ThumbnailProvider(ImageCollectionModel* collection)
    : Collection(collection)
{
}

QImage ThumbnailProvider::GetCachedThumbnail(
    const std::filesystem::path& path)
{
    const QString key =
        QString::fromStdString(path.string());

    QMutexLocker lock(&CacheMutex);

    const auto it = Cache.constFind(key);

    return it == Cache.cend()
        ? QImage{}
    : it.value();
}

QQuickImageResponse* ThumbnailProvider::requestImageResponse(
    const QString& id,
    const QSize& requestedSize
)
{
    bool ok = false;
    const int index = id.toInt(&ok);

    if (!ok || !Collection ||
        index < 0 || index >= Collection->rowCount())
        return new ThumbnailResponse(
            {}, requestedSize, {}, {},
            &Cache, &CacheMutex, &CachedFolder
        );

    const std::filesystem::path path = Collection->GetEntryAt(index).filePath;
    const QString key = QString::fromStdString(path.string());
    const QString folder = QString::fromStdString(path.parent_path().string());

    {
        QMutexLocker lock(&CacheMutex);
        if (CachedFolder != folder)
        {
            Cache.clear();
            CachedFolder = folder;
        }
    }

    return new ThumbnailResponse(
        path, requestedSize, key, folder,
        &Cache, &CacheMutex, &CachedFolder
    );
}

void ThumbnailProvider::ClearCache()
{
    QMutexLocker lock(&CacheMutex);
    Cache.clear();
    CachedFolder.clear();
}
