#pragma once

#include <cstdint>
#include <climits>
#include <array>
#include <algorithm>
#include "stm32f4xx.h"
#include "defines.h"
#include "tm_stm32f4_delay.h"
//#include "tm_stm32f4_disco.h"
#include "tm_stm32f4_spi.h"
#include "tm_stm32f4_spi_dma.h"
#include "tm_stm32f4_gpio.h"
#include "FrameTimeManager.h"
#include "DisplayBuffers.hpp"
#include "ClockManager.hpp"
#include "DebugHelpers.hpp"
#include "Bluetooth.hpp"
#include "MessageManager.hpp"



enum class LedDriverMode {
    dotCorrectionMode,
    greyScaleMode
};


inline void GPIO_PulsePins(GPIO_TypeDef *port, uint16_t pins, int delayTime = 20) {
    GPIO_SetBits(port, pins);


    GPIO_ResetBits(port, pins);
}




class LedDriver {
public:
    static constexpr auto modePin = GPIO_Pin_4;
    static constexpr auto xlatPin = GPIO_Pin_6;
    static constexpr auto blankPin = GPIO_Pin_3;
    static constexpr auto myDMA = DMA2;
    static constexpr auto myDMAStream = DMA2_Stream3;
    static constexpr auto myDMAChannel = DMA_Channel_3;


    LedDriver();

    // sets the dot correction data for the driver
    // note: does not send the data
    void setDotCorrection(DotCorrectionData& dcData) {
        dotCorrectionData = dcData;
    }

    // changes the mode of the led driver
    void setMode(LedDriverMode newMode) {
        if(newMode == currentMode)
            return ;
        DEBUG_LOG("changing from %s to %s\n",
                  currentMode == LedDriverMode::dotCorrectionMode ? "dcMode" : "gsMode",
                  newMode == LedDriverMode::dotCorrectionMode ? "dcMode" : "gsMode");
        currentMode = newMode;
    }



    // starts DMA to SPI dispatch of the current segment
    // buffer
    void sendGreyScaleData();

    // sends dot correction data
    void sendDotCorrectionData();

    //
    void enable();

    //
    void disable();

    LedDriverMode mode() {
        return currentMode;
    }

    void resetDrawCounter() {
        DEBUG_LOG("resetting draw counter\n");
        segmentIndex = 0;
    }

    int getDrawCounter() {
        return segmentIndex;
    }

    void goToNextSegment() {
        ++segmentIndex;
    }

    void setImage(ImageBuffer& buffer) {
        currentBuffer = &buffer;
    }

    ImageBuffer *getCurrentImage() {
        return currentBuffer;
    }
private:
    LedDriverMode currentMode;
    DotCorrectionData dotCorrectionData;
    ImageBuffer *currentBuffer;
    int segmentIndex;

};

enum class DrawMode {
    clockMode,
    messageMode
};



class DrawManager {
public:
    static DrawManager& get() {
        static DrawManager instance;

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


    void run() {
        BluetoothData recievedData;

        clockManager.init();
        BluetoothManager& bluetooth = BluetoothManager::get();

        // set the driver to start by first sending dot correction
        // data
        ledDriver.setMode(LedDriverMode::dotCorrectionMode);

        ledDriver.enable();
        ledDriver.setMode(LedDriverMode::greyScaleMode);

        clockManager.drawClockFace();
        frameTimeManager.init();
        frameTimeManager.enable();


        while(true) {
            // if we got new bluetooth data
            if(bluetooth.recieved(recievedData)) {
                if(recievedData.requestedDrawMode == RequestedDrawMode::clockMode) {
                    if(mode() != DrawMode::clockMode) {
                        setMode(DrawMode::clockMode);
                        clockManager.drawClockFace();
                    }
                }
                else if(recievedData.requestedDrawMode == RequestedDrawMode::messageMode) {
                    setMode(DrawMode::messageMode);
                    clockManager.disableClockFace();
                    messageManager.display(recievedData.message, recievedData.messageLength);
                }

            }
            switch(mode()) {
            case DrawMode::clockMode:
                clockManager.adjustClockHandles();
                break;

            }
        }

    }

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
        if(ledDriver.getCurrentImage() == &primaryBuffer) {
            ledDriver.setImage(secondaryBuffer);
        }
        else {
            ledDriver.setImage(primaryBuffer);
        }
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

        ledDriver.disable();

        currentMode = newMode;
    }

    DrawMode mode() {
        return currentMode;
    }

private:
    DrawMode currentMode;
    LedDriver ledDriver;
    ClockManager clockManager;
    FrameTimeManager frameTimeManager;
    MessageManager messageManager;
    DotCorrectionData dotCorrectionData;

    ImageBuffer primaryBuffer;
    ImageBuffer secondaryBuffer;

};


