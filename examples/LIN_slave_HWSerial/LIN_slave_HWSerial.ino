/*********************

Example code for LIN slave node using HardwareSerial interface

Optional Tx direction switching for RS485 interface (e.g. MAX485) is by defining 'PIN_TXEN'. 
In this case, permanently enable Rx (REN=GND) for receiving echo

Note:
  - frame synchronization for some implementations is via inter-frame pause -> not standard compliant! For details see README.md
  - handling of frames can be done inside callback functions. Console output below is optional
  - on AVR 
    - propose to use NeoHWSerial instead (uncomment USE_NEOSERIAL in file LIN_slave_NeoHWSerial_AVR.h)
    - to avoid linker conflict, only use NeoSerial in your code, not Serial

Tested boards:
  - Arduino Mega 2560       https://docs.arduino.cc/hardware/mega-2560/
  - Arduino Due             https://docs.arduino.cc/hardware/due/
  - Arduino Nano Every      https://docs.arduino.cc/hardware/nano-every/
  x Adafruit Trinket (no HW-Serial available)   https://www.adafruit.com/product/1501
  - Arduino Uno R4 Minima   https://docs.arduino.cc/hardware/uno-r4-minima/ 
  - Arduino Nano ESP32-S3   https://docs.arduino.cc/hardware/nano-esp32/
  - ESP32 WROOM-32UE        https://documentation.espressif.com/esp32-wroom-32e_esp32-wroom-32ue_datasheet_en.pdf
  - ESP32-C3 Super Mini     ttps://www.sudo.is/docs/esphome/boards/esp32c3supermini/
  - ESP8266 D1 Mini         https://www.wemos.cc/en/latest/d1/d1_mini.html
  - Nucleo-STM32L432KC      https://www.st.com/en/evaluation-tools/nucleo-l432kc.html

**********************/

////////////////////
// Arduino Mega settings
////////////////////
#if defined(ARDUINO_AVR_MEGA2560)

  #include <NeoHWSerial.h>                              // use NeoHWSerial to avoid linker conflict for UART ISRs
  #include <LIN_slave_NeoHWSerial_AVR.h>                // matching LIN slave header

  //#define PIN_TXEN            17                        // optional Tx direction pin (=DE) for RS485 physical I/F. Comment out for LIN I/F 
  #define PIN_TOGGLE          30                        // pin to show CPU idle
  #define PIN_ERROR           32                        // LIN error status pin (high=error)
  #define SERIAL_CONSOLE      NeoSerial                 // serial I/F for console output (comment for no output)

  // setup LIN node. Parameters: interface, version, name, timeout, [TxEN]
  #if defined(PIN_TXEN)
    LIN_Slave_NeoHWSerial_AVR  LIN(NeoSerial1, LIN_Slave_Base::LIN_V2, "Slave", 1000, PIN_TXEN);
  #else
    LIN_Slave_NeoHWSerial_AVR  LIN(NeoSerial1, LIN_Slave_Base::LIN_V2, "Slave", 1000);
  #endif


////////////////////
// Arduino Due settings
////////////////////
#elif defined(ARDUINO_SAM_DUE)

  #include <LIN_slave_HardwareSerial.h>                 // matching LIN slave header

  //#define PIN_TXEN            17                        // optional Tx direction pin (=DE) for RS485 physical I/F. Comment out for LIN I/F 
  #define PIN_TOGGLE          30                        // pin to show CPU idle
  #define PIN_ERROR           32                        // LIN error status pin (high=error)
  #define SERIAL_CONSOLE      Serial                    // serial I/F for console output (comment for no output). Use SerialUSB for native USB port

  // setup LIN node. Parameters: interface, pause-sync, version, name, timeout, [TxEN]
  #if defined(PIN_TXEN)
    LIN_Slave_HardwareSerial    LIN(Serial1, 1000, LIN_Slave_Base::LIN_V2, "Slave", 1000, PIN_TXEN);
  #else
    LIN_Slave_HardwareSerial    LIN(Serial1, 1000, LIN_Slave_Base::LIN_V2, "Slave", 1000);
  #endif


