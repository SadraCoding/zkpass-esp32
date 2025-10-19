# zkPass: ESP32 Hardware Authentication Protocol

This repository contains the full implementation of the **zkPass protocol**, a hardware-based authentication system utilizing an **ESP32 microcontroller**, an **SSD1306 OLED display**, a **Python backend**, and a **client-side Chrome Extension**.

## Table of Contents

- [Prerequisites](#prerequisites)
- [Step 1: Hardware Assembly](#step-1-hardware-assembly)
- [Step 2: Program the ESP32](#step-2-program-the-esp32)
- [Step 3: Configure the ESP32](#step-3-configure-the-esp32)
- [Step 4: Run the Backend Server](#step-4-run-the-backend-server)
- [Step 5: Install the Chrome Extension](#step-5-install-the-chrome-extension)
- [Step 6: Launch the Demo](#step-6-launch-the-demo)
- [License](#license)

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

Install Python dependencies:
```bash
pip install -r ../../requirements.txt
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

   Navigate to the backend directory:
   ```bash
   cd zkpass-web/backend
   ```
   Start the server:
   ```bash
   cd zkpass-web/backend
   python server.py
   ```

   