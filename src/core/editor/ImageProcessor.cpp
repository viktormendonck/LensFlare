#include "ImageProcessor.h"

#include <cmath>

namespace lensflare::image {

    EditableImage ImageProcessor::process(
        const EditableImage& source,
        const EditState& editState
    ) const
    {
        if (source.IsNull()) return {};

        EditableImage result = source;
        applyExposure(result, editState.exposure);
        return result;
    }

    void ImageProcessor::applyExposure(
        EditableImage& image,
        const float exposure
    )
    {
        if (exposure == 0.0f)
            return;

        const float multiplier = std::exp2(exposure);
        std::vector<float>& pixels = image.GetPixels();
        for (float& channel : pixels)
        {
            channel *= multiplier;
        }
    }

}