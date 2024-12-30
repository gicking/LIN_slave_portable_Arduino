/*********************

Example code for LIN slave node using generic SoftwareSerial interface

Note:
  - frame synchronization via inter-frame pause, due to lack of framing error detection in Arduino -> not standard compliant!
  - if available, propose to use Serial using proper BREAK detection, e.g. NeoHWSerial (AVR) or ESP32
  - handling of frames can be done inside callback punctions. Console output below is optional 

Supported (=successfully tested) boards:
 - Arduino Mega 2560      https://store.arduino.cc/products/arduino-mega-2560-rev3
 - Adafruit Trinket       https://www.adafruit.com/product/1501

Unsupported (=test failed) boards:
 - ESP8266 D1 mini        https://www.wemos.cc/en/latest/d1/d1_mini.html
 - ESP32 Wroom-32UE       https://www.etechnophiles.com/esp32-dev-board-pinout-specifications-datasheet-and-schematic/

**********************/

// include files
#include "LIN_slave_SoftwareSerial.h"

// board pin definitions. Note: for supported Rx pins see https://docs.arduino.cc/learn/built-in-libraries/software-serial/
#if defined(ARDUINO_AVR_MEGA2560)
  #include <NeoHWSerial.h>        // use NeoHWSerial to avoid linker conflict for UART ISRs
  #define PIN_LIN_TX    18        // transmit pin for LIN
  #define PIN_LIN_RX    10        // receive pin for LIN
  #define PIN_TOGGLE    30        // pin to demonstrate background operation
  #define PIN_ERROR     32        // indicate LIN return status
  #define SERIAL_DEBUG	NeoSerial // serial I/F for debug output (comment for no output) 
#elif defined(ARDUINO_ESP8266_WEMOS_D1MINI)
  #define PIN_LIN_TX    D8
  #define PIN_LIN_RX    D7
  #define PIN_TOGGLE    D1
  #define PIN_ERROR     D2
  #define SERIAL_DEBUG	Serial1   // Use Tx-only UART1 on pin D4 via UART<->USB adapter
#elif defined(ARDUINO_ESP32_WROOM_DA)
  #define PIN_LIN_TX    17
  #define PIN_LIN_RX    16
  #define PIN_TOGGLE    19
  #define PIN_ERROR     18
  #define SERIAL_DEBUG	Serial
#elif defined(ARDUINO_AVR_TRINKET3) || defined(ARDUINO_AVR_TRINKET5)
  #define PIN_LIN_TX    2
  #define PIN_LIN_RX    0
  #define PIN_TOGGLE    1
  #define PIN_ERROR     3
  // Trinket has no HW-Serial!
#else
  #error adapt parameters to board   
#endif

// setup LIN node
LIN_Slave_SoftwareSerial  LIN(PIN_LIN_RX, PIN_LIN_TX, false, LIN_Slave_Base::LIN_V2, "Slave");


// call once
void setup()
{
  // for debug output
  #if defined(SERIAL_DEBUG)
    SERIAL_DEBUG.begin(115200);
    while(!SERIAL_DEBUG);
  #endif // SERIAL_DEBUG

  // indicate background operation
  pinMode(PIN_TOGGLE, OUTPUT);

  // indicate LIN status via pin
  pinMode(PIN_ERROR, OUTPUT);

  // open LIN interface
  LIN.begin(19200);

  // Register callback functions for frame IDs with expected data lengths
  LIN.registerMasterRequestHandler(0x1A, handle_Request, 4);
  LIN.registerSlaveResponseHandler(0x05, handle_Response, 6);
  
} // setup()



void loop()
{
  // call LIN slave protocol handler often
  LIN.handler();

  // indicate core load
  digitalWrite(PIN_TOGGLE, !digitalRead(PIN_TOGGLE));

  // indicate error status via pin
  digitalWrite(PIN_ERROR, LIN.getError());

  // if LIN frame has finished, print it
  if (LIN.getState() == LIN_Slave_Base::STATE_DONE)
  {
    LIN_Slave_Base::frame_t   Type;
    LIN_Slave_Base::error_t   error;
    uint8_t                   Id;
    uint8_t                   NumData;
    uint8_t                   Data[8];

    // get frame data & error status
    LIN.getFrame(Type, Id, NumData, Data);
    error = LIN.getError();

    // indicate status via pin
    digitalWrite(PIN_ERROR, error);

    // print result
    #if defined(SERIAL_DEBUG)
      if (Type == LIN_Slave_Base::MASTER_REQUEST)
      {
        SERIAL_DEBUG.print(LIN.nameLIN);
        SERIAL_DEBUG.print(", request, ID=0x");
        SERIAL_DEBUG.print(Id, HEX);
        if (error != LIN_Slave_Base::NO_ERROR)
        { 
          SERIAL_DEBUG.print(", err=0x");
          SERIAL_DEBUG.println(error, HEX);
        }
        else
        {
          SERIAL_DEBUG.print(", data=");        
          for (uint8_t i=0; (i < NumData); i++)
          {
            SERIAL_DEBUG.print("0x");
            SERIAL_DEBUG.print((int) Data[i], HEX);
            SERIAL_DEBUG.print(" ");
          }
          SERIAL_DEBUG.println();
        }
      }
      else
      {
        SERIAL_DEBUG.print(LIN.nameLIN);
        SERIAL_DEBUG.print(", response, ID=0x");
        SERIAL_DEBUG.print(Id, HEX);
        if (error != LIN_Slave_Base::NO_ERROR)
        { 
          SERIAL_DEBUG.print(", err=0x");
          SERIAL_DEBUG.println(error, HEX);
        }
        else
        {
          SERIAL_DEBUG.print(", data=");        
          for (uint8_t i=0; (i < NumData); i++)
          {
            SERIAL_DEBUG.print("0x");
            SERIAL_DEBUG.print((int) Data[i], HEX);
            SERIAL_DEBUG.print(" ");
          }
          SERIAL_DEBUG.println();
        }
      }
    #endif // SERIAL_DEBUG

    // reset state machine & error
    LIN.resetStateMachine();
    LIN.resetError();

  } // if LIN frame finished

} // loop()



// Example for user-defined Master Request handler
void handle_Request(uint8_t NumData, uint8_t* Data)
{
  // avoid unused parameter warning
  (void) NumData;
  (void) Data;

  // add code to response on received data

} // handle_Request()



// Example for user-defined Slave Response handler
void handle_Response(uint8_t NumData, uint8_t* Data)
{
  // avoid unused parameter warning
  (void) NumData;
  (void) Data;
  
  // set dummy data for response
  for (uint8_t i=0; i<NumData; i++)
    Data[i] = 0x10 + i;

} // handle_Response()