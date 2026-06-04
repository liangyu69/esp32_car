#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>


// ==================== 电机引脚定义 ====================
#define LEFT_A   3
#define LEFT_B   2
#define RIGHT_A  10
#define RIGHT_B  12

// ==================== 蓝牙配置 ====================
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"


// ==================== PWM 配置 ====================
#define PWM_FREQ 1000
#define PWM_RES 8
#define SPEED_NORMAL 180
#define SPEED_TURN 150
#define SPEED_SLOW 100

// PWM 通道
#define CH_LA 0
#define CH_LB 1
#define CH_RA 2
#define CH_RB 3

// 全局变量：蓝牙特征指针
BLECharacteristic *pCharacteristic;


// ==================== 初始化电机 ====================
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
  ledcWrite(CH_LA, SPEED_NORMAL);
  ledcWrite(CH_LB, 0);
  ledcWrite(CH_RA, SPEED_NORMAL);
  ledcWrite(CH_RB, 0);
}

void back() {
  ledcWrite(CH_LA, 0);
  ledcWrite(CH_LB, SPEED_NORMAL);
  ledcWrite(CH_RA, 0);
  ledcWrite(CH_RB, SPEED_NORMAL);
}

void turnLeft() {
  ledcWrite(CH_LA, SPEED_SLOW);    // 左轮慢（不反转）
  ledcWrite(CH_LB, 0);
  ledcWrite(CH_RA, SPEED_NORMAL);  // 右轮快
  ledcWrite(CH_RB, 0);
}

void turnRight() {
  ledcWrite(CH_LA, SPEED_NORMAL);  // 左轮快
  ledcWrite(CH_LB, 0);
  ledcWrite(CH_RA, SPEED_SLOW);    // 右轮慢
  ledcWrite(CH_RB, 0);
}

void stopCar() {
  ledcWrite(CH_LA, 0);
  ledcWrite(CH_LB, 0);
  ledcWrite(CH_RA, 0);
  ledcWrite(CH_RB, 0);
}



// ==================== 蓝牙回调类 ====================
class MyCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    
    if (value.length() > 0) {
      int cmd = value[0] - '0';
      
      switch(cmd) {
        case 1: forward(); break;
        case 2: back(); break;
        case 3: turnLeft(); break;
        case 4: turnRight(); break;
        case 5: stopCar(); break;
      }
    }
  }
};

// ==================== 蓝牙初始化（封装成一个函数）====================
void bleInit() {
  // 1. 初始化蓝牙设备，设置广播名称
  BLEDevice::init("ESP32_Car");
  
  // 2. 创建蓝牙服务器
  BLEServer *pServer = BLEDevice::createServer();
  
  // 3. 创建服务（用定义好的 UUID）
  BLEService *pService = pServer->createService(SERVICE_UUID);
  
  // 4. 创建特征（用定义好的 UUID，设置可读可写属性）
  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_WRITE
  );
  
  // 5. 设置回调（收到指令时自动调用 MyCallbacks 里的 onWrite）
  pCharacteristic->setCallbacks(new MyCallbacks());
  
  // 6. 设置初始值
  pCharacteristic->setValue("Ready");
  
  // 7. 启动服务
  pService->start();
  
  // 8. 开始广播（让手机能搜索到）
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
}

// ==================== 初始化 ====================
void setup() {
  motorInit();
  bleInit();                 // 添加蓝牙初始化
  stopCar();
}

// ==================== 主循环 ====================
void loop() {
  
  delay(50);
}