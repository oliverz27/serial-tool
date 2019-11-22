/*--------------------------------------------------------------------

  Receive Data - Example
  Created by: Oliver G. Padolina @ VeriLo 11/22/2019

  This example will print any incoming data from serial device
  
  -------------------------------------------------------------------*/

#include <SerialTool.h>

const uint16_t MAX_NUM        = 32;
const uint16_t TIME_OUT       = 1000;
const char *MY_START_MARKER  = '<';
const char *MY_END_MARKER    = '>';

SerialTool mySerial(&Serial, MY_START_MARKER, MY_END_MARKER);
char mySerialData[MAX_NUM];

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600, MAX_NUM, mySerialData, TIME_OUT);
  mySerial.setDebugMode(&Serial, "SERIAL_TEST:");
  Serial.println("START");
}

void loop() {
  mySerial.receiveData();
  
  if (mySerial.isNewData() && mySerial.contains("OK")) {
    Serial.println(mySerialData);
  }
}