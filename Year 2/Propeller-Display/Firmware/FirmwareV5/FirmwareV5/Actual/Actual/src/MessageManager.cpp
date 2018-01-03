#include "MessageManager.hpp"
#include "LedDriver.hpp"
#include <utility>
#include <bitset>
#include <array>
//
//  Font data for Arial Unicode MS 6pt
//

using namespace std;
// null primary implementation
struct MsgFontMap {
    struct FontBitmap {
        array<unsigned, 9> bitmap;


        /// should the bitmap
        bool shouldDraw(int x, int y) {
            return !!(bitmap[y] & (1 << x));
        }
    };

    using Pair = pair<unsigned char, FontBitmap>;

    array<Pair, 37> map = {{
            make_pair('0', FontBitmap{{0x38, 0x44, 0x44, 0x44, 0x44, 0x44, 0x38, 0x00, 0x00}}),
            make_pair('1', FontBitmap{{0x10, 0x30, 0x50, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00}}),
            make_pair('2', FontBitmap{{0x38, 0x44, 0x04, 0x08, 0x10, 0x20, 0x7C, 0x00, 0x00}}),
            make_pair('3', FontBitmap{{0x38, 0x44, 0x04, 0x18, 0x04, 0x44, 0x38, 0x00, 0x00}}),
            make_pair('4', FontBitmap{{0x08, 0x18, 0x28, 0x48, 0x7C, 0x08, 0x08, 0x00, 0x00}}),
            make_pair('5', FontBitmap{{0x3C, 0x20, 0x78, 0x44, 0x04, 0x44, 0x38, 0x00, 0x00}}),
            make_pair('6', FontBitmap{{0x38, 0x44, 0x40, 0x78, 0x44, 0x44, 0x38, 0x00, 0x00}}),
            make_pair('7', FontBitmap{{0x7C, 0x04, 0x08, 0x10, 0x10, 0x20, 0x20, 0x00, 0x00}}),
            make_pair('8', FontBitmap{{0x38, 0x44, 0x44, 0x38, 0x44, 0x44, 0x38, 0x00, 0x00}}),
            make_pair('9', FontBitmap{{0x38, 0x44, 0x44, 0x3C, 0x04, 0x44, 0x38, 0x00, 0x00}}),
            make_pair('a', FontBitmap{{0x00, 0x00, 0x18, 0x04, 0x1C, 0x24, 0x1C, 0x00, 0x00}}),
            make_pair('b', FontBitmap{{0x20, 0x20, 0x38, 0x24, 0x24, 0x24, 0x38, 0x00, 0x00}}),
            make_pair('c', FontBitmap{{0x00, 0x00, 0x18, 0x20, 0x20, 0x20, 0x18, 0x00, 0x00}}),
            make_pair('d', FontBitmap{{0x04, 0x04, 0x1C, 0x24, 0x24, 0x24, 0x1C, 0x00, 0x00}}),
            make_pair('e', FontBitmap{{0x00, 0x00, 0x18, 0x24, 0x3C, 0x20, 0x1C, 0x00, 0x00}}),
            make_pair('f', FontBitmap{{0x04, 0x08, 0x1C, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00}}),
            make_pair('g', FontBitmap{{0x00, 0x00, 0x1C, 0x24, 0x24, 0x24, 0x1C, 0x04, 0x38}}),
            make_pair('h', FontBitmap{{0x20, 0x20, 0x38, 0x24, 0x24, 0x24, 0x24, 0x00, 0x00}}),
            make_pair('i', FontBitmap{{0x08, 0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00}}),
            make_pair('j', FontBitmap{{0x08, 0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x10}}),
            make_pair('k', FontBitmap{{0x20, 0x20, 0x24, 0x28, 0x38, 0x28, 0x24, 0x00, 0x00}}),
            make_pair('l', FontBitmap{{0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00}}),
            make_pair('m', FontBitmap{{0x00, 0x00, 0x76, 0x49, 0x49, 0x49, 0x49, 0x00, 0x00}}),
            make_pair('n', FontBitmap{{0x00, 0x00, 0x38, 0x24, 0x24, 0x24, 0x24, 0x00, 0x00}}),
            make_pair('o', FontBitmap{{0x00, 0x00, 0x18, 0x24, 0x24, 0x24, 0x18, 0x00, 0x00}}),
            make_pair('p', FontBitmap{{0x00, 0x00, 0x38, 0x24, 0x24, 0x24, 0x38, 0x20, 0x20}}),
            make_pair('q', FontBitmap{{0x00, 0x00, 0x1C, 0x24, 0x24, 0x24, 0x1C, 0x04, 0x04}}),
            make_pair('r', FontBitmap{{0x00, 0x00, 0x18, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00}}),
            make_pair('s', FontBitmap{{0x00, 0x00, 0x18, 0x20, 0x10, 0x08, 0x30, 0x00, 0x00}}),
            make_pair('t', FontBitmap{{0x08, 0x08, 0x1C, 0x08, 0x08, 0x08, 0x04, 0x00, 0x00}}),
            make_pair('u', FontBitmap{{0x00, 0x00, 0x24, 0x24, 0x24, 0x24, 0x1C, 0x00, 0x00}}),
            make_pair('v', FontBitmap{{0x00, 0x00, 0x44, 0x28, 0x28, 0x28, 0x10, 0x00, 0x00}}),
            make_pair('w', FontBitmap{{0x00, 0x00, 0x92, 0xAA, 0xAA, 0x44, 0x44, 0x00, 0x00}}),
            make_pair('x', FontBitmap{{0x00, 0x00, 0x44, 0x28, 0x10, 0x28, 0x44, 0x00, 0x00}}),
            make_pair('y', FontBitmap{{0x00, 0x00, 0x44, 0x28, 0x28, 0x28, 0x10, 0x10, 0x60}}),
            make_pair('z', FontBitmap{{0x00, 0x00, 0x38, 0x08, 0x10, 0x20, 0x38, 0x00, 0x00}}),
            make_pair(' ', FontBitmap{{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}})
        }
    };

