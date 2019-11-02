#include <Arduino.h>
#include "servo.h"

#define PWM_CHANNEL 0 // 控制舵机用的系统pwn通道 共16个 这里用0号

const int MAX_COMPARE = ((1 << 16) - 1); // 65535
const int TAU_MSEC = 20;
const int TAU_USEC = (TAU_MSEC * 1000);

void init_servo()
{
    pinMode(PIN_SERVO, OUTPUT);
    ledcSetup(PWM_CHANNEL, 50, 16); // channel X, 50 Hz, 16-bit depth
    ledcAttachPin(PIN_SERVO, PWM_CHANNEL);
    move_servo(DEFAULT_SERVO_POS);
}

void move_servo(float pos)
{
    ledcWrite(PWM_CHANNEL, map(pos * 500 + 1000, 0, TAU_USEC, 0, MAX_COMPARE));
}