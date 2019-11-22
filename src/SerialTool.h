/*--------------------------------------------------------------------

  SerialTool.h - Serial Tool Library
  Created by: Oliver Padolina @ VeriLo on 11/22/2019
  
  A library for arduino that makes coding serial communication easy to
  integrate on any devices. Supports any serial devices, Like SIM800L
  (GSM), NEO6MV2 (GPS).

  -------------------------------------------------------------------*/
 
#ifndef SerialTool_h
#define SerialTool_h
#include "Arduino.h"
#include <SoftwareSerial.h>

#define SUCCESS 1
#define FAILED 0

class SerialTool
{
  public:
    SerialTool(SoftwareSerial *ss, const char startMarker,
      const char endMarker);
    SerialTool(HardwareSerial *hs, const char startMarker,
      const char endMarker);
    void begin(const uint32_t baud, const uint16_t numberOfChars,
      const char *receivedChars, const uint16_t timeout);
    void receiveData();
    bool isNewData();
    bool contains(const char *data);
    void sendCommand(const char *command);
    void listen();
    bool isDataReceived(const char *data);
    void parseData(const char *splitter, char **parsedChars);
    bool sendOnceAndWaitForData(const char *command, const char *data);
    bool sendRetryAndWaitForData(const char *command, const char *data,
      uint8_t retries);
    void setDebugMode(HardwareSerial *hs, const char *debugName);
    void setDebugMode(SoftwareSerial *ss, const char *debugName);
    void clearBuffer();
    
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
 