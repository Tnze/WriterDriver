#include <Arduino.h>
// 设置

// ----------------DEFAULT----------------

#define DEFAULT_SERVO_POS 0.5 // 默认舵机位置

// ----------------PIN----------------
#define PIN_SERVO 2            // 舵机PWM控制管脚
#define PIN_MOTOR_1 25, 26     // 电机1 (dir, step)
#define PIN_MOTOR_2 32, 35     // 电机2 (dir, step)
#define PIN_TRAVEL_SWITCH_1 27 // 限位开关1
#define PIN_TRAVEL_SWITCH_2 14 // 限位开关2

//========================================//
//                  设置                  //
//========================================//
#define Xstep 14  //X步进电机              //
#define Ystep 12  //Y步进电机              //
#define Xlimit 26 //X限位开关              //
#define Ylimit 27 //Y限位开关              //
//========================================//
