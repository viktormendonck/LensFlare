#pragma once

#include <cstddef>

namespace lensflare::raw {

struct RawProcessingConstants
{
    static constexpr int CameraChannelCount = 4;
    static constexpr int OutputChannelCount = 3;

    static constexpr std::size_t CancellationCheckInterval = 65536;

    // Determine genuine sensor clipping before WB/scaling/demosaic.
    static constexpr float HighlightClippingThreshold = 0.995f;

    // Dilate each clipped CFA sample slightly because demosaic spreads
    // that sample's influence into neighbouring output pixels.
    static constexpr int HighlightMaskDilationRadius = 1;

    // Post-demosaic, edge-aware channel-ratio reconstruction.
    static constexpr int HighlightGuideRadius = 4;
    static constexpr int HighlightGuideRadiusSquared =
        HighlightGuideRadius * HighlightGuideRadius;

    // Iteration allows reconstructed boundary pixels to become guides
    // for progressively deeper clipped regions.
    static constexpr int HighlightReconstructionIterations = 3;

    // Reject guide pixels whose luminance/structure is too different.
    static constexpr float HighlightGuideSensitivity = 10.0f;

    // Prevent very dark guide values from causing unstable ratios.
    static constexpr float HighlightMinimumGuideValue = 32.0f;

    static constexpr float HighlightMinimumWeight = 0.0001f;

    // Limit pathological local channel-ratio estimates.
    static constexpr float HighlightMaximumRatio = 8.0f;

    // Keep reconstruction inside the 16-bit working buffer.
    static constexpr float UInt16Maximum = 65535.0f;
};

}
