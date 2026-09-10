/*
 * ZW111 指纹录入（ESP32 + HLK_fingerprint 库）
 * 接线：V_Touch/VCC→3V3, GND→GND, TX→D16, RX→D17
 * 用法：串口监视器 115200，输入编号(1~100)回车，按提示按两次同一根手指
 */
#include <HLK_fingerprint.h>

FingerprintModule fp(Serial1, 16, 17);   // 16=收(接模块TX)，17=发(接模块RX)

void setup() {
  Serial.begin(115200);
  if (!fp.begin()) {                     // 默认波特率57600，同时验证密码
    Serial.println("没连上！检查：VCC和V_Touch都接3V3、TX↔RX交叉");
    while (1) delay(1);
  }
  Serial.println("指纹模块连接成功！输入编号(1~100)回车，再按提示按两次手指");
}

void loop() {
  if (Serial.available()) {
    int id = Serial.parseInt();
    if (id < 1 || id > 100) {
      Serial.println("编号要在1~100之间");
      return;
    }
    Serial.print("录入编号 "); Serial.println(id);
    int16_t r = fp.enrollFingerprint(id);   // 同一根手指按两次
    if (r >= 0) Serial.println("录入成功！编号 " + String(r));
    else Serial.println("录入失败，重新来");
  }
}
