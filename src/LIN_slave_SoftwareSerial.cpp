/**
  \file     LIN_slave_SoftwareSerial.cpp
  \brief    LIN slave emulation library using a generic SoftwareSerial interface (if available).
  \details  This library provides a slave node emulation for a LIN bus via a generic SoftwareSerial interface.
            Due to "approximate" BREAK detection is less robust than specific board implementations! 
            For an explanation of the LIN bus and protocol e.g. see https://en.wikipedia.org/wiki/Local_Interconnect_Network
  \author   Georg Icking-Konert
*/

// assert platform which supports SoftwareSerial. Note: ARDUINO_ARCH_ESP32 requires library ESPSoftwareSerial
#if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32) 

// include files
#include <LIN_slave_SoftwareSerial.h>


/**************************
 * PROTECTED METHODS
**************************/

/**
  \brief      Get break detection flag
  \details    Get break detection flag. Is hardware dependent
  \return status of break detection
*/
bool LIN_Slave_SoftwareSerial::_getBreakFlag()
{
  // return BREAK detection flag
  return this->flagBreak;

} // LIN_Slave_SoftwareSerial::_getBreakFlag()



/**
  \brief      Clear break detection flag
  \details    Clear break detection flag
*/
void LIN_Slave_SoftwareSerial::_resetBreakFlag()
{
  // clear BREAK detection flag
  this->flagBreak = false;

} // LIN_Slave_SoftwareSerial::_resetBreakFlag()



/**************************
 * PUBLIC METHODS
**************************/

/**
  \brief      Constructor for LIN node class using generic SoftwareSerial
  \details    Constructor for LIN node class for using generic SoftwareSerial. Inherit all methods from LIN_Slave_Base, only different constructor
  \param[in]  PinRx         GPIO used for reception
  \param[in]  PinTx         GPIO used for transmission
  \param[in]  InverseLogic  use inverse logic (default = false)
  \param[in]  Version     LIN protocol version (default = v2)
  \param[in]  NameLIN     LIN node name (default = "Slave")
  \param[in]  MinFramePause   min. inter-frame pause [us] to detect new frame (default = 1000)
  \param[in]  TimeoutRx       timeout [us] for bytes in frame (default = 1500)
  \param[in]  PinTxEN     optional Tx enable pin (high active) e.g. for LIN via RS485 (default = -127/none)
*/
LIN_Slave_SoftwareSerial::LIN_Slave_SoftwareSerial(uint8_t PinRx, uint8_t PinTx, bool InverseLogic, LIN_Slave_Base::version_t Version, 
  const char NameLIN[], uint16_t MinFramePause, uint32_t TimeoutRx, const int8_t PinTxEN) : 
  LIN_Slave_Base(Version, NameLIN, TimeoutRx, PinTxEN), SWSerial(PinRx, PinTx, InverseLogic)
{  
  // store parameters in class variables
  this->pinRx = PinRx;
  this->pinTx = PinTx;
  this->inverseLogic = InverseLogic;
  this->minFramePause = MinFramePause;
  
  // optional debug output 
  #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 2)
    LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
    LIN_SLAVE_DEBUG_SERIAL.println(": LIN_Slave_SoftwareSerial()");
  #endif

} // LIN_Slave_SoftwareSerial::LIN_Slave_SoftwareSerial()



/**
  \brief      Open serial interface
  \details    Open serial interface with specified baudrate
  \param[in]  Baudrate    communication speed [Baud] (default = 19200)
*/
void LIN_Slave_SoftwareSerial::begin(uint16_t Baudrate)
{
  // call base class method
  LIN_Slave_Base::begin(Baudrate);  

  // open serial interface incl. used pins
  this->SWSerial.end();
  this->SWSerial.begin(this->baudrate);

  // initialize variables
  this->_resetBreakFlag();

  // optional debug output
  #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 2)
    LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
    LIN_SLAVE_DEBUG_SERIAL.println(": LIN_Slave_SoftwareSerial::begin()");
    LIN_SLAVE_DEBUG_SERIAL.flush();
  #endif

} // LIN_Slave_SoftwareSerial::begin()



/**
  \brief      Close serial interface
  \details    Close serial interface. Here dummy!
*/
void LIN_Slave_SoftwareSerial::end()
{
  // call base class method
  LIN_Slave_Base::end();
    
  // close serial interface
  this->SWSerial.end();

  // optional debug output
  #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 2)
    LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
    LIN_SLAVE_DEBUG_SERIAL.println(": LIN_Slave_SoftwareSerial::end()");
    LIN_SLAVE_DEBUG_SERIAL.flush();
  #endif

} // LIN_Slave_SoftwareSerial::end()



/**
  \brief      Handle LIN protocol and call user-defined frame handlers
  \details    Handle LIN protocol and call user-defined frame handlers, both for master request and slave response frames. 
              BREAK detection is based on inter-frame timing only (Arduino doesn't store framing error) -> less reliable.
*/
void LIN_Slave_SoftwareSerial::handler()
{
  // sync frames based on inter-frame pause (not standard compliant!) 
  static uint32_t   usLastByte = 0;
  
  // byte received -> check it
  if (this->_serialAvailable())
  {
    // if 0x00 received and long time since last byte, start new frame  
    if ((this->_serialPeek() == 0x00) && ((micros() - usLastByte) > this->minFramePause))
      this->flagBreak = true;

    // store time of this receive
    usLastByte = micros();

    // call base-class handler
    LIN_Slave_Base::handler();

    // SoftwareSerial is blocking while sending -> skip reading echo
    if (this->state == LIN_Slave_Base::STATE_RECEIVING_ECHO)
    {
      this->_serialFlush();
      this->state = LIN_Slave_Base::STATE_DONE;
    }

  } // if byte received

} // LIN_Slave_SoftwareSerial::handler()


#endif // ARDUINO_ARCH_AVR || ARDUINO_ARCH_ESP8266 || ARDUINO_ARCH_ESP32

/*-----------------------------------------------------------------------------
    END OF FILE
-----------------------------------------------------------------------------*/
