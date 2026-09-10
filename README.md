# 指纹门锁 · ESP32 + HLK-ZW111 + SG90

用 ESP32 驱动海凌科 ZW111 指纹模块和 SG90 舵机做的一套指纹开锁装置：手指按上去 → 匹配成功 → 舵机拉线开门，2 秒后回位，橡皮筋把锁钮拉回上锁位。

> A cheap DIY fingerprint door lock: **ESP32 + HLK-ZW111 fingerprint module + SG90 servo**, about 40–60 CNY total. Full build guide (Chinese) in [`docs/`](docs/).

## 特点

- 总成本 40~60 元，零件都是常见货
- 不改门锁结构：舵机拉线开锁 + 橡皮筋回位，拆掉即恢复原样
- 断电后仍能用手拨锁钮开锁，不影响逃生
- 自带两套程序（录指纹 / 开门），桌上实测通过

## 硬件清单

| 零件 | 型号 / 搜索词 | 参考价 |
|---|---|---|
| 开发板 | ESP32 开发板 CH340C Type-C（WROOM） | 15~25 元 |
| 指纹模块 | HLK-ZW111（海凌科）**需带连接线** | 5~20 元 |
| 舵机 | SG90 9g 180 度 | 5~10 元 |
| 杜邦线 | 套盒（公对公 / 公对母 / 母对母） | 5~10 元 |
| 数据线 | Type-C，**必须能传数据** | — |
| 拉线 | 风筝线或 0.5mm 细钢丝 1 米 | 几元 |
| 回位 | 橡皮筋 + 3M VHB 双面胶 + 小扎带 | 10 元左右 |

## 接线

指纹模块（6 根，V_Touch 最容易漏）：

| ZW111 引脚 | 接 ESP32 | 说明 |
|---|---|---|
| V_Touch | 3V3 | 触摸供电，必须接 |
| VCC | 3V3 | 只吃 3.3V，**不能接 5V** |
| GND | GND | |
| TX | D16 | 与板子 RX 交叉 |
| RX | D17 | 与板子 TX 交叉 |
| TOUCH_OUT | D4 | 可选，程序用不到 |

舵机：棕/黑 → GND，红 → 5V，橙/黄 → D13。

## 软件依赖

- Arduino IDE 2.x
- esp32 板卡支持（开发板管理器搜索 `esp32`，by Espressif Systems）
- 库：`HLK_fingerprint`（作者 GavinnnTann）、`ESP32Servo`

不要装 Adafruit Fingerprint Sensor Library——它是 AS608/R307 那套协议，不兼容 ZW111 的 EF-01 协议。

## 快速开始

1. **录指纹**：上传 [`firmware/enroll_fingerprint`](firmware/enroll_fingerprint)，串口监视器 115200，输入编号（1~100）回车，按提示同一根手指按两次。同一根手指建议录两个编号，识别率更高。
2. **开门**：上传 [`firmware/fingerprint_doorlock`](firmware/fingerprint_doorlock)。成功标准：手指按模块 → 板载蓝灯亮、舵机转 110°、2 秒后回位。
3. **装门**：先在桌上把电子部分调通再装门。舵机粘在锁钮旁，线绑舵机臂外孔与锁钮，橡皮筋朝上锁位拉回；`openAngle` 按行程微调（110 → 120 → 130）。

完整步骤见 [`docs/ZW111指纹门锁_最终方案.md`](docs/ZW111指纹门锁_最终方案.md)；完全没接触过开发板的话看 [`docs/零基础操作手册.md`](docs/零基础操作手册.md)。

## 目录结构

```
firmware/
  enroll_fingerprint/   录指纹程序
  fingerprint_doorlock/ 开门程序
docs/
  ZW111指纹门锁_最终方案.md   完整方案（买件→接线→装软件→装机）
  零基础操作手册.md           面向零基础的分步手册
```

## 踩过的坑

- ZW111 是 6 针，V_Touch 不接 3V3 直接不工作；VCC 和 V_Touch 都接 3V3，绝不能接 5V
- 板子只有一个 3V3 时，用"一母分二公"杜邦线把 VCC 和 V_Touch 并联
- Adafruit 指纹库不兼容 ZW111，必须用 HLK_fingerprint
- 板子印着 WROVER（不是 WROOM）时，D16/D17 被 PSRAM 占用：把 `fp(Serial1, 16, 17)` 改成 `fp(Serial1, 26, 27)`，接线改为模块 TX→D26、RX→D27
- 上传失败多半是 USB 线只能供电不能传数据；按住 BOOT 键再点上传也能救
- `matchFingerprint` 返回 -1（指纹不对，闪灯 3 下）和 -2（没人按手指，待机）要分开处理，否则待机时会一直闪

## 安全提示

- 装门前先跟宿管 / 家长确认不影响消防逃生
- 拔掉电后必须能用手轻松拨开锁钮；做不到就把橡皮筋换松一点
- 这是个人学习项目，请自行评估在真实门锁上的风险

## 许可证

[MIT](LICENSE)
