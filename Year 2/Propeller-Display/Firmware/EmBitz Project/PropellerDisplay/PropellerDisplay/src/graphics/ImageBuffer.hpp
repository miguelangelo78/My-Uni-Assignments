#pragma once

class SegmentBuffer {
public:

    SegmentBuffer() {
        std::fill(buffer.begin(), buffer.end(), 0);
    }

    void setColour(int channelIndex, unsigned int value) {
        if (channelIndex % 2 == 0) {
            // high
            buffer[channelIndex * 12 / 8] = value & 0xFF;
            buffer[channelIndex * 12 / 8 + 1] = (buffer[channelIndex * 12 / 8 + 1] & 0xF0) | (0xF00 & value) >> 8;
        } else {
            buffer[channelIndex * 12 / 8] = (buffer[channelIndex * 12 / 8] & 0x0F) | (value & 0x0F) << 4;
            buffer[(channelIndex)* 12 / 8 + 1] = (value & 0xFF0) >> 4;
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

class ImageBuffer {
public:
    ImageBuffer() {}

    // get segment buffer at index
    SegmentBuffer& getSegment(int index) {
        return buffer[index];
    }

    SegmentBuffer& operator[](const int index) {
        return buffer[index];
    }

    // set segment at index
    void setSegment(int index, SegmentBuffer& segmentBuffer) {
        buffer[index] = segmentBuffer;
    }

    // number of segment buffer in the image buffer
    constexpr static std::size_t size() {
        return 72;
    }

private:
    std::array<SegmentBuffer, 72> buffer;
};
