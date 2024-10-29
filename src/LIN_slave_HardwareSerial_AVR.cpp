/**
  \file     LIN_slave_HardwareSerial_AVR.cpp
  \brief    LIN slave emulation library using a HardwareSerial interface of 8-bit AVR
  \details  This library provides a slave node emulation for a LIN bus via a HardwareSerial interface of 8-bit AVR.
            For an explanation of the LIN bus and protocol e.g. see https://en.wikipedia.org/wiki/Local_Interconnect_Network
  \author   Georg Icking-Konert
*/

// assert AVR platform
#if defined(ARDUINO_ARCH_AVR)

// include files
#include <LIN_slave_HardwareSerial_AVR.h>

// definition of static class variables (see https://stackoverflow.com/a/51091696)
bool LIN_Slave_HardwareSerial_AVR::flagBreak[LIN_SLAVE_AVR_MAX_SERIAL];


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
  bool LIN_Slave_HardwareSerial_AVR::_onSerialReceive0(uint8_t byte, uint8_t status)
  {
    // on BREAK (=0x00 with framing error) set class variable
    if ((byte ==0x00) && (status & (0x01<< FE0)))
      (LIN_Slave_HardwareSerial_AVR::flagBreak)[0] = true;

    // return true -> byte is stored in Serial0 buffer
	  return true;

  } // LIN_Slave_HardwareSerial_AVR::_onSerialReceive0()
#endif



#if defined(HAVE_HWSERIAL1)
  /**
    \brief      Static callback function for AVR Serial1 error
    \details    Static callback function for AVR Serial1 error
    \param[in]  byte    received data
    \param[in]  status  UART status byte
  */
  bool LIN_Slave_HardwareSerial_AVR::_onSerialReceive1(uint8_t byte, uint8_t status)
  {
    // on BREAK (=0x00 with framing error) set class variable
    if ((byte ==0x00) && (status & (0x01<< FE0)))
      (LIN_Slave_HardwareSerial_AVR::flagBreak)[1] = true;

    // return true -> byte is stored in Serial1 buffer
	  return true;

  } // LIN_Slave_HardwareSerial_AVR::_onSerialReceive1()
#endif



#if defined(HAVE_HWSERIAL2)
  /**
    \brief      Static callback function for AVR Serial2 error
    \details    Static callback function for AVR Serial2 error
    \param[in]  byte    received data
    \param[in]  status  UART status byte
  */
  bool LIN_Slave_HardwareSerial_AVR::_onSerialReceive2(uint8_t byte, uint8_t status)
  {
    // on BREAK (=0x00 with framing error) set class variable
    if ((byte ==0x00) && (status & (0x01<< FE0)))
      (LIN_Slave_HardwareSerial_AVR::flagBreak)[2] = true;

    // return true -> byte is stored in Serial2 buffer
	  return true;

  } // LIN_Slave_HardwareSerial_AVR::_onSerialReceive2()
#endif



#if defined(HAVE_HWSERIAL3)
  /**
    \brief      Static callback function for AVR Serial3 error
    \details    Static callback function for AVR Serial3 error
    \param[in]  byte    received data
    \param[in]  status  UART status byte
  */
  bool LIN_Slave_HardwareSerial_AVR::_onSerialReceive3(uint8_t byte, uint8_t status)
  {
    // on BREAK (=0x00 with framing error) set class variable
    if ((byte ==0x00) && (status & (0x01<< FE0)))
      (LIN_Slave_HardwareSerial_AVR::flagBreak)[3] = true;

    // return true -> byte is stored in Serial3 buffer
  	return true;

  } // LIN_Slave_HardwareSerial_AVR::_onSerialReceive3()
#endif



/**************************
 * PROTECTED METHODS
**************************/

/**
  \brief      Get break detection flag. Is hardware dependent
  \details    Get break detection flag. Is hardware dependent
  \return status of break detection
*/
bool LIN_Slave_HardwareSerial_AVR::_getBreakFlag()
{
  // return BREAK detection flag of respective Serialx
  return (LIN_Slave_HardwareSerial_AVR::flagBreak)[LIN_Slave_HardwareSerial_AVR::idxSerial];

} // LIN_Slave_HardwareSerial_AVR::_getBreakFlag()



/**
  \brief      Clear break detection flag. Is hardware dependent
  \details    Clear break detection flag. Is hardware dependent
*/
void LIN_Slave_HardwareSerial_AVR::_resetBreakFlag()
{
  // clear BREAK detection flag of respective Serialx
  (LIN_Slave_HardwareSerial_AVR::flagBreak)[LIN_Slave_HardwareSerial_AVR::idxSerial] = false;

} // LIN_Slave_HardwareSerial_AVR::_resetBreakFlag()



