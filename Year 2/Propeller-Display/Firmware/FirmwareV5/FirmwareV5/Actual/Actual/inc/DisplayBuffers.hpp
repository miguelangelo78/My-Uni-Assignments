#pragma once
#include <array>
#include <cstdint>
#include <cstddef>
#include <climits>
#include <algorithm>
#include "ColourT.hpp"




#include <stdio.h>
#define BIT(n)                  ( 1<<(n) )
#define BIT_MASK(len) ( BIT(len)-1 )
#define BF_MASK(start, len)     ( BIT_MASK(len)<<(start) )

static inline uint32_t bf_merge(uint32_t y, uint32_t x, uint32_t shift, uint32_t len) {
    uint32_t mask= BIT_MASK(len);
    return (y &~ (mask<<shift)) | (x & mask)<<shift;
}

class SegmentBuffer {
public:

    SegmentBuffer() {
        std::fill(buffer.begin(), buffer.end(), 0);
    }



    void setColour(int channelIndex, unsigned int value) {
        /*     if (channelIndex % 2 == 0) {
                 // high
                 buffer[channelIndex * 12 / 8] = value & 0xFF;
                 buffer[channelIndex * 12 / 8 + 1] = (buffer[channelIndex * 12 / 8 + 1] & 0xF0) | (0xF00 & value) >> 8;
             } else {
                 buffer[channelIndex * 12 / 8] = (buffer[channelIndex * 12 / 8] & 0x0F) | (value & 0x0F) << 4;
                 buffer[(channelIndex)* 12 / 8 + 1] = (value & 0xFF0) >> 4;
             }
             */

        if ( !(channelIndex & 1) ) { /* If index is not odd */
            /* The first 8 bits are in the leading octet,
            The last 4 bits are the lowest 4 in the trailing octet */

            buffer[channelIndex * 3 / 2] = value;

            buffer[channelIndex * 3 / 2 + 1] &= 0xF0;

            buffer[channelIndex * 3 / 2 + 1] |= value >> 8;
        } else {
            /* The first 4 bits are the high 4 in the leading octet,
            The last 8 bits are in the trailing octet */

            --channelIndex;

            buffer[channelIndex * 3 / 2 + 1] &= 0x0F;

            buffer[channelIndex * 3 / 2 + 1] = value & 0xFu << 4;

            buffer[channelIndex * 3 / 2 + 2] = value >> 4;
        }
    }


    // sets the pixel at index
    void setPixel(int pixelIndex, ColourT colour) {
        setColour(pixelIndex * 3, colour.red());
        setColour(pixelIndex * 3 + 1, colour.blue());
        setColour(pixelIndex * 3 + 2, colour.green());
    }

    static std::size_t size() {
        return 32;
    }

    static std::size_t sizeInBytes() {
        return 144;
    }
    std::uint8_t *getPointer() {
        return buffer.data();
    }

private:
    std::array<std::uint8_t, 144> buffer;
};


class DotCorrectionData {
public:
    DotCorrectionData() {
        std::fill(buffer.begin(), buffer.end(), 0xFF);
    }

    void setCorrection(int dotIndex) {

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

class ImageBuffer {
public:
    ImageBuffer() {

    }
    // get segment buffer at index
    SegmentBuffer& getSegment(int index) {
        return buffer[index];
    }

    // set segment at index
    void setSegment(int index, SegmentBuffer& segmentBuffer) {
        buffer[index] = segmentBuffer;
    }

    // number of segment buffer in the image buffer
    constexpr static std::size_t size() {
        return 120;
    }

private:
    std::array<SegmentBuffer, 120> buffer;
};


