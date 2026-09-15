#include "RawDeveloper.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <utility>

namespace lensflare::raw {

lensflare::image::EditableImage RawDeveloper::Develop(
    LibRawProcessor& processor,
    const CancelCheck& cancelled) const
{
    if (IsCancelled(cancelled))
        return {};

    const auto filters =
        processor.imgdata.idata.filters;

    if (filters == 0)
    {
        throw std::runtime_error(
            "The float RAW pipeline currently requires a CFA sensor"
        );
    }

    if (filters < 1000)
    {
        throw std::runtime_error(
            "The float RAW pipeline currently supports standard Bayer sensors only"
        );
    }

    std::cout
        << "[Lensflare RAW] RawDeveloper::Develop entered"
        << '\n';

    const int result =
        processor.raw2image_ex(1);

    if (result != LIBRAW_SUCCESS)
    {
        throw std::runtime_error(
            std::string("Preparing RAW image failed: ") +
            libraw_strerror(result)
        );
    }

    if (IsCancelled(cancelled))
        return {};

    std::cout
        << "[Lensflare RAW] raw2image_ex ok"
        << " | image="
        << processor.imgdata.sizes.iwidth
        << "x"
        << processor.imgdata.sizes.iheight
        << " | black="
        << processor.imgdata.color.black
        << " | white="
        << processor.imgdata.color.maximum
        << '\n';

    std::cout
        << "[Lensflare RAW] camera WB"
        << " | R=" << processor.imgdata.color.cam_mul[0]
        << " | G1=" << processor.imgdata.color.cam_mul[1]
        << " | B=" << processor.imgdata.color.cam_mul[2]
        << " | G2=" << processor.imgdata.color.cam_mul[3]
        << '\n';

    // Capture which physical CFA sites were genuinely clipped while
    // the buffer is still unscaled sensor-linear data.
    auto highlightMask =
        Reconstructor.BuildMask(
            processor,
            cancelled
        );

    if (IsCancelled(cancelled))
        return {};

    // Let LibRaw perform the camera-aware color scaling it normally
    // performs before interpolation instead of approximating it here.
    processor.imgdata.params.use_camera_wb = 1;
    processor.imgdata.params.use_auto_wb = 0;
    processor.imgdata.params.no_auto_bright = 1;
    processor.imgdata.params.bright = 1.0f;

    std::cout
        << "[Lensflare RAW] scale_colors start"
        << '\n';

    processor.ScaleColors();

    std::cout
        << "[Lensflare RAW] scale_colors finished"
        << '\n';

    if (IsCancelled(cancelled))
        return {};

    processor.imgdata.params.four_color_rgb = 0;

    std::cout
        << "[Lensflare RAW] pre_interpolate start"
        << '\n';

    processor.PreInterpolate();

    std::cout
        << "[Lensflare RAW] pre_interpolate finished"
        << '\n';

    if (IsCancelled(cancelled))
        return {};

    std::cout
        << "[Lensflare RAW] AHD demosaic start"
        << '\n';

    processor.AhdInterpolate();

    std::cout
        << "[Lensflare RAW] AHD demosaic finished"
        << '\n';

    if (IsCancelled(cancelled))
        return {};

    std::cout
        << "[Lensflare RAW] guided highlight reconstruction start"
        << '\n';

    Reconstructor.Reconstruct(
        processor,
        std::move(highlightMask),
        cancelled
    );

    std::cout
        << "[Lensflare RAW] guided highlight reconstruction finished"
        << '\n';

    if (IsCancelled(cancelled))
        return {};

    std::cout
        << "[Lensflare RAW] converting camera RGB to working RGB"
        << '\n';

    auto image =
        ConvertToWorkingImage(
            processor,
            cancelled
        );

    if (image.IsNull() ||
        IsCancelled(cancelled))
    {
        return {};
    }

    std::array<float, 3> minimum{
        std::numeric_limits<float>::max(),
        std::numeric_limits<float>::max(),
        std::numeric_limits<float>::max()
    };

    std::array<float, 3> maximum{
        std::numeric_limits<float>::lowest(),
        std::numeric_limits<float>::lowest(),
        std::numeric_limits<float>::lowest()
    };

    std::array<std::size_t, 3> aboveOne{};

    const auto& pixels =
        image.GetPixels();

    for (std::size_t i = 0;
         i < pixels.size();
         i += OutputChannelCount)
    {
        for (int channel = 0;
             channel < OutputChannelCount;
             ++channel)
        {
            const float value =
                pixels[
                    i +
                    static_cast<std::size_t>(channel)
                ];

            minimum[channel] =
                std::min(
                    minimum[channel],
                    value
                );

            maximum[channel] =
                std::max(
                    maximum[channel],
                    value
                );

            if (value > 1.0f)
                ++aboveOne[channel];
        }
    }

    std::cout
        << "[Lensflare RAW] working RGB stats"
        << " | R=[" << minimum[0] << ", " << maximum[0] << "]"
        << " >1=" << aboveOne[0]
        << " | G=[" << minimum[1] << ", " << maximum[1] << "]"
        << " >1=" << aboveOne[1]
        << " | B=[" << minimum[2] << ", " << maximum[2] << "]"
        << " >1=" << aboveOne[2]
        << '\n';

    return ApplyOrientation(
        image,
        processor.imgdata.sizes.flip
    );
}

bool RawDeveloper::IsCancelled(
    const CancelCheck& cancelled)
{
    return cancelled && cancelled();
}

lensflare::image::EditableImage
RawDeveloper::ConvertToWorkingImage(
    const LibRawProcessor& processor,
    const CancelCheck& cancelled)
{
    const int width =
        processor.imgdata.sizes.iwidth;

    const int height =
        processor.imgdata.sizes.iheight;

    if (width <= 0 || height <= 0)
        return {};

    constexpr float UInt16ToFloat =
        1.0f / 65535.0f;

    lensflare::image::EditableImage output(
        width,
        height
    );

    auto& destination =
        output.GetPixels();

    const std::size_t pixelCount =
        static_cast<std::size_t>(width) *
        static_cast<std::size_t>(height);

    for (std::size_t i = 0;
         i < pixelCount;
         ++i)
    {
        if ((i & 0xFFFF) == 0 &&
            IsCancelled(cancelled))
        {
            return {};
        }

        const float cameraRed =
            static_cast<float>(
                processor.imgdata.image[i][0]
            ) * UInt16ToFloat;

        const float cameraGreen =
            static_cast<float>(
                processor.imgdata.image[i][1]
            ) * UInt16ToFloat;

        const float cameraBlue =
            static_cast<float>(
                processor.imgdata.image[i][2]
            ) * UInt16ToFloat;

        const float cameraGreenTwo =
            static_cast<float>(
                processor.imgdata.image[i][3]
            ) * UInt16ToFloat;

        const float linearRed =
            processor.imgdata.color.rgb_cam[0][0] * cameraRed +
            processor.imgdata.color.rgb_cam[0][1] * cameraGreen +
            processor.imgdata.color.rgb_cam[0][2] * cameraBlue +
            processor.imgdata.color.rgb_cam[0][3] * cameraGreenTwo;

        const float linearGreen =
            processor.imgdata.color.rgb_cam[1][0] * cameraRed +
            processor.imgdata.color.rgb_cam[1][1] * cameraGreen +
            processor.imgdata.color.rgb_cam[1][2] * cameraBlue +
            processor.imgdata.color.rgb_cam[1][3] * cameraGreenTwo;

        const float linearBlue =
            processor.imgdata.color.rgb_cam[2][0] * cameraRed +
            processor.imgdata.color.rgb_cam[2][1] * cameraGreen +
            processor.imgdata.color.rgb_cam[2][2] * cameraBlue +
            processor.imgdata.color.rgb_cam[2][3] * cameraGreenTwo;

        const std::size_t destinationIndex =
            i * OutputChannelCount;

        destination[destinationIndex] =
            linearRed;

        destination[destinationIndex + 1] =
            linearGreen;

        destination[destinationIndex + 2] =
            linearBlue;
    }

    return output;
}

lensflare::image::EditableImage
RawDeveloper::ApplyOrientation(
    const lensflare::image::EditableImage& source,
    int orientation)
{
    if (source.IsNull() || orientation == 0)
        return source;

    const int sourceWidth =
        source.GetWidth();

    const int sourceHeight =
        source.GetHeight();

    const auto& sourcePixels =
        source.GetPixels();

    if (orientation == 3)
    {
        lensflare::image::EditableImage result(
            sourceWidth,
            sourceHeight
        );

        auto& destination =
            result.GetPixels();

        for (int y = 0;
             y < sourceHeight;
             ++y)
        {
            for (int x = 0;
                 x < sourceWidth;
                 ++x)
            {
                const int destinationX =
                    sourceWidth - 1 - x;

                const int destinationY =
                    sourceHeight - 1 - y;

                const std::size_t sourceIndex =
                    (
                        static_cast<std::size_t>(y) *
                        sourceWidth +
                        x
                    ) *
                    OutputChannelCount;

                const std::size_t destinationIndex =
                    (
                        static_cast<std::size_t>(destinationY) *
                        sourceWidth +
                        destinationX
                    ) *
                    OutputChannelCount;

                destination[destinationIndex] =
                    sourcePixels[sourceIndex];

                destination[destinationIndex + 1] =
                    sourcePixels[sourceIndex + 1];

                destination[destinationIndex + 2] =
                    sourcePixels[sourceIndex + 2];
            }
        }

        return result;
    }

    if (orientation == 5 ||
        orientation == 6)
    {
        lensflare::image::EditableImage result(
            sourceHeight,
            sourceWidth
        );

        auto& destination =
            result.GetPixels();

        const int destinationWidth =
            result.GetWidth();

        for (int y = 0;
             y < sourceHeight;
             ++y)
        {
            for (int x = 0;
                 x < sourceWidth;
                 ++x)
            {
                const int destinationX =
                    orientation == 5
                        ? y
                        : sourceHeight - 1 - y;

                const int destinationY =
                    orientation == 5
                        ? sourceWidth - 1 - x
                        : x;

                const std::size_t sourceIndex =
                    (
                        static_cast<std::size_t>(y) *
                        sourceWidth +
                        x
                    ) *
                    OutputChannelCount;

                const std::size_t destinationIndex =
                    (
                        static_cast<std::size_t>(destinationY) *
                        destinationWidth +
                        destinationX
                    ) *
                    OutputChannelCount;

                destination[destinationIndex] =
                    sourcePixels[sourceIndex];

                destination[destinationIndex + 1] =
                    sourcePixels[sourceIndex + 1];

                destination[destinationIndex + 2] =
                    sourcePixels[sourceIndex + 2];
            }
        }

        return result;
    }

    return source;
}

}
