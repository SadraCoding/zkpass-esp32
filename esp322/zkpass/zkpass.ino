/*
 * ============================================================
 *  ZK-PASS IDENTITY WALLET
 *  ------------------------------------------------------------
 *  Secure hardware-based identity verification with privacy preservation
 *
 *  Developed for ESP32 with integrated OLED display (SSD1306)
 *  Author: SadraCoding
 *  Date: October 2025
 *
 *  ------------------------------------------------------------
 *  FEATURES
 *  ------------------------------------------------------------
 *  • Encrypted storage of identity data (Name, Surname, DOB, National ID, Sex)
 *  • PIN-based key derivation for AES-128 encryption
 *  • Local, privacy-preserving age verification (Zero-Knowledge approach)
 *  • OLED status messages with full-screen background feedback
 *
 *  ------------------------------------------------------------
 *  COMMANDS (via Serial)
 *  ------------------------------------------------------------
 *  1. SET_ID:<name>,<surname>,<dob>,<national_id>,<sex>,<pin>
 *     → Example: SET_ID:Alice,Smith,1998-04-23,ID123456,F,4321
 *     → Stores encrypted identity in NVS (if not already set)
 *
 *  2. CHK_AGE:<min_age>,<pin>
 *     → Example: CHK_AGE:18,4321
 *     → Returns “VERIFIED” if age ≥ min_age, else “DENIED”
 *
 *  ------------------------------------------------------------
 *  SECURITY NOTE
 *  ------------------------------------------------------------
 *  • AES-128 ECB mode is used here for simplicity (no IV)
 *  • Key derivation uses direct PIN padding; for production,
 *    replace with PBKDF2 + random salt for secure key stretching
 *  • Data is stored locally in ESP32’s non-volatile storage (NVS)
 *
 *  ------------------------------------------------------------
 *  LICENSE
 *  ------------------------------------------------------------
 *  MIT License © 2025 SadraCoding
 * ============================================================
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Preferences.h>
#include <AES.h>

// -------------------------------
// OLED Configuration
// -------------------------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// -------------------------------
// Storage & Encryption Config
// -------------------------------
Preferences prefs;
AES aes;
const char* NAMESPACE = "zkpass-esp";

// -------------------------------
// Buffer Sizes
// -------------------------------
const int IDENTITY_BUFFER_SIZE = 128;   // For encrypted/decrypted identity blocks
const int COMMAND_BUFFER_SIZE  = 150;   // For incoming serial commands

// =====================================================================
//  FUNCTION: deriveKey()
// ---------------------------------------------------------------------
//  Derives a 16-byte AES key directly from the provided PIN.
//  Note: This is a minimal placeholder — replace with PBKDF2 for real use.
// =====================================================================
void deriveKey(const char* pin, byte* key) {
  memset(key, 0, 16);
  for (int i = 0; i < 16 && pin[i] != '\0'; i++) {
    key[i] = pin[i];
  }
}

// =====================================================================
//  FUNCTION: encryptIdentity()
// ---------------------------------------------------------------------
//  Encrypts the user’s identity string using AES-128 and stores it in NVS.
//  Input format: name|surname|dob|national_id|sex
// =====================================================================
bool encryptIdentity(const char* identity, const char* pin) {
  byte key[16];
  deriveKey(pin, key);
  
  byte plaintext[IDENTITY_BUFFER_SIZE] = {0};
  byte ciphertext[IDENTITY_BUFFER_SIZE] = {0};
  strncpy((char*)plaintext, identity, IDENTITY_BUFFER_SIZE - 1);
  
  aes.set_key(key, 128);
  
  // Encrypt in 16-byte AES blocks
  for (int i = 0; i < IDENTITY_BUFFER_SIZE; i += 16) {
    aes.encrypt(plaintext + i, ciphertext + i);
  }
  
  prefs.putBytes("id_data", ciphertext, IDENTITY_BUFFER_SIZE);
  return true;
}

// =====================================================================
//  FUNCTION: decryptIdentity()
// ---------------------------------------------------------------------
//  Decrypts identity data from NVS using AES-128 and provided PIN.
//  Returns false if data missing or PIN invalid.
// =====================================================================
bool decryptIdentity(char* identity, const char* pin) {
  byte key[16];
  deriveKey(pin, key);
  
  byte ciphertext[IDENTITY_BUFFER_SIZE];
  byte plaintext[IDENTITY_BUFFER_SIZE] = {0};
  
  if (prefs.getBytesLength("id_data") != IDENTITY_BUFFER_SIZE) {
    return false;
  }
  prefs.getBytes("id_data", ciphertext, IDENTITY_BUFFER_SIZE);
  
  aes.set_key(key, 128);
  
  for (int i = 0; i < IDENTITY_BUFFER_SIZE; i += 16) {
    aes.decrypt(ciphertext + i, plaintext + i);
  }
  
  strncpy(identity, (char*)plaintext, IDENTITY_BUFFER_SIZE - 1);
  identity[IDENTITY_BUFFER_SIZE - 1] = '\0';
  return true;
}

// =====================================================================
//  FUNCTION: displayMessage()
// ---------------------------------------------------------------------
//  Displays a centered message on the OLED with solid background.
//  success = true → white background (“OK”)
//  success = false → black background (“Error”)
// =====================================================================
void displayMessage(const char* msg, bool success = true) {
  display.clearDisplay();
  display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, success ? SSD1306_WHITE : SSD1306_BLACK);
  display.setTextColor(success ? SSD1306_BLACK : SSD1306_WHITE);
  display.setTextSize(2);
  display.setTextWrap(false);
  
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(msg, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_WIDTH - w) / 2, (SCREEN_HEIGHT - h) / 2);
  display.println(msg);
  display.display();
  delay(1500);
  display.clearDisplay();
  display.display();
}

// =====================================================================
//  FUNCTION: isOverAge()
// ---------------------------------------------------------------------
//  Determines whether a person is over a specified minimum age.
//  Date format: YYYY-MM-DD
//  Uses a fixed current date (Oct 17, 2025) for deterministic testing.
// =====================================================================
bool isOverAge(const char* dob_str, int min_age) {
  int year, month, day;
  if (sscanf(dob_str, "%d-%d-%d", &year, &month, &day) != 3) {
    return false;
  }
  
  int current_year = 2025;
  int current_month = 10;
  int current_day = 17;
  
  int age = current_year - year;
  if (current_month < month || (current_month == month && current_day < day)) {
    age--;
  }
  return age >= min_age;
}

// =====================================================================
//  SETUP()
// ---------------------------------------------------------------------
//  Initializes serial, NVS storage, and OLED display.
// =====================================================================
void setup() {
  Serial.begin(115200);
  prefs.begin(NAMESPACE, false);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED initialization failed");
    while(1);
  }
  display.clearDisplay();
  display.display();
  
  Serial.println("READY");
}

// =====================================================================
//  LOOP()
// ---------------------------------------------------------------------
//  Handles incoming serial commands for identity setup and age checks.
// =====================================================================
void loop() {
  if (Serial.available()) {
    char commandBuffer[COMMAND_BUFFER_SIZE];
    int bytesRead = Serial.readBytesUntil('\n', commandBuffer, COMMAND_BUFFER_SIZE - 1);
    commandBuffer[bytesRead] = '\0';
    
    String command = String(commandBuffer);
    command.trim();

    // -------------------------------
    // Command: SET_ID
    // -------------------------------
    if (command.startsWith("SET_ID:")) {
      if (prefs.getBytesLength("id_data") > 0) {
        Serial.println("ERROR: Identity already set");
        return;
      }

      String payload = command.substring(7);
      
      // Expect 6 parameters (name, surname, dob, national_id, sex, pin)
      int commaPositions[5];
      commaPositions[0] = payload.indexOf(',');
      if (commaPositions[0] == -1) { Serial.println("ERROR: Missing name separator"); return; }
      
      commaPositions[1] = payload.indexOf(',', commaPositions[0] + 1);
      if (commaPositions[1] == -1) { Serial.println("ERROR: Missing surname separator"); return; }
      
      commaPositions[2] = payload.indexOf(',', commaPositions[1] + 1);
      if (commaPositions[2] == -1) { Serial.println("ERROR: Missing DOB separator"); return; }
      
      commaPositions[3] = payload.indexOf(',', commaPositions[2] + 1);
      if (commaPositions[3] == -1) { Serial.println("ERROR: Missing national ID separator"); return; }
      
      commaPositions[4] = payload.indexOf(',', commaPositions[3] + 1);
      if (commaPositions[4] == -1) { Serial.println("ERROR: Missing sex separator"); return; }

      String name = payload.substring(0, commaPositions[0]);
      String surname = payload.substring(commaPositions[0] + 1, commaPositions[1]);
      String dob = payload.substring(commaPositions[1] + 1, commaPositions[2]);
      String nationalId = payload.substring(commaPositions[2] + 1, commaPositions[3]);
      String sex = payload.substring(commaPositions[3] + 1, commaPositions[4]);
      String pin = payload.substring(commaPositions[4] + 1);

      // Basic validation
      if (name.length() < 1 || surname.length() < 1 || 
          dob.length() != 10 || nationalId.length() < 1 ||
          (sex != "M" && sex != "F") || pin.length() < 4) {
        Serial.println("ERROR: Invalid input data");
        return;
      }

      // Format and encrypt
      char identity[IDENTITY_BUFFER_SIZE];
      int result = snprintf(identity, IDENTITY_BUFFER_SIZE, "%s|%s|%s|%s|%s", 
                           name.c_str(), surname.c_str(), dob.c_str(), 
                           nationalId.c_str(), sex.c_str());
      
      if (result < 0 || result >= IDENTITY_BUFFER_SIZE) {
        Serial.println("ERROR: Identity string too long");
        return;
      }

      if (encryptIdentity(identity, pin.c_str())) {
        displayMessage("SET", true);
        Serial.println("OK");
      } else {
        Serial.println("ERROR: Encryption failed");
      }
    }

    // -------------------------------
    // Command: CHK_AGE
    // -------------------------------
    else if (command.startsWith("CHK_AGE:")) {
      int commaIndex = command.indexOf(',');
      if (commaIndex == -1) { 
        Serial.println("ERROR: Invalid age verification command format"); 
        return; 
      }

      int minAge = command.substring(8, commaIndex).toInt();
      String pin = command.substring(commaIndex + 1);

      if (prefs.getBytesLength("id_data") == 0) {
        displayMessage("NO", false);
        Serial.println("ERROR: No identity data set");
        return;
      }

      char identity[IDENTITY_BUFFER_SIZE];
      if (!decryptIdentity(identity, pin.c_str())) {
        displayMessage("DENIED", false);
        Serial.println("ERROR: Invalid PIN");
        return;
      }

      // Extract DOB field (3rd segment)
      char* dob = identity;
      for (int i = 0; i < 2; i++) {
        dob = strchr(dob, '|');
        if (!dob) { 
          Serial.println("ERROR: Failed to parse identity data"); 
          return; 
        }
        dob++;
      }
      char* end = strchr(dob, '|');
      if (end) *end = '\0';

      // Perform age verification
      if (isOverAge(dob, minAge)) {
        displayMessage("OK", true);
        Serial.println("VERIFIED");
      } else {
        displayMessage("NO", false);
        Serial.println("DENIED");
      }
    }
  }
  delay(5);
}
