#include <Arduino.h>
#include "writer_config.h"

void init_motor(); // 初始化电机

class Motor
{
public:
    int stepPin;   // pin for Motor step port
    int dirPin;    // pin for Motor direction port
    int location;  // Motor Now location
    int direction; // Motor Now direction

    void SetOrigin();                 // 设置当前位置为原点
    void Move(int exc);               // 移动exc(mm)
    void Move(int exc, float rate);   // 以速度rate(m/s) 移动exc(mm)
    void MoveTo(int pos);             // 移动到pos(mm)
    void MoveTo(int pos, float rate); // 以速度rate(m/s) 移动到pos(mm)

private:
    void RawMove(int steps, int duration); // 在duration(μs)内 向当前方向移动steps步
};

Motor motor_1, motor_2;