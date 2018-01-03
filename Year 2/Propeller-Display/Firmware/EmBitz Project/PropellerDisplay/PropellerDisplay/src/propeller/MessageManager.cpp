#include "MessageManager.hpp"
#include "PropellerManager.hpp"
#include <utility>
//
//  Font data for Arial Unicode MS 6pt
//


// Character bitmaps for Arial Unicode MS 6pt
const uint8_t textBitmap[] = {
    // @0 '0' (8 pixels wide)
    0x38, //   ###
    0x44, //  #   #
    0x44, //  #   #
    0x44, //  #   #
    0x44, //  #   #
    0x44, //  #   #
    0x38, //   ###
    0x00, //
    0x00, //

    // @9 '1' (8 pixels wide)
    0x10, //    #
    0x30, //   ##
    0x50, //  # #
    0x10, //    #
    0x10, //    #
    0x10, //    #
    0x10, //    #
    0x00, //
    0x00, //

    // @18 '2' (8 pixels wide)
    0x38, //   ###
    0x44, //  #   #
    0x04, //      #
    0x08, //     #
    0x10, //    #
    0x20, //   #
    0x7C, //  #####
    0x00, //
    0x00, //

    // @27 '3' (8 pixels wide)
    0x38, //   ###
    0x44, //  #   #
    0x04, //      #
    0x18, //    ##
    0x04, //      #
    0x44, //  #   #
    0x38, //   ###
    0x00, //
    0x00, //

    // @36 '4' (8 pixels wide)
    0x08, //     #
    0x18, //    ##
    0x28, //   # #
    0x48, //  #  #
    0x7C, //  #####
    0x08, //     #
    0x08, //     #
    0x00, //
    0x00, //

    // @45 '5' (8 pixels wide)
    0x3C, //   ####
    0x20, //   #
    0x78, //  ####
    0x44, //  #   #
    0x04, //      #
    0x44, //  #   #
    0x38, //   ###
    0x00, //
    0x00, //

    // @54 '6' (8 pixels wide)
    0x38, //   ###
    0x44, //  #   #
    0x40, //  #
    0x78, //  ####
    0x44, //  #   #
    0x44, //  #   #
    0x38, //   ###
    0x00, //
    0x00, //

    // @63 '7' (8 pixels wide)
    0x7C, //  #####
    0x04, //      #
    0x08, //     #
    0x10, //    #
    0x10, //    #
    0x20, //   #
    0x20, //   #
    0x00, //
    0x00, //

    // @72 '8' (8 pixels wide)
    0x38, //   ###
    0x44, //  #   #
    0x44, //  #   #
    0x38, //   ###
    0x44, //  #   #
    0x44, //  #   #
    0x38, //   ###
    0x00, //
    0x00, //

    // @81 '9' (8 pixels wide)
    0x38, //   ###
    0x44, //  #   #
    0x44, //  #   #
    0x3C, //   ####
    0x04, //      #
    0x44, //  #   #
    0x38, //   ###
    0x00, //
    0x00, //

    // @90 'a' (8 pixels wide)
    0x00, //
    0x00, //
    0x18, //    ##
    0x04, //      #
    0x1C, //    ###
    0x24, //   #  #
    0x1C, //    ###
    0x00, //
    0x00, //

    // @99 'b' (8 pixels wide)
    0x20, //   #
    0x20, //   #
    0x38, //   ###
    0x24, //   #  #
    0x24, //   #  #
    0x24, //   #  #
    0x38, //   ###
    0x00, //
    0x00, //

    // @108 'c' (8 pixels wide)
    0x00, //
    0x00, //
    0x18, //    ##
    0x20, //   #
    0x20, //   #
    0x20, //   #
    0x18, //    ##
    0x00, //
    0x00, //

    // @117 'd' (8 pixels wide)
    0x04, //      #
    0x04, //      #
    0x1C, //    ###
    0x24, //   #  #
    0x24, //   #  #
    0x24, //   #  #
    0x1C, //    ###
    0x00, //
    0x00, //

    // @126 'e' (8 pixels wide)
    0x00, //
    0x00, //
    0x18, //    ##
    0x24, //   #  #
    0x3C, //   ####
    0x20, //   #
    0x1C, //    ###
    0x00, //
    0x00, //

    // @135 'f' (8 pixels wide)
    0x04, //      #
    0x08, //     #
    0x1C, //    ###
    0x08, //     #
    0x08, //     #
    0x08, //     #
    0x08, //     #
    0x00, //
    0x00, //

    // @144 'g' (8 pixels wide)
    0x00, //
    0x00, //
    0x1C, //    ###
    0x24, //   #  #
    0x24, //   #  #
    0x24, //   #  #
    0x1C, //    ###
    0x04, //      #
    0x38, //   ###

    // @153 'h' (8 pixels wide)
    0x20, //   #
    0x20, //   #
    0x38, //   ###
    0x24, //   #  #
    0x24, //   #  #
    0x24, //   #  #
    0x24, //   #  #
    0x00, //
    0x00, //

    // @162 'i' (8 pixels wide)
    0x08, //     #
    0x00, //
    0x08, //     #
    0x08, //     #
    0x08, //     #
    0x08, //     #
    0x08, //     #
    0x00, //
    0x00, //

    // @171 'j' (8 pixels wide)
    0x08, //     #
    0x00, //
    0x08, //     #
    0x08, //     #
    0x08, //     #
    0x08, //     #
    0x08, //     #
    0x08, //     #
    0x10, //    #

    // @180 'k' (8 pixels wide)
    0x20, //   #
    0x20, //   #
    0x24, //   #  #
    0x28, //   # #
    0x38, //   ###
    0x28, //   # #
    0x24, //   #  #
    0x00, //
    0x00, //

    // @189 'l' (8 pixels wide)
    0x08, //     #
    0x08, //     #
    0x08, //     #
    0x08, //     #
    0x08, //     #
    0x08, //     #
    0x08, //     #
    0x00, //
    0x00, //

    // @198 'm' (8 pixels wide)
    0x00, //
    0x00, //
    0x76, //  ### ##
    0x49, //  #  #  #
    0x49, //  #  #  #
    0x49, //  #  #  #
    0x49, //  #  #  #
    0x00, //
    0x00, //

    // @207 'n' (8 pixels wide)
    0x00, //
    0x00, //
    0x38, //   ###
    0x24, //   #  #
    0x24, //   #  #
    0x24, //   #  #
    0x24, //   #  #
    0x00, //
    0x00, //

    // @216 'o' (8 pixels wide)
    0x00, //
    0x00, //
    0x18, //    ##
    0x24, //   #  #
    0x24, //   #  #
    0x24, //   #  #
    0x18, //    ##
    0x00, //
    0x00, //

    // @225 'p' (8 pixels wide)
    0x00, //
    0x00, //
    0x38, //   ###
    0x24, //   #  #
    0x24, //   #  #
    0x24, //   #  #
    0x38, //   ###
    0x20, //   #
    0x20, //   #

    // @234 'q' (8 pixels wide)
    0x00, //
    0x00, //
    0x1C, //    ###
    0x24, //   #  #
    0x24, //   #  #
    0x24, //   #  #
    0x1C, //    ###
    0x04, //      #
    0x04, //      #

    // @243 'r' (8 pixels wide)
    0x00, //
    0x00, //
    0x18, //    ##
    0x10, //    #
    0x10, //    #
    0x10, //    #
    0x10, //    #
    0x00, //
    0x00, //

    // @252 's' (8 pixels wide)
    0x00, //
    0x00, //
    0x18, //    ##
    0x20, //   #
    0x10, //    #
    0x08, //     #
    0x30, //   ##
    0x00, //
    0x00, //

    // @261 't' (8 pixels wide)
    0x08, //     #
    0x08, //     #
    0x1C, //    ###
    0x08, //     #
    0x08, //     #
    0x08, //     #
    0x04, //      #
    0x00, //
    0x00, //

    // @270 'u' (8 pixels wide)
    0x00, //
    0x00, //
    0x24, //   #  #
    0x24, //   #  #
    0x24, //   #  #
    0x24, //   #  #
    0x1C, //    ###
    0x00, //
    0x00, //

    // @279 'v' (8 pixels wide)
    0x00, //
    0x00, //
    0x44, //  #   #
    0x28, //   # #
    0x28, //   # #
    0x28, //   # #
    0x10, //    #
    0x00, //
    0x00, //

    // @288 'w' (8 pixels wide)
    0x00, //
    0x00, //
    0x92, // #  #  #
    0xAA, // # # # #
    0xAA, // # # # #
    0x44, //  #   #
    0x44, //  #   #
    0x00, //
    0x00, //

    // @297 'x' (8 pixels wide)
    0x00, //
    0x00, //
    0x44, //  #   #
    0x28, //   # #
    0x10, //    #
    0x28, //   # #
    0x44, //  #   #
    0x00, //
    0x00, //

    // @306 'y' (8 pixels wide)
    0x00, //
    0x00, //
    0x44, //  #   #
    0x28, //   # #
    0x28, //   # #
    0x28, //   # #
    0x10, //    #
    0x10, //    #
    0x60, //  ##

    // @315 'z' (8 pixels wide)
    0x00, //
    0x00, //
    0x38, //   ###
    0x08, //     #
    0x10, //    #
    0x20, //   #
    0x38, //   ###
    0x00, //
    0x00, //
};

