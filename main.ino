/*
  This:   绘图机固件
  Board:  NodeMCU-ESP32
  Author: Tnze
*/
#include <Arduino.h>
#include "writer_config.h"
#include "servo.h"
#include "motor.h"
#include <math.h>

Motor m1(PIN_MOTOR_1);
Motor m2(PIN_MOTOR_2);

void setup()
{
  Serial.begin(115200);
  init_servo();
  Serial.printf("printer lunched");
}

void loop()
{
  float from = 0.1f, to = 1.5f;
  for (float pos = from; pos < to; pos += 0.01f)
  {
    delay(10);
    move_servo(pos);
  }
  for (float pos = to; pos > from; pos -= 0.01f)
  {
    delay(10);
    move_servo(pos);
  }
}
