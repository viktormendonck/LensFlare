#include "LibRawProcessor.h"

namespace lensflare::raw {

void LibRawProcessor::ScaleColors()
{
    scale_colors();
}

void LibRawProcessor::PreInterpolate()
{
    pre_interpolate();
}

void LibRawProcessor::AhdInterpolate()
{
    ahd_interpolate();
}

int LibRawProcessor::ColorAt(
    int row,
    int column)
{
    return COLOR(row, column);
}

}
