#pragma once

#include <QQuickAsyncImageProvider>

class ImageCollectionModel;

class ThumbnailProvider : public QQuickAsyncImageProvider
{
public:
    explicit ThumbnailProvider(ImageCollectionModel* collection);

    QQuickImageResponse* requestImageResponse(
        const QString& id,
        const QSize& requestedSize
    ) override;

private:
    ImageCollectionModel* Collection;
};
