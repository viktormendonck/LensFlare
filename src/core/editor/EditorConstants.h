#pragma once

namespace lensflare::image {

class EditorConstants final
{
public:
    static constexpr int PreviewLongEdge = 2560;
    static constexpr int PreviewShortEdge = 1440;
    static constexpr int SourceChannelCount = 3;
    static constexpr int PreviewChannelCount = 4;
};

}
