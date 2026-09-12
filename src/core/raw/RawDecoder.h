#pragma once
#include <cstdint>
#include <filesystem>
#include <memory>
#include <vector>

struct Image
{
    int width;
    int height;
    int channels;
    std::vector<std::uint8_t> pixels;
};

class RawDecoder {
public:
    virtual ~RawDecoder() = default;
    virtual Image decode(const std::filesystem::path& path) const;

    static void throwLibRawError(const std::string& operation, int errorCode);
};






