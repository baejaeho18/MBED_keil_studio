#include "mbed.h"




DigitalOut motorLeftPin1(D7);    // 왼쪽 모터 회전 방향 핀 1
DigitalOut motorLeftPin2(D8);    // 왼쪽 모터 회전 방향 핀 2
DigitalOut motorRightPin1(D9);   // 오른쪽 모터 회전 방향 핀 1
DigitalOut motorRightPin2(D4);  // 오른쪽 모터 회전 방향 핀 2
PwmOut motorLeftPWMPin(D5);      // 왼쪽 모터 속도 제어 핀PA
PwmOut motorRightPWMPin(D6);     // 오른쪽 모터 속도 제어 핀 PB


// 초기 설정
void setup() {
  motorLeftPin1 = 0;
  motorLeftPin2 = 0;
  motorRightPin1 = 0;
  motorRightPin2 = 0;
  motorLeftPWMPin.period_ms(20);
  motorRightPWMPin.period_ms(20);
  motorLeftPWMPin.write(0.0);
  motorRightPWMPin.write(0.0);
}

// 앞으로 이동
void moveForward() {
  motorLeftPin1 = 0;
  motorLeftPin2 = 1;
  motorRightPin1 = 0;
  motorRightPin2 = 1;

  motorLeftPWMPin.write(0.5);
  motorRightPWMPin.write(0.5);
}

// 뒤로 이동
void moveBackward() {
  motorLeftPin1 = 1;
  motorLeftPin2 = 0;
  motorRightPin1 = 1;
  motorRightPin2 = 0;
  motorLeftPWMPin.write(0.5);
  motorRightPWMPin.write(0.5);
}

// 좌회전
void turnLeft() {
  motorLeftPin1 = 0;
  motorLeftPin2 = 0;
  motorRightPin1 = 0;
  motorRightPin2 = 1;
  motorLeftPWMPin.write(0.0);
  motorRightPWMPin.write(0.5);
}

// 우회전
void turnRight() {
  motorLeftPin1 = 0;
  motorLeftPin2 = 1;
  motorRightPin1 = 0;
  motorRightPin2 = 0;
  motorLeftPWMPin.write(0.5);
  motorRightPWMPin.write(0.0);
}

// 정지
void stopMoving() {
  motorLeftPin1 = 0;
  motorLeftPin2 = 0;
  motorRightPin1 = 0;
  motorRightPin2 = 0;
  motorLeftPWMPin.write(0.0);
  motorRightPWMPin.write(0.0);
}