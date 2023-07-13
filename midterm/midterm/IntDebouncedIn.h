#ifndef INTDEBOUNCEIN_H
#define INTDEBOUNCEIN_H

#include "mbed.h"

#define DEFAULT_TIME 20ms

typedef enum {
    Released, // 0
    PrePressed1, // 1
    PrePressed2, // 2
    Pressed, // 3
    PreReleased1, // 4
    PreReleased2 // 5
} swState ; // state머신을 위한 변수설정

class IntDebounceIn {
    public:
        IntDebounceIn(PinName pin, std::chrono::milliseconds debounceTime = DEFAULT_TIME) ;
        void fall(Callback<void()> Callback_Func) ;
        void rise(Callback<void()> Callback_Func) ;
    private:
        InterruptIn interrupt ;
        swState state ;
        Ticker debounce_ticker ;

        Callback<void()> fall_Callback ; 
        Callback<void()> rise_Callback ;

        void fall_Interrupt() ;
        void rise_Interrupt() ;
        void debounce() ;
};

#endif
