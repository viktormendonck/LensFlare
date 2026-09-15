#pragma once

#include <functional>

#include "editor/EditableImage.h"
#include "HighlightReconstructor.h"
#include "LibRawProcessor.h"

namespace lensflare::raw {

class RawDeveloper
{
public:
    using CancelCheck = std::function<bool()>;

    lensflare::image::EditableImage Develop(
        LibRawProcessor& processor,
        const CancelCheck& cancelled = {}
    ) const;

private:
    static constexpr int OutputChannelCount = 3;

    static bool IsCancelled(const CancelCheck& cancelled);

    static lensflare::image::EditableImage ConvertToWorkingImage(
        const LibRawProcessor& processor,
        const CancelCheck& cancelled
    );

    static lensflare::image::EditableImage ApplyOrientation(
        const lensflare::image::EditableImage& source,
        int orientation
    );

    HighlightReconstructor Reconstructor;
};

}
