/*********************

Example code for LIN slave node using generic HardwareSerial interface

Note:
  - frame synchronization via inter-frame pause, due to lack of framing error detection in Arduino -> not standard compliant!
  - if available use respective platform implementation using proper BREAK detection 

Supported (=successfully tested) boards:
 - Arduino Mega 2560      https://store.arduino.cc/products/arduino-mega-2560-rev3

**********************/

// include files
#include "LIN_slave_HardwareSerial.h"

// board pin definitions (GPIOn is referred to as n)
#define PIN_TOGGLE    6         // pin to demonstrate background operation
#define PIN_ERROR     7         // indicate LIN return status

// setup LIN node
LIN_Slave_HardwareSerial  LIN_slave_node(Serial2, LIN_Slave_Base::LIN_V2, "Slave");


// call once
void setup()
{
  // open console
  Serial.begin(115200);
  while(!Serial);

  // indicate background operation
  pinMode(PIN_TOGGLE, OUTPUT);

  // indicate LIN status via pin
  pinMode(PIN_ERROR, OUTPUT);

  // open LIN interface
  LIN_slave_node.begin(19200);

  // Register handlers for frame IDs with expected data lengths
  LIN_slave_node.registerMasterRequestHandler(0x1A, handle_Master, 4);
  LIN_slave_node.registerSlaveResponseHandler(0x05, handle_Slave, 6);

} // setup()



void loop()
{
  // call LIN slave protocol handler often
  LIN_slave_node.handler();

  // indicate error status via pin
  digitalWrite(PIN_ERROR, LIN_slave_node.getError());

  // check for LIN error
  if (LIN_slave_node.getError() != LIN_Slave_Base::NO_ERROR)
  {
    // print error code
    Serial.print("LIN slave error ");
    Serial.println((int) LIN_slave_node.getError());
    
    // reset error (is latched)
    LIN_slave_node.resetError();
  
  } // if LIN error

  // indicate core load
  digitalWrite(PIN_TOGGLE, !digitalRead(PIN_TOGGLE));

} // loop()


// Example for user-defined Master Request handler
void handle_Master(uint8_t numData, uint8_t* data)
{
  // print received data
  Serial.print("Handle Request: Rx =");
  for (int i = 0; i < numData; i++)
  {
    Serial.print(" 0x");
    Serial.print(data[i], HEX);
  }
  Serial.println();

} // handle_Master()


// Example for user-defined Slave Response handler
void handle_Slave(uint8_t numData, uint8_t* data)
{  
  // set dummy data
  for (uint8_t i=0; i<numData; i++)
    data[i] = 2*i;
  
  // print response data
  Serial.print("Handle Response: Tx =");
  for (int i = 0; i < numData; i++)
  {
    Serial.print(" 0x");
    Serial.print(data[i], HEX);
  }
  Serial.println();

} // handle_Slave()
