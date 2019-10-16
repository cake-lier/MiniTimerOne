#include "MiniTimerOne.h"

void isrDefault(void) {
}

MiniTimerOne *MiniTimerOne::SINGLETON = new MiniTimerOne();

MiniTimerOne *MiniTimerOne::getInstance(void) {
    return SINGLETON;
}

MiniTimerOne::MiniTimerOne(void) {
    _clockSelectBits = 0;
    _isrCallback = isrDefault;
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
    //calculate clockSelectBits
}

void MiniTimerOne::attachInterrupt(void (*isr)(void) = isrDefault) {
    _isrCallback = isr;
    TIMSK1 |= _BV(OCIE1A);
}

void MiniTimerOne::detachInterrupt(void) {
    TIMSK1 = 0;
}

void MiniTimerOne::start(void) {
    TCCR1B |= _clockSelectBits;
}

void MiniTimerOne::stop(void) {
    TCCR1B = _BV(WGM12);
}

void MiniTimerOne::reset(void) {
    TCNT1 = 0;
}

void (*MiniTimerOne::getCallback(void))(void) {
    return _isrCallback;
}

MiniTimerOne MiniTimer1 = *MiniTimerOne::getInstance();

ISR(TIMER1_COMPA_vect) {
    MiniTimerOne.getCallback();
}
