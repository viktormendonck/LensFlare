#include "ImageCollectionModel.h"
#include "helpers.h"

#include <algorithm>
#include <filesystem>

ImageCollectionModel::ImageCollectionModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int ImageCollectionModel::rowCount(
    const QModelIndex& parent
) const
{
    if (parent.isValid())
        return 0;
    return static_cast<int>(Images.size());
}

QHash<int, QByteArray> ImageCollectionModel::roleNames() const
{
    return {
        { FileNameRole, "fileName" },
        { FilePathRole, "filePath" },
        { ExtensionNameRole, "extensionName" },
        { ThumbnailUrlRole, "thumbnail" }
    };
}

QVariant ImageCollectionModel::data(
    const QModelIndex& index,
    int role
) const
{
    if (!index.isValid())
        return {};

    if (index.row() < 0 ||
        index.row() >= static_cast<int>(Images.size()))
    {
        return {};
    }

    const ImageEntry& image = Images[index.row()];

    switch (role)
    {
    case FileNameRole:
        return QString::fromStdString(
            image.filePath.filename().string()
        );

    case FilePathRole:
        return QString::fromStdString(
            image.filePath.string()
        );

    case ExtensionNameRole:
        return QString::fromStdString(
            image.filePath.extension().string()
        );
    case ThumbnailUrlRole:
        return QString(
            "image://lensflare-thumbnail/%1"
        ).arg(index.row());
    default:
        return {};
    }
}

void ImageCollectionModel::LoadSiblingsFromFile(const std::filesystem::path& ImagePath)
{
    beginResetModel();

    Images.clear();

    const std::filesystem::path directory = ImagePath.parent_path();

    for (const auto& entry : std::filesystem::directory_iterator(directory))
    {
        if (!entry.is_regular_file())continue;
        const std::filesystem::path& path = entry.path();
        if (!lensFlare::IsSupportedImageFile(path))continue;
        Images.push_back({path});
    }

    std::ranges::sort(Images,
        [](const ImageEntry& a, const ImageEntry& b)
        {
            return a.filePath.filename()
                < b.filePath.filename();
        }
    );
    endResetModel();
}

int ImageCollectionModel::FindIndexForPath(const std::filesystem::path& path) const
{
    for (int i = 0; i < static_cast<int>(Images.size()); ++i)
    {
        if (Images[i].filePath == path)
            return i;
    }
    return -1;
}
