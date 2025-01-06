/**
  \file     LIN_slave_NeoHWSerial_AVR.cpp
  \brief    LIN slave emulation library using a NeoHWSerial interface of AVR
  \details  This library provides a slave node emulation for a LIN bus via a NeoHWSerial interface of AVR
            For an explanation of the LIN bus and protocol e.g. see https://en.wikipedia.org/wiki/Local_Interconnect_Network
  \note     Use NeoHWSerial for BREAK detection using framing error (standard HardwareSerial only uses inter-frame pause)
  \author   Georg Icking-Konert
*/

// include files
#include <LIN_slave_NeoHWSerial_AVR.h>

// optional file, see LIN_slave_NeoHWSerial_AVR.h
#if defined(_LIN_SLAVE_NEOHWSERIAL_AVR_H_)

// definition of static class variables (see https://stackoverflow.com/a/51091696)
bool LIN_Slave_NeoHWSerial_AVR::flagBreak[LIN_SLAVE_AVR_MAX_SERIAL];


/**************************
 * PRIVATE METHODS
**************************/

#if defined(HAVE_HWSERIAL0)
  /**
    \brief      Static callback function for AVR Serial0 error
    \details    Static callback function for AVR Serial0 error
    \param[in]  byte    received data
    \param[in]  status  UART status byte
  */
  bool LIN_Slave_NeoHWSerial_AVR::_onSerialReceive0(uint8_t byte, uint8_t status)
  {
    // on BREAK (=0x00 with framing error) set class variable
    if ((byte ==0x00) && (status & (0x01 << FE0)))
      (LIN_Slave_NeoHWSerial_AVR::flagBreak)[0] = true;

    // return true -> byte is stored in Serial0 buffer
    return true;

  } // LIN_Slave_NeoHWSerial_AVR::_onSerialReceive0()
#endif



#if defined(HAVE_HWSERIAL1)
  /**
    \brief      Static callback function for AVR Serial1 error
    \details    Static callback function for AVR Serial1 error
    \param[in]  byte    received data
    \param[in]  status  UART status byte
  */
  bool LIN_Slave_NeoHWSerial_AVR::_onSerialReceive1(uint8_t byte, uint8_t status)
  {
    // on BREAK (=0x00 with framing error) set class variable
    if ((byte ==0x00) && (status & (0x01<< FE1)))
      (LIN_Slave_NeoHWSerial_AVR::flagBreak)[1] = true;

    // return true -> byte is stored in Serial1 buffer
    return true;

  } // LIN_Slave_NeoHWSerial_AVR::_onSerialReceive1()
#endif



#if defined(HAVE_HWSERIAL2)
  /**
    \brief      Static callback function for AVR Serial2 error
    \details    Static callback function for AVR Serial2 error
    \param[in]  byte    received data
    \param[in]  status  UART status byte
  */
  bool LIN_Slave_NeoHWSerial_AVR::_onSerialReceive2(uint8_t byte, uint8_t status)
  {
    // on BREAK (=0x00 with framing error) set class variable
    if ((byte ==0x00) && (status & (0x01<< FE2)))
      (LIN_Slave_NeoHWSerial_AVR::flagBreak)[2] = true;

    // return true -> byte is stored in Serial2 buffer
    return true;

  } // LIN_Slave_NeoHWSerial_AVR::_onSerialReceive2()
#endif



#if defined(HAVE_HWSERIAL3)
  /**
    \brief      Static callback function for AVR Serial3 error
    \details    Static callback function for AVR Serial3 error
    \param[in]  byte    received data
    \param[in]  status  UART status byte
  */
  bool LIN_Slave_NeoHWSerial_AVR::_onSerialReceive3(uint8_t byte, uint8_t status)
  {
    // on BREAK (=0x00 with framing error) set class variable
    if ((byte ==0x00) && (status & (0x01<< FE3)))
      (LIN_Slave_NeoHWSerial_AVR::flagBreak)[3] = true;

    // return true -> byte is stored in Serial3 buffer
    return true;

  } // LIN_Slave_NeoHWSerial_AVR::_onSerialReceive3()
#endif



/**************************
 * PROTECTED METHODS
**************************/

/**
  \brief      Get break detection flag
  \details    Get break detection flag. Is hardware dependent
  \return status of break detection
*/
bool LIN_Slave_NeoHWSerial_AVR::_getBreakFlag()
{
  // return BREAK detection flag of respective Serialx
  return (LIN_Slave_NeoHWSerial_AVR::flagBreak)[LIN_Slave_NeoHWSerial_AVR::idxSerial];

} // LIN_Slave_NeoHWSerial_AVR::_getBreakFlag()



