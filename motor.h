#include <Arduino.h>
#include "writer_config.h"

class MotorGroup;
class Motor
{
    friend class MotorGroup;

private:
    int dirPin;   // pin for Motor direction port
    int stepPin;  // pin for Motor step port
    int location; // Motor Now location

public:
    Motor(int pin_dir, int pin_step);

    // void Move(float exc);             // 移动exc(mm)
    // void Move(float exc, float rate); // 以速度rate(m/s) 移动exc(mm)

    void MoveTo(float pos);             // 移动到pos(mm)
    void MoveTo(float pos, float rate); // 以速度rate(m/s) 移动到pos(mm)

    // private:
    void RawMove(int steps, int duration); // 在duration(μs)内 向当前方向移动steps步
};

class MotorGroup
{
public:
    Motor mX, mY;
    void MoveTo(float posX, float posY);             // 移动到指定坐标
    void MoveTo(float posX, float posY, float rate); // 以速度rate(m/s) 移动到指定坐标
};