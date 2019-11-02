#include <Arduino.h>
#include "motor.h"

Motor motor_1 = {PIN_MOTOR_1, 0},
      motor_2 = {PIN_MOTOR_2, 0};

void motor_setup()
{
    //  初始化管脚
    pinMode(motor_1.stepPin, OUTPUT);
    pinMode(motor_2.stepPin, OUTPUT);
    pinMode(PIN_TRAVEL_SWITCH_1, INPUT_PULLUP);
    pinMode(PIN_TRAVEL_SWITCH_2, INPUT_PULLUP);
}

//步进电机向前移动1步
void Motor_Step(Motor *m)
{
    digitalWrite(m->stepPin, HIGH);
    delayMicroseconds(3);
    digitalWrite(m->stepPin, LOW);
    delayMicroseconds(2);
}

//步进电机在time(ms)指定的时间移动指定step步
void Motor_Move(Motor *m, int moveStep, int moveTime)
{
    int delayTime = (moveTime * 1000 - moveStep * 5) / moveStep;
    for (int i = 0; i < moveStep; i++)
    {
        Motor_Step(m);
        delayMicroseconds(delayTime);
    }
}
