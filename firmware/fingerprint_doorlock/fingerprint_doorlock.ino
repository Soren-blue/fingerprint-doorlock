/*
 * ZW111 指纹开门（ESP32 + HLK_fingerprint + ESP32Servo）
 * 接线：指纹模块 V_Touch/VCC→3V3, GND→GND, TX→D16, RX→D17
 *       舵机 棕/黑→GND, 红→5V, 橙/黄→D13
 * 行为：匹配成功 → LED亮、舵机转110°保持2秒回位；失败 → LED闪3下
 */
#include <HLK_fingerprint.h>
#include <ESP32Servo.h>

FingerprintModule fp(Serial1, 16, 17);
Servo myServo;

int servoPin = 13;
int openAngle = 110;   // 开锁角度，按门锁行程微调(110→120→130)
int idleAngle = 0;     // 待机角度

void setup() {
  Serial.begin(115200);
  pinMode(2, OUTPUT);              // 板载LED
  myServo.attach(servoPin);
  myServo.write(idleAngle);
  if (fp.begin()) Serial.println("指纹模块OK");
  else Serial.println("指纹模块没连上！");
}

void loop() {
  uint16_t score;
  int16_t id = fp.matchFingerprint(score);   // >=0=匹配；-1=指纹不对；-2=没人按
  if (id >= 0) {
    Serial.print("匹配成功 ID="); Serial.println(id);
    digitalWrite(2, HIGH);
    myServo.write(openAngle);
    delay(2000);
    myServo.write(idleAngle);
    digitalWrite(2, LOW);
    delay(1000);
  } else if (id == -1) {
    Serial.println("指纹不匹配");
    for (int i = 0; i < 3; i++) {
      digitalWrite(2, HIGH);
      delay(100);
      digitalWrite(2, LOW);
      delay(100);
    }
  }
  // id == -2：没人按手指，保持待机不闪灯
  delay(100);
}
