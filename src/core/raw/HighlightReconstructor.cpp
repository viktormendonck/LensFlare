#include "HighlightReconstructor.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>

#include "RawProcessingConstants.h"

namespace lensflare::raw {

HighlightMask HighlightReconstructor::BuildMask(
    LibRawProcessor& processor,
    const CancelCheck& cancelled) const
{
    HighlightMask mask;

    if (IsCancelled(cancelled))
        return mask;

    const int width = processor.imgdata.sizes.iwidth;
    const int height = processor.imgdata.sizes.iheight;

    if (width <= 0 || height <= 0 || !processor.imgdata.image)
        return mask;

    const float sensorWhite =
        static_cast<float>(processor.imgdata.color.maximum);

    if (sensorWhite <= 0.0f)
        return mask;

    const float clippingThreshold =
        sensorWhite *
        RawProcessingConstants::HighlightClippingThreshold;

    mask.Width = width;
    mask.Height = height;
    mask.Channels.assign(
        static_cast<std::size_t>(width) *
        static_cast<std::size_t>(height),
        0
    );

    for (int y = 0; y < height; ++y)
    {
        if ((y & 0x7F) == 0 && IsCancelled(cancelled))
            return {};

        for (int x = 0; x < width; ++x)
        {
            const int cfaChannel =
                processor.ColorAt(y, x);

            const int rgbChannel =
                ToRgbChannel(cfaChannel);

            if (rgbChannel < 0)
                continue;

            const std::size_t pixelIndex =
                static_cast<std::size_t>(y) *
                static_cast<std::size_t>(width) +
                static_cast<std::size_t>(x);

            const float value =
                static_cast<float>(
                    processor.imgdata.image[pixelIndex][cfaChannel]
                );

            if (value < clippingThreshold)
                continue;

            ++mask.ClippedSamples;

            for (int offsetY =
                     -RawProcessingConstants::HighlightMaskDilationRadius;
                 offsetY <=
                     RawProcessingConstants::HighlightMaskDilationRadius;
                 ++offsetY)
            {
                for (int offsetX =
                         -RawProcessingConstants::HighlightMaskDilationRadius;
                     offsetX <=
                         RawProcessingConstants::HighlightMaskDilationRadius;
                     ++offsetX)
                {
                    SetMasked(
                        mask,
                        x + offsetX,
                        y + offsetY,
                        rgbChannel
                    );
                }
            }
        }
    }

    std::cout
        << "[Lensflare RAW] highlight mask"
        << " | sensor white=" << sensorWhite
        << " | threshold=" << clippingThreshold
        << " | clipped CFA samples=" << mask.ClippedSamples
        << '\n';

    return mask;
}

void HighlightReconstructor::Reconstruct(
    LibRawProcessor& processor,
    HighlightMask mask,
    const CancelCheck& cancelled) const
{
    if (mask.IsNull() ||
        IsCancelled(cancelled) ||
        !processor.imgdata.image)
    {
        return;
    }

    const int width = processor.imgdata.sizes.iwidth;
    const int height = processor.imgdata.sizes.iheight;

    if (width != mask.Width || height != mask.Height)
        return;

    for (int iteration = 0;
         iteration <
             RawProcessingConstants::HighlightReconstructionIterations;
         ++iteration)
    {
        if (IsCancelled(cancelled))
            return;

        std::vector<PixelUpdate> updates;

        for (int y = 0; y < height; ++y)
        {
            if ((y & 0x3F) == 0 && IsCancelled(cancelled))
                return;

            for (int x = 0; x < width; ++x)
            {
                const std::size_t pixelIndex =
                    static_cast<std::size_t>(y) *
                    static_cast<std::size_t>(width) +
                    static_cast<std::size_t>(x);

                const std::uint8_t channelMask =
                    mask.Channels[pixelIndex];

                if (channelMask == 0)
                    continue;

                for (int channel = 0;
                     channel <
                         RawProcessingConstants::OutputChannelCount;
                     ++channel)
                {
                    if (!IsMasked(mask, pixelIndex, channel))
                        continue;

                    float value = 0.0f;

                    if (!EstimateChannel(
                        processor,
                        mask,
                        x,
                        y,
                        channel,
                        value))
                    {
                        continue;
                    }

                    updates.push_back({
                        static_cast<std::uint32_t>(pixelIndex),
                        static_cast<std::uint16_t>(
                            std::clamp(
                                value,
                                0.0f,
                                RawProcessingConstants::UInt16Maximum
                            )
                        ),
                        static_cast<std::uint8_t>(channel)
                    });
                }
            }
        }

        if (updates.empty())
        {
            std::cout
                << "[Lensflare RAW] guided highlight pass "
                << iteration
                << " | reconstructed=0"
                << '\n';
            break;
        }

        for (const PixelUpdate& update : updates)
        {
            processor.imgdata.image[
                update.PixelIndex
            ][update.Channel] = update.Value;

            mask.Channels[update.PixelIndex] &=
                static_cast<std::uint8_t>(
                    ~(1u << update.Channel)
                );
        }

        std::cout
            << "[Lensflare RAW] guided highlight pass "
            << iteration
            << " | reconstructed="
            << updates.size()
            << '\n';
    }
}

bool HighlightReconstructor::IsCancelled(
    const CancelCheck& cancelled)
{
    return cancelled && cancelled();
}

int HighlightReconstructor::ToRgbChannel(
    int cfaChannel)
{
    switch (cfaChannel)
    {
    case 0:
        return 0;
    case 1:
    case 3:
        return 1;
    case 2:
        return 2;
    default:
        return -1;
    }
}

bool HighlightReconstructor::IsMasked(
    const HighlightMask& mask,
    std::size_t pixelIndex,
    int channel)
{
    return (
        mask.Channels[pixelIndex] &
        static_cast<std::uint8_t>(1u << channel)
    ) != 0;
}

void HighlightReconstructor::SetMasked(
    HighlightMask& mask,
    int x,
    int y,
    int channel)
{
    if (x < 0 || x >= mask.Width ||
        y < 0 || y >= mask.Height ||
        channel < 0 ||
        channel >=
            RawProcessingConstants::OutputChannelCount)
    {
        return;
    }

    const std::size_t pixelIndex =
        static_cast<std::size_t>(y) *
        static_cast<std::size_t>(mask.Width) +
        static_cast<std::size_t>(x);

    mask.Channels[pixelIndex] |=
        static_cast<std::uint8_t>(
            1u << channel
        );
}

bool HighlightReconstructor::EstimateChannel(
    LibRawProcessor& processor,
    const HighlightMask& mask,
    int x,
    int y,
    int channel,
    float& result)
{
    const int width = processor.imgdata.sizes.iwidth;
    const int height = processor.imgdata.sizes.iheight;

    const std::size_t currentIndex =
        static_cast<std::size_t>(y) *
        static_cast<std::size_t>(width) +
        static_cast<std::size_t>(x);

    const unsigned short* currentPixel =
        processor.imgdata.image[currentIndex];

    const float currentGuide =
        GetGuideValue(
            currentPixel,
            channel
        );

    if (currentGuide <=
        RawProcessingConstants::HighlightMinimumGuideValue)
    {
        return false;
    }

    float weightedRatio = 0.0f;
    float totalWeight = 0.0f;

    for (int deltaY =
             -RawProcessingConstants::HighlightGuideRadius;
         deltaY <=
             RawProcessingConstants::HighlightGuideRadius;
         ++deltaY)
    {
        const int neighbourY = y + deltaY;

        if (neighbourY < 0 || neighbourY >= height)
            continue;

        for (int deltaX =
                 -RawProcessingConstants::HighlightGuideRadius;
             deltaX <=
                 RawProcessingConstants::HighlightGuideRadius;
             ++deltaX)
        {
            if (deltaX == 0 && deltaY == 0)
                continue;

            const int distanceSquared =
                deltaX * deltaX +
                deltaY * deltaY;

            if (distanceSquared >
                RawProcessingConstants::
                    HighlightGuideRadiusSquared)
            {
                continue;
            }

            const int neighbourX = x + deltaX;

            if (neighbourX < 0 || neighbourX >= width)
                continue;

            const std::size_t neighbourIndex =
                static_cast<std::size_t>(neighbourY) *
                static_cast<std::size_t>(width) +
                static_cast<std::size_t>(neighbourX);

            if (IsMasked(
                mask,
                neighbourIndex,
                channel))
            {
                continue;
            }

            const unsigned short* neighbourPixel =
                processor.imgdata.image[neighbourIndex];

            const float neighbourGuide =
                GetGuideValue(
                    neighbourPixel,
                    channel
                );

            if (neighbourGuide <=
                RawProcessingConstants::HighlightMinimumGuideValue)
            {
                continue;
            }

            float ratio =
                static_cast<float>(
                    neighbourPixel[channel]
                ) / neighbourGuide;

            ratio = std::clamp(
                ratio,
                0.0f,
                RawProcessingConstants::HighlightMaximumRatio
            );

            const float weight =
                CalculateWeight(
                    currentGuide,
                    neighbourGuide,
                    deltaX,
                    deltaY
                );

            if (weight <= 0.0f)
                continue;

            weightedRatio +=
                ratio * weight;

            totalWeight += weight;
        }
    }

    if (totalWeight <=
        RawProcessingConstants::HighlightMinimumWeight)
    {
        return false;
    }

    result =
        currentGuide *
        (weightedRatio / totalWeight);

    return std::isfinite(result) &&
           result >= 0.0f;
}

float HighlightReconstructor::GetGuideValue(
    const unsigned short* pixel,
    int reconstructedChannel)
{
    float sum = 0.0f;
    int count = 0;

    for (int channel = 0;
         channel <
             RawProcessingConstants::OutputChannelCount;
         ++channel)
    {
        if (channel == reconstructedChannel)
            continue;

        sum += static_cast<float>(
            pixel[channel]
        );

        ++count;
    }

    if (count == 0)
        return 0.0f;

    return sum /
           static_cast<float>(count);
}

float HighlightReconstructor::CalculateWeight(
    float currentGuide,
    float neighbourGuide,
    int deltaX,
    int deltaY)
{
    const float reference =
        std::max(
            std::max(
                currentGuide,
                neighbourGuide
            ),
            RawProcessingConstants::HighlightMinimumGuideValue
        );

    const float normalizedDifference =
        std::abs(
            currentGuide -
            neighbourGuide
        ) / reference;

    const float edgeWeight =
        1.0f /
        (
            1.0f +
            RawProcessingConstants::HighlightGuideSensitivity *
            normalizedDifference *
            normalizedDifference
        );

    const int distanceSquared =
        deltaX * deltaX +
        deltaY * deltaY;

    const float spatialWeight =
        1.0f /
        static_cast<float>(
            1 + distanceSquared
        );

    return edgeWeight * spatialWeight;
}

}
