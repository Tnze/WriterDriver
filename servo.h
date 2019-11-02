#include <Arduino.h>
#include "writer_config.h"

// 用舵机来控制笔上下运动

// 初始化舵机
void init_servo();

// 将舵机移动到指定位置
void move_servo(float pos);