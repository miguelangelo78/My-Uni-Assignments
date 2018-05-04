#pragma once
#include "stm32f4xx.h"

// this class manages the timing and scheduling of the
// led draw code
class FrameTimeManager {
public:
    static constexpr auto  myTimer = TIM5;
    static constexpr uint16_t basePrescale = 0;
    static constexpr uint16_t baseReload = (uint16_t) 12500;
    static constexpr auto myTimerIRQChannel = TIM5_IRQn;


    // initialize frame management stuff including
    // hall effect and timer
    void init();

    // enables the timer
    void enable();

    // disable the timer
    void disable();

    __inline void alterTimerDiv(long int delta) {
        if(timerReload <= 1 + delta || timerReload >= UINT16_MAX - delta) {
            timerReload -= delta;

            TIM_SetAutoreload(myTimer, timerReload);
        }
    }

    // decrement the timer division decreasing the update
    // frequency of drawing the segment
    __inline void decrementTimerDivision() {
        if(timerReload <= 1) {
            timerReload -= 1;

            TIM_SetAutoreload(myTimer, timerReload);
        }
    }

    // increment the timer division increasing the update
    // frequency of the drawing the segment
    __inline void incrementTimerDivision() {
        if(timerReload >= UINT16_MAX - 1) {
            timerReload += 1;

            TIM_SetAutoreload(myTimer, timerReload);
        }
    }

    void moveTimerDivision(int delta);

    // resets the draw counter
    __inline void resetDrawTimer() {

        TIM_SetCounter(myTimer, timerReload);

    }





private:
    int timerReload;
};

