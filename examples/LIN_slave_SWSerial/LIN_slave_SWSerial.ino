/*********************

Example code for LIN slave node using generic SoftwareSerial interface

Optional Tx direction switching for RS485 interface (e.g. MAX485) is by defining 'PIN_TXEN'. 
In this case, permanently enable Rx (REN=GND) for receiving echo

Note:
  - frame synchronization via inter-frame pause, due to lack of framing error detection in SoftwareSerial -> not standard compliant!
  - if available, propose to use Serial using proper BREAK detection, e.g. NeoHWSerial (AVR) or ESP32
  - handling of frames can be done inside callback functions. Console output below is optional 
  - for ESP8266 during programming and boot, D8(=Tx) must be left open!
  - for supported Rx pins see https://docs.arduino.cc/learn/built-in-libraries/software-serial/

Tested boards:
  - Arduino Mega 2560       https://docs.arduino.cc/hardware/mega-2560/
  x Arduino Due (SoftwareSerial not available)   https://docs.arduino.cc/hardware/due/
  - Arduino Nano Every      https://docs.arduino.cc/hardware/nano-every/
  - Adafruit Trinket        https://www.adafruit.com/product/1501
  - Arduino Uno R4 Minima (blocking)  https://docs.arduino.cc/hardware/uno-r4-minima/ 
  - Arduino Nano ESP32-S3   https://docs.arduino.cc/hardware/nano-esp32/
  - ESP32 WROOM-32UE        https://documentation.espressif.com/esp32-wroom-32e_esp32-wroom-32ue_datasheet_en.pdf
  - ESP8266 D1 Mini         https://www.wemos.cc/en/latest/d1/d1_mini.html
  - Nucleo-STM32L432KC      https://www.st.com/en/evaluation-tools/nucleo-l432kc.html

**********************/

// include files
#include "LIN_slave_SoftwareSerial.h"

////////////////////
// Arduino Mega settings
////////////////////
#if defined(ARDUINO_AVR_MEGA2560)
  #include <NeoHWSerial.h>                              // use NeoHWSerial to avoid linker conflict for UART ISRs
  #define PIN_LIN_TX      18                            // transmit pin for LIN
  #define PIN_LIN_RX      10                            // receive pin for LIN. Options: 10, 11, 12, 13, 14, 15, 50, 51, 52, 53, A8, A9, A10, A11, A12, A13, A14, A15
  //#define PIN_TXEN      17                              // optional Tx direction pin (=DE) for RS485 physical I/F. Comment out for LIN I/F 
  #define PIN_TOGGLE      30                            // pin to demonstrate background operation
  #define PIN_ERROR       32                            // indicate LIN return status
  #define SERIAL_CONSOLE  NeoSerial                     // serial I/F for console output (comment for no output)

////////////////////
// Arduino Nano Every settings
////////////////////
#elif defined(ARDUINO_AVR_NANO_EVERY)
  #define PIN_LIN_TX      2
  #define PIN_LIN_RX      3                             // options: any GPIO
  //#define PIN_TXEN      7
  #define PIN_TOGGLE      4
  #define PIN_ERROR       6
  #define SERIAL_CONSOLE  Serial

////////////////////
// Arduino Uno R4 Minima settings
////////////////////
#elif defined(ARDUINO_UNOR4_MINIMA)
  #define PIN_LIN_TX      1
  #define PIN_LIN_RX      0                             // options: D0, D1, D2, D3, D8, D14, D15, A1, A2, A3, A4, A5
  //#define PIN_TXEN      10
  #define PIN_TOGGLE      4
  #define PIN_ERROR       6
  #define SERIAL_CONSOLE  Serial

////////////////////
// Adafruit Trinket settings
////////////////////
#elif defined(ARDUINO_AVR_TRINKET3) || defined(ARDUINO_AVR_TRINKET5)
  #define PIN_LIN_TX      2
  #define PIN_LIN_RX      0                             // options: any GPIO
  //#define PIN_TXEN      4
  #define PIN_TOGGLE      1
  #define PIN_ERROR       3
  // Trinket has no HW-Serial!

////////////////////
// Arduino Nano ESP32 board settings (using Arduino ESP32 core)
////////////////////
#elif defined(ARDUINO_NANO_ESP32)
  #define PIN_LIN_TX      3
  #define PIN_LIN_RX      4                             // options: any GPIO
  //#define PIN_TXEN      10
  #define PIN_TOGGLE      5
  #define PIN_ERROR       6
  #define SERIAL_CONSOLE  Serial

