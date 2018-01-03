#include "FrameTimeManager.h"
#include "PropellerManager.hpp"
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

    // set the external interrupt line to call an interrupt and
    // trigger the interrupt when the GPIO goes either high or
    // low
    extiInit.EXTI_Line = EXTI_Line12;
    extiInit.EXTI_LineCmd = ENABLE;
    extiInit.EXTI_Mode = EXTI_Mode_Interrupt;
    extiInit.EXTI_Trigger = EXTI_Trigger_Rising_Falling;

    EXTI_Init(&extiInit);

    // enable IRQ
    nvicInit.NVIC_IRQChannel = EXTI15_10_IRQn;
    nvicInit.NVIC_IRQChannelPreemptionPriority = 0x00;
    nvicInit.NVIC_IRQChannelSubPriority = 0x01;
    nvicInit.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&nvicInit);
}

static void initTimer() {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

    TIM_TimeBaseInitTypeDef timInit;
    timInit.TIM_Period = 60; //60
    timInit.TIM_Prescaler = 440; // 440
    timInit.TIM_ClockDivision = TIM_CKD_DIV1;
    timInit.TIM_CounterMode = TIM_CounterMode_Up;
    timInit.TIM_RepetitionCounter = 0;
    TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
    TIM_TimeBaseInit(TIM5, &timInit);


    NVIC_InitTypeDef nvicInit;
    nvicInit.NVIC_IRQChannel = TIM5_IRQn;
    nvicInit.NVIC_IRQChannelCmd = ENABLE;
    nvicInit.NVIC_IRQChannelPreemptionPriority = 0;
    nvicInit.NVIC_IRQChannelSubPriority = 0;

    NVIC_Init(&nvicInit);
}
void FrameTimeManager::init() {
    timerDivision = 440;
    initHallEffectIRQ();
    initTimer();
}

void FrameTimeManager::enable() {
    // enable frame timer
    TIM_Cmd(TIM5, ENABLE);
}

void FrameTimeManager::disable() {
    // disable the timer
    TIM_Cmd(TIM5, DISABLE);

    // reset the counter
    TIM_SetCounter(TIM5, 0);
}

// periodic interrupt that displays the next segment
extern "C" void TIM5_IRQHandler() {
    if(TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET) {

        LedDriver& driver = PropellerMan::get().getLedDriver();

        // clear the interrupt
        TIM_ClearITPendingBit(TIM5, TIM_IT_Update);

        // if we have not finished drawing all the segments
        // then draw the next segment
        if(driver.getDrawCounter() < ImageBuffer::size())
            driver.sendGreyScaleData();
    #ifdef MY_CONTINOUS_LED_FLASHING && if MY_CONTINOUS_LED_FLASHING == 1
        //  reset the draw counter
        else
            driver.resetDrawCounter();
    #endif
    }
}

// isr that is called when the led pass a full circle i.e. pass the magnet
// triggering the hall effect sensor
extern "C" void EXTI15_10_IRQHandler() {

    // make sure interrupt is from line 12 that is
    // a hall effect sensor
    if(EXTI_GetITStatus(EXTI_Line12) != RESET) {
        FrameTimeManager& frameTimeManager = PropellerMan::get().getFrameTimeManager();
        LedDriver& driver = PropellerMan::get().getLedDriver();
        ClockManager& clkManager = PropellerMan::get().getClockManager();

        // clear the interrupt flag
        EXTI_ClearITPendingBit(EXTI_Line12);

        // calculate the difference between the number of draw calls
        // actually made and the number that should have been made
        int deltaDrawCounter = driver.getDrawCounter() - ImageBuffer::size();


        // give up on trying to adjust the timer is gap too large
        if(deltaDrawCounter > 15 || deltaDrawCounter < -15) {

        }
        // if there were too many draw calls made the timer
        // is too fast scale back the timer
        else if(deltaDrawCounter > 0) {
            frameTimeManager.decrementTimerDivision();
        }

        // if there were too few draw calls made the timer
        // is too slow speed the time up
        else if(deltaDrawCounter < 0) {
            frameTimeManager.incrementTimerDivision();
        }

        // if we need to swap image buffers then
        // swap them and notify the clock manager
        // that we swapped them
        if(clkManager.shouldSwapBuffers()) {
            PropellerMan::get().swapBuffers();
            clkManager.notifyBufferSwapped();
        }

        // reset timer
        frameTimeManager.resetDrawTimer();
        driver.resetDrawCounter();
        driver.sendGreyScaleData();
    }
}
