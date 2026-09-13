#pragma once
#include <cstdint>
#include <filesystem>
#include <functional>
#include <vector>

struct Image
{
    int width{0};
    int height{0};
    int channels{0};
    std::vector<std::uint8_t> pixels;
};

struct Thumbnail
{
    std::vector<std::uint8_t> data;
    int width{0};
    int height{0};
    int channels{0};
    bool isJpeg{false};
};

class RawDecoder
{
public:
    using CancelCheck = std::function<bool()>;

    virtual ~RawDecoder() = default;

    virtual Image decode(
        const std::filesystem::path& path,
        const CancelCheck& cancelled = {}
    ) const;

    static Thumbnail LoadThumbnail(const std::filesystem::path& path);
    static float GetPreviewAspectRatio(const std::filesystem::path& path);

    static void throwLibRawError(
        const std::string& operation,
        int errorCode
    );

private:
    static bool IsCancelled(const CancelCheck& cancelled)
    {
        return cancelled && cancelled();
    }
};
