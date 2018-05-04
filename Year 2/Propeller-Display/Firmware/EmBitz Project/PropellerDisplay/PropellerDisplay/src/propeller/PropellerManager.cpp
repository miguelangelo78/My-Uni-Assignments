#include "PropellerManager.hpp"

void PropellerMan::run() {
    // wake up rtc and the clock drawer
    clockManager.init();

    // get bluetooth singleton
    BluetoothManager& bluetooth = BluetoothManager::get();
    bluetooth.init();

    // set the driver to start by first sending dot correction
    // data
    ledDriver.setMode(LedDriverMode::dotCorrectionMode);

    // enable led driver
    ledDriver.enable();

    // set the led driver to greyscale mode
    ledDriver.setMode(LedDriverMode::greyScaleMode);

    // draw clock face on both buffers
    // clockManager.drawClockFace(); // NOTE: DON'T FORGET TO UNCOMMENT LATER

    // initialize frame time manager and enable
    // after this the system is live timer interrupts
    // and hall effect interrupt are enabled be careful
    frameTimeManager.init();
    frameTimeManager.enable();

    BluetoothData receivedData;
    // polling loop to do slower stuff
    while(true) {
        // if we recieved a whole bluetooth data then
        // respond to it
        if(bluetooth.received(&receivedData)) {
            #if 1
            bluetooth.sendPacket(&receivedData); /*Echo back*/
            #endif
            if(receivedData.requestedDrawMode == RequestedDrawMode::clockMode) {
                if(mode() != DrawMode::clockMode) {
                    setMode(DrawMode::clockMode);
                    clockManager.drawClockFace();
                }
            }
            else if(receivedData.requestedDrawMode == RequestedDrawMode::messageMode) {
                setMode(DrawMode::messageMode);
                clockManager.disableClockFace();
                messageManager.display(receivedData.message, receivedData.messageLength);
            }
        }
        // if we are in clock mode adjust clock handles if time
        // has changed
        switch(mode()) {
        case DrawMode::clockMode:
            clockManager.adjustClockHandles();
            break;
        }
    }
}
