// ==================== 避障迷宫小车（右手法则） ====================
// 传感器：有障碍 = 0，无障碍 = 1

#include <Arduino.h>

// ==================== 电机引脚 ====================
#define LEFT_A   3
#define LEFT_B   2
#define RIGHT_A  10
#define RIGHT_B  12

// ==================== 传感器引脚 ====================
#define SENSOR_L  6
#define SENSOR_M  8
#define SENSOR_R  18

// ==================== 电机配置 ====================
#define PWM_FREQ 1000
#define PWM_RES 8
#define SPEED_NORMAL 180      // 提高直行速度
#define SPEED_TURN 180        // 提高转弯速度
#define SPEED_TURN_BACK 150   // 倒车速度
#define BACK_DELAY 150
#define ADJUST_DELAY 60      // 微调转弯时间增加
#define TURN_DELAY 600        // 正常转弯时间增加
#define LEFT_WALL_TIMEOUT 1000

#define CH_LA 0
#define CH_LB 1
#define CH_RA 2
#define CH_RB 3

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

void forward() {
  ledcWrite(CH_LA, SPEED_NORMAL);
  ledcWrite(CH_LB, 0);
  ledcWrite(CH_RA, SPEED_NORMAL);
  ledcWrite(CH_RB, 0);
}

void forwardRight() {
  // 右倾前进：左轮快，右轮慢
  ledcWrite(CH_LA, SPEED_NORMAL );
  ledcWrite(CH_LB, 0);
  ledcWrite(CH_RA, SPEED_NORMAL - 150);
  ledcWrite(CH_RB, 0);
}

void backUp() {
  ledcWrite(CH_LA, 0);
  ledcWrite(CH_LB, SPEED_TURN_BACK);
  ledcWrite(CH_RA, 0);
  ledcWrite(CH_RB, SPEED_TURN_BACK);
  delay(BACK_DELAY);
  stopCar();
}

void adjustLeft() {
  // 微调左转（直接左转，不后退）
  ledcWrite(CH_LA, 0);
  ledcWrite(CH_LB, SPEED_TURN);
  ledcWrite(CH_RA, SPEED_TURN);
  ledcWrite(CH_RB, 0);
  delay(ADJUST_DELAY);
  stopCar();
}

void turnLeft() {
  backUp();
  ledcWrite(CH_LA, 0);
  ledcWrite(CH_LB, SPEED_TURN);
  ledcWrite(CH_RA, SPEED_TURN);
  ledcWrite(CH_RB, 0);
  delay(TURN_DELAY);
  stopCar();
}

void turnRight() {
  ledcWrite(CH_LA, SPEED_TURN);
  ledcWrite(CH_LB, 0);
  ledcWrite(CH_RA, 0);
  ledcWrite(CH_RB, SPEED_TURN);
  delay(TURN_DELAY);
  stopCar();
}

void turnBack() {
  stopCar();
  delay(100);
  backUp();
  delay(50);
  // 右转180度
  ledcWrite(CH_LA, SPEED_TURN);
  ledcWrite(CH_LB, 0);
  ledcWrite(CH_RA, 0);
  ledcWrite(CH_RB, SPEED_TURN);
  delay(800);
  stopCar();
  delay(100);
}

void stopCar() {
  ledcWrite(CH_LA, 0);
  ledcWrite(CH_LB, 0);
  ledcWrite(CH_RA, 0);
  ledcWrite(CH_RB, 0);
}

void sensorInit() {
  pinMode(SENSOR_L, INPUT);
  pinMode(SENSOR_M, INPUT);
  pinMode(SENSOR_R, INPUT);
}

void avoidObstacle() {
  static unsigned long leftWallStartTime = 0;
  static bool leftWallDetected = false;
  
  int L = digitalRead(SENSOR_L);
  int M = digitalRead(SENSOR_M);
  int R = digitalRead(SENSOR_R);
  
  // ===== 规则4：前方遇到障碍物 → 后退后左转 =====
  if (M == 0) {
    turnLeft();
    leftWallDetected = false;
    return;
  }
  
  // ===== 规则3：左侧存在障碍物超过1000ms后右转 =====
  if (L == 0) {
    if (!leftWallDetected) {
      leftWallDetected = true;
      leftWallStartTime = millis();
    }
    
    if (millis() - leftWallStartTime >= LEFT_WALL_TIMEOUT) {
      turnRight();
      leftWallDetected = false;
      return;
    }
    
    forward();
    return;
  } else {
    leftWallDetected = false;
  }
  
  // ===== 规则2：右侧存在障碍物 → 直接左转微调 =====
  if (R == 0) {
    adjustLeft();    // 直接左转微调，不后退
    forward();       // 然后直行
    return;
  }
  
  // ===== 规则1：无感应物时优先右倾寻找障碍物 =====
  if (L == 1 && M == 1 && R == 1) {
    forwardRight();
    return;
  }
  
  // ===== 默认：直行 =====
  forward();
}

void setup() {
  motorInit();
  sensorInit();
  stopCar();
  delay(500);
}

void loop() {
  avoidObstacle();
  delay(50);
}
