/*--------------------------------------------------------------------

  SerialTool.cpp - Serial Tool Library
  Created by: Oliver Padolina @ VeriLo on 03/11/2020
  
  A library for arduino that makes coding serial communication easy to
  integrate on any devices. Supports any serial devices, Like SIM800L
  (GSM), NEO6MV2 (GPS).

  -------------------------------------------------------------------*/
 
#include "Arduino.h"
#include "SerialTool.h"

/*-------------------------------------------------------------------
  CONSTRUCTORS
  -------------------------------------------------------------------*/
/*------------------------------------------------------------------- 
  SERIAL TOOL (Software Serial) 
    @brief Instantiates sensor with Software Serial
    @param ss - SoftwareSerial instance
    @param startMarker - Start marker for incoming character
    @param endMarker - End marker for incoming character
  -------------------------------------------------------------------*/
SerialTool::SerialTool(SoftwareSerial *ss, char startMarker,
  char endMarker)
{
  _hwSerial = false;
  _port = ss;
  
  _startMarker = startMarker;
  _endMarker = endMarker;
}
/*------------------------------------------------------------------- 
  SERIAL TOOL (Hardware Serial)
    @brief Instantiates sensor with Hardware Serial
    @param hs - HardwareSerial instance
    @param startMarker - Start marker for incoming character
    @param endMarker - End marker for incoming character
  -------------------------------------------------------------------*/
SerialTool::SerialTool(HardwareSerial *hs, char startMarker,
  char endMarker)
{
  _hwSerial = true;
  _port = hs;
  
  _startMarker = startMarker;
  _endMarker = endMarker;
}
  
/*-------------------------------------------------------------------
  PUBLIC FUNCTIONS
  -------------------------------------------------------------------*/
  
/*-------------------------------------------------------------------
  BEGIN
    @brief Initializes serial interface and baud rate
    @param baudrate - Sensor's UART baud rate (9600, 57600, 115200)
    @param numberOfChars - Maximum number of character received
      (Max Num is 65536)
    @param receivedChars - Received characters
    @param timeout - Timeout for waiting incoming character
      (Max Num is 65536)
  -------------------------------------------------------------------*/
void SerialTool::begin(const uint32_t baudrate, const uint16_t numberOfChars, char *receivedChars, const uint16_t timeout)
{
  if (_hwSerial) {
    static_cast<HardwareSerial*>(_port)->begin(baudrate);  
  } else {
    static_cast<SoftwareSerial*>(_port)->begin(baudrate);  
  }
  _numberOfChars = numberOfChars;
  _receivedChars = receivedChars;
  _baudrate = baudrate;
  _timeout = timeout;
}

/*-------------------------------------------------------------------
  RECEIVE DATA
    @brief Receive any incoming character data from serial by getting
      data inside start marker and end marker
  -------------------------------------------------------------------*/
void SerialTool::receiveData()
{
  static bool receiveInProgress = false;
  static uint8_t index = 0;
  char currentChar;
 
  while (_port->available() > 0 && _newDataReceived == false) {
    currentChar = _port->read();
    
    if (receiveInProgress == true) {
      if (currentChar != _endMarker) {
        _receivedChars[index] = currentChar;
        index++;
        if (index >= _numberOfChars) {
          index = _numberOfChars - 1;
        }
      } else {
        _receivedChars[index] = '\0'; // terminate the string
        receiveInProgress = false;
        index = 0;
        _newDataReceived = true;
      }
    } else if (currentChar == _startMarker) {
      receiveInProgress = true;
    }
  }
}

/*-------------------------------------------------------------------
  IS NEW DATA
    @brief Notifies that there is a valid data received
  -------------------------------------------------------------------*/
bool SerialTool::isNewData()
{
  if (_newDataReceived == true) {
    _newDataReceived = false;
    _receivedCharsCopy = _receivedChars;
    if (_debug) {
      _debugger->write("---------");
      _debugger->write(_debugName);
      _debugger->write(": ");
      _debugger->write(_receivedChars);
      _debugger->write("\n");
    }
    return true;
  }
  return false;
}

/*-------------------------------------------------------------------
  CONTAINS
    @brief Check if data contains specific word/character
    @param data - Word/Character to be find on data
  -------------------------------------------------------------------*/
bool SerialTool::contains(char *data)
{
  return strstr(_receivedChars, data);
}

/*-------------------------------------------------------------------
  SEND COMMAND
    @brief Send command to serial
    @param command - Word/Character to be send on serial
  -------------------------------------------------------------------*/
