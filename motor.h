#include <Arduino.h>
#include "writer_config.h"

typedef struct
{
    int stepPin;  //pin for Motor step port
    int dirPin;   //pin for Motor direction port
    int location; //Motor Now location
} Motor;