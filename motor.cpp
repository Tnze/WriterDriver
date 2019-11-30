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
    int exc = abs(target - location);                     // 要移动的距离

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
void MotorGroup::MoveTo(float posX, float posY) { MoveTo(posX, posY, DEFAULT_MOTOR_RATE); }

void MotorGroup::MoveTo(float posX, float posY, float rate)
{
    // 计算目标位置
    int targetX = STEPS(posX);
    int targetY = STEPS(posY);
    // 设置方向
    digitalWrite(mX.dirPin, targetX > mX.location ? HIGH : LOW);
    digitalWrite(mY.dirPin, targetY > mY.location ? HIGH : LOW);
    // 计算步数
    int excX = abs(targetX - mX.location);
    int excY = abs(targetY - mY.location);
    // 记录位置
    mX.location = targetX;
    mY.location = targetY;

    int duration = sqrt(excX * excX + excY * excY) / rate * 1000;

    if (excX == 0 && excY == 0)
    {
        delayMicroseconds(duration);
        return;
    }

    if (excX > excY)
    {
        int iy = 0;
        int dt = duration / excX - 4; // 每步后延迟时间
        for (int ix = 0; ix < excX; ix++)
        {
            digitalWrite(mX.stepPin, HIGH);
            if (excY && ix > iy * excX / excY)
            {
                digitalWrite(mY.stepPin, HIGH);
                iy++;
            }
            delayMicroseconds(2);
            digitalWrite(mX.stepPin, LOW);
            digitalWrite(mY.stepPin, LOW);
            delayMicroseconds(dt);
        }
        while (iy < excY)
        {
            digitalWrite(mY.stepPin, HIGH);
            delayMicroseconds(2);
            digitalWrite(mY.stepPin, LOW);
        }
    }
    else
    {
        int ix = 0;
        int dt = duration / excY - 4; // 每步后延迟时间
        for (int iy = 0; iy < excY; iy++)
        {
            digitalWrite(mY.stepPin, HIGH);
            if (excX && iy > ix * excY / excX)
            {
                digitalWrite(mX.stepPin, HIGH);
                ix++;
            }
            delayMicroseconds(2);
            digitalWrite(mX.stepPin, LOW);
            digitalWrite(mY.stepPin, LOW);
            delayMicroseconds(dt);
        }
        while (ix < excX)
        {
            digitalWrite(mX.stepPin, HIGH);
            delayMicroseconds(2);
            digitalWrite(mX.stepPin, LOW);
        }
    }
}