////////////////////
// Arduino Nano Every settings
////////////////////
#elif defined(ARDUINO_AVR_NANO_EVERY)

  #include <LIN_slave_HardwareSerial.h>                 // matching LIN slave header

  //#define PIN_TXEN            7                         // optional Tx direction pin (=DE) for RS485 physical I/F. Comment out for LIN I/F 
  #define PIN_TOGGLE          4                         // pin to show CPU idle
  #define PIN_ERROR           6                         // LIN error status pin (high=error)
  #define SERIAL_CONSOLE      Serial                    // serial I/F for console output (comment for no output)

  // setup LIN node. Parameters: interface, pause-sync, version, name, timeout, [TxEN]
  #if defined(PIN_TXEN)
    LIN_Slave_HardwareSerial    LIN(Serial1, 1000, LIN_Slave_Base::LIN_V2, "Slave", 1000, PIN_TXEN);
  #else
    LIN_Slave_HardwareSerial    LIN(Serial1, 1000, LIN_Slave_Base::LIN_V2, "Slave", 1000);
  #endif


////////////////////
// Arduino Uno R4 Minima settings
////////////////////
#elif defined(ARDUINO_UNOR4_MINIMA)

  #include <LIN_slave_HardwareSerial.h>                 // matching LIN slave header

  //#define PIN_TXEN            10                        // optional Tx direction pin (=DE) for RS485 physical I/F. Comment out for LIN I/F 
  #define PIN_TOGGLE          4                         // pin to show CPU idle
  #define PIN_ERROR           6                         // LIN error status pin (high=error)
  #define SERIAL_CONSOLE      Serial                    // serial I/F for console output (comment for no output)

  // setup LIN node. Parameters: interface, pause-sync, version, name, timeout, [TxEN]
  #if defined(PIN_TXEN)
    LIN_Slave_HardwareSerial    LIN(Serial1, 1000, LIN_Slave_Base::LIN_V2, "Slave", 1000, PIN_TXEN);
  #else
    LIN_Slave_HardwareSerial    LIN(Serial1, 1000, LIN_Slave_Base::LIN_V2, "Slave", 1000);
  #endif


////////////////////
// Arduino Nano ESP32 board settings (using Arduino ESP32 core)
////////////////////
#elif defined(ARDUINO_NANO_ESP32)

  #include <LIN_slave_HardwareSerial_ESP32.h>           // matching LIN slave header

  //#define PIN_TXEN            10                        // optional Tx direction pin (=DE) for RS485 physical I/F. Comment out for LIN I/F 
  #define PIN_LIN_TX          3                         // LIN transmit pin
  #define PIN_LIN_RX          4                         // LIN receive pin
  #define PIN_TOGGLE          5                         // pin to show CPU idle
  #define PIN_ERROR           6                         // LIN error status pin (high=error)
  #define SERIAL_CONSOLE      Serial                    // serial I/F for console output (comment for no output)

  // setup LIN node. Parameters: interface, Rx, Tx, version, name, timeout, [TxEN]
  #if defined(PIN_TXEN)
    LIN_Slave_HardwareSerial_ESP32  LIN(Serial2, PIN_LIN_RX, PIN_LIN_TX, LIN_Slave_Base::LIN_V2, "Slave", PIN_TXEN);
  #else
    LIN_Slave_HardwareSerial_ESP32  LIN(Serial2, PIN_LIN_RX, PIN_LIN_TX, LIN_Slave_Base::LIN_V2, "Slave");
  #endif


////////////////////
// Espressif ESP32-WROOM-32UE board settings (using Espressif ESP32 core)
////////////////////
#elif defined(ARDUINO_ESP32_WROOM_DA)

  #include <LIN_slave_HardwareSerial_ESP32.h>           // matching LIN slave header

  //#define PIN_TXEN            21                        // optional Tx direction pin (=DE) for RS485 physical I/F. Comment out for LIN I/F 
  #define PIN_LIN_TX          17                        // LIN transmit pin
  #define PIN_LIN_RX          16                        // LIN receive pin
  #define PIN_TOGGLE          19                        // pin to show CPU idle
  #define PIN_ERROR           18                        // LIN error status pin (high=error)
  #define SERIAL_CONSOLE      Serial                    // serial I/F for console output (comment for no output)

  // setup LIN node. Parameters: interface, Rx, Tx, version, name, timeout, [TxEN]
  #if defined(PIN_TXEN)
    LIN_Slave_HardwareSerial_ESP32  LIN(Serial2, PIN_LIN_RX, PIN_LIN_TX, LIN_Slave_Base::LIN_V2, "Slave", PIN_TXEN);
  #else
    LIN_Slave_HardwareSerial_ESP32  LIN(Serial2, PIN_LIN_RX, PIN_LIN_TX, LIN_Slave_Base::LIN_V2, "Slave");
  #endif


