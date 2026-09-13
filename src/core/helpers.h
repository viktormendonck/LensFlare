#pragma once
#include <array>
#include <filesystem>

namespace lensFlare
{
    inline constexpr std::array supportedExtensions{
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
}
