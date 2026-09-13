#pragma once

#include <filesystem>
#include <QHash>
#include <QImage>
#include <QMutex>
#include <QQuickAsyncImageProvider>

class ImageCollectionModel;

class ThumbnailProvider : public QQuickAsyncImageProvider
{
public:
    explicit ThumbnailProvider(ImageCollectionModel* collection);

    QImage GetCachedThumbnail(const std::filesystem::path& path);

    QQuickImageResponse* requestImageResponse(
        const QString& id,
        const QSize& requestedSize
    ) override;

    void ClearCache();

private:
    ImageCollectionModel* Collection;
    QHash<QString, QImage> Cache;
    QMutex CacheMutex;
    QString CachedFolder;
};
