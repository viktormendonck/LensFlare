#include "RawDecoder.h"

#include <algorithm>
#include <cstring>
#include <libraw/libraw.h>
#include <stdexcept>
#include <string>

void RawDecoder::throwLibRawError(
    const std::string& operation,
    int errorCode)
{
    throw std::runtime_error(
        operation + " failed: " + libraw_strerror(errorCode)
    );
}

Image RawDecoder::decode(
    const std::filesystem::path& path,
    const CancelCheck& cancelled) const
{
    if (IsCancelled(cancelled)) return {};

    LibRaw processor;
    processor.imgdata.params.output_bps = 8;
    processor.imgdata.params.use_camera_wb = 1;
    processor.imgdata.params.no_auto_bright = 0;

    int result = processor.open_file(path.string().c_str());
    if (result != LIBRAW_SUCCESS)
        throwLibRawError("Opening RAW file", result);

    if (IsCancelled(cancelled)) return {};

    result = processor.unpack();
    if (result != LIBRAW_SUCCESS)
        throwLibRawError("Unpacking RAW file", result);

    if (IsCancelled(cancelled)) return {};

    result = processor.dcraw_process();
    if (result != LIBRAW_SUCCESS)
        throwLibRawError("Processing RAW file", result);

    if (IsCancelled(cancelled)) return {};

    int memoryError = LIBRAW_SUCCESS;
    libraw_processed_image_t* processed =
        processor.dcraw_make_mem_image(&memoryError);

    if (!processed || memoryError != LIBRAW_SUCCESS)
    {
        if (processed) LibRaw::dcraw_clear_mem(processed);
        throwLibRawError("Creating processed image", memoryError);
    }

    try
    {
        if (processed->type != LIBRAW_IMAGE_BITMAP)
            throw std::runtime_error("LibRaw returned an unsupported image type");

        if (processed->bits != 8)
            throw std::runtime_error("Lensflare milestone 1 expects 8-bit LibRaw output");

        if (processed->colors != 3)
            throw std::runtime_error("Lensflare milestone 1 expects RGB LibRaw output");

        if (IsCancelled(cancelled))
        {
            LibRaw::dcraw_clear_mem(processed);
            return {};
        }

        Image image;
        image.width = static_cast<int>(processed->width);
        image.height = static_cast<int>(processed->height);
        image.channels = static_cast<int>(processed->colors);
        image.pixels.resize(processed->data_size);
        std::memcpy(
            image.pixels.data(),
            processed->data,
            processed->data_size
        );

        LibRaw::dcraw_clear_mem(processed);
        return image;
    }
    catch (...)
    {
        LibRaw::dcraw_clear_mem(processed);
        throw;
    }
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
        std::swap(width, height);

    return static_cast<float>(width) /
           static_cast<float>(height);
}
