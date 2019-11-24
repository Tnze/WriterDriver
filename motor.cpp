#include <Arduino.h>
#include "motor.h"

// 移动length(mm)所需要的步数
inline int STEPS(float length)
{
    // 电机转一圈移动的距离
    const int round_distance = NUMBER_OF_TOOTH / TOOTH_DISTANCE;
    return (length / round_distance * MOTOR_STEPS);
}

Motor::Motor(int pin_dir, int pin_step)
{
    dirPin = pin_dir;
    stepPin = pin_step;
    pinMode(pin_dir, OUTPUT);
    pinMode(pin_step, OUTPUT);
    location = 0;
}

void Motor::MoveTo(float pos) { MoveTo(pos, DEFAULT_MOTOR_RATE); }

void Motor::MoveTo(float pos, float rate)
{
    Serial.printf("[Debug]move to: [%d, %.2f]\n", pos, rate);

    int target = STEPS(pos);                              // 目标位置(步)
    digitalWrite(dirPin, target > location ? HIGH : LOW); // 设置方向
    int exc = target > location                           // 要移动的距离
                  ? target - location
                  : location - target;

    RawMove(exc, (exc / rate) * 1000);
    location = target;
}

void Motor::RawMove(int steps, int duration)
{
    if (steps == 0)
    {
        delayMicroseconds(duration);
        return;
    }
    Serial.printf("[Debug]raw move: [%d, %d]\n", steps, duration);
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