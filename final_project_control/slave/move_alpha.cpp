#include "mbed.h"

#define FORWARD 1
#define BACKWARD 2
#define RIGHT 3
#define LEFT 4
#define STOP 0

#define weight 0.3
#define accel 0.1
#define rotate_accel 0.8

DigitalOut motorLeftPin1(A1) ;    // 왼쪽 모터 회전 방향 핀 1
DigitalOut motorLeftPin2(A0) ;    // 왼쪽 모터 회전 방향 핀 2
DigitalOut motorRightPin1(A2) ;   // 오른쪽 모터 회전 방향 핀 1
DigitalOut motorRightPin2(A3) ;  // 오른쪽 모터 회전 방향 핀 2
PwmOut motorLeftPWMPin(D6) ;      // 왼쪽 모터 속도 제어 핀PA 
PwmOut motorRightPWMPin(D5) ;     // 오른쪽 모터 속도 제어 핀 PB
//pwm 핀은 변경불가능하고  나머지 핀을 사용할 경우 다른 핀으로 변경만 해주면 됩니다. 

void moveAlpha(float stack, int direction)
{
    float speed = accel * (stack + 1) * weight ;

    if (direction == FORWARD)
    {
        motorLeftPin1 = 0 ;
        motorLeftPin2 = 1 ;
        motorRightPin1 = 0 ;
        motorRightPin2 = 1 ;

        motorLeftPWMPin.write(speed) ;
        motorRightPWMPin.write(speed) ; 
    }
    else if (direction == BACKWARD) 
    {
        motorLeftPin1 = 1 ;
        motorLeftPin2 = 0 ;
        motorRightPin1 = 1 ;
        motorRightPin2 = 0 ;
        motorLeftPWMPin.write(speed) ;
        motorRightPWMPin.write(speed) ;
    }
    else if (direction == LEFT) 
    {
        motorLeftPin1 = 1 ;
        motorLeftPin2 = 0 ;
        motorRightPin1 = 0 ;
        motorRightPin2 = 1 ;
        motorLeftPWMPin.write(speed * rotate_accel) ;
        motorRightPWMPin.write(speed * rotate_accel) ;
    }
    else if (direction == RIGHT) 
    {
        motorLeftPin1 = 0 ;  
        motorLeftPin2 = 1 ;
        motorRightPin1 = 1 ;
        motorRightPin2 = 0 ;
        motorLeftPWMPin.write(speed*rotate_accel) ;
        motorRightPWMPin.write(speed*rotate_accel) ;
    }
    else if (direction == STOP) 
    {
        motorLeftPin1 = 0 ;
        motorLeftPin2 = 0 ;
        motorRightPin1 = 0 ;
        motorRightPin2 = 0 ;
        motorLeftPWMPin.write(0.0) ;
        motorRightPWMPin.write(0.0) ;
    }
}

// 초기 설정
void setup() 
{
  motorLeftPin1 = 0 ;
  motorLeftPin2 = 0 ;
  motorRightPin1 = 0 ;
  motorRightPin2 = 0 ;
  motorLeftPWMPin.period_ms(20) ;
  motorRightPWMPin.period_ms(20) ;
  motorLeftPWMPin.write(0.0) ;
  motorRightPWMPin.write(0.0) ;
}


// // 앞으로 이동
// void moveForward(float a, int stack) 
// {
//   motorLeftPin1 = 0;
//   motorLeftPin2 = 1;
//   motorRightPin1 = 0;
//   motorRightPin2 = 1;

//   motorLeftPWMPin.write(a * 0.2*(stack+1));
//   motorRightPWMPin.write(a * 0.2*(stack+1));
// }

// // 뒤로 이동
// void moveBackward(float a, int stack) {
//   motorLeftPin1 = 1;
//   motorLeftPin2 = 0;
//   motorRightPin1 = 1;
//   motorRightPin2 = 0;
//   motorLeftPWMPin.write(a * 0.2*(stack+1)/2);
//   motorRightPWMPin.write(a * 0.2*(stack+1)/2);
// }

// // 좌회전
// void turnLeft(float a, int stack) {
  
//   motorLeftPin1 = 1;
//   motorLeftPin2 = 0;
//   motorRightPin1 = 0;
//   motorRightPin2 = 1;
//   motorLeftPWMPin.write(a * 0.2*(stack+1)/2);
//   motorRightPWMPin.write(a * 0.2*(stack+1)/2);
// }

// // 우회전
// void turnRight(float stack, int stack) 
// {
// 
//   motorLeftPin1 = 0 ;
//   motorLeftPin2 = 1 ;
//   motorRightPin1 = 1 ;
//   motorRightPin2 = 0 ;
//   motorLeftPWMPin.write(a * 0.2 * (stack + 1)) ;
//   motorRightPWMPin.write(a * 0.2 * (stack + 1)) ;
// // }

// // 정지
// void stopMoving(float stack) {
//   motorLeftPin1 = 0;
//   motorLeftPin2 = 0;
//   motorRightPin1 = 0;
//   motorRightPin2 = 0;
//   motorLeftPWMPin.write(0.0);
//   motorRightPWMPin.write(0.0);
// }
