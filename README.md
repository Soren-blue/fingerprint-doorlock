# 指纹门锁 · ESP32 + HLK-ZW111 + SG90

用 ESP32 驱动海凌科 ZW111 指纹模块和 SG90 舵机做的一套指纹开锁装置：手指按上去 → 匹配成功 → 舵机拉线开门，2 秒后舵机回位。

> A cheap DIY fingerprint door lock: **ESP32 + HLK-ZW111 fingerprint module + SG90 servo**, about 40–60 CNY total. Full build guide (Chinese) in [`docs/`](docs/).

## 特点

- 总成本 40~60 元，零件都是常见货
- 不改门锁结构：舵机拉线开锁，拆掉即恢复原样
- 指纹模块和舵机由电源模块独立供电（3.3V / 5V），避免舵机转动时把板子拉垮
- 断电后仍能用手拨锁钮开锁，不影响逃生
- 自带两套程序（录指纹 / 开门），桌上实测通过

## 硬件清单

| 零件 | 型号 / 搜索词 | 参考价 |
|---|---|---|
| 开发板 | ESP32 开发板 CH340C Type-C（WROOM） | 15~25 元 |
| 指纹模块 | HLK-ZW111（海凌科）**需带连接线** | 5~20 元 |
| 舵机 | SG90 9g 180 度 | 5~10 元 |
| 杜邦线 | 套盒，另需 **1 根以上"一母对二公"**（把 3V3 分给 VCC 和 V_Touch） | 5~10 元 |
| 数据线 | Type-C，**必须能传数据** | — |
| 电源模块 | 能同时输出 5V 和 3.3V（MB102 面包板电源模块或 5V/3.3V 降压电源板） | 5~10 元 |
| 连接件 | 大号缝衣针 + 缝衣线（也可用 0.5mm 细钢丝） | 家里有就不用买 |
| 固定 | 3M VHB 双面胶 + 小扎带 | 10 元左右 |

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

电源模块与 3V3 分线：

- 电源模块输入接 5V 电源（充电头）：**5V 输出 → 舵机红线**，**3.3V 输出 → 指纹模块的 VCC 和 V_Touch**
- VCC 和 V_Touch 要接同一个 3V3：用一根 **一母对二公** 杜邦线，母头插 3V3，两个公头分别插 VCC 和 V_Touch
- 不用电源模块时，直接从 ESP32 的 3V3 引脚引，同样用"一母对二公"分成两根
- **所有 GND 必须共地**：ESP32、电源模块、舵机、指纹模块的 GND 连在一起
- 电源模块若要给板子供电，只接板子的 5V / VIN，**不要接 3V3**

## 软件依赖

- Arduino IDE 2.x
- esp32 板卡支持（开发板管理器搜索 `esp32`，by Espressif Systems）
- 库：`HLK_fingerprint`（作者 GavinnnTann）、`ESP32Servo`

不要装 Adafruit Fingerprint Sensor Library——它是 AS608/R307 那套协议，不兼容 ZW111 的 EF-01 协议。

## 快速开始

1. **录指纹**：上传 [`firmware/enroll_fingerprint`](firmware/enroll_fingerprint)，串口监视器 115200，输入编号（1~100）回车，按提示同一根手指按两次。同一根手指建议录两个编号，识别率更高。
2. **开门**：上传 [`firmware/fingerprint_doorlock`](firmware/fingerprint_doorlock)。成功标准：手指按模块 → 板载蓝灯亮、舵机转 110°、2 秒后回位。
3. **装门**：先在桌上把电子部分调通再装门。舵机粘在锁钮旁，用缝衣针穿线把舵机臂外孔和锁钮连起来；`openAngle` 按行程微调（110 → 120 → 130）。

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
- 拔掉电后必须能用手轻松拨开锁钮
- 这是个人学习项目，请自行评估在真实门锁上的风险

## 许可证

[MIT](LICENSE)
