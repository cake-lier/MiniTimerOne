/*
 * Library for managing Arduino Timer 1 in a minimal way, with the ATMega328P processor.
 */

#ifndef __MINI_TIMER_ONE_H__
#define __MINI_TIMER_ONE_H__

#include "Arduino.h"

class MiniTimer1 {
    public:
        MiniTimer1();
        /* initializes the timer, setting the time counter to 0 and defining the timer mode (CTC) */
        void init(void);
        /* sets the period of the timer, every period an interrupt is called */
        void setPeriod(unsigned long int period);
        /* sets the function called when the interrupt is fired, and enables the firing of the interrupts */
        void attachInterrupt(void (*isr)());
        /* disables the firing of the interrupts */
        void detachInterrupt(void);
        /* starts the timer */
        void start(void);
        /* stops the timer */
        void stop(void);
        /* sets the counter of the timer to 0 */
        void reset(void);
    private:
        char _clockSelectBits;
}

#endif