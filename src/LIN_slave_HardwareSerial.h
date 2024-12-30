/**
  \file     LIN_slave_HardwareSerial.h
  \brief    LIN slave emulation library using a generic HardwareSerial interface.
  \details  This library provides a slave node emulation for a LIN bus via a generic HardwareSerial interface.
            Due to "approximate" BREAK detection is less robust than specific board implementations! 
            For an explanation of the LIN bus and protocol e.g. see https://en.wikipedia.org/wiki/Local_Interconnect_Network
  \author   Georg Icking-Konert
*/

// for AVR platform use NeoHWSerial or enable this file and disable LIN_slave_NeoHWSerial_AVR.*
#if !defined(ARDUINO_ARCH_AVR)
//if (1)

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
  // PROTECTED VARIABLES
  protected:

    HardwareSerial        *pSerial;             //!< pointer to serial interface used for LIN
    bool                  flagBreak;            //!< a break was detected, is set in handle
    uint16_t              minFramePause;        //!< min. inter-frame pause [us] to start new frame (not standard compliant!)


  // PROTECTED METHODS
  protected:

    /// @brief Get break detection flag
    virtual bool _getBreakFlag(void);

    /// @brief Clear break detection flag
    void _resetBreakFlag(void);


    /// @brief check if a byte is available in Rx buffer
    inline bool _serialAvailable(void) { return pSerial->available(); }

    /// @brief peek next byte from Rx buffer
    inline uint8_t _serialPeek(void) { return pSerial->peek(); }

    /// @brief read next byte from Rx buffer
    inline uint8_t _serialRead(void) { return pSerial->read(); }

    /// @brief write bytes to Tx buffer
    inline void _serialWrite(uint8_t buf[], uint8_t num) { pSerial->write(buf, num); }

    /// @brief flush Tx buffer
    inline void _serialFlush(void) { pSerial->flush(); }


  // PUBLIC METHODS
  public:

    /// @brief Class constructor
    LIN_Slave_HardwareSerial(HardwareSerial &Interface, LIN_Slave_Base::version_t Version = LIN_Slave_Base::LIN_V2, 
      const char NameLIN[] = "Slave", uint16_t MinFramePause=1000L, uint32_t TimeoutRx = 1500L);
     
    /// @brief Open serial interface
    void begin(uint16_t Baudrate = 19200);
    
    /// @brief Close serial interface
    void end(void);

    /// @brief Handle LIN protocol and call user-defined frame handlers
    virtual void handler(void);

}; // class LIN_Slave_HardwareSerial


/*-----------------------------------------------------------------------------
    END OF MODULE DEFINITION FOR MULTIPLE INLUSION
-----------------------------------------------------------------------------*/
#endif // _LIN_SLAVE_HW_SERIAL_H_

#endif // !ARDUINO_ARCH_AVR

/*-----------------------------------------------------------------------------
    END OF FILE
-----------------------------------------------------------------------------*/
