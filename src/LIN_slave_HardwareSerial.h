/**
  \file     LIN_slave_HardwareSerial.h
  \brief    LIN slave emulation library using a generic HardwareSerial interface.
  \details  This library provides a slave node emulation for a LIN bus via a generic HardwareSerial interface.
            Due to "approximate" BREAK detection is less robust than specific board implementations! 
            For an explanation of the LIN bus and protocol e.g. see https://en.wikipedia.org/wiki/Local_Interconnect_Network
  \author   Georg Icking-Konert
*/

/*-----------------------------------------------------------------------------
  MODULE DEFINITION FOR MULTIPLE INCLUSION
-----------------------------------------------------------------------------*/
#ifndef _LIN_SLAVE_HW_SERIAL_H_
#define _LIN_SLAVE_HW_SERIAL_H_


/*-----------------------------------------------------------------------------
  INCLUDE FILES
-----------------------------------------------------------------------------*/

// include required libraries
#include <LIN_slave_Base.h>


/*-----------------------------------------------------------------------------
  GLOBAL CLASS
-----------------------------------------------------------------------------*/

/**
  \brief  LIN slave node class via generic HardwareSerial

  \details LIN slave node class via generic HardwareSerial.
*/
class LIN_Slave_HardwareSerial : public LIN_Slave_Base
{
  // PRIVATE VARIABLES
  private:

    bool                  flagBreak;                  //!< a break was detected, is set in handle
    uint16_t              maxPause;                   //!< min. inter-frame pause [us] to start new frame (not standard compliant!)


  // PROTECTED METHODS
  protected:

    /// @brief Get break detection flag
    bool _getBreakFlag(void);

    /// @brief Clear break detection flag
    void _resetBreakFlag(void);


  // PUBLIC METHODS
  public:

    /// @brief Class constructor
    LIN_Slave_HardwareSerial(HardwareSerial &Interface, LIN_Slave_Base::version_t Version, const char NameLIN[], uint16_t MaxPause=500);
     
    /// @brief Open serial interface
    void begin(uint16_t Baudrate);
    
    /// @brief Close serial interface
    void end(void);

    /// @brief Handle LIN protocol and call user-defined frame handlers
    virtual void handler(void);

}; // class LIN_Slave_HardwareSerial


/*-----------------------------------------------------------------------------
    END OF MODULE DEFINITION FOR MULTIPLE INLUSION
-----------------------------------------------------------------------------*/
#endif // _LIN_SLAVE_HW_SERIAL_H_

/*-----------------------------------------------------------------------------
    END OF FILE
-----------------------------------------------------------------------------*/