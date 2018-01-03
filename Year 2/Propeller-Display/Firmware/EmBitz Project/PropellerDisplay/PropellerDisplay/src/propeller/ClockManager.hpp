#pragma once

#include "tm_stm32f4_rtc.h"
#include <cstring>
#include "ColourT.hpp"

// class that is in charge of managing the time keeping
// using the RTC and drawing the clock on the image buffer
class ClockManager {
public:
   static TM_RTC_t rtcTime;

    // initialise the clock management system
    ClockManager() : swapBuffers{false}, clockFaceDisabled{true} {
        // set the time to 0
        std::memset((void *) &oldRtcTime, 0, sizeof(TM_RTC_t));
    }


    // initializes clock system
    void init();

    // calculate and copy the new position of clock handles
    void adjustClockHandles();


    // check to see if the buffers should be swapped
    bool shouldSwapBuffers() {
        return swapBuffers && !clockFaceDisabled;
    }

    // tell the clock system that you have swapped the buffers
    void notifyBufferSwapped() {
        swapBuffers = false;
    }

    // draw the clock face on both image buffers
    void drawClockFace();

    // stop drawing the clock
    void disableClockFace();


private:

    void moveHourHandle();

    void moveMinuteHandle();

    void moveSecondHandle();

    bool clockFaceDisabled;
    bool swapBuffers;
    ColourT backGroundColour;
    ColourT hourHandleColour;
    ColourT minuteHandleColour;
    ColourT secondHandleColour;
    TM_RTC_t oldRtcTime;
};

