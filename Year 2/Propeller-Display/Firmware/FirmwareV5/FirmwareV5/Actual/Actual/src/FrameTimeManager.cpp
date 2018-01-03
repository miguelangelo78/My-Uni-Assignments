#include "FrameTimeManager.h"
#include "LedDriver.hpp"
#include "ClockManager.hpp"





static void initHallEffectIRQ() {
    GPIO_InitTypeDef gpioInit;
    EXTI_InitTypeDef extiInit;
    NVIC_InitTypeDef nvicInit;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);


    // configure the GPIO pin as an input
    gpioInit.GPIO_Mode = GPIO_Mode_IN;
    gpioInit.GPIO_OType = GPIO_OType_PP;
    gpioInit.GPIO_Pin = GPIO_Pin_12;
    gpioInit.GPIO_PuPd = GPIO_PuPd_NOPULL;
    gpioInit.GPIO_Speed = GPIO_Speed_100MHz;

    GPIO_Init(GPIOC, &gpioInit);

    // we are notifying the microcontroller that PC12 the hall
    // effect sensor is going to use interrupt line 12
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource12);

    extiInit.EXTI_Line = EXTI_Line12;
    extiInit.EXTI_LineCmd = ENABLE;
    extiInit.EXTI_Mode = EXTI_Mode_Interrupt;
    extiInit.EXTI_Trigger = EXTI_Trigger_Falling;

    EXTI_Init(&extiInit);

    nvicInit.NVIC_IRQChannel = EXTI15_10_IRQn;
    nvicInit.NVIC_IRQChannelPreemptionPriority = 0x00;
    nvicInit.NVIC_IRQChannelSubPriority = 0x01;
    nvicInit.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&nvicInit);


}

static void initTimer() {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

    TIM_TimeBaseInitTypeDef timInit;

    timInit.TIM_Period = FrameTimeManager::baseReload; //60
    timInit.TIM_Prescaler = FrameTimeManager::basePrescale; // 440
    timInit.TIM_ClockDivision = TIM_CKD_DIV1;
    timInit.TIM_CounterMode = TIM_CounterMode_Down;
    timInit.TIM_RepetitionCounter = 0;
    TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
    TIM_TimeBaseInit(TIM5, &timInit);


    NVIC_InitTypeDef nvicInit;
    nvicInit.NVIC_IRQChannel = FrameTimeManager::myTimerIRQChannel;
    nvicInit.NVIC_IRQChannelCmd = ENABLE;
    nvicInit.NVIC_IRQChannelPreemptionPriority = 0;
    nvicInit.NVIC_IRQChannelSubPriority = 0;

    NVIC_Init(&nvicInit);
}
void FrameTimeManager::init() {
    initHallEffectIRQ();
    initTimer();

    this->timerReload = FrameTimeManager::baseReload;
}

void FrameTimeManager::enable() {
    // enable frame timer
    TIM_Cmd(myTimer, ENABLE);
}

void FrameTimeManager::disable() {
    TIM_Cmd(myTimer, DISABLE);

    TIM_SetCounter(myTimer, timerReload);
}



// periodic interrupt that displays the next segment
extern "C" void TIM5_IRQHandler() {
    if(TIM_GetITStatus(FrameTimeManager::myTimer, TIM_IT_Update) != RESET) {
            TIM_ClearITPendingBit(FrameTimeManager::myTimer, TIM_IT_Update);
        LedDriver& driver = DrawManager::get().getLedDriver();
        FrameTimeManager& frameTimeManager = DrawManager::get().getFrameTimeManager();

        // draw the code
        if(driver.getDrawCounter() < ImageBuffer::size()) {
            driver.sendGreyScaleData();
        }
        else {
            if(driver.getDrawCounter() < ImageBuffer::size() + 10)
                driver.goToNextSegment();
          // driver.resetDrawCounter();

        }

    }
}

// isr that is called when the led pass a full circle i.e. pass the magnet
// triggering the hall effect sensor
extern "C" void EXTI15_10_IRQHandler() {

    // make sure interrupt is from line 12 that is
    // a hall effect sensor
    if(EXTI_GetITStatus(EXTI_Line12) != RESET) {
        // clear the interrupt flag
        EXTI_ClearITPendingBit(EXTI_Line12);

        FrameTimeManager& frameTimeManager = DrawManager::get().getFrameTimeManager();
        LedDriver& driver = DrawManager::get().getLedDriver();
        ClockManager& clkManager = DrawManager::get().getClockManager();

        if(clkManager.shouldSwapBuffers()) {
            DrawManager::get().swapBuffers();

            if(DrawManager::get().mode() == DrawMode::clockMode) {
                DrawManager::get().getClockManager().notifyBufferSwapped();
            }
        }

        // calculate the difference between the number of draw calls
        // actually made and the number that should have been made
        long int deltaDrawCounter = driver.getDrawCounter() - ImageBuffer::size();

        constexpr bool useQuickAdjust = true;

        if(useQuickAdjust) {
            frameTimeManager.alterTimerDiv(deltaDrawCounter);
        }

        else {
            if(deltaDrawCounter > 0) {
                frameTimeManager.incrementTimerDivision();
            }

            // if there were too few draw calls made the timer
            // is too slow speed the time up
            else if(deltaDrawCounter < 0) {
                frameTimeManager.decrementTimerDivision();
            }
        }



        // reset timer
        frameTimeManager.resetDrawTimer();
        driver.resetDrawCounter();

        driver.sendGreyScaleData();

        TIM_Cmd(FrameTimeManager::myTimer, ENABLE);

    }
}
