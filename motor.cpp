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

void Motor::Step()
{
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(3);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(2);
}

void Motor::Move(int step, int time)
{
    int delayTime = (time * 1000 - step * 5) / step;
    for (int i = 0; i < step; i++)
    {
        Step();
        delayMicroseconds(delayTime);
    }
}
