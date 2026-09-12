//
// Created by lazage on 9/12/26.
//

#include "helpers.h"

#include <algorithm>

bool lensFlare::IsSupportedImageFile(const std::filesystem::path& path)
{
    return IsSupportedImageExtension(path.extension().string());
}

bool lensFlare::IsSupportedImageExtension(std::string extension)
{
    std::ranges::transform(extension,extension.begin(),
        [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        }
    );// make all lowercase just to be sure
    return std::ranges::find(supportedExtensions,extension) != supportedExtensions.end();
}
