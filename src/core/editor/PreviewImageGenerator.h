#pragma once

#include <QImage>
#include <QSize>

#include "EditableImage.h"

namespace lensflare::image {

class PreviewImageGenerator
{
public:
    QImage Create(const EditableImage& source) const;

private:
    static QSize CalculatePreviewSize(const EditableImage& source);
    static float SampleChannel(
        const EditableImage& source,
        float x,
        float y,
        int channel
    );
};

}
