# zkPass: ESP32 Hardware Authentication Protocol

This repository contains the full implementation of the **zkPass protocol**, a hardware-based authentication system utilizing an **ESP32 microcontroller**, an **SSD1306 OLED display**, a **Python backend**, and a **client-side Chrome Extension**.

## Table of Contents

- [Step 1: Hardware Assembly](#step-1-hardware-assembly)
- [Step 2: Program the ESP32](#step-2-program-the-esp32)
- [Step 3: Configure the ESP32](#step-3-configure-the-esp32)
- [Step 4: Run the Backend Server](#step-4-run-the-backend-server)
- [Step 5: Install the Chrome Extension](#step-5-install-the-chrome-extension)
- [Step 6: Launch the Demo](#step-6-launch-the-demo)
- [Security & Privacy Overview](#security--privacy-overview)
- [Author & Contact](#author--contact)
- [License](#license)

---
### Demo Video

https://github.com/user-attachments/assets/480c70c2-6ea1-4d93-a931-5ff00d78fa48

---
### Hardware

- ESP32 Development Board
- SSD1306 0.96" I2C OLED Display
- Soldering Iron & Solder
- Breadboard and Jumper Wires (or perfboard for a permanent connection)
- Micro-USB Cable

### Software

- Arduino IDE
- Python 3.8+
- Google Chrome
- Git

## Step 1: Hardware Assembly

You need to connect the SSD1306 OLED display to the ESP32. This requires soldering pins to the display if it doesn't already have them.

### Soldering:

1. Place the 4-pin header through the holes on the SSD1306 module.
2. Heat the junction of a pin and its copper pad with the soldering iron for a few seconds.
3. Touch the solder to the heated junction until it flows smoothly around the pin.
4. Remove the solder, then the iron. The joint should be shiny and cone-shaped.
5. Repeat for all four pins.

### Wiring:

Connect the components using the I2C interface as follows:

| SSD1306 Pin | ESP32 Pin | Description        |
|-------------|-----------|--------------------|
| VCC         | 3V3       | 3.3V Power         |
| GND         | GND       | Ground             |
| SCL         | GPIO 22   | Serial Clock       |
| SDA         | GPIO 21   | Serial Data        |

## Step 2: Program the ESP32

### Arduino IDE Setup

1. **Install ESP32 Board Support:**
   - In the Arduino IDE, go to **File > Preferences**.
   - Add this URL to **Additional Boards Manager URLs**:
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
   - Go to **Tools > Board > Boards Manager**, search for `"esp32"`, and install the package by **Espressif Systems**.

2. **Install Libraries:**
   - Go to **Sketch > Include Library > Manage Libraries**.
   - Search for and install both:
     - **Adafruit GFX Library**
     - **Adafruit SSD1306**

3. **Open the Sketch:**
   - Open the file `esp32/zkpass/zkpass.ino` in the Arduino IDE.

### Code Modification

> **IMPORTANT**: Before uploading, you **must update the date** in the `zkpass.ino` file.  
> Find the line that sets the date and change it to the **current date**. This is crucial for the protocol to work correctly.

### Upload the Code

1. Connect your ESP32 to your computer.
2. Select your board (e.g., **DOIT ESP32 DEVKIT V1**) under **Tools > Board > ESP32 Arduino**.
3. Select the correct **Port** under **Tools > Port**.
4. Click the **"Upload"** button (right arrow icon).

## Step 3: Configure the ESP32

Navigate to the backend directory And Install Python dependencies
```bash
pip install -r requirements.txt
```


This step sends the necessary configuration from your computer to the freshly programmed ESP32.
1. Navigate to the project directory:
   ```bash
   cd esp32
   ```
   Run the configuration script
   ```bash
   python config.py
   ```
Follow any on-screen prompts to complete the configuration.

---

## Step 4: Run the Backend Server

The backend server manages communication between the ESP32 and the web client.

   Navigate to the backend directory And Start the server
   ```bash
   cd zkpass-web/backend
   python server.py
   ```
   "Keep this terminal window open. The server is now running and listening for requests."

---


## Step 5: Install the Chrome Extension

The extension allows the web demo to interface with the backend.

1. Open **Google Chrome** and navigate to:

2. Enable the **"Developer mode"** toggle in the top-right corner.

3. Click the **"Load unpacked"** button.

4. In the file dialog, select the `zkpass-web/chrome-extension` folder from this repository.

> The zkPass extension should now appear in your list of extensions.


## Step 6: Launch the Demo

You can now test the full system.

1. Make sure the backend server from **Step 4** is still running.
2. Open the file `zkpass-web/demo/index.html` in **Google Chrome**.
3. You should now be able to interact with the demo page and see the zkPass protocol in action!


## Security & Privacy Overview

**Why zkPass Matters**

zkPass leverages **Zero-Knowledge Proofs (ZKPs)** to enable strong, privacy-preserving authentication. With ZKPs, a user can prove they possess valid credentials **without revealing the credentials themselves**, eliminating risks of phishing, replay attacks, and credential theft.

**End-to-End Hardware Security**

- All sensitive cryptographic operations(including key generation, AES-256 encryption, and ZKP computations)are performed **entirely within the ESP32’s secure environment**.
- **No private data ever leaves the device**. Credentials, keys, and intermediate values remain isolated in the ESP32’s memory.
- Communication with the backend and browser is limited to **public challenges and zero-knowledge responses**, ensuring zero leakage of personal or identifying information.
- The system uses **AES-256** for internal data protection, aligned with modern cryptographic best practices.

This design ensures that even if the host computer or network is compromised, your identity remains safe.

## Author & Contact

Developed with care by **Sadra Milani Moghaddam**.  
🌐 Learn more at: [https://sadramilani.ir](https://sadramilani.ir)

## License

This project is licensed under the terms of the [LICENSE](LICENSE) file.