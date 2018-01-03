#include "ClockManager.hpp"
#include "PropellerManager.hpp"
#include "ClockFonts.hpp"

TM_RTC_t ClockManager::rtcTime = {};

void ClockManager::init() {
    swapBuffers = false;

    backGroundColour = ColourT {0x00, 0x0, 0xFD0};
    hourHandleColour = ColourT {0, 0xFFF, 0};
    minuteHandleColour = ColourT {0xFFF, 0, 0};
    secondHandleColour = ColourT {0, 0, 0};

    // if the clock has not been set, initialise the clock
    // to a certain time
    if(TM_RTC_Init(TM_RTC_ClockSource_Internal) == 0) {
        TM_RTC_SetDateTimeString((char*)"21.02.16;01:58:30");
        TM_RTC_GetDateTime(&rtcTime, TM_RTC_Format_BIN);

        oldRtcTime = rtcTime;
    }

    // otherwise do nothing
    else
        ;

    TM_RTC_Interrupts(TM_RTC_Int_1s);
}

void ClockManager::adjustClockHandles() {
    if(!clockFaceDisabled) {
        if(rtcTime.seconds != oldRtcTime.seconds) {
            moveSecondHandle();
            oldRtcTime = rtcTime;
        }

        //
        if(rtcTime.minutes != oldRtcTime.minutes) {
            moveMinuteHandle();
            oldRtcTime = rtcTime;
        }
        //
        if(rtcTime.hours != oldRtcTime.hours) {
            moveHourHandle();
            oldRtcTime = rtcTime;
        }
    }
}

static void drawClockFaceImpl(ImageBuffer& face, ColourT bg) {
    // set background
    for(int i = 0; i != face.size(); ++i) {
        for(int j = 0; j != SegmentBuffer::size(); ++j) {
            face[i].setPixel(j, bg);
        }
    }

    // draw dashes
    ColourT dashes = ColourT {0, 0, 0};

    for(int i = 0; i != 12; ++i) {
        for(int j = 0; j != 4; ++j) {
            face[i * 72 / 60].setPixel(j, dashes);
        }
    }
}

void ClockManager::drawClockFace() {
    drawClockFaceImpl(*PropellerMan::get().getUpdateImage(), backGroundColour);

    PropellerMan::get().swapBuffers();

    drawClockFaceImpl(*PropellerMan::get().getUpdateImage(), backGroundColour);
    clockFaceDisabled = false;
}

void ClockManager::disableClockFace() {
    clockFaceDisabled = true;
}

void ClockManager::moveHourHandle() {
    auto buffer = PropellerMan::get().getUpdateImage();

    int oldClockHandlePos = (oldRtcTime.hours % 12) * 72 / 12;
    int currentClockHandlePos = (rtcTime.hours % 12) * 72 / 12;

    for(int i = SegmentBuffer::size() * 3 / 4; i != SegmentBuffer::size() ; ++i) {
        buffer->getSegment(oldClockHandlePos).setPixel(i, backGroundColour);
        buffer->getSegment(currentClockHandlePos).setPixel(i, hourHandleColour);
    }

    swapBuffers = true;
}

void ClockManager::moveMinuteHandle() {
    auto buffer = PropellerMan::get().getUpdateImage();

    int oldClockHandlePos = oldRtcTime.minutes * 72 / 60;
    int currentClockHandlePos = rtcTime.minutes * 72 / 60;

    for(int i = SegmentBuffer::size() * 2 / 4; i != SegmentBuffer::size(); ++i) {
        buffer->getSegment(oldClockHandlePos).setPixel(i, backGroundColour);
        buffer->getSegment(currentClockHandlePos).setPixel(i, minuteHandleColour);
    }

    swapBuffers = true;
}

void ClockManager::moveSecondHandle() {
    auto buffer = PropellerMan::get().getUpdateImage();

    int oldClockHandlePos = oldRtcTime.seconds * 72 / 60;
    int currentClockHandlePos = rtcTime.seconds * 72 / 60;

    for(int i = SegmentBuffer::size() / 4; i != SegmentBuffer::size(); ++i) {
        buffer->getSegment(oldClockHandlePos).setPixel(i, backGroundColour);
        buffer->getSegment(currentClockHandlePos).setPixel(i, secondHandleColour);
    }

    swapBuffers = true;
}

extern "C" void TM_RTC_RequestHandler() {
    /* 1 Second has passed and the data has stopped transmitting. Timing out... */
    BluetoothManager & bt = BluetoothManager::get();
    bt.rx_timeout();

    TM_RTC_GetDateTime(&ClockManager::rtcTime, TM_RTC_Format_BIN);
    DEBUG_LOG("time is %d:%d:%d\n", ClockManager::rtcTime.hours,
              ClockManager::rtcTime.minutes, ClockManager::rtcTime.seconds); /* UNCOMMENT LATER */
}