/**************************
 * PUBLIC METHODS
**************************/

/**
  \brief      Constructor for LIN node class using AVR HardwareSerial
  \details    Constructor for LIN node class for using AVR HardwareSerial. Inherit all methods from LIN_Slave_Base, only different constructor
  \param[in]  Interface   serial interface for LIN. Use NeoHWSerial for attachInterrupt() support
  \param[in]  Baudrate    communication speed [Baud]
  \param[in]  NameLIN     LIN node name 
*/
LIN_Slave_HardwareSerial_AVR::LIN_Slave_HardwareSerial_AVR(NeoHWSerial &Interface, LIN_Slave_Base::version_t Version, const char NameLIN[]) : 
  LIN_Slave_Base::LIN_Slave_Base(Version, NameLIN)
{  
  // store parameters in class variables
  this->pSerial    = &Interface;          // pointer to used HW serial

  // optional debug output
  #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 2)
    //LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
    //LIN_SLAVE_DEBUG_SERIAL.println(": LIN_Slave_HardwareSerial_AVR()");
  #endif

} // LIN_Slave_HardwareSerial_AVR::LIN_Slave_HardwareSerial_AVR()



/**
  \brief      Open serial interface
  \details    Open serial interface with specified baudrate
  \param[in]  Baudrate    communication speed [Baud]
*/
void LIN_Slave_HardwareSerial_AVR::begin(uint16_t Baudrate)
{
  // call base class method
  LIN_Slave_Base::begin(Baudrate);  

  // open serial interface incl. used pins
  ((NeoHWSerial*) (this->pSerial))->end();
  ((NeoHWSerial*) (this->pSerial))->begin(this->baudrate);
  while(!(*((NeoHWSerial*) (this->pSerial)))) { }

  // Attach corresponding error callback to Serialx receive handler
  #if defined(HAVE_HWSERIAL0)
    if (pSerial == &NeoSerial)
    { 
      LIN_Slave_HardwareSerial_AVR::idxSerial = 0;
      this->_resetBreakFlag();
      ((NeoHWSerial*) (this->pSerial))->attachInterrupt(LIN_Slave_HardwareSerial_AVR::_onSerialReceive0);
    }
  #endif
  #if defined(HAVE_HWSERIAL1)
    if (pSerial == &NeoSerial1)
    { 
      LIN_Slave_HardwareSerial_AVR::idxSerial = 1;
      this->_resetBreakFlag();
      ((NeoHWSerial*) (this->pSerial))->attachInterrupt(LIN_Slave_HardwareSerial_AVR::_onSerialReceive1);
    }
  #endif
  #if defined(HAVE_HWSERIAL2)
    if (pSerial == &NeoSerial2)
    { 
      LIN_Slave_HardwareSerial_AVR::idxSerial = 2;
      this->_resetBreakFlag();
      ((NeoHWSerial*) (this->pSerial))->attachInterrupt(LIN_Slave_HardwareSerial_AVR::_onSerialReceive2);
    }
  #endif
  #if defined(HAVE_HWSERIAL3)
    if (pSerial == &NeoSerial3)
    { 
      LIN_Slave_HardwareSerial_AVR::idxSerial = 3;
      this->_resetBreakFlag();
      ((NeoHWSerial*) (this->pSerial))->attachInterrupt(LIN_Slave_HardwareSerial_AVR::_onSerialReceive3);
    }
  #endif

  // optional debug output
  #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 2)
    LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
    LIN_SLAVE_DEBUG_SERIAL.println(": LIN_Slave_HardwareSerial_AVR::begin()");
  #endif

} // LIN_Slave_HardwareSerial_AVR::begin()



/**
  \brief      Close serial interface
  \details    Close serial interface. Here dummy!
*/
void LIN_Slave_HardwareSerial_AVR::end()
{
  // call base class method
  LIN_Slave_Base::end();
    
  // close serial interface
  ((NeoHWSerial*) (this->pSerial))->end();

  // optional debug output
  #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 2)
    LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
    LIN_SLAVE_DEBUG_SERIAL.println(": LIN_Slave_HardwareSerial_AVR::end()");
  #endif

} // LIN_Slave_HardwareSerial_AVR::end()

#endif // ARDUINO_ARCH_AVR

/*-----------------------------------------------------------------------------
    END OF FILE
-----------------------------------------------------------------------------*/