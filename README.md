# 🛡️ ZK-Pass: Zero-Knowledge Age Verification System

> **Prove you're old enough — without revealing who you are.**

ZK-Pass is a hardware-based identity wallet built on ESP32 with OLED feedback. It enables **privacy-preserving, offline age verification** using AES-128 encryption and PIN-based access. Designed for real-world use in retail, gaming, or adult content platforms.

---

## 🔧 Step 1: Solder SSD1306 OLED to ESP32

The OLED communicates via **I²C**, requiring only 4 connections. Soldering is recommended for permanent builds.

### 🔌 Wiring (SSD1306 → ESP32)

| SSD1306 | ESP32     |
|---------|-----------|
| VCC     | **3.3V**  |
| GND     | **GND**   |
| SCL     | **GPIO 22** |
| SDA     | **GPIO 21** |

> ⚠️ **Do NOT connect VCC to 5V** — SSD1306 is 3.3V logic only.

### 🔧 Soldering Instructions

1. Solder header pins to your SSD1306 module (if not pre-soldered).
2. Solder thin insulated wires from OLED to ESP32:
   - VCC → 3.3V
   - GND → GND
   - SCL → GPIO 22
   - SDA → GPIO 21
3. Insulate joints with heat-shrink tubing.
4. Test with Arduino IDE:  
   `File > Examples > Adafruit SSD1306 > ssd1306_128x64_i2c`

---

## 💻 Step 2: Program ESP32 with `zkpass.ino`

### 📥 Requirements
- Arduino IDE with ESP32 board support
- Install libraries:
  - `Adafruit SSD1306`
  - `Adafruit GFX Library`
  - `AES by rweather`

### ⚠️ Critical: Update Current Date in Code

**Before uploading**, locate and update these lines in `zkpass.ino`:

```cpp
// 🔄 UPDATE THESE VALUES TO CURRENT DATE
int current_year = 2025;    // Change to current year
int current_month = 10;     // Change to current month (1-12)
int current_day = 17;       // Change to current day (1-31)
