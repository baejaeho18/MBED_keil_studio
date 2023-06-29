/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"

DigitalOut greenLED(LED1);
DigitalOut redLED(D2);
InterruptIn button (BUTTON1);


volatile int interval=500;

void buttonPressed()
{
    redLED = !redLED;
    if(interval<4000){
        interval += 500;
    }
    else
        interval = 500;
}

int main()
{
    button.fall(&buttonPressed);

    while (true) {
       
       greenLED = !greenLED;
       ThisThread::sleep_for(std::chrono::milliseconds(interval));

    }
}
