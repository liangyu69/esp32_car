// ==================== 高速寻迹小车 ====================
// 特点：直行高速，转弯自动降速

#include <Arduino.h>

// ==================== 电机引脚 ====================
#define LEFT_A   3
#define LEFT_B   2
#define RIGHT_A  10
#define RIGHT_B  12

// ==================== 传感器引脚 ====================
#define LEFT_SENSOR  7
#define RIGHT_SENSOR 8

// ==================== PWM 配置 ====================
#define PWM_FREQ 1000
#define PWM_RES 8

// 速度常量（关键调整）
#define SPEED_FAST 255     // 直行高速（0-255）
#define SPEED_TURN 40       // 转弯速度（慢，保证不冲出）
#define SPEED_SLOW 60

#define CH_LA 0
#define CH_LB 1
#define CH_RA 2
#define CH_RB 3

// ==================== 电机初始化 ====================
void motorInit() {
  ledcSetup(CH_LA, PWM_FREQ, PWM_RES);
  ledcSetup(CH_LB, PWM_FREQ, PWM_RES);
  ledcSetup(CH_RA, PWM_FREQ, PWM_RES);
  ledcSetup(CH_RB, PWM_FREQ, PWM_RES);
  
  ledcAttachPin(LEFT_A, CH_LA);
  ledcAttachPin(LEFT_B, CH_LB);
  ledcAttachPin(RIGHT_A, CH_RA);
  ledcAttachPin(RIGHT_B, CH_RB);
  
  stopCar();
}

// ==================== 移动函数 ====================
void forward() {
  ledcWrite(CH_LA, SPEED_SLOW);
  ledcWrite(CH_LB, 0);
  ledcWrite(CH_RA, SPEED_SLOW);
  ledcWrite(CH_RB, 0);
}

// 高速直行
void forwardFast() {
  ledcWrite(CH_LA, SPEED_FAST);
  ledcWrite(CH_LB, 0);
  ledcWrite(CH_RA, SPEED_FAST);
  ledcWrite(CH_RB, 0);
}

// 原地左转（降速）
void turnLeft() {
  ledcWrite(CH_LA, 0);
  ledcWrite(CH_LB, SPEED_TURN);
  ledcWrite(CH_RA, SPEED_TURN);
  ledcWrite(CH_RB, 0);
}

// 原地右转（降速）
void turnRight() {
  ledcWrite(CH_LA, SPEED_TURN);
  ledcWrite(CH_LB, 0);
  ledcWrite(CH_RA, 0);
  ledcWrite(CH_RB, SPEED_TURN);
}

void stopCar() {
  ledcWrite(CH_LA, 0);
  ledcWrite(CH_LB, 0);
  ledcWrite(CH_RA, 0);
  ledcWrite(CH_RB, 0);
}

// ==================== 传感器初始化 ====================
void sensorInit() {
  pinMode(LEFT_SENSOR, INPUT);
  pinMode(RIGHT_SENSOR, INPUT);
}

// ==================== 寻迹逻辑（高速版）====================
void lineFollow() {
  int left = digitalRead(LEFT_SENSOR);
  int right = digitalRead(RIGHT_SENSOR);
  
  if (left == 0 && right == 0) {
    // 全白 → 黑线在中间 → 高速直行
    forwardFast();
  }
  else if (left == 0 && right == 1) {
    // 右边压黑线 → 降速右转
    turnRight();
  }
  else if (left == 1 && right == 0) {
    // 左边压黑线 → 降速左转
    turnLeft();
  }
  else {
    // 全黑 → 粗线区域或终点 
    forward();
  }
}

// ==================== 初始化 ====================
void setup() {
  motorInit();
  sensorInit();
  stopCar();
  delay(500);
}

// ==================== 主循环 ====================
void loop() {
  lineFollow();
  delay(30);  // 循环间隔缩短，响应更快
}