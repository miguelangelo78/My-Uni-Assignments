#include "ClockManager.hpp"
#include "LedDriver.hpp"
#include "ClockFonts.hpp"
#include "ColourT.hpp"
#include <bitset>

using namespace std;

TM_RTC_t ClockManager::rtcTime = {};

namespace {

PixelLocation fillImageC(ImageBuffer& image,
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
PixelLocation fillImageIfC(ImageBuffer& image, ColourT colour, Function fun, int first = 0, int last = ImageBuffer::size(),
                           int offsetPixel = 0,
                           int pixelLast = SegmentBuffer::size()) {
    int i = first;
    int j = offsetPixel;

    for(; i != last; ++i) {
        for(j = offsetPixel; j != pixelLast; ++j) {
            if(!!fun(PixelLocation {i, j})) {
                image.getSegment(i).setPixel(j, colour);
            }
        }
    }

    return PixelLocation {i, j};
}


}




void ClockManager::init() {
    // on initialisation buffer should not be swapped
    // as the system hasnt drawn on either image buffer
    swapBuffers = false;

    drawClockFace();

    // if the clock has not been set, initialise the clock
    // to a certain time
    if(TM_RTC_Init(TM_RTC_ClockSource_Internal) == 0) {
        TM_RTC_SetDateTimeString("21.02.16;01:58:30");
        TM_RTC_GetDateTime(&rtcTime, TM_RTC_Format_BIN);

        oldRtcTime = rtcTime;
    }

    // otherwise do nothing
    else
        ;

    TM_RTC_Interrupts(TM_RTC_Int_1s);
}


// null primary implementation
struct ClockFontMap {
    struct FontBitmap {
        array<unsigned, 5> bitmap;

      //  FontBitmap(array<unsigned, 5> arr) : bitmap{arr} {

       // }
        bool shouldDraw(int x, int y) {
            bool test = !!(bitmap[y] & (1 << x));
            return test;
        }
    };

    using Pair = pair<unsigned char, FontBitmap>;

    array<Pair, 5> map = {{
            make_pair('1', FontBitmap{{0x2, 0x3, 0x2, 0x2, 0x7}}),
            make_pair('2', FontBitmap{{0x7, 0x8, 0x4, 0x2, 0xF}}),
            make_pair('3', FontBitmap{{31,  21,  21,  21,  21}}),
            make_pair('6', FontBitmap{{31,  17,  31,  16,  31}}),
            make_pair('9', FontBitmap{{29,  21,  21,  21,  31}})
        }
    };

    FontBitmap& getCharBitmap(unsigned char n) {
        return find_if(begin(map), end(map), [=](const Pair& entry) {
            return n == entry.first;
        })->second;
    }

    static constexpr int fontHeight() {
        return 5;
    }

    static constexpr int fontWidth() {
        return 5;
    }


};



namespace {
struct FontMatcher {
    FontMatcher(ClockFontMap::FontBitmap * bt, int first, int offsetPixels) : bitmap{bt}, first{first}, offsetPixels{offsetPixels} { }
    bool operator()(PixelLocation loc) {
        return bitmap->shouldDraw(loc.segmentIndex - first, loc.pixelIndex - offsetPixels);
    }

    ClockFontMap::FontBitmap* bitmap;
    int first;
    int offsetPixels;
};

void drawFontC(ImageBuffer& image, unsigned char letter, ColourT colour, int first = 0, int offsetPixels = 0) {
    static ClockFontMap clockFontMap;

    auto& bitmap = clockFontMap.getCharBitmap(letter);



    fillImageIfC(image, colour, FontMatcher{&bitmap, first, offsetPixels}, first, first + ClockFontMap::fontWidth(), offsetPixels, offsetPixels + ClockFontMap::fontHeight());
}
}

struct CachedImageBuffer : public ImageBuffer {
    CachedImageBuffer(ColourT bg) {
        auto& face = *this;

        // set background
       // fillImageC(face, bg);

        // draw dashes
        ColourT dashes = ColourT {0xFFF, 0xFFF, 0};


//       fillImageIfC(face, dashes, [](PixelLocation loc) { return  (loc.segmentIndex % 60) == 0; },
        //          0, ImageBuffer::size(), 0, 5);

        for(int i = 0; i != ImageBuffer::size(); i += ImageBuffer::size() / 60)
            face.getSegment(i).setPixel(0, dashes);


            ColourT clockFontColour = ColourT {0xFFF, 0x000, 0x000};


            drawFontC(face, '2', clockFontColour);

            drawFontC(face, '1', clockFontColour, ImageBuffer::size() - 4);

            drawFontC(face, '3', clockFontColour, ImageBuffer::size() / 4 - 2);
            drawFontC(face, '6', clockFontColour, ImageBuffer::size() / 2 - 2);

            drawFontC(face, '9', clockFontColour, 3 * ImageBuffer::size() / 4 - 2);

    }
};

static void drawClockFaceImpl(ImageBuffer& face, ColourT bg) {
    static CachedImageBuffer cachedImage {bg};

    face= cachedImage;

}




void ClockManager::drawClockFace() {

    drawClockFaceImpl(*DrawManager::get().getUpdateImage(), backGroundColour);

    DrawManager::get().swapBuffers();

    drawClockFaceImpl(*DrawManager::get().getUpdateImage(), backGroundColour);
    clockFaceDisabled = false;
}

void ClockManager::disableClockFace() {
    clockFaceDisabled = true;
}

namespace {
unsigned calcHourPos(TM_RTC_t rtc) {
    return (rtc.hours % 12) * ImageBuffer::size() / 12;
}

unsigned calcMinPos(TM_RTC_t rtc) {
    return (rtc.minutes % 60) * ImageBuffer::size() / 60;
}

unsigned calcSecPos(TM_RTC_t rtc) {
    return (rtc.seconds % 60) * ImageBuffer::size() / 60;
}
}



namespace {
bool isTimeEqualC(TM_RTC_t rtc1, TM_RTC_t rtc2) {
    return memcmp((void *)&rtc1, (void *)&rtc2, sizeof(TM_RTC_t)) != 0;
}
}


void ClockManager::adjustClockHandles() {
    // if the buffer has been changed but not been
    // swapped
    if(!shouldSwapBuffers() && isTimeEqualC(oldRtcTime, rtcTime)) {
        auto& image = *DrawManager::get().getUpdateImage();

        drawClockFaceImpl(image, backGroundColour);
        int secondHandlePosition = calcSecPos(rtcTime);
        int minuteHandlePosition = calcMinPos(rtcTime);
        int hourHandlePosition = calcHourPos(rtcTime);

        fillImageC(image, makeColor(0xFFF, 0x000, 0x000), secondHandlePosition, secondHandlePosition + 1, 6);
        fillImageC(image, makeColor(0x000, 0x000, 0xFFF), minuteHandlePosition, minuteHandlePosition + 1, 12);
        fillImageC(image, makeColor(0x000, 0xFFF, 0x000), hourHandlePosition, hourHandlePosition + 1, 16);

        swapBuffers = true;

    }
}




extern "C" void TM_RTC_RequestHandler() {
    TM_RTC_GetDateTime(&ClockManager::rtcTime, TM_RTC_Format_BIN);
}
