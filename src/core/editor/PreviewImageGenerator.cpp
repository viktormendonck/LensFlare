#include "PreviewImageGenerator.h"

#include <algorithm>
#include <cmath>

#include <QtCore/qfloat16.h>

#include "EditorConstants.h"

namespace lensflare::image {

QImage PreviewImageGenerator::Create(const EditableImage& source) const
{
    if (source.IsNull())
        return {};

    const QSize previewSize = CalculatePreviewSize(source);
    QImage preview(previewSize, QImage::Format_RGBA16FPx4);

    const float scaleX = static_cast<float>(source.GetWidth()) /
                         static_cast<float>(previewSize.width());
    const float scaleY = static_cast<float>(source.GetHeight()) /
                         static_cast<float>(previewSize.height());

    for (int y = 0; y < previewSize.height(); ++y)
    {
        auto* row = reinterpret_cast<qfloat16*>(preview.scanLine(y));
        const float sourceY = (static_cast<float>(y) + 0.5f) * scaleY - 0.5f;

        for (int x = 0; x < previewSize.width(); ++x)
        {
            const float sourceX = (static_cast<float>(x) + 0.5f) * scaleX - 0.5f;
            const int destinationIndex = x * EditorConstants::PreviewChannelCount;

            row[destinationIndex] = qfloat16{SampleChannel(source, sourceX, sourceY, 0)};
            row[destinationIndex + 1] = qfloat16{SampleChannel(source, sourceX, sourceY, 1)};
            row[destinationIndex + 2] = qfloat16{SampleChannel(source, sourceX, sourceY, 2)};
            row[destinationIndex + 3] = qfloat16{1.0f};
        }
    }

    return preview;
}

QSize PreviewImageGenerator::CalculatePreviewSize(const EditableImage& source)
{
    const bool landscape = source.GetWidth() >= source.GetHeight();
    const int maxWidth = landscape
        ? EditorConstants::PreviewLongEdge
        : EditorConstants::PreviewShortEdge;
    const int maxHeight = landscape
        ? EditorConstants::PreviewShortEdge
        : EditorConstants::PreviewLongEdge;

    const float widthScale = static_cast<float>(maxWidth) /
                             static_cast<float>(source.GetWidth());
    const float heightScale = static_cast<float>(maxHeight) /
                              static_cast<float>(source.GetHeight());
    const float scale = std::min({1.0f, widthScale, heightScale});

    const int width = std::max(
        1,
        static_cast<int>(std::round(static_cast<float>(source.GetWidth()) * scale))
    );
    const int height = std::max(
        1,
        static_cast<int>(std::round(static_cast<float>(source.GetHeight()) * scale))
    );

    return {width, height};
}

float PreviewImageGenerator::SampleChannel(
    const EditableImage& source,
    float x,
    float y,
    int channel)
{
    const float clampedX = std::clamp(
        x,
        0.0f,
        static_cast<float>(source.GetWidth() - 1)
    );
    const float clampedY = std::clamp(
        y,
        0.0f,
        static_cast<float>(source.GetHeight() - 1)
    );

    const int x0 = static_cast<int>(std::floor(clampedX));
    const int y0 = static_cast<int>(std::floor(clampedY));
    const int x1 = std::min(x0 + 1, source.GetWidth() - 1);
    const int y1 = std::min(y0 + 1, source.GetHeight() - 1);

    const float tx = clampedX - static_cast<float>(x0);
    const float ty = clampedY - static_cast<float>(y0);

    const auto& pixels = source.GetPixels();

    const auto sample = [&pixels, &source, channel](int sampleX, int sampleY)
    {
        const std::size_t index =
            (static_cast<std::size_t>(sampleY) *
             static_cast<std::size_t>(source.GetWidth()) +
             static_cast<std::size_t>(sampleX)) *
            EditorConstants::SourceChannelCount +
            static_cast<std::size_t>(channel);

        return pixels[index];
    };

    const float topLeft = sample(x0, y0);
    const float topRight = sample(x1, y0);
    const float bottomLeft = sample(x0, y1);
    const float bottomRight = sample(x1, y1);

    const float top = topLeft + (topRight - topLeft) * tx;
    const float bottom = bottomLeft + (bottomRight - bottomLeft) * tx;
    return top + (bottom - top) * ty;
}

}
