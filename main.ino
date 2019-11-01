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
}
