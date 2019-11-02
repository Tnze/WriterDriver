#include <Arduino.h>
#include "writer_config.h"

void init_motor(); // 初始化电机

class Motor
{
public:
    int stepPin;  // pin for Motor step port
    int dirPin;   // pin for Motor direction port
    int location; // Motor Now location

    void Step();                   // 步进电机向前移动1步
    void Move(int step, int time); // 步进电机在time(ms)指定的时间移动指定step步
};

Motor motor_1, motor_2;