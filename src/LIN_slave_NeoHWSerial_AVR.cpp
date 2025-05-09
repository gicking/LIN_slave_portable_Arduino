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

// optional on AVR, see LIN_slave_NeoHWSerial_AVR.h
#if defined(_LIN_SLAVE_NEOHWSERIAL_AVR_H_)


// definition of static class variables (see https://stackoverflow.com/a/51091696)
bool LIN_Slave_NeoHWSerial_AVR::flagBreak[];


/**************************
 * PRIVATE METHODS
**************************/

#if defined(HAVE_HWSERIAL0)

  /**
    \brief      Static callback function for AVR Serial0 error
    \details    Static callback function for AVR Serial0 error
                Note: received BREAK byte is consumed here to support also sync on SYNC byte. 
    \param[in]  byte    received data
    \param[in]  status  UART status byte
  */
  bool LIN_Slave_NeoHWSerial_AVR::_onSerialReceive0(uint8_t byte, uint8_t status)
  {
    // on BREAK (=0x00 with framing error) set class variable and don't store in queue (return false)
    if ((byte ==0x00) && (status & (0x01<< FE0)))
    {
      // set BREAK flag for Serial0
      (LIN_Slave_NeoHWSerial_AVR::flagBreak)[0] = true;

      // print debug message
      DEBUG_PRINT_STATIC(3, "Rx=0x%02X, BRK", byte);

      // return true -> byte is dropped
      return false;
    }

    // print debug message
    DEBUG_PRINT_STATIC(3, "Rx=0x%02X", byte);

    // return true -> byte is stored in Serial0 buffer
    return true;

  } // LIN_Slave_NeoHWSerial_AVR::_onSerialReceive0()

#endif // HAVE_HWSERIAL0


#if defined(HAVE_HWSERIAL1)

  /**
    \brief      Static callback function for AVR Serial1 error
    \details    Static callback function for AVR Serial1 error
                Note: received BREAK byte is consumed here to support also sync on SYNC byte. 
    \param[in]  byte    received data
    \param[in]  status  UART status byte
  */
  bool LIN_Slave_NeoHWSerial_AVR::_onSerialReceive1(uint8_t byte, uint8_t status)
  { 
    // on BREAK (=0x00 with framing error) set class variable and don't store in queue (return false)
    if ((byte ==0x00) && (status & (0x01<< FE0)))
    {
      // set BREAK flag for Serial1
      (LIN_Slave_NeoHWSerial_AVR::flagBreak)[1] = true;

      // print debug message
      DEBUG_PRINT_STATIC(3, "Rx=0x%02X, BRK", byte);

      // return true -> byte is dropped
      return false;
    }

    // print debug message
    DEBUG_PRINT_STATIC(3, "Rx=0x%02X", byte);

    // return true -> byte is stored in Serial1 buffer
    return true;

  } // LIN_Slave_NeoHWSerial_AVR::_onSerialReceive1()

#endif // HAVE_HWSERIAL1


#if defined(HAVE_HWSERIAL2)

  /**
    \brief      Static callback function for AVR Serial2 error
    \details    Static callback function for AVR Serial2 error
                Note: received BREAK byte is consumed here to support also sync on SYNC byte. 
    \param[in]  byte    received data
    \param[in]  status  UART status byte
  */
  bool LIN_Slave_NeoHWSerial_AVR::_onSerialReceive2(uint8_t byte, uint8_t status)
  {
    // on BREAK (=0x00 with framing error) set class variable and don't store in queue (return false)
    if ((byte ==0x00) && (status & (0x01<< FE0)))
    {
      // set BREAK flag for Serial2
      (LIN_Slave_NeoHWSerial_AVR::flagBreak)[2] = true;

      // print debug message
      DEBUG_PRINT_STATIC(3, "Rx=0x%02X, BRK", byte);

      // return true -> byte is dropped
      return false;
    }

    // print debug message
    DEBUG_PRINT_STATIC(3, "Rx=0x%02X", byte);

    // return true -> byte is stored in Serial2 buffer
    return true;

  } // LIN_Slave_NeoHWSerial_AVR::_onSerialReceive2()

#endif // HAVE_HWSERIAL2