////////////////////
// ESP32-C3 Super Mini board settings (using Espressif ESP32 core)
////////////////////
#elif defined(ARDUINO_ESP32C3_DEV)

  #include <LIN_slave_HardwareSerial_ESP32.h>           // matching LIN slave header

  //#define PIN_TXEN            10                        // optional Tx direction pin (=DE) for RS485 physical I/F. Comment out for LIN I/F 
  #define PIN_LIN_TX          1                         // LIN transmit pin
  #define PIN_LIN_RX          0                         // LIN receive pin
  #define PIN_TOGGLE          3                         // pin to show CPU idle
  #define PIN_ERROR           8                         // LIN error status pin (high=error)
  #define SERIAL_CONSOLE      Serial                    // serial I/F for console output (comment for no output). Enable CDC (native USB)

  // setup LIN node. Parameters: interface, Rx, Tx, version, name, timeout, [TxEN]
  #if defined(PIN_TXEN)
    LIN_Slave_HardwareSerial_ESP32  LIN(Serial1, PIN_LIN_RX, PIN_LIN_TX, LIN_Slave_Base::LIN_V2, "Slave", PIN_TXEN);
  #else
    LIN_Slave_HardwareSerial_ESP32  LIN(Serial1, PIN_LIN_RX, PIN_LIN_TX, LIN_Slave_Base::LIN_V2, "Slave");
  #endif


////////////////////
// Wemos ESP8266 D1 Mini settings
////////////////////
#elif defined(ARDUINO_ESP8266_WEMOS_D1MINI)

  #include <LIN_slave_HardwareSerial_ESP8266.h>         // matching LIN slave header

  //#define PIN_TXEN            D3                        // optional Tx direction pin (=DE) for RS485 physical I/F. Comment out for LIN I/F 
  #define PIN_TOGGLE          D1                        // pin to show CPU idle
  #define PIN_ERROR           D2                        // LIN error status pin (high=error)
  #define SERIAL_CONSOLE      Serial1                   // serial I/F for console output (comment for no output). Use Tx-only UART1 on pin D4 via UART<->USB adapter

  // setup LIN node. Swap Serial pins to use Tx=D8 & Rx=D7. Parameters: swapPins, pause-sync, version, name, timeout, TxEN
  #if defined(PIN_TXEN)
    LIN_Slave_HardwareSerial_ESP8266   LIN(true, 1000, LIN_Slave_Base::LIN_V2, "Slave", 1000, PIN_TXEN);
  #else
    LIN_Slave_HardwareSerial_ESP8266   LIN(true, 1000, LIN_Slave_Base::LIN_V2, "Slave", 1000);
  #endif


////////////////////
// Nucleo-STM32L432KC settings
////////////////////
#elif defined(ARDUINO_NUCLEO_L432KC)

  #include <LIN_slave_HardwareSerial_STM32.h>           // matching LIN slave header. Requires as of v3.0.0 core

  //#define PIN_TXEN            D5                        // optional Tx direction pin (=DE) for RS485 physical I/F. Comment out for LIN I/F 
  #define PIN_TOGGLE          D3                        // pin to show CPU idle
  #define PIN_ERROR           D4                        // LIN error status pin (high=error)
  #define SERIAL_CONSOLE      Serial                    // serial I/F for console output (comment for no output)

  Uart                        Serial1(D1, D0);          // Serial1 not always instantiated by default

  // setup LIN node. Parameters: interface, pause-sync, version, name, timeout, [TxEN]
  #if defined(PIN_TXEN)
    LIN_Slave_HardwareSerial_STM32    LIN(Serial1, 1000, LIN_Slave_Base::LIN_V2, "Slave", 1000, PIN_TXEN);
  #else
    LIN_Slave_HardwareSerial_STM32    LIN(Serial1, 1000, LIN_Slave_Base::LIN_V2, "Slave", 1000);
  #endif


// board not yet included
#else
  #error board not yet supported, exit!
#endif



// call once
void setup()
{
  // open optional console
  #if defined(SERIAL_CONSOLE)

    // Nucleo-STM32L432KC, if solder bridges for VCP via STLink have been removed 
    #if defined(ARDUINO_NUCLEO_L432KC) && (1)
      Serial2.setTx(PA_2_ALT1);   // pin A7 on Nucleo-STM32L432KC / uC pin 8
      Serial2.setRx(PA_3_ALT1);   // pin A2 on Nucleo-STM32L432KC / uC pin 9. Optional Rx pin
    #endif 

    SERIAL_CONSOLE.begin(115200);
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
