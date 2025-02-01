/**
  \file     LIN_slave_HardwareSerial_ESP8266.h
  \brief    LIN slave emulation library using a HardwareSerial interface of ESP8266
  \details  This library provides a slave node emulation for a LIN bus via a HardwareSerial interface of ESP8266.
            Due to "approximate" BREAK detection is less robust than specific board implementations! 
            For an explanation of the LIN bus and protocol e.g. see https://en.wikipedia.org/wiki/Local_Interconnect_Network
  \author   Georg Icking-Konert
*/

// assert ESP8266 platform
#if defined(ARDUINO_ARCH_ESP8266)

/*-----------------------------------------------------------------------------
  MODULE DEFINITION FOR MULTIPLE INCLUSION
-----------------------------------------------------------------------------*/
#ifndef _LIN_SLAVE_HW_SERIAL_ESP8266_H_
#define _LIN_SLAVE_HW_SERIAL_ESP8266_H_


/*-----------------------------------------------------------------------------
  INCLUDE FILES
-----------------------------------------------------------------------------*/

// include required libraries
#include <LIN_slave_HardwareSerial.h>


/*-----------------------------------------------------------------------------
  GLOBAL CLASS
-----------------------------------------------------------------------------*/

/**
  \brief  LIN slave node class via ESP8266 HardwareSerial

  \details LIN slave node class via ESP8266 HardwareSerial. Is derived from generic HW-Serial class (only different constructor)
*/
class LIN_Slave_HardwareSerial_ESP8266 : public LIN_Slave_HardwareSerial
{
  // PRIVATE VARIABLES
  private:

    bool                  swapPins;           //!< use alternate pins for Serial0


  // PUBLIC METHODS
  public:

    /// @brief Class constructor
    LIN_Slave_HardwareSerial_ESP8266(bool SwapPins = false, uint16_t MinFramePause=1000L,
      LIN_Slave_Base::version_t Version = LIN_Slave_Base::LIN_V2, const char NameLIN[] = "Slave", uint32_t TimeoutRx = 1500L, const int8_t PinTxEN = INT8_MIN);
          
    /// @brief Open serial interface
    void begin(uint16_t Baudrate = 19200);
    
    /// @brief Close serial interface
    void end(void);

}; // class LIN_Slave_HardwareSerial_ESP8266


/*-----------------------------------------------------------------------------
    END OF MODULE DEFINITION FOR MULTIPLE INLUSION
-----------------------------------------------------------------------------*/
#endif // _LIN_SLAVE_HW_SERIAL_ESP8266_H_

#endif // ARDUINO_ARCH_ESP8266

/*-----------------------------------------------------------------------------
    END OF FILE
-----------------------------------------------------------------------------*/
