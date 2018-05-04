
/* Include core modules */
#include "stm32f4xx.h"
/* Include my libraries here */
#include "defines.h"
#include "tm_stm32f4_delay.h"
//#include "tm_stm32f4_disco.h"
#include "tm_stm32f4_spi.h"
#include "tm_stm32f4_spi_dma.h"
#include "tm_stm32f4_gpio.h"
#include <array>
#include <limits.h>
#include "LedDriver.hpp"
#include <stdio.h>

/* the stuff below initialises local static variables in c++ */
__extension__ typedef std::int64_t __guard __attribute__((mode(__DI__)));

extern "C" int __cxa_guard_acquire(__guard g) {
    return !*reinterpret_cast<char *>(g);
}

extern "C" void __cxa_guard_release(__guard g) {
    *reinterpret_cast<char *>(g) = 1;
}







int main(void) {
    /* Initialize system and set clock to 120MHz */
    SystemInit();
    SystemCoreClockUpdate();

    DotCorrectionData dotCorrectionData;

    ImageBuffer imageBuffer;

    ColourT colour = makeColor(0, 0, 0);


    for(int i = 0; i != imageBuffer.size(); ++i) {
        for(int j = 0; j != SegmentBuffer::size(); ++j) {
            imageBuffer.getSegment(i).setPixel(j, colour);
        }

    }



    DrawManager& drawManager = DrawManager::get();

    drawManager.setMode(DrawMode::clockMode);
    drawManager.setImage(imageBuffer);
    drawManager.getLedDriver().setDotCorrection(dotCorrectionData);


    while(true) {
        drawManager.run();
    }
}
