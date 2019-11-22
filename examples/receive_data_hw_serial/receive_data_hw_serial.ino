/*--------------------------------------------------------------------

 * Receive Data - Hardware Serial - Example
 * Created by: Oliver G. Padolina @ VeriLo 11/22/2019

  -------------------------------------------------------------------*/

#include <SerialTool.h>
#include <SoftwareSerial.h>

// GSM - Serial
const uint16_t GSM_MAX_NUM    = 128;
const uint16_t GSM_TIME_OUT   = 5000;
const char *GSM_START_MARKER  = '\n';
const char *GSM_END_MARKER    = '\r';
SerialTool gsm(&Serial1, GSM_START_MARKER, GSM_END_MARKER);
char gsmData[GSM_MAX_NUM];

// BT - Serial
const uint16_t BT_MAX_NUM   = 32;
const uint16_t BT_TIME_OUT  = 1000;
const char *BT_START_MARKER = '<';
const char *BT_END_MARKER   = '>';
SerialTool bt(&Serial2, BT_START_MARKER, BT_END_MARKER);
char btData[BT_MAX_NUM];

// For Timer
unsigned long previousTime = 0;

void setup() {
  Serial.begin(9600);
  gsm.begin(9600, GSM_MAX_NUM, gsmData, GSM_TIME_OUT);
  gsm.setDebugMode(&Serial, "GSM:");
  bt.begin(9600, BT_MAX_NUM, btData, BT_TIME_OUT);
  gsm.setDebugMode(&Serial, "BT:");
  Serial.println("START");
}

void loop() {
  //gsm.listen(); // Not needed when using Harware Serial
  gsm.receiveData();
  
  // Check if <OK> received
  if (gsm.isNewData() && gsm.contains("OK")) {
    Serial.print("GSM: ");
    Serial.println(gsmData);
  }
  
  //bt.listen(); // Not needed when using Harware Serial
  bt.receiveData();

  sendBTEvery2Seconds();
  
  // Check if <OK> received  
  if (bt.isNewData() && bt.contains("OK")) {
    Serial.print("BT: ");
    Serial.println(btData);
  }
}

void sendBTEvery2Seconds() {
  // Repeat every 2 seconds
  unsigned long currentTime = millis();
  if (currentTime - previousTime >= 2) {
    previousTime = currentTime;
    
    char message[20];
    // Concatenate char
    strcpy(message, "<BT: TEST-");
    strcat(message, createRandomNumberInChar());
    strcat(message, ">\n");
    
    bt.sendCommand(message); // Send Command
    Serial.print("Sending data via BT: ");
    Serial.println(message);
  }
}

char *createRandomNumberInChar() {
  uint8_t rand = random(0, 10); // Create Random number from 0 to 10
  char randC[3];
  itoa(rand, randC, 10); // Convert integer to char
  return randC;
}