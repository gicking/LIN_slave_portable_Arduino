/**
  \file     LIN_slave_SoftwareSerial.cpp
  \brief    LIN slave emulation library using a generic SoftwareSerial interface (if available).
  \details  This library provides a slave node emulation for a LIN bus via a generic SoftwareSerial interface.
            Due to "approximate" BREAK detection is less robust than specific board implementations! 
            For an explanation of the LIN bus and protocol e.g. see https://en.wikipedia.org/wiki/Local_Interconnect_Network
  \author   Georg Icking-Konert
*/

// include files
#include <LIN_slave_SoftwareSerial.h>



/**************************
 * PROTECTED METHODS
**************************/

/**
  \brief      Get break detection flag
  \details    Get break detection flag
  \return status of break detection
*/
bool LIN_Slave_SoftwareSerial::_getBreakFlag()
{
  // return BREAK detection flag
  return this->flagBreak;

} // LIN_Slave_SoftwareSerial::_getBreakFlag()



/**
  \brief      Clear break detection flag. Is software dependent
  \details    Clear break detection flag. Is software dependent
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
  \param[in]  Interface   serial interface for LIN. Use Serial for attachInterrupt() support
  \param[in]  Baudrate    communication speed [Baud]
  \param[in]  NameLIN     LIN node name 
*/
LIN_Slave_SoftwareSerial::LIN_Slave_SoftwareSerial(SoftwareSerial &Interface, LIN_Slave_Base::version_t Version, const char NameLIN[]) : 
  LIN_Slave_Base::LIN_Slave_Base(Version, NameLIN)
{  
  // store parameters in class variables
  this->pSerial    = &Interface;          // pointer to used HW serial

  // optional debug output
  #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 2)
    //LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
    //LIN_SLAVE_DEBUG_SERIAL.println(": LIN_Slave_SoftwareSerial()");
  #endif

} // LIN_Slave_SoftwareSerial::LIN_Slave_SoftwareSerial()



/**
  \brief      Open serial interface
  \details    Open serial interface with specified baudrate
  \param[in]  Baudrate    communication speed [Baud]
*/
void LIN_Slave_SoftwareSerial::begin(uint16_t Baudrate)
{
  // call base class method
  LIN_Slave_Base::begin(Baudrate);  

  // open serial interface incl. used pins
  ((SoftwareSerial*) (this->pSerial))->end();
  ((SoftwareSerial*) (this->pSerial))->begin(this->baudrate);
  while(!(*((SoftwareSerial*) (this->pSerial)))) { }

  // initialize variables
  this->_resetBreakFlag();

  // optional debug output
  #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 2)
    LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
    LIN_SLAVE_DEBUG_SERIAL.println(": LIN_Slave_SoftwareSerial::begin()");
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
  ((SoftwareSerial*) (this->pSerial))->end();

  // optional debug output
  #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 2)
    LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
    LIN_SLAVE_DEBUG_SERIAL.println(": LIN_Slave_SoftwareSerial::end()");
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
  if (((SoftwareSerial*) (this->pSerial))->available())
  {
    // if 0x00 received and long time since last byte, start new frame  
    if ((((SoftwareSerial*) (this->pSerial))->peek() == 0x00) && ((micros() - usLastByte) > LIN_SLAVE_SWSERIAL_INTERFRAME_PAUSE))
      this->flagBreak = true;

    // store time of this receive
    usLastByte = micros();

    // call base-class handler
    LIN_Slave_Base::handler();

  } // if byte received

} // LIN_Slave_SoftwareSerial::handler()

/*-----------------------------------------------------------------------------
    END OF FILE
-----------------------------------------------------------------------------*/