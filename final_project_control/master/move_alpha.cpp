#include "mbed.h"




DigitalOut motorLeftPin1(A1);    // 왼쪽 모터 회전 방향 핀 1
DigitalOut motorLeftPin2(A0);    // 왼쪽 모터 회전 방향 핀 2
DigitalOut motorRightPin1(A2);   // 오른쪽 모터 회전 방향 핀 1
DigitalOut motorRightPin2(A3);  // 오른쪽 모터 회전 방향 핀 2
PwmOut motorLeftPWMPin(D6);      // 왼쪽 모터 속도 제어 핀PA 
PwmOut motorRightPWMPin(D5);     // 오른쪽 모터 속도 제어 핀 PB
//pwm 핀은 변경불가능하고  나머지 핀을 사용할 경우 다른 핀으로 변경만 해주면 됩니다. 


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
void moveForward(float a) {
  motorLeftPin1 = 0;
  motorLeftPin2 = 1;
  motorRightPin1 = 0;
  motorRightPin2 = 1;

  motorLeftPWMPin.write(a);
  motorRightPWMPin.write(a);
}

// 뒤로 이동
void moveBackward(float a) {
  motorLeftPin1 = 1;
  motorLeftPin2 = 0;
  motorRightPin1 = 1;
  motorRightPin2 = 0;
  motorLeftPWMPin.write(a);
  motorRightPWMPin.write(a);
}

// 좌회전
void turnLeft(float a) {
  motorLeftPin1 = 0;
  motorLeftPin2 = 0;
  motorRightPin1 = 0;
  motorRightPin2 = 1;
  motorLeftPWMPin.write(0.0);
  motorRightPWMPin.write(a);
}

// 우회전
void turnRight(float a) {
  motorLeftPin1 = 0;
  motorLeftPin2 = 1;
  motorRightPin1 = 1;
  motorRightPin2 = 0;
  motorLeftPWMPin.write(a);
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