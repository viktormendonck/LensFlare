#include "helpers.h"

#include <algorithm>
#include <cctype>

bool lensFlare::IsSupportedImageFile(
    const std::filesystem::path& path)
{
    return IsSupportedImageExtension(
        path.extension().string()
    );
}

bool lensFlare::IsSupportedImageExtension(std::string extension)
{
    std::ranges::transform(
        extension,
        extension.begin(),
        [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        }
    );

    return std::ranges::find(
        supportedExtensions,
        extension
    ) != supportedExtensions.end();
}
