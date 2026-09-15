#pragma once

#include <libraw/libraw.h>

namespace lensflare::raw {

class LibRawProcessor final : public LibRaw
{
public:
    void ScaleColors();
    void PreInterpolate();
    void AhdInterpolate();

    int ColorAt(int row, int column);
};

}