    FontBitmap& getCharBitmap(unsigned char n) {
        return find_if(begin(map), end(map), [=](const Pair& entry) {
            return n == entry.first;
        })->second;
    }

    static constexpr int fontHeight() {
        return 9;
    }

    static constexpr int fontWidth() {
        return 8;
    }


};



namespace {

PixelLocation fillImage(ImageBuffer& image,
                        ColourT colour,
                        int first = 0,
                        int last = ImageBuffer::size(),
                        int pixelOffset = 0,
                        int pixelLast = SegmentBuffer::size()) {
    int i = first;
    int j = pixelOffset;

    for(; i != last; ++i) {
        for(j = pixelOffset; j != pixelLast; ++j) {
            image.getSegment(i).setPixel(j, colour);
        }
    }

    return PixelLocation {i, j};
}

template<class Function>
PixelLocation fillImageIf(ImageBuffer& image, ColourT colour, Function fun, int first = 0, int last = ImageBuffer::size(),
                          int offsetPixel = 0,
                          int pixelLast = SegmentBuffer::size()) {
    int i = first;
    int j = offsetPixel;

    for(; i != last; ++i) {
        for(j = offsetPixel; j != pixelLast; ++j) {
            if(fun(PixelLocation {i, j})) {
                image.getSegment(i).setPixel(j, colour);
            }
        }
    }

    return PixelLocation {i, j};
}


}

namespace {
PixelLocation drawFont(ImageBuffer& image, unsigned char letter, ColourT colour, int first = 0, int offsetPixels = 0) {
    static MsgFontMap msgFontMap;
    unsigned startSeg = first;
    unsigned startPix = offsetPixels;

    auto bitmap = msgFontMap.getCharBitmap(letter);

    auto shouldDraw = [&](PixelLocation loc) {
        return bitmap.shouldDraw(loc.segmentIndex - startSeg, loc.pixelIndex - startPix);
    };

    return fillImageIf(image, colour, shouldDraw, first, first + MsgFontMap::fontWidth(), offsetPixels, offsetPixels + MsgFontMap::fontHeight());
}
}




void MessageManager::init() {
    numCharDisplayed = 0;
    bgColour = ColourT {0x0, 0x0, 0x0};
    textColour = ColourT {0x0FF, 0x00, 0x00};

}



static void displayImpl(ImageBuffer& img, const std::array<std::uint8_t, 23>& msg, int count, ColourT bgColour, ColourT textColor) {
    int segIndex = 0;
    int pixOffset = 0;
    int i = 0;

    // draw the background of the message area
    fillImage(img, bgColour);

    // draw the font consecutively around the
    while((segIndex != ImageBuffer::size() - MsgFontMap::fontWidth() - 1) && (i != count)) {
        PixelLocation lastPixel = drawFont(img, msg[i++], textColor, segIndex, 0);

        segIndex += lastPixel.segmentIndex + 1;
    }
}

void MessageManager::display(std::array<uint8_t, 23>& msg, int count) {
    displayedMsg = msg;
    displayImpl(*DrawManager::get().getUpdateImage(), displayedMsg, count, bgColour, textColour);

    DrawManager::get().swapBuffers();
}