/**
  \brief      Clear break detection flag. Is hardware dependent
  \details    Clear break detection flag. Is hardware dependent
*/
void LIN_Slave_NeoHWSerial_AVR::_resetBreakFlag()
{
  // clear BREAK detection flag of respective Serialx
  (LIN_Slave_NeoHWSerial_AVR::flagBreak)[LIN_Slave_NeoHWSerial_AVR::idxSerial] = false;

} // LIN_Slave_NeoHWSerial_AVR::_resetBreakFlag()



/**************************
 * PUBLIC METHODS
**************************/

/**
  \brief      Constructor for LIN node class using AVR NeoHWSerial
  \details    Constructor for LIN node class for using AVR NeoHWSerial. Inherit all methods from LIN_Slave_Base, only different constructor
  \param[in]  Interface       serial interface for LIN. Use NeoHWSerial for attachInterrupt() support
  \param[in]  Version         LIN protocol version (default = v2)
  \param[in]  NameLIN         LIN node name (default = "Slave")
  \param[in]  TimeoutRx       timeout [us] for bytes in frame (default = 1500)
  \param[in]  PinTxEN     optional Tx enable pin (high active) e.g. for LIN via RS485 (default = -127/none)
*/
LIN_Slave_NeoHWSerial_AVR::LIN_Slave_NeoHWSerial_AVR(NeoHWSerial &Interface, LIN_Slave_Base::version_t Version, 
  const char NameLIN[], uint32_t TimeoutRx, const int8_t PinTxEN) : 
  LIN_Slave_Base::LIN_Slave_Base(Version, NameLIN, TimeoutRx, PinTxEN)
{  
  // store parameters in class variables
  this->pSerial    = &Interface;          // pointer to used HW serial

  // optional debug output
  #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 2)
    LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
    LIN_SLAVE_DEBUG_SERIAL.println(": LIN_Slave_NeoHWSerial_AVR()");
    LIN_SLAVE_DEBUG_SERIAL.flush();
  #endif

} // LIN_Slave_NeoHWSerial_AVR::LIN_Slave_NeoHWSerial_AVR()



/**
  \brief      Open serial interface
  \details    Open serial interface with specified baudrate
  \param[in]  Baudrate    communication speed [Baud] (default = 19200)
*/
void LIN_Slave_NeoHWSerial_AVR::begin(uint16_t Baudrate)
{
  // call base class method
  LIN_Slave_Base::begin(Baudrate);  

  // open serial interface incl. used pins
  pSerial->end();
  pSerial->begin(this->baudrate);
  while(!(*pSerial)) { }

  // Attach corresponding error callback to Serialx receive handler
  #if defined(HAVE_HWSERIAL0)
    if (pSerial == &NeoSerial)
    { 
      LIN_Slave_NeoHWSerial_AVR::idxSerial = 0;
      pSerial->attachInterrupt(LIN_Slave_NeoHWSerial_AVR::_onSerialReceive0);
    }
  #endif
  #if defined(HAVE_HWSERIAL1)
    if (pSerial == &NeoSerial1)
    { 
      LIN_Slave_NeoHWSerial_AVR::idxSerial = 1;
      pSerial->attachInterrupt(LIN_Slave_NeoHWSerial_AVR::_onSerialReceive1);
    }
  #endif
  #if defined(HAVE_HWSERIAL2)
    if (pSerial == &NeoSerial2)
    { 
      LIN_Slave_NeoHWSerial_AVR::idxSerial = 2;
      pSerial->attachInterrupt(LIN_Slave_NeoHWSerial_AVR::_onSerialReceive2);
    }
  #endif
  #if defined(HAVE_HWSERIAL3)
    if (pSerial == &NeoSerial3)
    { 
      LIN_Slave_NeoHWSerial_AVR::idxSerial = 3;
      pSerial->attachInterrupt(LIN_Slave_NeoHWSerial_AVR::_onSerialReceive3);
    }
  #endif

  // initialize variables
  this->_resetBreakFlag();

  // optional debug output
  #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 2)
    LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
    LIN_SLAVE_DEBUG_SERIAL.println(": LIN_Slave_NeoHWSerial_AVR::begin()");
    LIN_SLAVE_DEBUG_SERIAL.flush();
  #endif

} // LIN_Slave_NeoHWSerial_AVR::begin()



/**
  \brief      Close serial interface
  \details    Close serial interface. Here dummy!
*/
void LIN_Slave_NeoHWSerial_AVR::end()
{
  // call base class method
  LIN_Slave_Base::end();
    
  // close serial interface
  pSerial->end();

  // optional debug output
  #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 2)
    LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
    LIN_SLAVE_DEBUG_SERIAL.println(": LIN_Slave_NeoHWSerial_AVR::end()");
    LIN_SLAVE_DEBUG_SERIAL.flush();
  #endif

} // LIN_Slave_NeoHWSerial_AVR::end()

#endif // ARDUINO_ARCH_AVR

/*-----------------------------------------------------------------------------
    END OF FILE
-----------------------------------------------------------------------------*/
