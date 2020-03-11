/*--------------------------------------------------------------------

  SerialTool.h - Serial Tool Library
  Created by: Oliver Padolina @ VeriLo on 03/11/2020
  
  A library for arduino that makes coding serial communication easy to
  integrate on any devices. Supports any serial devices, Like SIM800L
  (GSM), NEO6MV2 (GPS).

  -------------------------------------------------------------------*/
 
#ifndef SerialTool_h
#define SerialTool_h
#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif
#include <SoftwareSerial.h>

#define SUCCESS 1
#define FAILED 0

class SerialTool
{
  public:
    SerialTool(SoftwareSerial *ss, char startMarker,
      char endMarker);
    SerialTool(HardwareSerial *hs, char startMarker,
      char endMarker);
    void begin(const uint32_t baud, const uint16_t numberOfChars,
      char *receivedChars, const uint16_t timeout);
    void receiveData();
    bool isNewData();
    bool contains(char *data);
    void sendCommand(char *command);
    void listen();
    bool isDataReceived(char *data);
    void parseData(char *splitter, char **parsedChars);
    bool sendOnceAndWaitForData(char *command, char *data);
    bool sendRetryAndWaitForData(char *command, char *data,
      uint8_t retries);
    void setDebugMode(HardwareSerial *hs, char *debugName);
    void setDebugMode(SoftwareSerial *ss, char *debugName);
    void clearBuffer();
    void write(char *data);
    bool waitForData(char *data, uint32_t timeout);
    
  private:
    Stream *_port;
    uint32_t _baudrate;
    bool _hwSerial;
    uint16_t _numberOfChars;
    char *_receivedChars;
    char *_receivedCharsCopy;
    uint16_t _timeout;
    bool _newDataReceived = false;
    char _startMarker;
    char _endMarker;
    char *_debugName;
    HardwareSerial *_debugger;
    bool _debug = false;
};

#endif
 
