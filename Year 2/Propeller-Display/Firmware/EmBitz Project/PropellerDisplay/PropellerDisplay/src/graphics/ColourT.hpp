#pragma once
#include <cstdint>

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