struct TextFontCharInfo {
    uint32_t charWidth;
    uint32_t offset;
};

// Character descriptors for Arial Unicode MS 6pt
// { [Char width in bits], [Offset into arialUnicodeMS_6ptCharBitmaps in bytes] }
const TextFontCharInfo textFontDescriptors[] = {
    {8, 0}, 		// 0
    {8, 9}, 		// 1
    {8, 18}, 		// 2
    {8, 27}, 		// 3
    {8, 36}, 		// 4
    {8, 45}, 		// 5
    {8, 54}, 		// 6
    {8, 63}, 		// 7
    {8, 72}, 		// 8
    {8, 81}, 		// 9
    {0, 0}, 		// :
    {0, 0}, 		// ;
    {0, 0}, 		// <
    {0, 0}, 		// =
    {0, 0}, 		// >
    {0, 0}, 		// ?
    {0, 0}, 		// @
    {0, 0}, 		// A
    {0, 0}, 		// B
    {0, 0}, 		// C
    {0, 0}, 		// D
    {0, 0}, 		// E
    {0, 0}, 		// F
    {0, 0}, 		// G
    {0, 0}, 		// H
    {0, 0}, 		// I
    {0, 0}, 		// J
    {0, 0}, 		// K
    {0, 0}, 		// L
    {0, 0}, 		// M
    {0, 0}, 		// N
    {0, 0}, 		// O
    {0, 0}, 		// P
    {0, 0}, 		// Q
    {0, 0}, 		// R
    {0, 0}, 		// S
    {0, 0}, 		// T
    {0, 0}, 		// U
    {0, 0}, 		// V
    {0, 0}, 		// W
    {0, 0}, 		// X
    {0, 0}, 		// Y
    {0, 0}, 		// Z
    {0, 0}, 		// [
    {0, 0}, 		// \
    {0, 0}, 		// ]
    {0, 0}, 		// ^
    {0, 0}, 		// _
    {0, 0}, 		// `
    {8, 90}, 		// a
    {8, 99}, 		// b
    {8, 108}, 		// c
    {8, 117}, 		// d
    {8, 126}, 		// e
    {8, 135}, 		// f
    {8, 144}, 		// g
    {8, 153}, 		// h
    {8, 162}, 		// i
    {8, 171}, 		// j
    {8, 180}, 		// k
    {8, 189}, 		// l
    {8, 198}, 		// m
    {8, 207}, 		// n
    {8, 216}, 		// o
    {8, 225}, 		// p
    {8, 234}, 		// q
    {8, 243}, 		// r
    {8, 252}, 		// s
    {8, 261}, 		// t
    {8, 270}, 		// u
    {8, 279}, 		// v
    {8, 288}, 		// w
    {8, 297}, 		// x
    {8, 306}, 		// y
    {8, 315}, 		// z
};