#if defined(HAVE_HWSERIAL3)

  /**
    \brief      Static callback function for AVR Serial3 error
    \details    Static callback function for AVR Serial3 error
                Note: received BREAK byte is consumed here to support also sync on SYNC byte. 
    \param[in]  byte    received data
    \param[in]  status  UART status byte
  */
  bool LIN_Slave_NeoHWSerial_AVR::_onSerialReceive3(uint8_t byte, uint8_t status)
  {
    // on BREAK (=0x00 with framing error) set class variable and don't store in queue (return false)
    if ((byte ==0x00) && (status & (0x01<< FE0)))
    {
      // set BREAK flag for Serial3
      (LIN_Slave_NeoHWSerial_AVR::flagBreak)[3] = true;

      // print debug message
      DEBUG_PRINT_STATIC(3, "Rx=0x%02X, BRK", byte);

      // return true -> byte is dropped
      return false;
    }

    // print debug message
    DEBUG_PRINT_STATIC(3, "Rx=0x%02X", byte);

    // return true -> byte is stored in Serial3 buffer
    return true;

  } // LIN_Slave_NeoHWSerial_AVR::_onSerialReceive3()

#endif // HAVE_HWSERIAL3



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
  return (this->flagBreak)[this->idxSerial];

} // LIN_Slave_NeoHWSerial_AVR::_getBreakFlag()



/**
  \brief      Clear break detection flag. Is hardware dependent
  \details    Clear break detection flag. Is hardware dependent
*/
void LIN_Slave_NeoHWSerial_AVR::_resetBreakFlag()
{
  // clear BREAK detection flag of respective Serialx
  (this->flagBreak)[this->idxSerial] = false;

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
LIN_Slave_NeoHWSerial_AVR::LIN_Slave_NeoHWSerial_AVR(NeoHWSerial &Interface, 
  LIN_Slave_Base::version_t Version, const char NameLIN[], uint32_t TimeoutRx, const int8_t PinTxEN) : 
  LIN_Slave_Base::LIN_Slave_Base(Version, NameLIN, TimeoutRx, PinTxEN)
{  
  // Debug serial initialized in begin() -> no debug output here

  // store parameters in class variables
  this->pSerial    = &Interface;          // pointer to used HW serial

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

  // open serial interface incl. used pins with optional timeout
  this->pSerial->end();
  this->pSerial->begin(this->baudrate);
  #if defined(LIN_SLAVE_LIN_PORT_TIMEOUT) && (LIN_SLAVE_LIN_PORT_TIMEOUT > 0)
    uint32_t startMillis = millis();
    while ((!(*(this->pSerial))) && (millis() - startMillis < LIN_SLAVE_LIN_PORT_TIMEOUT));
  #else
    while(!(*(this->pSerial)));
  #endif

  // Attach corresponding error callback to Serialx receive handler
  #if defined(HAVE_HWSERIAL0)
    if (this->pSerial == &NeoSerial)
    { 
      this->idxSerial = 0;
      this->pSerial->attachInterrupt(this->_onSerialReceive0);
    }
  #endif
  #if defined(HAVE_HWSERIAL1)
    if (this->pSerial == &NeoSerial1)
    { 
      this->idxSerial = 1;
      pSerial->attachInterrupt(this->_onSerialReceive1);
    }
  #endif
  #if defined(HAVE_HWSERIAL2)
    if (this->pSerial == &NeoSerial2)
    { 
      this->idxSerial = 2;
      pSerial->attachInterrupt(this->_onSerialReceive2);
    }
  #endif
  #if defined(HAVE_HWSERIAL3)
    if (this->pSerial == &NeoSerial3)
    { 
      this->idxSerial = 3;
      pSerial->attachInterrupt(this->_onSerialReceive3);
    }
  #endif

  // initialize variables
  this->_resetBreakFlag();

  // optional debug output (debug level 2)
  #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 2)
    LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
    LIN_SLAVE_DEBUG_SERIAL.println(": LIN_Slave_NeoHWSerial_AVR::begin()");
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
  this->pSerial->end();

  // print debug message
  DEBUG_PRINT(2, " ");

} // LIN_Slave_NeoHWSerial_AVR::end()

#endif // ARDUINO_ARCH_AVR

/*-----------------------------------------------------------------------------
    END OF FILE
-----------------------------------------------------------------------------*/
