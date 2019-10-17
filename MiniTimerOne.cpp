/*
 * Interrupt utility for 16 bit Timer1 on ATmega328P
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
    TCCR1B = _BV(WGM12); // setting the timer mode, CTC, and disabling the timer clock source (stopping it)
    TCCR1A = 0; // no flag in this register is needed
    TIMSK1 = 0; // disabling interrupts capture
    cli(); // disabling interrupts, because TCNT1 setting is not an atomic operation
    TCNT1 = 0; // resetting the timer count
    sei(); // re-enabling interrupts
}

void MiniTimerOne::setPeriod(unsigned long int period) {
    const unsigned long int cycles = CYCLES_PER_MICROSEC * period;
    const int prescalerValues[] = { 1, 8, 64, 256, 1024 };
    const unsigned int csBitsValues[] = { _BV(CS10), _BV(CS11), _BV(CS11) | _BV(CS10), _BV(CS12), _BV(CS12) | _BV(CS10) };
    bool timerSet = false;

    for (int i = 0; i < 5 && !timerSet; i++) {
        if (cycles < TIMER1_MAX_VALUE * prescalerValues[i]) {
            /*
             * To start the timer three bits will need to be set in the TCCR1B register.
             * How those bits needs to be set depends on how long the period of the timer is, 
             * but setting them will start the timer, so their value is temporarily stored in _clockSelectBits.
             */
            this->_clockSelectBits = csBitsValues[i]; 
            cli(); // disabling interrupts, because OCR1A setting is not an atomic operation
            OCR1A = cycles / prescalerValues[i]; // setting the value with which the counter (TCNT1) will be compared
            sei();
            timerSet = true;
        }
    }
    /*
     * If value exceeds the maximum value the register can hold, even with the maximum prescaler value, it sets the register
     * value to the maximum value.
     */
    if (!timerSet) {
        this->_clockSelectBits = _BV(CS12) | _BV(CS10);
        cli(); // disabling interrupts, because OCR1A setting is not an atomic operation
        OCR1A = TIMER1_MAX_VALUE - 1; 
        sei();
    }
}

void MiniTimerOne::attachInterrupt(void (*isr)(void)) {
    this->_isrCallback = isr; // setting the function to be called on the interrupt capture
    TIMSK1 |= _BV(OCIE1A); // enabling interrupts capture
}

void MiniTimerOne::detachInterrupt(void) {
    TIMSK1 = 0; // disabling interrupts capture
    this->_isrCallback = isrDefault; // resetting function called on interrupt to the default one
}

void MiniTimerOne::start(void) {
    TCCR1B |= this->_clockSelectBits; // setting the clock source, and consequently starting the timer
}

void MiniTimerOne::stop(void) {
    TCCR1B = _BV(WGM12); // disabling the timer clock source (stopping it), and setting the timer mode (CTC)
}

void MiniTimerOne::reset(void) {
    cli(); // disabling interrupts, because TCNT1 setting is not an atomic operation
    TCNT1 = 0; // resetting the timer counter, to start a new period
    sei();
}

void (*MiniTimerOne::getCallback(void))(void) {
    return this->_isrCallback;
}

MiniTimerOne MiniTimer1 = *MiniTimerOne::getInstance();

ISR(TIMER1_COMPA_vect) {
    (MiniTimer1.getCallback())();
}
