#include "RawDecoder.h"

#include <algorithm>
#include <libraw/libraw.h>
#include <stdexcept>
#include <string>
#include <iostream>

#include "LibRawProcessor.h"

void RawDecoder::throwLibRawError(
    const std::string& operation,
    int errorCode)
{
    throw std::runtime_error(
        operation + " failed: " + libraw_strerror(errorCode)
    );
}

lensflare::image::EditableImage RawDecoder::decode(
    const std::filesystem::path& path,
    const CancelCheck& cancelled) const
{
    if (IsCancelled(cancelled))
        return {};

    std::cerr
        << "[Lensflare RAW] decode start: "
        << path
        << '\n';

    lensflare::raw::LibRawProcessor processor;

    int result = processor.open_file(path.string().c_str());
    if (result != LIBRAW_SUCCESS)
        throwLibRawError("Opening RAW file", result);

    std::cerr
        << "[Lensflare RAW] open_file ok"
        << " | make=" << processor.imgdata.idata.make
        << " | model=" << processor.imgdata.idata.model
        << " | raw=" << processor.imgdata.sizes.raw_width
        << "x" << processor.imgdata.sizes.raw_height
        << " | visible=" << processor.imgdata.sizes.width
        << "x" << processor.imgdata.sizes.height
        << " | flip=" << processor.imgdata.sizes.flip
        << '\n';

    if (IsCancelled(cancelled))
        return {};

    result = processor.unpack();
    if (result != LIBRAW_SUCCESS)
        throwLibRawError("Unpacking RAW file", result);

    std::cerr
        << "[Lensflare RAW] unpack ok"
        << " | black=" << processor.imgdata.color.black
        << " | white=" << processor.imgdata.color.maximum
        << " | filters=" << processor.imgdata.idata.filters
        << '\n';

    if (IsCancelled(cancelled))
        return {};

    auto image = Developer.Develop(
        processor,
        cancelled
    );

    std::cerr
        << "[Lensflare RAW] decode finished"
        << " | output=" << image.GetWidth()
        << "x" << image.GetHeight()
        << " | null=" << image.IsNull()
        << '\n';

    return image;
}

Thumbnail RawDecoder::LoadThumbnail(
    const std::filesystem::path& path)
{
    LibRaw raw;

    if (raw.open_file(path.string().c_str()) != LIBRAW_SUCCESS)
        return {};

    if (raw.unpack_thumb() != LIBRAW_SUCCESS)
        return {};

    int error = 0;
    libraw_processed_image_t* preview =
        raw.dcraw_make_mem_thumb(&error);

    if (!preview || error != LIBRAW_SUCCESS)
        return {};

    Thumbnail thumbnail;
    thumbnail.width = preview->width;
    thumbnail.height = preview->height;
    thumbnail.channels = preview->colors;
    thumbnail.isJpeg = preview->type == LIBRAW_IMAGE_JPEG;

    if (preview->type == LIBRAW_IMAGE_JPEG ||
        preview->type == LIBRAW_IMAGE_BITMAP)
    {
        thumbnail.data.assign(
            preview->data,
            preview->data + preview->data_size
        );
    }

    LibRaw::dcraw_clear_mem(preview);
    return thumbnail;
}

float RawDecoder::GetPreviewAspectRatio(
    const std::filesystem::path& path)
{
    LibRaw raw;

    if (raw.open_file(path.string().c_str()) != LIBRAW_SUCCESS)
        return 1.0f;

    int width = raw.imgdata.thumbnail.twidth;
    int height = raw.imgdata.thumbnail.theight;

    if (width <= 0 || height <= 0)
        return 1.0f;

    if (raw.imgdata.sizes.flip == 5 ||
        raw.imgdata.sizes.flip == 6)
    {
        std::swap(width, height);
    }

    return static_cast<float>(width) /
           static_cast<float>(height);
}
