#pragma once

#include <cstdint>
#include <climits>
#include <array>
#include <algorithm>
#include "stm32f4xx.h"
#include "defines.h"
#include "tm_stm32f4_delay.h"
#include "tm_stm32f4_spi.h"
#include "tm_stm32f4_spi_dma.h"
#include "tm_stm32f4_gpio.h"
#include "FrameTimeManager.h"
#include "ClockManager.hpp"
#include "DebugHelpers.hpp"
#include "Bluetooth.hpp"
#include "MessageManager.hpp"
#include "ImageBuffer.hpp"
#include "DotT.hpp"

enum class LedDriverMode {
    dotCorrectionMode,
    greyScaleMode
};

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
        DEBUG_LOG("changing mode from %s to %s\n",
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
