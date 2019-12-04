/*
  This:   绘图机固件
  Board:  NodeMCU-ESP32
  Author: Tnze
*/
#include <Arduino.h>
#include "writer_config.h"
#include "servo.h"
#include "motor.h"

#ifdef BLUETOOTH
#include "BluetoothSerial.h"
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
BluetoothSerial SerialBT;
#endif

MotorGroup motors{
    Motor(PIN_MOTOR_1),
    Motor(PIN_MOTOR_2),
};

Stream *Host;

void setup()
{
#ifdef BLUETOOTH
    SerialBT.begin(BLUETOOTH);
#endif
    Serial.begin(115200);
    Host = &Serial;

    init_servo();
    move_servo(PEN_UP);
    Host->println("//printer lunched");
}

int pen_status = 0;     // 笔的状态，0为提笔，1为落笔
int last_line_code = 0; // 串口行号

// 执行G-code，注意该解释器是随手写的，不健壮
int exec_gcode(String line)
{
    Host->println("// received: " + line);
    String args;
    int first_space = line.indexOf(' ');
    int num = line.substring(1, first_space).toInt();
    switch (line[0])
    {
    case 'G':
        if (pen_status != num) // 需要调整笔到高度
        {
            // Host->println("pen " + num ? "down" : "up");
            move_servo((pen_status = num) ? PEN_DOWN : PEN_UP);
            delay(300); // 等待舵机就位
        }

        args = line.substring(first_space + 1);
        float x, y;

        for (;;) // 读取目标位置
        {
            char c = args[0];
            int fs = args.indexOf(' ');
            (c == 'X' ? x : y) = (fs == -1 ? args.substring(1) : args.substring(1, fs)).toFloat();
            if (fs == -1)
                break;
            args = args.substring(fs + 1);
        }
        Host->printf("// move to (%f, %f)\n", x, y);
        motors.MoveTo(x, y);
        break;

    case 'M':
        switch (num)
        {
        case 110: // 重置行号
            args = line.substring(first_space + 1);
            for (;;) // 读取目标位置
            {
                char c = args[0];
                int fs = args.indexOf(' ');
                if (c == 'N')
                    last_line_code = (fs == -1 ? args.substring(1) : args.substring(1, fs)).toInt();
                if (fs == -1)
                    break;
                args = args.substring(fs + 1);
            }
            break;
        }
        break;

    default:
        Host->println("// unknown command " + line);
    }

    Host->println("ok");
    return 0;
}

// 检查N代码是否正确，并去除它
bool checkCMD(String &cmd, int &line_code)
{
    // 如果没有校验则直接返回true
    if (cmd[0] != 'N')
        return true;

    int start = cmd.indexOf(' ');
    int end = cmd.lastIndexOf('*');

    if (start == -1 || end == -1)
        return false; // 格式不正确

    // 计算码字
    line_code = cmd.substring(1, start).toInt();    // 行码
    int chek_code = cmd.substring(end + 1).toInt(); // 校验码

    // 计算本地校验码
    int cs = 0;
    for (int i = 0; i != end; i++)
        cs = cs ^ cmd[i];
    cs &= 0xff; // Defensive programming...

    // 去除原字符串校验部分
    cmd = cmd.substring(start + 1, end);

    return cs == chek_code;
}

void loop()
{
    // 从串口接受指令
    if (Serial.available())
        Host = &Serial;
#ifdef BLUETOOTH
    else if (SerialBT.available())
        Host = &SerialBT;
#endif
    else
        return;
    String cmd = Host->readStringUntil('\n'); // 读取一行

    if (!cmd.length())
        return;

    cmd = cmd.substring(0, cmd.indexOf(';')); // 去除注释

    int line_code;
    bool checked = cmd[0] == 'N';

    if (!checkCMD(cmd, line_code)) // 检查不通过，要求重发
        Host->printf("rs %d error: check code error\n", line_code);
    else if (checked && line_code != last_line_code + 1) // 行号不对，要求重发
        Host->printf("rs %d error: line number is not current line + 1. last line: %d\n",
                     last_line_code + 1,
                     last_line_code);
    else
    {
        exec_gcode(cmd);
        if (checked)
            last_line_code++;
    }
}
