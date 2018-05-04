#include "PropellerManager.hpp"
#include "DebugHelpers.hpp"

inline void GPIO_PulsePins(GPIO_TypeDef *port, uint16_t pins, int delayTime = 20) {
    GPIO_SetBits(port, pins);
    GPIO_ResetBits(port, pins);
}

LedDriver::LedDriver()
    : segmentIndex {0}, currentMode {LedDriverMode::dotCorrectionMode} {
}

void LedDriver::sendGreyScaleData() {

    // if we are starting in greyscale mode,
    // switch off the mode pin to tell the driver we are
    // in greyscale mode, pulse blank to clear previous
    // greyscale data and launch DMA to SPI of the first
    // segment of greyscale data
    if(TM_SPI_DMA_Send(SPI1, (uint8_t *)currentBuffer->getSegment(segmentIndex).getPointer(),
                       currentBuffer->getSegment(segmentIndex).sizeInBytes()) == 0)
    {
        DEBUG_LOG("SPI DMA for gs data not sent\n");
    }

    while(TM_SPI_DMA_Working(SPI1));
}

void LedDriver::sendDotCorrectionData() {

    // if we are starting on dot correction mode turn
    // on the mode pin to tell the chip we are in dot
    // correction mode, next request DMA to send dot
    // correction data through the SPI

    GPIO_SetBits(GPIOA, modePin);
    GPIO_SetBits(GPIOA, blankPin);

    if(TM_SPI_DMA_Send(SPI1, (uint8_t *)dotCorrectionData.getPointer(), dotCorrectionData.size()) == 0) {
        DEBUG_LOG("SPI DMA Request for dcData not sent\n");
    }

    while(TM_SPI_DMA_Working(SPI1));
}

void LedDriver::enable() {
    /* unlock GPIOA */
    GPIOA->LCKR = 0;
    /* Init SPI */
    TM_SPI_InitFull(SPI1, TM_SPI_PinsPack_1,
                    SPI_BaudRatePrescaler_2, TM_SPI_Mode_0, SPI_Mode_Master, SPI_FirstBit_LSB);

    /* Init SPI DMA */
    TM_SPI_DMA_Init(SPI1);

    /* initialise GPIO mode blank and xlat */
    TM_GPIO_Init(GPIOA, blankPin | modePin | xlatPin, TM_GPIO_Mode_OUT,
                 TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_Fast);

    TM_SPI_DMA_EnableInterrupts(SPI1);

    sendDotCorrectionData();

    //  setMode(LedDriverMode::greyScaleMode);
}

void LedDriver::disable() {

}

/* Called when half transfer is completed for specific stream */
extern "C" void TM_DMA_HalfTransferCompleteHandler(DMA_Stream_TypeDef* DMA_Stream) {
    /* Check if interrupt is for correct stream */
    if (DMA_Stream == TM_SPI_DMA_GetStreamTX(SPI1)) {
        DEBUG_LOG("transfer half complete\n");
    }
}

/* Called when transfer error occurs for specific stream */
void TM_DMA_TransferErrorHandler(DMA_Stream_TypeDef* DMA_Stream) {
    /* Check if interrupt is for correct stream */
    if (DMA_Stream == TM_SPI_DMA_GetStreamTX(SPI1)) {
        DEBUG_LOG("transfer error\n");
    }
}

// this function is called whenever the DMA is complete for any segment in
// the led driver
extern "C"  void TM_DMA_TransferCompleteHandler(DMA_Stream_TypeDef* DMA_Stream) {
    if(TM_SPI_DMA_GetStreamTX(SPI1) == DMA_Stream) {
        LedDriver& driver = PropellerMan::get().getLedDriver();

        // if the dot correction data has just been sent turn off
        // mode pin and pulse xlat to latch the dot correction
        if(driver.mode() == LedDriverMode::dotCorrectionMode) {
            GPIO_PulsePins(GPIOA, LedDriver::xlatPin, 20);
            GPIO_ResetBits(GPIOA, LedDriver::modePin | LedDriver::blankPin);
            DEBUG_LOG("dsData DMA complete\n");
        }
        // otherwise the greyscale data has just been completed
        // so pulse xlat to latch the data and pulse blank to clear
        // the data in the shift register, finally call next segment to
        // increment the segment index counter
        else if(driver.mode() == LedDriverMode::greyScaleMode) {
            GPIO_PulsePins(GPIOA, LedDriver::xlatPin, 20);
            GPIO_PulsePins(GPIOA, LedDriver::blankPin, 20);
            DEBUG_LOG("gsData DMA Complete\n");
            driver.goToNextSegment();
        }
    }
}
