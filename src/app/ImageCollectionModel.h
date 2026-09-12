#pragma once
#include <filesystem>
#include <QAbstractListModel>
#include <QString>
#include <vector>

struct ImageEntry
{
    std::filesystem::path filePath;
};

class ImageCollectionModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles
    {
        FileNameRole = Qt::UserRole + 1,
        FilePathRole,
        ExtensionNameRole
    };

    explicit ImageCollectionModel(QObject* parent = nullptr);

    int rowCount(
        const QModelIndex& parent = QModelIndex()
    ) const override;
    QHash<int, QByteArray> roleNames() const;

    QVariant data(
        const QModelIndex& index,
        int role = Qt::DisplayRole
    ) const override;

    void LoadSiblingsFromFile(const std::filesystem::path& ImagePath);

private:
    std::vector<ImageEntry> Images;
};

