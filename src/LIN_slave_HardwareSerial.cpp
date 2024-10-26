/**
  \file     LIN_slave_HardwareSerial.cpp
  \brief    LIN slave emulation library using a generic HardwareSerial interface.
  \details  This library provides a slave node emulation for a LIN bus via a generic HardwareSerial interface.
            Due to "approximate" BREAK detection is less robust than specific board implementations! 
            For an explanation of the LIN bus and protocol e.g. see https://en.wikipedia.org/wiki/Local_Interconnect_Network
  \author   Georg Icking-Konert
*/

// include files
#include <LIN_slave_HardwareSerial.h>


/**************************
 * PROTECTED METHODS
**************************/

/**
  \brief      Get break detection flag
  \details    Get break detection flag
  \return status of break detection
*/
bool LIN_Slave_HardwareSerial::_getBreakFlag()
{
  // return BREAK detection flag
  return this->flagBreak;

} // LIN_Slave_HardwareSerial::_getBreakFlag()



/**
  \brief      Clear break detection flag. Is hardware dependent
  \details    Clear break detection flag. Is hardware dependent
*/
void LIN_Slave_HardwareSerial::_resetBreakFlag()
{
  // clear BREAK detection flag
  this->flagBreak = false;

} // LIN_Slave_HardwareSerial::_resetBreakFlag()



/**************************
 * PUBLIC METHODS
**************************/

/**
  \brief      Constructor for LIN node class using generic HardwareSerial
  \details    Constructor for LIN node class for using generic HardwareSerial. Inherit all methods from LIN_Slave_Base, only different constructor
  \param[in]  Interface   serial interface for LIN. Use Serial for attachInterrupt() support
  \param[in]  Baudrate    communication speed [Baud]
  \param[in]  NameLIN     LIN node name 
*/
LIN_Slave_HardwareSerial::LIN_Slave_HardwareSerial(HardwareSerial &Interface, LIN_Slave_Base::version_t Version, const char NameLIN[]) : 
  LIN_Slave_Base::LIN_Slave_Base(Version, NameLIN)
{  
  // store parameters in class variables
  this->pSerial    = &Interface;          // pointer to used HW serial

  // optional debug output
  #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 2)
    //LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
    //LIN_SLAVE_DEBUG_SERIAL.println(": LIN_Slave_HardwareSerial()");
  #endif

} // LIN_Slave_HardwareSerial::LIN_Slave_HardwareSerial()



/**
  \brief      Open serial interface
  \details    Open serial interface with specified baudrate
  \param[in]  Baudrate    communication speed [Baud]
*/
void LIN_Slave_HardwareSerial::begin(uint16_t Baudrate)
{
  // call base class method
  LIN_Slave_Base::begin(Baudrate);  

  // open serial interface incl. used pins
  ((HardwareSerial*) (this->pSerial))->end();
  ((HardwareSerial*) (this->pSerial))->begin(this->baudrate);
  while(!(*((HardwareSerial*) (this->pSerial)))) { }

  // initialize variables
  this->_resetBreakFlag();

  // optional debug output
  #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 2)
    LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
    LIN_SLAVE_DEBUG_SERIAL.println(": LIN_Slave_HardwareSerial::begin()");
  #endif

} // LIN_Slave_HardwareSerial::begin()



/**
  \brief      Close serial interface
  \details    Close serial interface. Here dummy!
*/
void LIN_Slave_HardwareSerial::end()
{
  // call base class method
  LIN_Slave_Base::end();
    
  // close serial interface
  ((HardwareSerial*) (this->pSerial))->end();

  // optional debug output
  #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 2)
    LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
    LIN_SLAVE_DEBUG_SERIAL.println(": LIN_Slave_HardwareSerial::end()");
  #endif

} // LIN_Slave_HardwareSerial::end()



/**
  \brief      Handle LIN protocol and call user-defined frame handlers
  \details    Handle LIN protocol and call user-defined frame handlers, both for master request and slave response frames. 
              BREAK detection is based on inter-frame timing only (Arduino doesn't store framing error) -> less reliable.
*/
void LIN_Slave_HardwareSerial::handler()
{
  // sync frames based on inter-frame pause (not standard compliant!) 
  static uint32_t   usLastByte = 0;
  
  // byte received -> check it
  if (((HardwareSerial*) (this->pSerial))->available())
  {
    // if 0x00 received and long time since last byte, start new frame  
    if ((((HardwareSerial*) (this->pSerial))->peek() == 0x00) && ((micros() - usLastByte) > LIN_SLAVE_HWSERIAL_INTERFRAME_PAUSE))
      this->flagBreak = true;

    // store time of this receive
    usLastByte = micros();

    // call base-class handler
    LIN_Slave_Base::handler();

  } // if byte received

} // LIN_Slave_HardwareSerial::handler()

/*-----------------------------------------------------------------------------
    END OF FILE
-----------------------------------------------------------------------------*/