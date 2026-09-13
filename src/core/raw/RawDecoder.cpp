#include "RawDecoder.h"

#include <algorithm>
#include <cstdint>
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

lensflare::image::EditableImage RawDecoder::decode(
    const std::filesystem::path& path,
    const CancelCheck& cancelled) const
{
    if (IsCancelled(cancelled))
        return {};

    LibRaw processor;
    processor.imgdata.params.output_bps = 16;
    processor.imgdata.params.use_camera_wb = 1;
    processor.imgdata.params.use_auto_wb = 0;
    processor.imgdata.params.no_auto_bright = 1;
    processor.imgdata.params.bright = 1.0f;
    processor.imgdata.params.gamm[0] = 1.0f;
    processor.imgdata.params.gamm[1] = 1.0f;

    int result = processor.open_file(path.string().c_str());
    if (result != LIBRAW_SUCCESS)
        throwLibRawError("Opening RAW file", result);

    if (IsCancelled(cancelled))
        return {};

    result = processor.unpack();
    if (result != LIBRAW_SUCCESS)
        throwLibRawError("Unpacking RAW file", result);

    if (IsCancelled(cancelled))
        return {};

    result = processor.dcraw_process();
    if (result != LIBRAW_SUCCESS)
        throwLibRawError("Processing RAW file", result);

    if (IsCancelled(cancelled))
        return {};

    int memoryError = LIBRAW_SUCCESS;
    libraw_processed_image_t* processed =
        processor.dcraw_make_mem_image(&memoryError);

    if (!processed || memoryError != LIBRAW_SUCCESS)
    {
        if (processed)
            LibRaw::dcraw_clear_mem(processed);

        throwLibRawError("Creating processed image", memoryError);
    }

    try
    {
        if (processed->type != LIBRAW_IMAGE_BITMAP)
            throw std::runtime_error("LibRaw returned an unsupported image type");

        if (processed->bits != 16)
            throw std::runtime_error("Lensflare expects 16-bit LibRaw output");

        if (processed->colors != 3)
            throw std::runtime_error("Lensflare expects RGB LibRaw output");

        if (IsCancelled(cancelled))
        {
            LibRaw::dcraw_clear_mem(processed);
            return {};
        }

        lensflare::image::EditableImage image(
            static_cast<int>(processed->width),
            static_cast<int>(processed->height)
        );

        auto& destination = image.GetPixels();
        const auto* source =
            reinterpret_cast<const std::uint16_t*>(processed->data);

        constexpr float normalization = 1.0f / 65535.0f;

        for (std::size_t i = 0; i < destination.size(); ++i)
        {
            if ((i & 0xFFFF) == 0 && IsCancelled(cancelled))
            {
                LibRaw::dcraw_clear_mem(processed);
                return {};
            }

            destination[i] = static_cast<float>(source[i]) * normalization;
        }

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
    {
        std::swap(width, height);
    }

    return static_cast<float>(width) /
           static_cast<float>(height);
}
