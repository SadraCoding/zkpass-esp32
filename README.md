# 🔐 zkPass Hardware Identity System (ESP32 Wallet)

### Privacy-Preserving Access Control for Adult Platforms and Beyond  
**Author:** [Sadra Milani Moghaddam](https://sadramilani.ir)

---

## 🧠 Overview

`zkPass` is a **Zero-Knowledge Proof (ZKP)** based identity verification system that allows users to **prove their age and identity** without revealing personal data.  
It is designed especially for **privacy-first authentication** on sensitive platforms — such as adult websites — using a **hardware wallet** built on the **ESP32** microcontroller with an **SSD1306 OLED display**.

This project provides:
- 🪪 **Hardware-based self-sovereign identity**
- 🔒 **ZKP (Zero-Knowledge Proof)** verification for privacy
- 🌐 **Browser extension** integration for seamless authentication
- ⚙️ **Python backend** and **web demo**

---

## ⚙️ Hardware Setup

### 🧰 Requirements
- **ESP32** development board  
- **SSD1306 OLED display** (I²C version)  
- Jumper wires & soldering tools  
- USB cable for programming  

### 🔌 Wiring and Soldering
Connect your components as follows:

| SSD1306 Pin | ESP32 Pin | Description         |
|--------------|------------|--------------------|
| VCC          | 3.3V       | Power              |
| GND          | GND        | Ground             |
| SCL          | GPIO 22    | I²C Clock          |
| SDA          | GPIO 21    | I²C Data           |

> ⚠️ **Tip:** Solder the pins firmly and ensure no shorts between SDA/SCL lines.  
> Use I²C scan code (optional) to verify your OLED is detected correctly before proceeding.

---

## 💻 Firmware Installation

### 1. Open the Code
Navigate to the main firmware file:
esp32/zkpass/zkpass.ino

Open it in **Arduino IDE** (or PlatformIO).

### 2. Update the Date Variables
Edit the following lines to reflect the current date:

```cpp
int current_year = 2025;    // Change to current year
int current_month = 10;     // Change to current month (1-12)
int current_day = 17;       // Change to current day (1-31)
```

### 3. Select Board and Port
In Arduino IDE:
Go to Tools → Board → ESP32 Dev Module
Choose the correct COM port
Press Upload

## Configuration

### 1. Create a Virtual Environment
python3 -m venv venv
source venv/bin/activate    # (On Windows: venv\Scripts\activate)

### 2. Install Requirements
pip install -r requirements.txt

### 3. Configure Your Identity
cd esp32
Run config.py and enter parameters in the terminal
This data will be encrypted using AES and never will be shared — it’s only used locally to generate ZK proofs.

## 🌐 Web Extension Setup

### 1. Install zkPass Chrome Extension
Open Chrome → chrome://extensions/
Enable Developer mode
Click Load unpacked
Select the folder:
zkpass-web/chrome-extension/
This installs the zkPass extension in debug mode.

## Run the Backend