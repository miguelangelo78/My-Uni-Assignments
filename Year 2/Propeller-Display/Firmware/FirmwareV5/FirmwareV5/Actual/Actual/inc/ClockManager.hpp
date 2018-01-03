#pragma once

#include "tm_stm32f4_rtc.h"
#include "DisplayBuffers.hpp"
#include <cstring>


class ClockManager {
public:
   static TM_RTC_t rtcTime;

    ClockManager() : swapBuffers{false}, clockFaceDisabled{true} {
        std::memset((void *) &oldRtcTime, 0, sizeof(TM_RTC_t));
        swapBuffers = false;

        backGroundColour = makeColor(0xFFF, 0x000, 0x000);
        hourHandleColour = makeColor(0x000, 0x000, 0xFFF);
        minuteHandleColour = makeColor(0x000, 0x000, 0xFFF);
        secondHandleColour = makeColor(0x000, 0x000, 0xFFF);
    }

    void init();

    void adjustClockHandles();


    bool shouldSwapBuffers() {
        return swapBuffers && !clockFaceDisabled;
    }

    void notifyBufferSwapped() {
        swapBuffers =false;
    }

    void drawClockFace();
    void disableClockFace();
private:



    volatile bool clockFaceDisabled;
    volatile bool swapBuffers;
    ColourT backGroundColour;
    ColourT hourHandleColour;
    ColourT minuteHandleColour;
    ColourT secondHandleColour;
    TM_RTC_t oldRtcTime;
};