struct TextFontInfo {
    uint8_t charHeight;
    uint8_t charWidth;
    uint8_t charStart;
    uint8_t charEnd;
    uint8_t preferredSpaceWidth;
    const TextFontCharInfo *descriptor;
    const uint8_t *bitmap;
};



void MessageManager::init() {
    numCharDisplayed = 0;
    bgColour = ColourT{0x0, 0x0, 0x0};
    textColour = ColourT{0x0FF, 0x00, 0x00};

}

void translateLetter(ImageBuffer& img, char ch, int x, int y, ColourT textColour) {
    int index = ch - '0';

    const uint8_t * fontBegin = &textBitmap[textFontDescriptors[index].offset];
    int yFont =0;
    int lastX =x;
    int lastY = 0;
    for(int yFont = 0; yFont != 9; ++yFont) {
        for(int xFont = 0; xFont != 8; ++xFont, ++x) {
            if(y == yFont && ((fontBegin[yFont] & (1 << xFont)) != 0))
                img.getSegment(x + lastX).setPixel(yFont, textColour);
        }
    }
}

static void displayImpl(ImageBuffer& img, const std::array<std::uint8_t, 23>& msg, int count, ColourT bgColour, ColourT textColor) {
    for(int i = 0; i != ImageBuffer::size(); ++i) {
        for(int j = 0;  SegmentBuffer::size(); ++j) {
            img.getSegment(i).setPixel(j, bgColour);
        }
    }

    int numDisplayed = 0;
    int xPos = 0;


    while(numDisplayed != count && xPos != ImageBuffer::size()) {
        translateLetter(img, msg[numDisplayed], xPos, 0, textColor);
        xPos += 9;
    }
}

void MessageManager::display(std::array<uint8_t, 23>& msg, int count) {
    displayedMsg = msg;
    displayImpl(*PropellerMan::get().getUpdateImage(), displayedMsg, count, bgColour, textColour);
}
