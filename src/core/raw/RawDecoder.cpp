#include "RawDecoder.h"
#include <libraw/libraw.h>

#include <cstring>
#include <stdexcept>
#include <string>


void RawDecoder::throwLibRawError(const std::string& operation, int errorCode)
{
    throw std::runtime_error(
        operation + " failed: " + libraw_strerror(errorCode)
    );
}

Image RawDecoder::decode(const std::filesystem::path& path) const
{
    LibRaw processor;
    processor.imgdata.params.output_bps = 8;
    processor.imgdata.params.use_camera_wb = 1;
    processor.imgdata.params.no_auto_bright = 0;

    const std::string filename = path.string();

    int result = processor.open_file(filename.c_str());
    if (result != LIBRAW_SUCCESS) {
        throwLibRawError("Opening RAW file", result);
    }

    result = processor.unpack();
    if (result != LIBRAW_SUCCESS) {
        throwLibRawError("Unpacking RAW file", result);
    }

    result = processor.dcraw_process();
    if (result != LIBRAW_SUCCESS) {
        throwLibRawError("Processing RAW file", result);
    }


    int memoryError = LIBRAW_SUCCESS;
    libraw_processed_image_t* processed = processor.dcraw_make_mem_image(&memoryError);
    if (processed == nullptr || memoryError != LIBRAW_SUCCESS) {
        if (processed != nullptr) {
            LibRaw::dcraw_clear_mem(processed);
        }
        throwLibRawError("Creating processed image", memoryError);
    }

    try {
        if (processed->type != LIBRAW_IMAGE_BITMAP) {
            throw std::runtime_error("LibRaw returned an unsupported image type");
        }

        if (processed->bits != 8) {
            throw std::runtime_error("Lensflare milestone 1 expects 8-bit LibRaw output");
        }

        if (processed->colors != 3) {
            throw std::runtime_error("Lensflare milestone 1 expects RGB LibRaw output");
        }

        Image image;
        image.width = static_cast<int>(processed->width);
        image.height = static_cast<int>(processed->height);
        image.channels = static_cast<int>(processed->colors);
        image.pixels.resize(processed->data_size);
        std::memcpy(image.pixels.data(), processed->data, processed->data_size);

        LibRaw::dcraw_clear_mem(processed);
        return image;
    } catch (...) {
        LibRaw::dcraw_clear_mem(processed);
        throw;
    }
}

