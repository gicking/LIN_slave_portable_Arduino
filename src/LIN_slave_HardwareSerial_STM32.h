/**
  \file     LIN_slave_HardwareSerial_STM32.h
  \brief    LIN slave emulation library using a STM32 HardwareSerial interface.
  \details  This library provides a slave node emulation for a LIN bus via a STM32 HardwareSerial interface.
            Due to "approximate" BREAK detection is less robust than specific board implementations! 
            For an explanation of the LIN bus and protocol e.g. see https://en.wikipedia.org/wiki/Local_Interconnect_Network
  \author   Georg Icking-Konert
*/

// assert STM32 platform
#if defined(ARDUINO_ARCH_STM32)

/*-----------------------------------------------------------------------------
  MODULE DEFINITION FOR MULTIPLE INCLUSION
-----------------------------------------------------------------------------*/
#ifndef _LIN_SLAVE_HW_SERIAL_STM32_H_
#define _LIN_SLAVE_HW_SERIAL_STM32_H_


/*-----------------------------------------------------------------------------
  INCLUDE FILES
-----------------------------------------------------------------------------*/

// include required libraries
#include <LIN_slave_Base.h>
#include <stm32_def.h>


/*-----------------------------------------------------------------------------
  GLOBAL MACROS
-----------------------------------------------------------------------------*/

/// account for breaking change in v3.0.0 for Serial, see https://github.com/stm32duino/Arduino_Core_STM32/releases/tag/3.0.0
#if defined(STM32_CORE_VERSION_MAJOR)
  #if (STM32_CORE_VERSION_MAJOR >= 3)
    #define HWSERIAL Uart
  #else
    #define HWSERIAL HardwareSerial
  #endif
#else
  #error "STM32_CORE_VERSION_MAJOR not defined"
#endif


/*-----------------------------------------------------------------------------
  GLOBAL CLASS
-----------------------------------------------------------------------------*/

/**
  \brief  LIN slave node class via STM32 HardwareSerial

  \details LIN slave node class via STM32 HardwareSerial.
*/
class LIN_Slave_HardwareSerial_STM32 : public LIN_Slave_Base
{
  // PROTECTED VARIABLES
  protected:

    HWSERIAL              *pSerial;             //!< pointer to serial interface used for LIN
    bool                  flagBreak;            //!< a break was detected, is set in handle
    uint16_t              minFramePause;        //!< min. inter-frame pause [us] to start new frame (not standard compliant!)


  // PROTECTED METHODS
  protected:

    /// @brief Get break detection flag
    virtual bool _getBreakFlag(void);

    /// @brief Clear break detection flag
    void _resetBreakFlag(void);


    /// @brief peek next byte from Rx buffer
    inline uint8_t _serialPeek(void) { return this->pSerial->peek(); }

    /// @brief read next byte from Rx buffer
    inline uint8_t _serialRead(void) { return this->pSerial->read(); }

    /// @brief write bytes to Tx buffer
    inline void _serialWrite(uint8_t buf[], uint8_t num) { this->pSerial->write(buf, num); }


  // PUBLIC METHODS
  public:

    /// @brief Class constructor
    LIN_Slave_HardwareSerial_STM32(HWSERIAL &Interface, uint16_t MinFramePause=1000L, 
      LIN_Slave_Base::version_t Version = LIN_Slave_Base::LIN_V2, const char NameLIN[] = "Slave", uint32_t TimeoutRx = 1500L, const int8_t PinTxEN = INT8_MIN);
     
    /// @brief Open serial interface
    void begin(uint16_t Baudrate = 19200);
    
    /// @brief Close serial interface
    void end(void);

    /// @brief check if a byte is available in Rx buffer
    inline bool available(void) { return this->pSerial->available(); }

    /// @brief Handle LIN protocol and call user-defined frame handlers
    virtual void handler(void);

}; // class LIN_Slave_HardwareSerial_STM32


/*-----------------------------------------------------------------------------
    END OF MODULE DEFINITION FOR MULTIPLE INLUSION
-----------------------------------------------------------------------------*/
#endif // _LIN_SLAVE_HW_SERIAL_STM32_H_

#endif // ARDUINO_ARCH_STM32

/*-----------------------------------------------------------------------------
    END OF FILE
-----------------------------------------------------------------------------*/
