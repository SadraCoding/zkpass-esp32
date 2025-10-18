import os
import serial
import time
from flask import Flask, request, jsonify
from flask_cors import CORS
import threading

PORT = input("Enter ESP32 port (e.g COM3): ")
ESP32_PORT = os.environ.get("ESP32_PORT", PORT) 
BAUD_RATE = 115200
SERIAL_TIMEOUT = 2

app = Flask(__name__)
CORS(app, resources={r"/verify-age": {"origins": "chrome-extension://*"}})


serial_lock = threading.Lock()
ser = None

def get_serial_connection():
    global ser
    if ser and ser.is_open:
        return ser
    try:
        ser = serial.Serial(ESP32_PORT, BAUD_RATE, timeout=SERIAL_TIMEOUT)
        time.sleep(2)
        print(f"Successfully connected to ESP32 on {ESP32_PORT}")
        return ser
    except serial.SerialException as e:
        print(f"Error: Could not open serial port {ESP32_PORT}. {e}")
        print("Please ensure the ESP32 is connected and the ESP32_PORT is set correctly.")
        ser = None
        return None

get_serial_connection()

@app.route('/verify-age', methods=['POST'])
def verify_age():
    data = request.get_json()
    if not data or 'min_age' not in data or 'pin' not in data:
        return jsonify({"error": "Missing min_age or pin"}), 400

    min_age = data['min_age']
    pin = data['pin']


    if not isinstance(min_age, int) or not (13 <= min_age <= 100):
        return jsonify({"error": "Invalid minimum age"}), 400
    if not isinstance(pin, str) or not (4 <= len(pin) <= 8 and pin.isdigit()):
        return jsonify({"error": "Invalid PIN format"}), 400

    command = f"CHK_AGE:{min_age},{pin}\n"

    with serial_lock:
        ser_conn = get_serial_connection()
        if not ser_conn:
            return jsonify({"error": "ESP32 not connected"}), 503

        try:
            ser_conn.flushInput()
            ser_conn.write(command.encode('utf-8'))
            print(f"Sent to ESP32: {command.strip()}")
            response = ser_conn.readline().decode('utf-8').strip()
            print(f"Received from ESP32: {response}")

            if response == "VERIFIED":
                return jsonify({"status": "VERIFIED"})
            elif response == "DENIED":
                return jsonify({"status": "DENIED"})
            elif not response:
                return jsonify({"error": "No response from ESP32 (timeout)"}), 504
            else:
                return jsonify({"error": "Invalid response from ESP32", "details": response}), 500

        except serial.SerialException as e:
            print(f"Serial communication error: {e}")
            if ser_conn:
                ser_conn.close()
            global ser
            ser = None
            return jsonify({"error": "Serial communication failed"}), 500
        except Exception as e:
            print(f"An unexpected error occurred: {e}")
            return jsonify({"error": "An internal server error occurred"}), 500


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
