/*
  This:   绘图机固件
  Board:  NodeMCU-ESP32
  Author: Tnze
*/
#include <Arduino.h>
#include "writer_config.h"
#include "servo.h"
#include "motor.h"

MotorGroup motors{
    Motor(PIN_MOTOR_1),
    Motor(PIN_MOTOR_2),
};

void setup()
{
    Serial.begin(115200);
    init_servo();
    move_servo(PEN_UP);
    Serial.printf("printer lunched");
}

int pen_status = 0; // 0为提笔，1为落笔
int exec_gcode(String line)
{
    if (line.length())
        Serial.println(line);

    switch (line[0])
    {
    case 'G':
        int first_space = line.indexOf(' ');
        int num = line.substring(1, first_space).toInt();
        if (pen_status != num)
        {
            Serial.println("pen " + num ? "down" : "up");
            move_servo((pen_status = num) ? PEN_DOWN : PEN_UP);
        }

        String args = line.substring(first_space + 1);
        float x, y;
        for (int i = 0; i < 2; i++)
        {
            char c = args[0];
            int fs = args.indexOf(' ');
            (c == 'X' ? x : y) = args.substring(1, fs).toFloat();
            args = args.substring(fs + 1);
        }
        Serial.printf("Move to (%f, %f)\n", x, y);
        motors.MoveTo(x, y);
        break;
    }
    return 0;
}

void loop()
{
    // 从串口接受指令
    String cmd = Serial.readStringUntil('\n');
    exec_gcode(cmd);
}
