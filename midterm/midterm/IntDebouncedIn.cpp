#include "IntDebouncedIn.h"

IntDebounceIn::IntDebounceIn(PinName pin, std::chrono::milliseconds debounceTime) :
    interrupt(pin),
    state(Released)  //0 (시작state)
{
    debounce_ticker.attach(callback(this, &IntDebounceIn::debounce), debounceTime);

    interrupt.fall(callback(this, &IntDebounceIn::fall_Interrupt));
    interrupt.rise(callback(this, &IntDebounceIn::rise_Interrupt));
}

void IntDebounceIn::fall(Callback<void()> Callback_Func) {
    fall_Callback =Callback_Func;
}

void IntDebounceIn::rise(Callback<void()> Callback_Func) {
    rise_Callback=Callback_Func;
}

void IntDebounceIn::fall_Interrupt() {
    if (state==Released) {
        state=PrePressed1;
    }
}

void IntDebounceIn::rise_Interrupt() {
    if (state==Pressed) {
        state=PreReleased1;
    }
}

void IntDebounceIn::debounce() {
    int input_value = interrupt.read();
    switch (state) {
        case PrePressed1:
            if (!input_value) {
                state =PrePressed2;
            } else {
                state=Released;
            }
            break;

        case PrePressed2:
            if (!input_value) {
                state = Pressed;
                fall_Callback();
            } else {
                state=Released;
            }
            break;

        case PreReleased1:
            if (input_value) {
                state =PreReleased2;
            } else {
                state=Pressed;
            }
            break;

        case PreReleased2:
            if (input_value) {
                state = Released;
                rise_Callback();
            } else {
                state=Pressed;
            }
            break;

        default:
            break;
    }
}