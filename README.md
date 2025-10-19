md
# zkpass-esp32

This project aims to provide a secure and privacy-preserving identity verification solution using Zero-Knowledge Proofs (ZKPs) and an ESP32 microcontroller. It leverages a Chrome extension to initiate verification requests, a Python backend to communicate with the ESP32, and the ESP32 itself to perform the ZKP calculations.

## Key Features & Benefits

*   **Privacy-Preserving Verification:** Verifies age (or other attributes) without revealing the actual data.
*   **Secure Communication:** Utilizes ZKPs for secure and tamper-proof identity verification.
*   **ESP32 Integration:** Leverages the low-cost and readily available ESP32 microcontroller.
*   **Chrome Extension Interface:** Provides a user-friendly interface through a Chrome extension.
*   **Modular Design:** Enables easy customization and integration with different applications.

## Prerequisites & Dependencies

Before you begin, ensure you have the following installed:

*   **Python 3.6+:** Required for the backend server and configuration tool.
*   **pip:** Python package installer.
*   **ESP32 Development Environment:** Arduino IDE or PlatformIO with ESP32 support.
*   **Google Chrome:** For using the Chrome extension.
*   **pyserial:** Python library for serial communication.
*   **flask:** Python web framework for the backend server.
*   **flask-cors:** Python library for handling Cross-Origin Resource Sharing (CORS).

You can install the Python dependencies using:

```bash
pip install -r zkpass/requirements.txt
```

## Installation & Setup Instructions

Follow these steps to set up and run the project:

1.  **Clone the repository:**

    ```bash
    git clone https://github.com/SadraCoding/zkpass-esp32.git
    cd zkpass-esp32
    ```

2.  **ESP32 Setup:**

    *   Open the `zkpass/zkpass.ino` file in the Arduino IDE or PlatformIO.
    *   Install the necessary libraries for your ESP32 development environment.  Refer to your ESP32 board documentation for specific library requirements and installation instructions.
    *   Upload the code to your ESP32 board.

3.  **Backend Setup:**

    *   Navigate to the `zkpass-web/backend` directory.
    *   Install the required Python packages:
        ```bash
        pip install -r requirements.txt
        ```
    *   Run the `server.py` file:

    ```bash
    python server.py
    ```
        *   The script will ask you to provide the port to communicate with your ESP32 (e.g., `COM3` on Windows, `/dev/ttyUSB0` on Linux). Enter the appropriate port.

4.  **Chrome Extension Setup:**

    *   Navigate to `chrome://extensions` in your Chrome browser.
    *   Enable "Developer mode" in the top right corner.
    *   Click "Load unpacked" and select the `zkpass-web/chrome-extension` directory.
    *   The ZK-Pass extension should now be installed.

5. **Configuration**

* Use the `esp32/config.py` script to configure the ESP32. Run the script and follow the prompts.
```bash
python esp32/config.py
```

## Usage Examples & API Documentation

### Chrome Extension

The Chrome extension injects a button (or modifies an existing button) on web pages that require age verification.

*   Click the ZK-Pass extension icon.
*   Provide the necessary information (e.g., minimum age).
*   The extension will communicate with the backend server and the ESP32 to perform the ZKP verification.

### Backend API

The backend server exposes a single API endpoint:

*   `POST /verify-age`:
    *   Accepts a JSON payload with `minAge` (integer) parameter.
    *   Communicates with the ESP32 to perform the verification.
    *   Returns a JSON response indicating whether the age verification was successful.

Example using `curl`:

```bash
curl -X POST -H "Content-Type: application/json" -H "Origin: chrome-extension://YOUR_EXTENSION_ID" -d '{"minAge": 18}' http://localhost:5000/verify-age
```
Replace `YOUR_EXTENSION_ID` with the actual ID of the loaded chrome extension (found on `chrome://extensions`).
Replace `5000` with the port flask is running on if it's not the default.

## Configuration Options

*   **`ESP32_PORT` (Environment Variable):**  Specifies the serial port used to communicate with the ESP32. You can set this environment variable or provide the port at runtime when running `server.py`.
*   **`BAUD_RATE` (in `server.py` and `esp32/config.py`):**  Specifies the baud rate for serial communication with the ESP32. Default is 115200.
*   **`SERIAL_TIMEOUT` (in `server.py` and `esp32/config.py`):** Specifies the serial timeout in seconds. Default is 2 seconds.
*   **`CORS` origin:** The backend server needs to be configured to accept requests from the Chrome extension. The `CORS` origin in `server.py` is currently set to `chrome-extension://*`. You'll need to update this to the correct extension ID for production use.

## Contributing Guidelines

We welcome contributions to this project! To contribute:

1.  Fork the repository.
2.  Create a new branch for your feature or bug fix.
3.  Implement your changes and add tests.
4.  Submit a pull request with a clear description of your changes.

## License Information

This project is licensed under the MIT License - see the `LICENSE` file for details.

## Acknowledgments

*   [Placeholder for acknowledgments.]