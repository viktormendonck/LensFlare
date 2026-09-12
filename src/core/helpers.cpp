//
// Created by lazage on 9/12/26.
//

#include "helpers.h"

#include <algorithm>
#include <libraw.h>

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

lensFlare::image::Thumbnail lensFlare::image::LoadThumbnail(std::filesystem::path path)
{
    LibRaw raw;

    if (raw.open_file(path.string().c_str()) != LIBRAW_SUCCESS)
        return {};

    if (raw.unpack_thumb() != LIBRAW_SUCCESS)
        return {};

    int error = 0;

    libraw_processed_image_t* preview =
        raw.dcraw_make_mem_thumb(&error);

    if (preview == nullptr || error != LIBRAW_SUCCESS)
        return {};

    lensFlare::image::Thumbnail thumbnail;

    thumbnail.width = preview->width;
    thumbnail.height = preview->height;
    thumbnail.channels = preview->colors;

    if (preview->type == LIBRAW_IMAGE_JPEG)
    {
        thumbnail.isJpeg = true;

        thumbnail.data.assign(
            preview->data,
            preview->data + preview->data_size
        );
    }
    else if (preview->type == LIBRAW_IMAGE_BITMAP)
    {
        thumbnail.isJpeg = false;

        thumbnail.data.assign(
            preview->data,
            preview->data + preview->data_size
        );
    }

    LibRaw::dcraw_clear_mem(preview);

    return thumbnail;
}

