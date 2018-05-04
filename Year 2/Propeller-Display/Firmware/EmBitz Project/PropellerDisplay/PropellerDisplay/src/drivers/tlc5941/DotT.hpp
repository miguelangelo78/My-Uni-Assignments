#pragma once

#include <array>
#include <cstdint>
#include <cstddef>
#include <climits>
#include <algorithm>
#include "ColourT.hpp"

class DotCorrectionData {
public:
    DotCorrectionData() {
        std::fill(buffer.begin(), buffer.end(), 0x3F);
    }

    void setCorrection(int dotIndex, uint8_t dotValue) {
        buffer[dotIndex < 72 ? dotIndex : 0] = dotValue & 0x3F;
    }

    std::size_t size() const {
        return buffer.size();
    }

    std::uint8_t *getPointer()  {
        return buffer.data();
    }

private:
    std::array<std::uint8_t, 72> buffer;
};
