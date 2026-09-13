#pragma once

#include "EditState.h"
#include "EditableImage.h"

namespace lensflare::image {

    class ImageProcessor
    {
    public:
        EditableImage process(const EditableImage& source,const EditState& editState) const;

    private:
        static void applyExposure(EditableImage& image,float exposure);
    };
}
