#include <Arduino.h>
#include "motor.h"

void init_motor()
{
    //  初始化管脚
    pinMode(motor_1.stepPin, OUTPUT);
    pinMode(motor_2.stepPin, OUTPUT);
    pinMode(PIN_TRAVEL_SWITCH_1, INPUT_PULLUP);
    pinMode(PIN_TRAVEL_SWITCH_2, INPUT_PULLUP);

    motor_1 = {PIN_MOTOR_1, 0};
    motor_2 = {PIN_MOTOR_2, 0};
}

void Motor::Move(float exc) { Move(exc, DEFAULT_MOTOR_RATE); }

void Motor::Move(float exc, float rate)
{
}

void Motor::MoveTo(float pos) { MoveTo(pos, DEFAULT_MOTOR_RATE); }

void Motor::MoveTo(float pos, float rate)
{
    int target = STEPS(pos);                              // 目标位置(步)
    digitalWrite(dirPin, target > location ? HIGH : LOW); // 设置方向
    int exc = target > location                           // 要移动的距离
                  ? target - location
                  : location - target;

    RawMove(exc, exc / rate);
}

void Motor::RawMove(int steps, int duration)
{
    int dt = duration / steps - 4; // 每步后延迟时间
    dt = dt > 0 ? dt : 0;          // 延迟不能小于0
    for (int i = 0; i < steps; i++)
    {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(2);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(2);
        delayMicroseconds(dt);
    }
}