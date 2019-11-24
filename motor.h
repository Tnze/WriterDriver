#include <Arduino.h>
#include "writer_config.h"

class Motor
{
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
