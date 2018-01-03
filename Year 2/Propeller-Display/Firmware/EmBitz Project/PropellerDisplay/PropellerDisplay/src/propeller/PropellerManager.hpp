#pragma once
#include "LedDriver.hpp"

enum class DrawMode {
    clockMode,
    messageMode
};

/* This class wraps up all the other classes, and is considered
    to model the actual physical device, thus the name */

class PropellerMan {
public:
    static PropellerMan& get() {
        static PropellerMan instance;

        return instance;
    }

    LedDriver& getLedDriver() {
        return ledDriver;
    }

    FrameTimeManager& getFrameTimeManager() {
        return frameTimeManager;
    }

    ClockManager& getClockManager() {
        return clockManager;
    }

    void run();

    void setImage(ImageBuffer& buffer) {
        // if we are displaying the clock clone the
        // image to two buffers implementing double
        // buffering on the image
        if(mode() == DrawMode::clockMode) {
            primaryBuffer = buffer;
            secondaryBuffer = buffer;
            ledDriver.setImage(primaryBuffer);
        }

        // if we are in message mode use one image as
        // the message is static
        else if(mode() == DrawMode::messageMode) {
            primaryBuffer = buffer;
        }
    }

    void swapBuffers() {
        if(ledDriver.getCurrentImage() == &primaryBuffer)
            ledDriver.setImage(secondaryBuffer);
        else
            ledDriver.setImage(primaryBuffer);
    }

    ImageBuffer *getCurrentImage() {
        return ledDriver.getCurrentImage();
    }

    ImageBuffer *getUpdateImage() {
        return ledDriver.getCurrentImage() == &primaryBuffer ? &secondaryBuffer : &primaryBuffer;
    }

    void setMode(DrawMode newMode) {
        if(newMode == mode())
            return;

        currentMode = newMode;
    }

    DrawMode mode() {
        return currentMode;
    }

private:

    PropellerMan() = default;
    PropellerMan(const PropellerMan& ) = delete;
    PropellerMan(PropellerMan&& ) = delete;

    DrawMode currentMode;
    LedDriver ledDriver;
    ClockManager clockManager;
    FrameTimeManager frameTimeManager;
    MessageManager messageManager;
    DotCorrectionData dotCorrectionData;

    ImageBuffer primaryBuffer;
    ImageBuffer secondaryBuffer;

};
