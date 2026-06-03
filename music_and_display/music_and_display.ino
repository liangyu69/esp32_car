
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ==================== 引脚定义 ====================
#define BUZZER_PIN 13
#define SDA_PIN 4
#define SCL_PIN 5

// ==================== OLED 配置 ====================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ==================== 显示参数 ====================
int groupID = 1;              // 组号
float ratedVoltage = 7.74;    // 电压 (V)
float ratedCurrent = 113.42;  // 电流 (mA)
float ratedPower = 0.88;      // 功率 (W)

// ==================== 音符频率 ====================
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523

// ==================== 小星星旋律 ====================
int melody[] = {
  NOTE_C4, NOTE_C4, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_A4, NOTE_G4,
  NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_C4,
  NOTE_G4, NOTE_G4, NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4,
  NOTE_C4, NOTE_C4, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_A4, NOTE_G4,
  NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_C4
};

int durations[] = {
  400, 400, 400, 400, 400, 400, 800,
  400, 400, 400, 400, 400, 400, 800,
  400, 400, 400, 400, 400, 400, 800,
  400, 400, 400, 400, 400, 400, 800,
  400, 400, 400, 400, 400, 400, 800
};

int notesCount = sizeof(melody) / sizeof(melody[0]);

// ==================== 蜂鸣器初始化 ====================
void buzzerInit() {
  pinMode(BUZZER_PIN, OUTPUT);
}

// 播放单个音符
void playTone(int frequency, int duration) {
  if (frequency == 0) {
    delay(duration);
  } else {
    tone(BUZZER_PIN, frequency);
    delay(duration);
    noTone(BUZZER_PIN);
  }
  delay(50);
}

// 播放小星星
void playTwinkleStar() {
  for (int i = 0; i < notesCount; i++) {
    playTone(melody[i], durations[i]);
  }
}

// ==================== OLED 初始化 ====================
void oledInit() {
  Wire.begin(SDA_PIN, SCL_PIN);
  
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while(1) delay(100);
  }
  
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  
  // 第1行：组号（大字体）
  display.setTextSize(2);
  display.setCursor(20, 0);
  display.print("Group:");
  display.print(groupID);
  
  // 第2行：电压（小字体）
  display.setTextSize(1);
  display.setCursor(0, 20);
  display.print("U:");
  display.print(ratedVoltage, 2);
  display.print(" V");
  
  // 第3行：电流（小字体）
  display.setCursor(0, 32);
  display.print("I:");
  display.print(ratedCurrent, 2);
  display.print(" mA");
  
  // 第4行：功率（小字体）
  display.setCursor(0, 44);
  display.print("P:");
  display.print(ratedPower, 2);
  display.print(" W");
  
  display.display();
}

void setup() {
  buzzerInit();
  oledInit();

}

void loop() {
  playTwinkleStar();
  delay(5000);  // 播完一次等5秒再重复

}
