/**
  \file     LIN_slave_SoftwareSerial.h
  \brief    LIN slave emulation library using a generic SoftwareSerial interface (if available).
  \details  This library provides a slave node emulation for a LIN bus via a generic SoftwareSerial interface.
            Due to "approximate" BREAK detection is less robust than specific board implementations! 
            For an explanation of the LIN bus and protocol e.g. see https://en.wikipedia.org/wiki/Local_Interconnect_Network
  \author   Georg Icking-Konert
*/

// assert platform which supports SoftwareSerial. Note: ARDUINO_ARCH_ESP32 requires library ESPSoftwareSerial
#if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32) 

/*-----------------------------------------------------------------------------
  MODULE DEFINITION FOR MULTIPLE INCLUSION
-----------------------------------------------------------------------------*/
#ifndef _LIN_SLAVE_SW_SERIAL_H_
#define _LIN_SLAVE_SW_SERIAL_H_


/*-----------------------------------------------------------------------------
  INCLUDE FILES
-----------------------------------------------------------------------------*/

// include required libraries
#include <LIN_slave_Base.h>
#include <SoftwareSerial.h>


/*-----------------------------------------------------------------------------
  GLOBAL CLASS
-----------------------------------------------------------------------------*/

/**
  \brief  LIN slave node class via generic SoftwareSerial

  \details LIN slave node class via generic SoftwareSerial.
*/
class LIN_Slave_SoftwareSerial : public LIN_Slave_Base
{
  // PRIVATE VARIABLES
  private:

    SoftwareSerial        SWSerial;           //!< SW serial interface used for LIN (no pointer!)
    uint8_t               pinRx;              //!< pin used for receive
    uint8_t               pinTx;              //!< pin used for transmit
    bool                  inverseLogic;       //!< use inverse logic
    bool                  flagBreak;          //!< a break was detected, is set in handle
    uint16_t              minFramePause;      //!< min. inter-frame pause [us] to start new frame (not standard compliant!)


  // PROTECTED METHODS
  protected:

    /// @brief Get break detection flag
    bool _getBreakFlag(void);

    /// @brief Clear break detection flag
    void _resetBreakFlag(void);


    /// @brief peek next byte from Rx buffer
    inline uint8_t _serialPeek(void) { return SWSerial.peek(); }

    /// @brief read next byte from Rx buffer
    inline uint8_t _serialRead(void) { return SWSerial.read(); }

    /// @brief write bytes to Tx buffer (blocking). Disable receive to avoid inter-byte pauses on AVR
    inline void _serialWrite(uint8_t buf[], uint8_t num)
    { 
      SWSerial.stopListening();
      SWSerial.write(buf, num); 
      SWSerial.flush();
      SWSerial.listen();
    }


  // PUBLIC METHODS
  public:

    /// @brief Class constructor
    LIN_Slave_SoftwareSerial(uint8_t PinRx, uint8_t PinTx, bool InverseLogic = false, uint16_t MinFramePause=1000L, 
      LIN_Slave_Base::version_t Version = LIN_Slave_Base::LIN_V2, const char NameLIN[] = "Slave", uint32_t TimeoutRx = 1500L, const int8_t PinTxEN = INT8_MIN);

    /// @brief Open serial interface
    void begin(uint16_t Baudrate = 19200);
    
    /// @brief Close serial interface
    void end(void);

    /// @brief check if a byte is available in Rx buffer
    inline bool available(void) { return SWSerial.available(); }

    /// @brief Handle LIN protocol and call user-defined frame handlers
    virtual void handler(void);

}; // class LIN_Slave_SoftwareSerial


/*-----------------------------------------------------------------------------
    END OF MODULE DEFINITION FOR MULTIPLE INLUSION
-----------------------------------------------------------------------------*/
#endif // _LIN_SLAVE_SW_SERIAL_H_

#endif // ARDUINO_ARCH_AVR || ARDUINO_ARCH_ESP8266 || ARDUINO_ARCH_ESP32

/*-----------------------------------------------------------------------------
    END OF FILE
-----------------------------------------------------------------------------*/
