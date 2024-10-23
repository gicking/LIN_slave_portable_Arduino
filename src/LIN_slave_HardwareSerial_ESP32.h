/**
  \file     LIN_slave_HardwareSerial_ESP32.h
  \brief    LIN slave emulation library using a HardwareSerial interface of ESP32
  \details  This library provides a slave node emulation for a LIN bus via a HardwareSerial interface of ESP32.
            For an explanation of the LIN bus and protocol e.g. see https://en.wikipedia.org/wiki/Local_Interconnect_Network
  \author   Georg Icking-Konert
*/

// assert ESP32 platform
#if defined(ARDUINO_ARCH_ESP32)

/*-----------------------------------------------------------------------------
  MODULE DEFINITION FOR MULTIPLE INCLUSION
-----------------------------------------------------------------------------*/
#ifndef _LIN_SLAVE_HW_SERIAL_ESP32_H_
#define _LIN_SLAVE_HW_SERIAL_ESP32_H_


/*-----------------------------------------------------------------------------
  INCLUDE FILES
-----------------------------------------------------------------------------*/

// include required libraries
#include <Arduino.h>
#include "LIN_slave_Base.h"


/*-----------------------------------------------------------------------------
  GLOBAL MACROS
-----------------------------------------------------------------------------*/

/// number of Serial interfaces (0..2)
#define LIN_SLAVE_ESP32_MAX_SERIAL  3


/*-----------------------------------------------------------------------------
  GLOBAL CLASS
-----------------------------------------------------------------------------*/

/**
  \brief  LIN slave node class via ESP32 HardwareSerial

  \details LIN slave node class via ESP32 HardwareSerial.
*/
class LIN_Slave_HardwareSerial_ESP32 : public LIN_Slave_Base
{
  // PROTECTED VARIABLES
  protected:

    uint8_t               pinRx;              //!< pin used for receive
    uint8_t               pinTx;              //!< pin used for transmit
    uint8_t               idxSerial;          //!< index to flagBreak[] of this instance

    // break flags for Serial0..2
    static bool           flagBreak[LIN_SLAVE_ESP32_MAX_SERIAL];


  // PRIVATE METHODS
  private:
  
    /// @brief Static callback function for ESP32 Serial0 error
    static void _onSerialReceiveError0(hardwareSerial_error_t Err);
  
    /// @brief Static callback function for ESP32 Serial1 error
    static void _onSerialReceiveError1(hardwareSerial_error_t Err);
  
    /// @brief Static callback function for ESP32 Serial2 error
    static void _onSerialReceiveError2(hardwareSerial_error_t Err);


  // PROTECTED METHODS
  protected:

    /// @brief Get break detection flag. Is hardware dependent
    bool _getBreakFlag(void);

    /// @brief Clear break detection flag. Is hardware dependent
    void _resetBreakFlag(void);


  // PUBLIC METHODS
  public:

    /// @brief Class constructor
    LIN_Slave_HardwareSerial_ESP32(HardwareSerial &Interface, uint8_t PinRx, uint8_t PinTx, LIN_Slave_Base::version_t Version, const char NameLIN[]);
     
    /// @brief Open serial interface
    void begin(uint16_t Baudrate);
    
    /// @brief Close serial interface
    void end(void);

}; // class LIN_Slave_HardwareSerial_ESP32


/*-----------------------------------------------------------------------------
    END OF MODULE DEFINITION FOR MULTIPLE INLUSION
-----------------------------------------------------------------------------*/
#endif // _LIN_SLAVE_HW_SERIAL_ESP32_H_

#endif // ARDUINO_ARCH_ESP32
