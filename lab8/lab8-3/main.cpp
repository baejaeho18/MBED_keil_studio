#include "mbed.h"

#define B 246.94
#define C 261.626
#define D 293.665
#define E 329.628
#define F 349.228
#define G 391.995
#define A 440.000
#define B_O 493.883
#define C_O 523.251
#define H 1
PwmOut buzzer(ARDUINO_UNO_D8) ;

float freq[] = {C, D, E, G, G, E, D, G, G, D, C, H, H, C, D, E, G, G, E, D, G, G, D, C, H, H, E, G, A, C_O ,C_O, A, G, E, D, C, D, H, H, C, D, E, G, G, E, D, E, D, C, C, H, H} ;
int beat[] = {4, 4, 8, 8, 8, 8, 8, 8, 8, 8, 32, 16, 8, 4, 4, 8, 8, 8, 8, 8, 8, 8, 8, 32, 16, 8, 4, 4, 8, 8, 8, 8, 8, 8, 8, 8, 32, 16, 8, 4, 4, 8, 8, 8, 8, 8, 8, 8, 8, 32, 16, 8} ;    //4:반의 반, 8:반박자, 16: 한박자, 32: 두박자

int main()
{
    int period_us ;
    int beat_ms ;

    while (true) 
    {
        for (int i = 0 ; i < 52 ; i++)
        {
            buzzer = 1.0 - 0.1 ;                         // set duty cycle (1.0 –0.1)
            period_us = 1000000 / freq[i] ;
            beat_ms = 62.5 * beat[i] ;
            if (freq[i] == H)
            {
                buzzer.suspend() ;
            }
            else
            {
                buzzer.resume() ;
                buzzer.period_us(period_us) ;          // set PWM period
            }

            ThisThread::sleep_for(chrono::milliseconds(beat_ms)) ;   // hold for beat period
            //buzzer = 1.0 ;
            //ThisThread::sleep_for(chrono::milliseconds(100)) ;
        }
    }
}
