#pragma once

#include <cstddef>
#include <vector>

namespace lensflare::image {

class EditableImage
{
public:
    EditableImage() = default;

    EditableImage(int width, int height)
        : Width(width),
          Height(height),
          Pixels(
              static_cast<std::size_t>(width) *
              static_cast<std::size_t>(height) * 3
          )
    {
    }

    int GetWidth() const { return Width; }
    int GetHeight() const { return Height; }

    bool IsNull() const
    {
        return Width <= 0 || Height <= 0 || Pixels.empty();
    }

    std::size_t PixelCount() const
    {
        return static_cast<std::size_t>(Width) *
               static_cast<std::size_t>(Height);
    }

    const std::vector<float>& GetPixels() const { return Pixels; }
    std::vector<float>& GetPixels() { return Pixels; }

    const float* GetData() const { return Pixels.data(); }
    float* GetData() { return Pixels.data(); }

private:
    int Width = 0;
    int Height = 0;
    std::vector<float> Pixels;
};

}
