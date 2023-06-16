/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"

DigitalOut led(LED1);
DigitalIn button (BUTTON1);


int main()
{
    int interval = 500;

    while (true) {
       
       if (button ==0){
           interval = interval<<1;
           if(interval>2000)
            interval = 250;
       }
       led = !led;
       ThisThread::sleep_for(
           std::chrono::milliseconds(interval));

    }
}
