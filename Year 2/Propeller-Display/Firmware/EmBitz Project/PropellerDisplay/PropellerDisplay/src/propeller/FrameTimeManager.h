#pragma once
#include "stm32f4xx.h"

// this class manages the timing and scheduling of the
// led draw code
class FrameTimeManager {
public:
    static constexpr auto  myTimer = TIM5;

    // initialize frame management stuff including
    // hall effect and timer
    void init();

    // enables the timer
    void enable();

    // disable the timer
    void disable();

    // decrement the timer division decreasing the update
    // frequency of drawing the segment
    __inline void decrementTimerDivision() {
        timerDivision -= 6;

        TIM_SetClockDivision(TIM5, timerDivision);
    }

    // increment the timer division increasing the update
    // frequency of the drawing the segment
    __inline void incrementTimerDivision() {
        timerDivision +=6;
        TIM_SetClockDivision(TIM5, timerDivision);
    }

    // resets the draw counter
    __inline void resetDrawTimer() {
        TIM_SetCounter(TIM5, 0);
    }

private:
    int timerDivision;
};

