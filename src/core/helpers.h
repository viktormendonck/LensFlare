#pragma once
#include <array>
#include <filesystem>
#include <vector>

namespace lensFlare
{
    static constexpr std::array supportedExtensions{
        ".cr2",
        ".cr3",
        ".nef",
        ".nrw",
        ".arw",
        ".raf",
        ".dng",
        ".orf",
        ".rw2",
        ".pef"
    };

    bool IsSupportedImageFile(const std::filesystem::path& path);
    bool IsSupportedImageExtension(std::string extension);
    namespace image
    {
        struct Thumbnail
        {
            std::vector<std::uint8_t> data;
            int width{0};
            int height{0};
            int channels{0};
            bool isJpeg{false};
        };
        Thumbnail LoadThumbnail(std::filesystem::path path);
    }
};


