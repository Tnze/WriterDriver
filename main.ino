/*
  This:   绘图机固件
  Board:  NodeMCU-ESP32
  Author: Tnze
*/
#include <Arduino.h>
#include "servo.h"

void setup()
{
    Serial.begin(115200);
    init_servo();
    Serial.printf("printer lunched");
}

void loop()
{
    move_servo(-1);
    delay(1000);
    move_servo(1);
    delay(1000);
}
