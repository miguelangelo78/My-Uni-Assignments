#pragma once

#include <bitset>
#include "DisplayBuffers.hpp"
/// color class is
struct ColourT {
    std::uint64_t value;

    ColourT() = default;
    ColourT(std::uint16_t r, std::uint16_t g, std::uint16_t b) {
        value = static_cast<std::uint64_t>(g) |
                static_cast<std::uint64_t>(b) << 12 |
                static_cast<std::uint64_t>(r) << 24;

    }

    std::uint32_t red() {
        return (value & 0x000000FFFul);
    }

    std::uint32_t blue() {
        return (value & 0x000FFF000ul) >> 12;
    }

    std::uint32_t green() {
        return (value & 0xFFF000000ul) >> 24;
    }
};


// lazy hack we noticed that we got byte wrong so
// bits get flipped before being passed using this helper
inline ColourT makeColor(std::uint16_t red, std::uint16_t blue, std::uint16_t green) {
    return ColourT (red, green,  blue);
}



struct PixelLocation {
    int segmentIndex;
    int pixelIndex;

    bool operator==(const PixelLocation& pixel) {
        return pixel.segmentIndex == segmentIndex &&
               pixel.pixelIndex == pixelIndex;
    }

    bool operator!=(const PixelLocation& pixel) {
        return !(*this == pixel);
    }


};

