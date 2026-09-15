#pragma once

#include <cstdint>
#include <functional>
#include <vector>

#include "LibRawProcessor.h"

namespace lensflare::raw {

struct HighlightMask
{
    int Width = 0;
    int Height = 0;
    std::size_t ClippedSamples = 0;
    std::vector<std::uint8_t> Channels;

    bool IsNull() const
    {
        return Width <= 0 ||
               Height <= 0 ||
               Channels.empty();
    }
};

class HighlightReconstructor
{
public:
    using CancelCheck = std::function<bool()>;

    HighlightMask BuildMask(
        LibRawProcessor& processor,
        const CancelCheck& cancelled = {}
    ) const;

    void Reconstruct(
        LibRawProcessor& processor,
        HighlightMask mask,
        const CancelCheck& cancelled = {}
    ) const;

private:
    struct PixelUpdate
    {
        std::uint32_t PixelIndex;
        std::uint16_t Value;
        std::uint8_t Channel;
    };

    static bool IsCancelled(const CancelCheck& cancelled);

    static int ToRgbChannel(int cfaChannel);

    static bool IsMasked(
        const HighlightMask& mask,
        std::size_t pixelIndex,
        int channel
    );

    static void SetMasked(
        HighlightMask& mask,
        int x,
        int y,
        int channel
    );

    static bool EstimateChannel(
        LibRawProcessor& processor,
        const HighlightMask& mask,
        int x,
        int y,
        int channel,
        float& result
    );

    static float GetGuideValue(
        const unsigned short* pixel,
        int reconstructedChannel
    );

    static float CalculateWeight(
        float currentGuide,
        float neighbourGuide,
        int deltaX,
        int deltaY
    );
};

}
