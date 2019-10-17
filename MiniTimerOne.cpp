/*
 *
 */

#include "MiniTimerOne.h"

#define TIMER1_MAX_VALUE 65536UL
#define CYCLES_PER_MICROSEC 16

void isrDefault(void) {
}

MiniTimerOne *MiniTimerOne::SINGLETON {};

MiniTimerOne *MiniTimerOne::getInstance(void) {
    return SINGLETON;
}

MiniTimerOne::MiniTimerOne(void) {
    this->_clockSelectBits = 0;
    this->_isrCallback = isrDefault;
}

void MiniTimerOne::init(void) {
    cli();
    TCCR1B = _BV(WGM12);
    TCCR1A = 0;
    TIMSK1 = 0;
    TCNT1 = 0;
    sei();
}

void MiniTimerOne::setPeriod(unsigned long int period = 0) {
    const unsigned long int cycles = CYCLES_PER_MICROSEC * period;
    const int prescalerValues[] = { 1, 8, 64, 256, 1024 };
    const unsigned int csBitsValues[] = { _BV(CS10), _BV(CS11), _BV(CS11) | _BV(CS10), _BV(CS12), _BV(CS12) | _BV(CS10) };
    bool timerSet = false;

    for (int i = 0; i < 5 && !timerSet; i++) {
        if (cycles < TIMER1_MAX_VALUE * prescalerValues[i]) {
            this->_clockSelectBits = csBitsValues[i];
            OCR1A = cycles / prescalerValues[i];
            timerSet = true;
        }
    }
    /*
     * If value exceeds the maximum value the register can hold, even with the maximum prescaler value, it sets the register
     * value to the maximum value.
     */
    if (!timerSet) {
        this->_clockSelectBits = _BV(CS12) | _BV(CS10);
        OCR1A = TIMER1_MAX_VALUE - 1; 
    }
}

void MiniTimerOne::attachInterrupt(void (*isr)(void) = isrDefault) {
    this->_isrCallback = isr;
    TIMSK1 |= _BV(OCIE1A);
}

void MiniTimerOne::detachInterrupt(void) {
    TIMSK1 = 0;
}

void MiniTimerOne::start(void) {
    TCCR1B |= this->_clockSelectBits;
}

void MiniTimerOne::stop(void) {
    TCCR1B = _BV(WGM12);
}

void MiniTimerOne::reset(void) {
    TCNT1 = 0;
}

void (*MiniTimerOne::getCallback(void))(void) {
    return this->_isrCallback;
}

MiniTimerOne MiniTimer1 = *MiniTimerOne::getInstance();

ISR(TIMER1_COMPA_vect) {
    (MiniTimer1.getCallback())();
}
