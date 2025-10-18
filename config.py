"""
ZK-Pass Identity Config Tool
"""

import serial
import serial.tools.list_ports
import re
import sys
import time
from datetime import datetime


class ZKPassConfig:
    BAUD_RATE = 115200
    SERIAL_TIMEOUT = 3
    COMMAND_TIMEOUT = 2.0
    
    def __init__(self):
        self.esp32_port = None
        self.serial_connection = None
    
    def detect_esp32_port(self):
        ports = serial.tools.list_ports.comports()
        esp32_identifiers = ['CP210', 'CH340', 'USB-SERIAL', 'Silicon Labs', 'ESP32', 'Espressif']
        
        for port in ports:
            port_info = f"{port.device} {port.description} {port.hwid}".upper()
            if any(identifier in port_info for identifier in esp32_identifiers):
                return port.device
        return None
    
    def validate_national_id(self, national_id):
        if not national_id:
            return False
        return national_id.isdigit()
    
    def validate_date(self, date_str):
        try:
            parsed_date = datetime.strptime(date_str, '%Y-%m-%d')
            current_year = datetime.now().year
            return 1900 <= parsed_date.year <= current_year
        except ValueError:
            return False
    
    def validate_name(self, name):
        clean_name = name.strip()
        if len(clean_name) < 2:
            return False
        allowed_chars = clean_name.replace(' ', '').replace('-', '')
        return allowed_chars.isalpha()
    
    def send_command(self, command, timeout=None):
        if timeout is None:
            timeout = self.COMMAND_TIMEOUT
        
        try:
            if not self.serial_connection or not self.serial_connection.is_open:
                return "ERROR: Serial connection not established"
            
            self.serial_connection.write(f"{command}\n".encode('utf-8'))
            self.serial_connection.flush()
            time.sleep(timeout)
            
            response = ""
            start_time = time.time()
            while time.time() - start_time < 3.0:
                if self.serial_connection.in_waiting:
                    line = self.serial_connection.readline()
                    if line:
                        response = line.decode('utf-8', errors='ignore').strip()
                        break
                time.sleep(0.1)
            
            return response if response else "ERROR: No response from device"
            
        except Exception as e:
            return f"ERROR: Serial communication failed - {str(e)}"
    
    def connect_to_device(self):
        try:
            self.serial_connection = serial.Serial(
                self.esp32_port, 
                self.BAUD_RATE, 
                timeout=self.SERIAL_TIMEOUT
            )
            time.sleep(2)
            return True
        except serial.SerialException as e:
            print(f" Failed to connect to {self.esp32_port}: {e}")
            return False
    
    def get_citizen_information(self):
        print("\n" + "=" * 60)
        print(" Enter Complete Citizen Information")
        print("=" * 60)
        
        while True:
            first_name = input("First Name: ").strip()
            if self.validate_name(first_name):
                break
            print(" Invalid first name. Must contain at least 2 letters.")
        
        while True:
            last_name = input("Last Name: ").strip()
            if self.validate_name(last_name):
                break
            print(" Invalid last name. Must contain at least 2 letters.")
        
        while True:
            national_id = input("National ID: ").strip()
            if self.validate_national_id(national_id):
                break
            print(" Invalid National ID. Must contain only digits.")
        
        while True:
            dob = input("Date of Birth (YYYY-MM-DD): ").strip()
            if self.validate_date(dob):
                break
            print(" Invalid date format. Please use YYYY-MM-DD.")
        
        while True:
            sex = input("Sex (M/F): ").strip().upper()
            if sex in ['M', 'F']:
                break
            print(" Invalid sex. Please enter 'M' for Male or 'F' for Female.")
        
        while True:
            pin = input("Device PIN (4-8 digits): ").strip()
            if re.match(r'^\d{4,8}$', pin):
                break
            print(" Invalid PIN. Must be 4-8 digits only.")
        
        return {
            'first_name': first_name,
            'last_name': last_name,
            'national_id': national_id,
            'dob': dob,
            'sex': sex,
            'pin': pin
        }
    
    def run(self):
        print("=" * 60)
        print(" ZK-Pass Identity Config Tool")
        print("   Secure Citizen Identity Provisioning")
        print("=" * 60)
        
        print("\n Searching for ZK-Pass device...")
        self.esp32_port = self.detect_esp32_port()
        
        if not self.esp32_port:
            self.handle_manual_port_selection()
        
        print(f" Connecting to device on {self.esp32_port}...")
        if not self.connect_to_device():
            sys.exit(1)
        
        print(" Device communication established.")
        
        citizen_info = self.get_citizen_information()
        self.display_confirmation(citizen_info)
        
        if self.program_device(citizen_info):
            self.test_device_functionality(citizen_info['pin'])
            self.display_success_message(citizen_info['pin'])
        else:
            print(" Device provisioning failed.")
            sys.exit(1)
    
    def handle_manual_port_selection(self):
        print(" ZK-Pass device not found automatically!")
        print("Please check:")
        print("  • Device is connected to computer")
        print("  • USB drivers are installed")
        
        print("\n Available ports:")
        ports = serial.tools.list_ports.comports()
        for port in ports:
            print(f"  • {port.device} - {port.description}")
        
        choice = input("\nEnter port manually? (yes/no): ").strip().lower()
        if choice in ['yes', 'y', '']:
            self.esp32_port = input("Enter COM port (e.g COM3): ").strip()
            if not self.esp32_port:
                print(" No port specified. Exiting.")
                sys.exit(1)
        else:
            print(" Device selection cancelled.")
            sys.exit(1)
    
    def display_confirmation(self, citizen_info):
        print(f"\n Review Information:")
        print(f"   Name: {citizen_info['first_name']} {citizen_info['last_name']}")
        print(f"   DOB: {citizen_info['dob']}")
        print(f"   National ID: {citizen_info['national_id']}")
        print(f"   Sex: {citizen_info['sex']}")
        print(f"   PIN: {'*' * len(citizen_info['pin'])}")
        
        confirm = input("\nConfirm and program device? (yes/no): ").strip().lower()
        if confirm not in ['yes', 'y']:
            print(" Operation cancelled by user.")
            sys.exit(0)
    
    def program_device(self, citizen_info):
        print("\n Programming device with citizen identity...")
        
        command = (f"SET_ID:{citizen_info['first_name']},{citizen_info['last_name']},"
                  f"{citizen_info['dob']},{citizen_info['national_id']},"
                  f"{citizen_info['sex']},{citizen_info['pin']}")
        
        response = self.send_command(command, timeout=3.0)
        return "OK" in response
    
    def test_device_functionality(self, pin):
        print(" Testing age verification functionality...")
        test_response = self.send_command(f"CHK_AGE:18,{pin}", timeout=2.0)
        
        if "VERIFIED" in test_response:
            print(" Test successful: Device confirms citizen is over 18 years old.")
        elif "DENIED" in test_response:
            print(" Test successful: Device confirms citizen is under 18 years old.")
        else:
            print(f"  Test inconclusive: {test_response}")
    
    def display_success_message(self, pin):
        print("\n" + "=" * 60)
        print(" Device provisioning completed successfully!")
        print("Please deliver the device to the citizen.")
        print(f"Device PIN: {pin}")
        print("Citizen should keep PIN secure and confidential.")
        print("=" * 60)


def main():
    try:
        import serial
        import serial.tools.list_ports
    except ImportError:
        print(" Required library 'pyserial' not installed!")
        print("Please install it with: pip install pyserial")
        sys.exit(1)
    
    config = ZKPassConfig()
    try:
        config.run()
    except KeyboardInterrupt:
        print("\n\n Operation cancelled by user (Ctrl+C).")
        sys.exit(0)
    except Exception as e:
        print(f" Unexpected error: {e}")
        sys.exit(1)
    finally:
        if 'config' in locals() and config.serial_connection:
            config.serial_connection.close()
            print("\n Serial connection closed.")


if __name__ == "__main__":
    main()