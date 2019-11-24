// ================SETTINGS===============
#include <Arduino.h>

// ----------------DEFAULT----------------

#define DEFAULT_SERVO_POS 0.5  // 默认舵机位置
#define DEFAULT_MOTOR_RATE 0.5 // 默认电机移动速度 (step/ms)

// ------------------PIN------------------
#define PIN_SERVO 2            // 舵机PWM控制管脚
#define PIN_MOTOR_1 25, 26     // 电机1 (dir, step)
#define PIN_MOTOR_2 33, 32     // 电机2 (dir, step)
#define PIN_TRAVEL_SWITCH_1 27 // 限位开关1
#define PIN_TRAVEL_SWITCH_2 14 // 限位开关2

// -----------------OFFSET----------------
#define NUMBER_OF_TOOTH 20 // 齿轮一圈的齿数
#define TOOTH_DISTANCE 2   // 皮带齿距(mm)
#define MOTOR_STEPS 200    // 步进电机转一圈步数
