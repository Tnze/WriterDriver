#include <Arduino.h>
#include "writer_config.h"

void init_motor(); // 初始化电机

class Motor
{
public:
    int stepPin;  // pin for Motor step port
    int dirPin;   // pin for Motor direction port
    int location; // Motor Now location

    void Move(float exc);             // 移动exc(mm)
    void Move(float exc, float rate); // 以速度rate(m/s) 移动exc(mm)

    void MoveTo(float pos);             // 移动到pos(mm)
    void MoveTo(float pos, float rate); // 以速度rate(m/s) 移动到pos(mm)

private:
    void RawMove(int steps, int duration); // 在duration(μs)内 向当前方向移动steps步
};

Motor motor_1, motor_2;