////////////////////
// Espressif ESP32-WROOM-32UE board settings (using Espressif ESP32 core)
////////////////////
#elif defined(ARDUINO_ESP32_WROOM_DA)
  #define PIN_LIN_TX      17
  #define PIN_LIN_RX      16
  //#define PIN_TXEN      21
  #define PIN_TOGGLE      19
  #define PIN_ERROR       18
  #define SERIAL_CONSOLE  Serial

////////////////////
// Wemos ESP8266 D1 Mini settings
////////////////////
#elif defined(ARDUINO_ESP8266_WEMOS_D1MINI)
  #define PIN_LIN_TX      D8                            // must be left open during programming and boot 
  #define PIN_LIN_RX      D7
  //#define PIN_TXEN      D3
  #define PIN_TOGGLE      D1
  #define PIN_ERROR       D2
  #define SERIAL_CONSOLE  Serial1                       // Use Tx-only UART1 on pin D4 via UART<->USB adapter

////////////////////
// Nucleo-STM32L432KC settings
////////////////////
#elif defined(ARDUINO_NUCLEO_L432KC)
  #define PIN_LIN_TX      0
  #define PIN_LIN_RX      1
  //#define PIN_TXEN      5
  #define PIN_TOGGLE      3
  #define PIN_ERROR       4
  #define SERIAL_CONSOLE  Serial

////////////////////
// Board not defined -> error
////////////////////
#else
  #error board not yet supported, exit!
#endif


// setup LIN node. Parameters: Rx, Tx, inverse, pause-sync, version, name, timeout, [TxEN]
#if defined(PIN_TXEN)
  LIN_Slave_SoftwareSerial  LIN(PIN_LIN_RX, PIN_LIN_TX, false, 1000, LIN_Slave_Base::LIN_V2, "Slave", 1000, PIN_TXEN);
#else
  LIN_Slave_SoftwareSerial  LIN(PIN_LIN_RX, PIN_LIN_TX, false, 1000, LIN_Slave_Base::LIN_V2, "Slave", 1000);
#endif


// call once
void setup()
{
  // open console with timeout
  #if defined(SERIAL_CONSOLE)
    SERIAL_CONSOLE.begin(115200);
    #if defined(SERIAL_CONSOLE_BEGIN_TIMEOUT) && (SERIAL_CONSOLE_BEGIN_TIMEOUT > 0)
      for (uint32_t startMillis = millis(); (!SERIAL_CONSOLE) && (millis() - startMillis < SERIAL_CONSOLE_BEGIN_TIMEOUT); );
    #else
      while (!SERIAL_CONSOLE);
    #endif
  #endif // SERIAL_CONSOLE

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
  // indicate core load
  digitalWrite(PIN_TOGGLE, !digitalRead(PIN_TOGGLE));

  // on byte received, handle it
  if (LIN.available())
  {
    // call LIN slave protocol handler often
    LIN.handler();


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
      #if defined(SERIAL_CONSOLE)
        if (Type == LIN_Slave_Base::MASTER_REQUEST)
        {
          SERIAL_CONSOLE.print(LIN.nameLIN);
          SERIAL_CONSOLE.print(", request, ID=0x");
          SERIAL_CONSOLE.print(Id, HEX);
          if (error != LIN_Slave_Base::NO_ERROR)
          { 
            SERIAL_CONSOLE.print(", err=0x");
            SERIAL_CONSOLE.println(error, HEX);
          }
          else
          {
            SERIAL_CONSOLE.print(", data=");        
            for (uint8_t i=0; (i < NumData); i++)
            {
              SERIAL_CONSOLE.print("0x");
              SERIAL_CONSOLE.print((int) Data[i], HEX);
              SERIAL_CONSOLE.print(" ");
            }
            SERIAL_CONSOLE.println();
          }
        }
        else
        {
          SERIAL_CONSOLE.print(LIN.nameLIN);
          SERIAL_CONSOLE.print(", response, ID=0x");
          SERIAL_CONSOLE.print(Id, HEX);
          if (error != LIN_Slave_Base::NO_ERROR)
          { 
            SERIAL_CONSOLE.print(", err=0x");
            SERIAL_CONSOLE.println(error, HEX);
          }
          else
          {
            SERIAL_CONSOLE.print(", data=");        
            for (uint8_t i=0; (i < NumData); i++)
            {
              SERIAL_CONSOLE.print("0x");
              SERIAL_CONSOLE.print((int) Data[i], HEX);
              SERIAL_CONSOLE.print(" ");
            }
            SERIAL_CONSOLE.println();
          }
        }
      #endif // SERIAL_CONSOLE

      // reset state machine & error
      LIN.resetStateMachine();
      LIN.resetError();

    } // if LIN frame finished

  } // if pending byte in Rx buffer 

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
