/**
  \file     LIN_slave_HardwareSerial_ESP8266.cpp
  \brief    LIN slave emulation library using hardware Serial0 interface of ESP8266
  \details  This library provides a slave node emulation for a LIN bus via hardware Serial0 interface of ESP8266.
            Due to "approximate" BREAK detection is less robust than specific board implementations! 
            For an explanation of the LIN bus and protocol e.g. see https://en.wikipedia.org/wiki/Local_Interconnect_Network
  \note     Serial.begin() causes a glitch on the bus. Therefore use Serial.updateBaudRate() instead.
  \note     Serial.flush() is omitted because it causes a 500us delay, see https://github.com/esp8266/Arduino/blob/master/cores/esp8266/HardwareSerial.cpp
  \author   Georg Icking-Konert
*/

// assert ESP8266 platform
#if defined(ARDUINO_ARCH_ESP8266)

// include files
#include <LIN_slave_HardwareSerial_ESP8266.h>



/**************************
 * PUBLIC METHODS
**************************/

/**
  \brief      Constructor for LIN node class using ESP8266 HardwareSerial 0
  \details    Constructor for LIN node class for using ESP8266 HardwareSerial 0. Inherit all methods from LIN_Slave_HardwareSerial, only different constructor
  \param[in]  SwapPins    use alternate Serial2 Rx/Tx pins (default = false)
  \param[in]  Version     LIN protocol version (default = v2)
  \param[in]  NameLIN     LIN node name (default = "Slave")
  \param[in]  TimeoutRx   timeout [us] for bytes in frame (default = 1500)
  \param[in]  PinTxEN     optional Tx enable pin (high active) e.g. for LIN via RS485 (default = -127/none)
*/
LIN_Slave_HardwareSerial_ESP8266::LIN_Slave_HardwareSerial_ESP8266(bool SwapPins, LIN_Slave_Base::version_t Version, const char NameLIN[], 
  uint32_t TimeoutRx, const int8_t PinTxEN) : 
  LIN_Slave_HardwareSerial(Serial, Version, NameLIN, TimeoutRx, PinTxEN)
{
  // store parameters in class variables
  this->swapPins   = SwapPins;            // use alternate pins Rx=D7 / Tx=D8 for Serial0

  // optional debug output
  #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 2)
    LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
    LIN_SLAVE_DEBUG_SERIAL.println(": LIN_Slave_HardwareSerial_ESP8266()");
    LIN_SLAVE_DEBUG_SERIAL.flush();
  #endif

} // LIN_Slave_HardwareSerial_ESP8266::LIN_Slave_HardwareSerial_ESP8266()



/**
  \brief      Open serial interface
  \details    Open serial interface with specified baudrate. Optionally use Serial2 pins
  \param[in]  Baudrate    communication speed [Baud] (default = 19200)
*/
void LIN_Slave_HardwareSerial_ESP8266::begin(uint16_t Baudrate)
{
  // call parent class method
  LIN_Slave_HardwareSerial::begin(Baudrate);

  // optionally route Serial0 to alternate pins
  if (this->swapPins == true)
    pSerial->swap();

  // optional debug output
  #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 2)
    LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
    LIN_SLAVE_DEBUG_SERIAL.println(": LIN_Slave_HardwareSerial_ESP8266::begin()");
    LIN_SLAVE_DEBUG_SERIAL.flush();
  #endif

} // LIN_Slave_HardwareSerial_ESP8266::begin()

#endif // ARDUINO_ARCH_ESP8266

/*-----------------------------------------------------------------------------
    END OF FILE
-----------------------------------------------------------------------------*/
