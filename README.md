<p align="center">
  <img width="200" height="200" src="./extras/documentation/Logo_large.jpg">
</p>

Portable LIN Slave Node Emulation
==================================

[Doxygen docs](https://gicking.github.io/LIN_slave_portable_Arduino/extras/documentation/html/index.html)


# Introduction

This Arduino library implements a Local Interconnect Network slave node emulation. For an explanation of the LIN bus and protocol e.g. see https://en.wikipedia.org/wiki/Local_Interconnect_Network.

Optionally LIN protocoll via RS485 is supported (see respective examples). In this case Rx-enable (=RE) must be statically enabled to receive LIN echo, and Tx-enable (=DE) is controlled by the Arduino. 

The class structure is very flexible and aims at supporting different Serial interfaces and architectures. It can easily be ported to other boards - in which case a pull request is highly appreciated... 

For a similar Arduino libary for LIN master emulation see https://github.com/gicking/LIN_master_portable_Arduino

## Supported functionality
  - background handling of frames via user-defined callback functions 
  - multiple, simultaneous LIN nodes
  - supports HardwareSerial and SoftwareSerial
  - supports LIN protocoll via RS485 with Tx direction switching
  
## Supported Boards (with additional LIN hardware)
  - AVR boards, e.g. [Arduino Uno](https://store.arduino.cc/products/arduino-uno-rev3), [Mega](https://store.arduino.cc/products/arduino-mega-2560-rev3) or [Nano](https://store.arduino.cc/products/arduino-nano)
  - ATtiny boards, e.g. [Adafruit Trinket](https://www.adafruit.com/product/1501) (only SoftwareSerial)
  - SAM boards, e.g. [Arduino Due](https://store.arduino.cc/products/arduino-due)
  - ESP32 boards, e.g. [Espressif Wroom-32U](https://www.etechnophiles.com/esp32-dev-board-pinout-specifications-datasheet-and-schematic/) 
  - ESP8266 boards, [Wemos D1 mini](https://www.wemos.cc/en/latest/d1/d1_mini.html)


## Notes
  - The `handler()` method must be called at least every 500us. Optionally it can be called from within [serialEvent()](https://reference.arduino.cc/reference/de/language/functions/communication/serial/serialevent/)
  - Framing errors (FE) on BREAK reception are treated differently by serial interface implementations. Therefore, frame synchronization is handled differently, specifically:
    - HardwareSerial on ESP32, NeoHWSerial on AVR:
      - BREAK is received, FE flag is available
      - sync on `Rx==0x00` (= BREAK) with `FE==true` 
      - assert that *following* `Rx==0x55` (= SYNC)
      - this is according to LIN standard and most robust
    - HardwareSerial on AVR, SAMD & ESP8266:
      - BREAK is received, FE flag **not** available
      - sync on `Rx==0x00` (= BREAK) after minimal inter-frame pause
      - assert that *following* `Rx==0x55` (= SYNC)
      - this is **not** according to LIN standard and less robust
    - SoftwareSerial on AVR
      - BREAK is received, FE flag **not** available
      - sync on `Rx==0x00` (= BREAK) after minimal inter-frame pause
      - assert that *following* `Rx==0x55` (= SYNC)
      - this is **not** according to LIN standard and less robust
    - SoftwareSerial on ESP32 & ESP8266
      - BREAK **dropped** due to nissing stop bit, FE flag **not** available
      - sync on `Rx==0x55` (= SYNC) after minimal inter-frame pause
      - this is **not** according to LIN standard and least robust

  - for AVR `Serial` and `NeoHWSerial` instances are incompatible and must not be used within the same sketch. If possible use only `NeoHWSerial` for best frame synchronization (see above). Alternatively comment out `USE_NEOSERIAL` in file `LIN_slave_NeoHWSerial_AVR.h` to use standard `Serial`
  

# Test Matrix

An *ok* in the below test matrix indicates that normal master request frames are received, slave responses are sent and bus disconnection is detected (-> error). No extensive testing of *all* possible error cases was performed. Please let me know if you experience unexpected errors.

![Test Matrix](./extras/testing/Board_Tests.png?)

Logic analyzer screenshots of LIN bus, idle pin and error pin levels are stored in folder "./extras/testing/*Board*"


Have fun!, Georg

----------------

Revision History
----------------

**v1.0 (2025-02-01)**
  - initial release

