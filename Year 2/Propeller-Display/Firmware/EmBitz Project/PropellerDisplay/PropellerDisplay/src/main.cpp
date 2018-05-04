// define this as 1 if you want to continue transmitting
// image even if hall effect is triggered
#define MY_CONTINOUS_LED_FLASHING 1

#include "PropellerManager.hpp"

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

    for(int i = 0; i != imageBuffer.size(); ++i)
        for(int j = 0; j != SegmentBuffer::size(); ++j)
            imageBuffer[i].setPixel(j, {0xFFF, 0xFFF, 0xFFF}); /* Red and Green only for now */

    PropellerMan& PropellerMan = PropellerMan::get();
    PropellerMan.setMode(DrawMode::clockMode);
    PropellerMan.setImage(imageBuffer);
    PropellerMan.getLedDriver().setDotCorrection(dotCorrectionData);

    while(true)
        PropellerMan.run();
}
