/**
  \file     LIN_slave_HardwareSerial_AVR.h
  \brief    LIN slave emulation library using a HardwareSerial interface of 8-bit AVR
  \details  This library provides a slave node emulation for a LIN bus via a HardwareSerial interface of 8-bit AVR.
            For an explanation of the LIN bus and protocol e.g. see https://en.wikipedia.org/wiki/Local_Interconnect_Network
  \author   Georg Icking-Konert
*/

// assert AVR platform
#if defined(ARDUINO_ARCH_AVR)

/*-----------------------------------------------------------------------------
  MODULE DEFINITION FOR MULTIPLE INCLUSION
-----------------------------------------------------------------------------*/
#ifndef _LIN_SLAVE_HW_SERIAL_AVR_H_
#define _LIN_SLAVE_HW_SERIAL_AVR_H_


/*-----------------------------------------------------------------------------
  INCLUDE FILES
-----------------------------------------------------------------------------*/

// include required libraries
#define HardwareSerial_h        // prevent Arduino HardwareSerial from being included
#include <NeoHWSerial.h>
#include <LIN_slave_Base.h>


/*-----------------------------------------------------------------------------
  GLOBAL MACROS
-----------------------------------------------------------------------------*/

/// determine number of Serial interfaces
#if !defined(LIN_SLAVE_AVR_MAX_SERIAL)
  #if defined(HAVE_HWSERIAL0)
    #define LIN_SLAVE_AVR_MAX_SERIAL    1
  #else
    #error no HardwareSerial available for this board
  #endif
  #if defined(HAVE_HWSERIAL1)
    #undef  LIN_SLAVE_AVR_MAX_SERIAL
    #define LIN_SLAVE_AVR_MAX_SERIAL    2
  #endif
  #if defined(HAVE_HWSERIAL2)
    #undef  LIN_SLAVE_AVR_MAX_SERIAL
    #define LIN_SLAVE_AVR_MAX_SERIAL    3
  #endif
  #if defined(HAVE_HWSERIAL3)
    #undef  LIN_SLAVE_AVR_MAX_SERIAL
    #define LIN_SLAVE_AVR_MAX_SERIAL    4
  #endif
#endif


/*-----------------------------------------------------------------------------
  GLOBAL CLASS
-----------------------------------------------------------------------------*/

/**
  \brief  LIN slave node class via AVR HardwareSerial

  \details LIN slave node class via AVR HardwareSerial.
*/
class LIN_Slave_HardwareSerial_AVR : public LIN_Slave_Base
{
  // PROTECTED VARIABLES
  protected:

    uint8_t               idxSerial;          //!< index to flagBreak[] of this instance

    // break flags for Serial0..N
    static bool           flagBreak[LIN_SLAVE_AVR_MAX_SERIAL];


  // PRIVATE METHODS
  private:

    #if defined(HAVE_HWSERIAL0)
      /// @brief Static callback function for AVR Serial0 error
      static bool _onSerialReceive0(uint8_t byte, uint8_t status);
    #endif
  
    #if defined(HAVE_HWSERIAL1)
      /// @brief Static callback function for AVR Serial1 error
      static bool _onSerialReceive1(uint8_t byte, uint8_t status);
    #endif
  
    #if defined(HAVE_HWSERIAL2)
      /// @brief Static callback function for AVR Serial2 error
      static bool _onSerialReceive2(uint8_t byte, uint8_t status);
    #endif
  
    #if defined(HAVE_HWSERIAL3)
      /// @brief Static callback function for AVR Serial3 error
      static bool _onSerialReceive3(uint8_t byte, uint8_t status);
    #endif
	
	
  // PROTECTED METHODS
  protected:

    /// @brief Get break detection flag. Is hardware dependent
    bool _getBreakFlag(void);

    /// @brief Clear break detection flag. Is hardware dependent
    void _resetBreakFlag(void);


  // PUBLIC METHODS
  public:

    /// @brief Class constructor
    LIN_Slave_HardwareSerial_AVR(NeoHWSerial &Interface, LIN_Slave_Base::version_t Version, const char NameLIN[]);
     
    /// @brief Open serial interface
    void begin(uint16_t Baudrate);
    
    /// @brief Close serial interface
    void end(void);

}; // class LIN_Slave_HardwareSerial_AVR


/*-----------------------------------------------------------------------------
    END OF MODULE DEFINITION FOR MULTIPLE INLUSION
-----------------------------------------------------------------------------*/
#endif // _LIN_SLAVE_HW_SERIAL_AVR_H_

#endif // ARDUINO_ARCH_AVR
