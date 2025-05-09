/*********************

Example code for LIN slave node using AVR NeoHWSerial interface

Note:
  - relies on NeoHWSerial for BREAK detection (install via Library manager)
  - to avoid linker conflict, only use NeoSerial in your code, not Serial
  - to use standard HardwareSerial (less robust), comment out USE_NEOSERIAL in file LIN_slave_NeoHWSerial_AVR.h
  - handling of frames can be done inside callback functions. Console output below is optional 

Supported (=successfully tested) boards:
 - Arduino Mega 2560      https://store.arduino.cc/products/arduino-mega-2560-rev3

**********************/

// include files
#include <NeoHWSerial.h>                      // use NeoHWSerial to avoid linker conflict for UART ISRs
#include <LIN_slave_NeoHWSerial_AVR.h>

// pin to demonstrate background operation
#define PIN_TOGGLE    30

// indicate LIN return status
#define PIN_ERROR     32

// serial I/F for console output (comment for no output)
#define SERIAL_CONSOLE  NeoSerial

// SERIAL_CONSOLE.begin() timeout [ms] (<=0 -> no timeout). Is relevant for native USB ports, if USB is not connected 
#define SERIAL_CONSOLE_BEGIN_TIMEOUT  3000


// setup LIN node. Parameters: interface, version, name, timeout, TxEN
LIN_Slave_NeoHWSerial_AVR  LIN(NeoSerial1, LIN_Slave_Base::LIN_V2, "Slave");


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