void SerialTool::sendCommand(char *command)
{
  _port->write(command);
  if (_debug) {
    _debugger->write("---------");
    _debugger->write(_debugName);
    _debugger->write("->: ");
    _debugger->write(command);
    _debugger->write("\n");
  }
}

/*-------------------------------------------------------------------
  LISTEN (Software Serial)
    @brief Listen to current port
  -------------------------------------------------------------------*/
void SerialTool::listen() {
  if (!_hwSerial) {
    static_cast<SoftwareSerial*>(_port)->listen();
  }
}

/*-------------------------------------------------------------------
  IS DATA RECEIVED
    @brief Check if specific data received from serial
    @param data - Word/Character to be find on data
  -------------------------------------------------------------------*/
bool SerialTool::isDataReceived(char *data) {
  unsigned long previousTime = millis();
  
  while (true) {
    unsigned long currentTime = millis();
    if (currentTime - previousTime >= _timeout) {
      return false;
    }
    receiveData();
    if (isNewData() && contains(data)) {
      return true;
    }
  }
}

/*-------------------------------------------------------------------
  PARSE DATA
    @brief Split data by delimeter character
    @param splitter - Word/Character to be used as delimiter
    @param parsedChars - array of char pointer used to store the
      splitted data
  -------------------------------------------------------------------*/
void SerialTool::parseData(char *splitter, char **parsedChars)
{
  char *parsedData;
  
  parsedData  = strtok(_receivedCharsCopy,splitter);
  parsedChars[0] = parsedData;
  
  uint16_t i = 1;
  while (parsedData != NULL) {
    parsedData  = strtok(NULL,splitter);
    parsedChars[i] = parsedData;
    i++;
  }
}

/*-------------------------------------------------------------------
  SEND ONCE AND WAIT FOR DATA
    @brief Send command and wait for data at specified timeout
    @param command - Word/Character to be send on serial
    @param data - Word/Character to be find on data
  -------------------------------------------------------------------*/
bool SerialTool::sendOnceAndWaitForData(char *command, char *data) {
  sendCommand(command);
  return isDataReceived(data);
}

/*-------------------------------------------------------------------
  SEND RETRY AND WAIT FOR DATA
    @brief Send command and wait for data at specified timeout and
      retries
    @param command - Word/Character to be send on serial
    @param data - Word/Character to be find on data
    @param retries - Number of retries times timeout (Max is 255)
  -------------------------------------------------------------------*/
bool SerialTool::sendRetryAndWaitForData(char *command, char *data, uint8_t retries) {
  uint8_t counter = 0;
  while (counter <= retries) {
    sendCommand(command);
    if (isDataReceived(data)) {
      return true;
    } else {
      counter++;
    }
  }
  return false;
}

/*-------------------------------------------------------------------
  SET DEBUG MODE (Hardware Serial)
    @brief Sets debug mode, to display any incoming data
    @param hs - Pointer of Hardware Serial
    @param debug name - Name of serial device e.g. GSM, GPS, etc.
  -------------------------------------------------------------------*/
void SerialTool::setDebugMode(HardwareSerial *hs, char *debugName) {
  _debugger = hs;
  _debugName = debugName;
  _debug = true;
}

/*-------------------------------------------------------------------
  SET DEBUG MODE (Software Serial)
    @brief Disable debug mode when using Software Serial
    @param ss - Pointer of Software Serial
    @param debug name - Name of serial device e.g. GSM, GPS, etc.
  -------------------------------------------------------------------*/
void SerialTool::setDebugMode(SoftwareSerial *ss, char *debugName) {
  _debug = false;
}

/*-------------------------------------------------------------------
  CLEAR BUFFER
    @brief Clear any excess received data on buffer
  -------------------------------------------------------------------*/
void SerialTool::clearBuffer() {
  while(_port->available() > 0) {
    _port->read();
  }
}

/*-------------------------------------------------------------------
  WRITE
    @brief Write data to Serial
  -------------------------------------------------------------------*/
void SerialTool::write(char *data) {
  _port->write(data);
}

/*-------------------------------------------------------------------
  WAIT FOR DATA
    @brief Wait for data at specified timeout
    @param data - Word/Character to be find on data
    @param timeout - time for timeout (Max is 65536)
  -------------------------------------------------------------------*/
bool SerialTool::waitForData(char *data, uint16_t timeout) {
  uint32_t timer = millis();
  while(true) {
    if (millis() - timer >= timeout) {
      return false;
    }
    receiveData();
    if (isNewData()) {
      if (contains(data)) {
        return true;
      }
    }
  }
}
