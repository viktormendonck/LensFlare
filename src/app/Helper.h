#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>

#include <QImage>

#include "editor/EditableImage.h"

inline QImage ToQImage(const lensflare::image::EditableImage& image)
{
    if (image.IsNull())
        return {};

    QImage qtImage(
        image.GetWidth(),
        image.GetHeight(),
        QImage::Format_RGB888
    );

    const auto& pixels = image.GetPixels();

    auto toByte = [](float value) -> uchar {
        value = std::max(value, 0.0f);
        value = std::pow(value, 1.0f / 2.2f);
        value = std::clamp(value, 0.0f, 1.0f);
        return static_cast<uchar>(value * 255.0f + 0.5f);
    };

    for (int y = 0; y < image.GetHeight(); ++y)
    {
        auto* dst = qtImage.scanLine(y);

        for (int x = 0; x < image.GetWidth(); ++x)
        {
            const std::size_t srcIndex =
                (static_cast<std::size_t>(y) *
                 static_cast<std::size_t>(image.GetWidth()) +
                 static_cast<std::size_t>(x)) * 3;

            const int dstIndex = x * 3;

            dst[dstIndex + 0] = toByte(pixels[srcIndex + 0]);
            dst[dstIndex + 1] = toByte(pixels[srcIndex + 1]);
            dst[dstIndex + 2] = toByte(pixels[srcIndex + 2]);
        }
    }

    return qtImage;
}
