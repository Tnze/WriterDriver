/*
  This:   绘图机固件
  Board:  NodeMCU-ESP32
  Author: Tnze
*/
//========================================//
//                  设置                  //
//========================================//
#define Xstep 14  //X步进电机              //
#define Ystep 12  //Y步进电机              //
#define Xlimit 26 //X限位开关              //
#define Ylimit 27 //Y限位开关              //
//========================================//

typedef struct {
  int stepPin;        //pin for Motor step port
  int dirPin;         //pin for Motor direction port
  int location;       //Motor Now location
  int limitSwitchPin; //pin for limit switch port
} Motor;

Motor Xmot = {Xstep, 0, 0, Xlimit},
      Ymot = {Ystep, 0, 0, Ylimit};

//步进电机向前移动1步
void Motor_Step(Motor *m) {
  digitalWrite(m->stepPin, HIGH);
  delayMicroseconds(3);
  digitalWrite(m->stepPin, LOW);
  delayMicroseconds(2);
}

//步进电机在time(ms)指定的时间移动指定step步
void Motor_Move(Motor *m, int moveStep, int moveTime){
  int delayTime = (moveTime * 1000 - moveStep * 5) / moveStep;
  for (int i = 0; i < moveStep; i++){
    Motor_Step(m);
    delayMicroseconds(delayTime);
  }
}

//单个电机回到home
void Motor_Home(Motor *m) {
  if (digitalRead(m->limitSwitchPin) == LOW) {
    digitalWrite(m->dirPin, HIGH);
    Motor_Move(m, 50, 500);
  }
  
  digitalWrite(m->dirPin, LOW);
  while (digitalRead(m->limitSwitchPin) == HIGH){
    Motor_Step(m);
    delay(100);
  }
  m->location = 0;
}

//所有电机回到home
void HomeAll() {
  Motor_Home(&Xmot);
  Motor_Home(&Ymot);
}

void setup() {
  //  初始化串口
  Serial.begin(115200);
  Serial.println("Writer Driver start");
  //  初始化管脚
  pinMode(Xmot.stepPin, OUTPUT);
  pinMode(Ymot.stepPin, OUTPUT);
  pinMode(Xmot.limitSwitchPin, INPUT_PULLUP);
  pinMode(Ymot.limitSwitchPin, INPUT_PULLUP);
}

//从串口读取一行文本
void SerialReadLine(char buf[]) {
  int len = 0;
  while(1) {
    if (Serial.available() > 0) {
      buf[len] = Serial.read();
      if (buf[len++] == '\n')
        break;
    }
  }
  buf[len]= '\0';
}

void loop() {
  
  //读取一行命令
  char cmd[64];
  SerialReadLine(cmd);
  //Serial.println(cmd); //命令回显
  //解析命令头
  char cmdHead[16];//命令头
  sscanf(cmd, "%s", cmdHead);
  //命令头分类
  if(!strcmp(cmdHead, "HOME_ALL")){//电机return home
    Serial.println("Start home all motor");
    HomeAll();
    Serial.println("End home all motor");
  }
  else if(!strcmp(cmdHead, "PING")){//测试传输延迟
    Serial.println("PONG");
  }
}
