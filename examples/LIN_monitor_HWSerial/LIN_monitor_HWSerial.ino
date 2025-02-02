/*********************

Example code for a LIN monitor without response using HardwareSerial

Note:
  - as listener, only register master request frames to avoid a response (-> bus conflict)
  - for AVR use NeoHWSerial for BREAK detection (install via Library manager). Also only use NeoSerial in your code, not Serial
  - act on received frame inside callback functions. Console output below is optional 

Supported (=successfully tested) boards:
 - Arduino Mega 2560      https://store.arduino.cc/products/arduino-mega-2560-rev3
 - Arduino Due            https://store.arduino.cc/products/arduino-due
 - ESP8266 D1 mini        https://www.wemos.cc/en/latest/d1/d1_mini.html
 - ESP32 Wroom-32UE       https://www.etechnophiles.com/esp32-dev-board-pinout-specifications-datasheet-and-schematic/

**********************/

///////////////////
// AVR architecture, e.g. Arduino Mega
///////////////////
#if defined(ARDUINO_ARCH_AVR)
  
  // include files
  #include <NeoHWSerial.h>                      // use NeoHWSerial to avoid linker conflict for UART ISRs
  #include <LIN_slave_NeoHWSerial_AVR.h>

  // serial I/F for debug output (comment for no output)
  #define SERIAL_DEBUG  NeoSerial

  // setup LIN node. Parameters: interface, version, name, timeout, TxEN
  LIN_Slave_NeoHWSerial_AVR  LIN(NeoSerial1, LIN_Slave_Base::LIN_V2, "Monitor");


///////////////////
// SAMD architecture, e.g. Arduino Due
///////////////////
#elif defined(ARDUINO_ARCH_SAM)
  
  // include files
  #include <LIN_slave_HardwareSerial.h>

  // serial I/F for debug output (comment for no output)
  #define SERIAL_DEBUG  Serial

  // setup LIN node. Parameters: interface, pause-sync, version, name, timeout, TxEN
  LIN_Slave_HardwareSerial  LIN(Serial1, 1000, LIN_Slave_Base::LIN_V2, "Monitor");


///////////////////
// ESP8266 architecture, e.g. ESP8266 D1 mini
///////////////////
#elif defined(ARDUINO_ARCH_ESP8266)
  
  // include files
  #include <LIN_slave_HardwareSerial_ESP8266.h>

  // serial I/F for debug output (comment for no output). Use Tx-only UART1 on pin D4 via UART<->USB adapter
  #define SERIAL_DEBUG  Serial1

  // setup LIN node. Swap Serial pins to use Tx=D8 & Rx=D7. Parameters: swapPins, pause-sync, version, name, timeout, TxEN
  LIN_Slave_HardwareSerial_ESP8266   LIN(true, 1000, LIN_Slave_Base::LIN_V2, "Monitor");


///////////////////
// ESP32 architecture, e.g. ESP32 Wroom-32UE
///////////////////
#elif defined(ARDUINO_ARCH_ESP32)
  
  // include files
  #include <LIN_slave_HardwareSerial_ESP32.h>

  // board pin definitions (GPIOn is referred to as n)
  #define PIN_LIN_RX    16        // receive pin for LIN
  #define PIN_LIN_TX    17        // transmit pin for LIN

  // serial I/F for debug output (comment for no output)
  #define SERIAL_DEBUG  Serial

  // setup LIN node. Parameters: interface, Rx, Tx, version, name, timeout, TxEN
  LIN_Slave_HardwareSerial_ESP32  LIN(Serial2, PIN_LIN_RX, PIN_LIN_TX, LIN_Slave_Base::LIN_V2, "Monitor");


// unknown architecture -> error
#else
  #error architecture not supported yet

#endif



// call once
void setup()
{
  // for debug output
  #if defined(SERIAL_DEBUG)
    SERIAL_DEBUG.begin(115200);
    while(!SERIAL_DEBUG);
  #endif // SERIAL_DEBUG

  // open LIN interface
  LIN.begin(19200);

  // Register callback functions for frame IDs with expected data lengths
  LIN.registerMasterRequestHandler(0x1A, handle_frame_0x1A, 4);
  LIN.registerMasterRequestHandler(0x1B, handle_frame_0x1B, 6);

} // setup()



void loop()
{
  // on byte received, call LIN slave protocol handler. Alternatively use serialEvent(), Ticker(), etc.
  if (LIN.available())
    LIN.handler();

} // loop()



// Example for user-defined frame handler
void handle_frame_0x1A(uint8_t NumData, uint8_t* Data)
{
  // avoid unused parameter warning
  (void) NumData;
  (void) Data;

  //////
  // add code to react on received data
  //////
  
  // optionally print frame
  #if defined(SERIAL_DEBUG)
        
    LIN_Slave_Base::error_t   error;
      
    // get frame data & error status
    error = LIN.getError();

    SERIAL_DEBUG.print("ID=0x1A: ");
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

  #endif // SERIAL_DEBUG

  // reset state machine & error
  LIN.resetStateMachine();
  LIN.resetError();

} // handle_frame_0x1A()



// Example for user-defined frame handler
void handle_frame_0x1B(uint8_t NumData, uint8_t* Data)
{
  // avoid unused parameter warning
  (void) NumData;
  (void) Data;

  //////
  // add code to react on received data
  //////
  
  // optionally print frame
  #if defined(SERIAL_DEBUG)
        
    LIN_Slave_Base::error_t   error;
      
    // get frame data & error status
    error = LIN.getError();

    SERIAL_DEBUG.print("ID=0x1B: ");
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

  #endif // SERIAL_DEBUG

  // reset state machine & error
  LIN.resetStateMachine();
  LIN.resetError();

} // handle_frame_0x1B()
