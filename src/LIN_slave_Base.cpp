/**
  \file     LIN_slave_Base.cpp
  \brief    Base class for LIN slave emulation (non-functional)
  \details  This library provides the base class for a slave node emulation of a LIN bus. 
            The base class is non-functional, as it lacks the actual communication interface.
            For an explanation of the LIN bus and protocol e.g. see https://en.wikipedia.org/wiki/Local_Interconnect_Network
  \author   Georg Icking-Konert
*/

// include files
#include "LIN_slave_Base.h"



/**************************
 * PROTECTED METHODS
**************************/

/**
  \brief      Calculate LIN frame checksum
  \details    Calculate LIN frame checksum as described in LIN1.x / LIN2.x specs
  \param[in]  NumData   number of data bytes in frame
  \param[in]  Data      frame data bytes
  \return     calculated checksum, depending on protocol version
*/
uint8_t LIN_Slave_Base::_calculateChecksum(uint8_t NumData, uint8_t Data[])
{
  uint16_t chk=0x00;

  // LIN2.x uses extended checksum which includes protected ID, i.e. including parity bits
  // LIN1.x uses classical checksum only over data bytes
  // Diagnostic frames with ID 0x3C and 0x3D/0x7D always use classical checksum (see LIN spec "2.3.1.5 Checkum")
  if (!((this->version == LIN_V1) || (pid == 0x3C) || (pid == 0x7D)))    // if version 2  & no diagnostic frames (0x3C=60 (PID=0x3C) or 0x3D=61 (PID=0x7D))
    chk = (uint16_t) this->pid;

  // loop over data bytes
  for (uint8_t i = 0; i < NumData; i++)
  {
    chk += (uint16_t) (Data[i]);
    if (chk>255)
      chk -= 255;
  }
  chk = (uint8_t)(0xFF - ((uint8_t) chk));   // bitwise invert

  // return frame checksum
  return (uint8_t) chk;

  // optional debug output
  #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 2)
    LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
    LIN_SLAVE_DEBUG_SERIAL.println(": LIN_Slave_Base::_calculateChecksum()");
  #endif

} // LIN_Slave_Base::_calculateChecksum()



/**
  \brief      Get break detection flag. Is hardware dependent
  \details    Get break detection flag. Is hardware dependent, here dummy!
  \return status of break detection
*/
bool LIN_Slave_Base::_getBreakFlag()
{
  // dummy for base class
  return false;

} // LIN_Slave_Base::_getBreakFlag()



/**
  \brief      Clear break detection flag. Is hardware dependent
  \details    Clear break detection flag. Is hardware dependent, here dummy!
*/
void LIN_Slave_Base::_resetBreakFlag()
{
  // dummy for base class

} // LIN_Slave_Base::_resetBreakFlag()



/**************************
 * PUBLIC METHODS
**************************/

/**
  \brief      LIN slave node constructor
  \details    LIN slave node constructor. Initialize class variables to default values.
              For an explanation of the LIN bus and protocol e.g. see https://en.wikipedia.org/wiki/Local_Interconnect_Network
  \param[in]  Version     LIN protocol version (required for checksum)
  \param[in]  NameLIN     LIN node name 
*/
LIN_Slave_Base::LIN_Slave_Base(LIN_Slave_Base::version_t Version, const char NameLIN[])
{  
  // store parameters in class variables
  this->version = Version;                                    // LIN protocol version (required for checksum)
  memcpy(this->nameLIN, NameLIN, LIN_SLAVE_BUFLEN_NAME);      // node name e.g. for debug

  // initialize slave node properties
  this->pSerial   = nullptr;                                  // pointer to serial I/F
  this->state     = LIN_Slave_Base::WAIT_FOR_BREAK;           // status of LIN state machine
  this->error     = LIN_Slave_Base::NO_ERROR;                 // last LIN error. Is latched
  for (uint8_t i=0; i<64; i++)
  {
    this->requestHandlers[i].handler = nullptr;               // handlers for master request IDs 0x00 - 0x3F
    this->responseHandlers[i].handler = nullptr;              // handlers for slave response IDs 0x00 - 0x3F
  }

  // initialize frame properties
  this->pid         = 0x00;                                   // protected frame identifier
  this->id          = 0x00;                                   // unprotected frame identifier
  this->numData     = 0;                                      // number of data bytes in frame
  for (uint8_t i=0; i<8; i++)
    this->bufRx[i] = 0x00;                                    // buffer for data bytes (max 8B)
  this->idxData = 0;                                          // current index in bufRx
  this->timeLastRx = 0;                                       // time [ms] of last received byte in frame

  // optional debug output
  #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 2)
    LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
    LIN_SLAVE_DEBUG_SERIAL.println(": LIN_Slave_Base::LIN_Slave_Base()");
  #endif

} // LIN_Slave_Base::LIN_Slave_Base()



/**
  \brief      Open serial interface
  \details    Open serial interface with specified baudrate. Here dummy!
  \param[in]  Baudrate    communication speed [Baud]
*/
void LIN_Slave_Base::begin(uint16_t Baudrate)
{
  // store parameters in class variables
  this->baudrate   = Baudrate;                                // communication baudrate [Baud]

  // initialize master node properties
  this->error = LIN_Slave_Base::NO_ERROR;                     // last LIN error. Is latched
  this->state = LIN_Slave_Base::WAIT_FOR_BREAK;               // status of LIN state machine

  // optional debug output
  #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 2)
    LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
    LIN_SLAVE_DEBUG_SERIAL.println(": LIN_Slave_Base::begin()");
  #endif

} // LIN_Slave_Base::begin()



/**
  \brief      Close serial interface
  \details    Close serial interface. Here dummy!
*/
void LIN_Slave_Base::end()
{
  // initialize master node properties
  this->state = LIN_Slave_Base::WAIT_FOR_BREAK;               // status of LIN state machine

  // optional debug output
  #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 2)
    LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
    LIN_SLAVE_DEBUG_SERIAL.println(": LIN_Slave_Base::end()");
  #endif

} // LIN_Slave_Base::end()



/**
  \brief      Attach callback function for master request frame
  \details    Attach callback function for master request frame. Are called by handler() after reception of a master request frame
  \param[in]  ID        frame ID (protected or unprotected)
  \param[in]  Handler   user callback function
  \param[in]  NumData   number of frame data bytes
*/
void LIN_Slave_Base::registerMasterRequestHandler(uint8_t ID, LIN_Slave_Base::LinMessageHandler Handler, uint8_t NumData)
{  
  // drop parity bits -> non-protected ID = 0..63
  ID &= 0x3F;

  // register user handler for master request frame
  this->requestHandlers[ID].handler = Handler;
  this->requestHandlers[ID].numData = NumData;

  // optional debug output
  #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 2)
    LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
    LIN_SLAVE_DEBUG_SERIAL.print(": registered request ID 0x");
    LIN_SLAVE_DEBUG_SERIAL.println(ID, HEX);
  #endif

} // LIN_Slave_Base::registerMasterRequestHandler()



/**
  \brief      Attach callback function for slave response frame
  \details    Attach callback function for slave response frame. Are called by handler() after reception of a PID
  \param[in]  ID        frame ID (protected or unprotected)
  \param[in]  Handler   user callback function
  \param[in]  NumData   number of frame data bytes
*/
void LIN_Slave_Base::registerSlaveResponseHandler(uint8_t ID, LIN_Slave_Base::LinMessageHandler Handler, uint8_t NumData)
{
  // drop parity bits -> non-protected ID = 0..63
  ID &= 0x3F;

  // register user handler for slave response frame
  this->responseHandlers[ID].handler = Handler;
  this->responseHandlers[ID].numData = NumData;

  // optional debug output
  #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 2)
    LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
    LIN_SLAVE_DEBUG_SERIAL.print(": registered response ID 0x");
    LIN_SLAVE_DEBUG_SERIAL.println(ID, HEX);
  #endif

} // LIN_Slave_Base::registerSlaveResponseHandler()



/**
  \brief      Handle LIN protocol and call user-defined frame handlers
  \details    Handle LIN protocol and call user-defined frame handlers, both for master request and slave response frames
*/
void LIN_Slave_Base::handler()
{
  uint8_t   chk_calc;

  // skip for base class to avoid crash
  if (this->pSerial == nullptr)
  {
    // optional debug output
    #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 1)
      LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
      LIN_SLAVE_DEBUG_SERIAL.println(": error: LIN_Slave_Base::handler() with pSerial==NULL");
    #endif

    // return immediately
    return;
  }

  
  // on receive timeout within frame revert state machine
  if ((this->state != LIN_Slave_Base::WAIT_FOR_BREAK) && ((millis() - this->timeLastRx) > LIN_SLAVE_RX_TIMEOUT))
  {
    this->state = LIN_Slave_Base::WAIT_FOR_BREAK;
    this->error = (LIN_Slave_Base::error_t) ((int) this->error | (int) LIN_Slave_Base::ERROR_TIMEOUT);

    // optional debug output
    #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 1)
      LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
      LIN_SLAVE_DEBUG_SERIAL.print(": error: frame timeout after ");
      LIN_SLAVE_DEBUG_SERIAL.print((long) (millis() - this->timeLastRx));
      LIN_SLAVE_DEBUG_SERIAL.println("ms");
    #endif

  } // Rx timeout


  // A byte was received -> handle it
  if (this->pSerial->available())
  {
    // read received byte and reset timeout timer
    uint8_t byteReceived = this->pSerial->read();
    this->timeLastRx = millis();

    // optional debug output
    #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 3)
      LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
      if (this->_getBreakFlag() == true)
        LIN_SLAVE_DEBUG_SERIAL.print(": BRK, Rx=0x");
      else
        LIN_SLAVE_DEBUG_SERIAL.print(": Rx=0x");
      LIN_SLAVE_DEBUG_SERIAL.println(byteReceived, HEX);
    #endif

    // detected LIN BREAK 
    if (this->_getBreakFlag() == true)
    {
      // clear BREAK flag again
      this->_resetBreakFlag();
      
      // valid BREAK (=0x00 w/o stop bit) -> start frame
      if (byteReceived == 0x00)
        this->state = LIN_Slave_Base::WAIT_FOR_SYNC;
      
      // wrong data (!=0x00) -> error
      else
      {
        this->state = LIN_Slave_Base::WAIT_FOR_BREAK;
        this->error = (LIN_Slave_Base::error_t) ((int) this->error | (int) LIN_Slave_Base::ERROR_BREAK);

        // optional debug output
        #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 1)
          LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
          LIN_SLAVE_DEBUG_SERIAL.print(": BREAK error, received 0x");
          LIN_SLAVE_DEBUG_SERIAL.println(byteReceived, HEX);
        #endif
      }
    } // BREAK detected


    // no BREAK detected -> handle byte
    else
    {
      // LIN protocol state machine
      switch (this->state)
      {
        // just to avoid compiler warning. State is handled above
        case LIN_Slave_Base::WAIT_FOR_BREAK:
          break;
        
        // break has been received, waiting for sync field
        case LIN_Slave_Base::WAIT_FOR_SYNC:
          
          // valid SYNC (=0x55) -> wait for ID
          if (byteReceived == 0x55)
          {
            this->state = LIN_Slave_Base::WAIT_FOR_PID;
            this->idxData = 0;
          } 

          // invalid SYNC (!=0x55) -> error
          else
          {
            // reset state machine
            this->state = LIN_Slave_Base::WAIT_FOR_BREAK;
            this->error = (LIN_Slave_Base::error_t) ((int) this->error | (int) LIN_Slave_Base::ERROR_SYNC);

            // optional debug output
            #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 1)
              LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
              LIN_SLAVE_DEBUG_SERIAL.print(": SYNC error, received 0x");
              LIN_SLAVE_DEBUG_SERIAL.println(byteReceived, HEX);
            #endif
          }

          break; // WAIT_FOR_SYNC


        // sync field has been received, waiting for protected ID
        case LIN_Slave_Base::WAIT_FOR_PID:

          this->pid = byteReceived;         // received (protected) ID
          this->id  = byteReceived & 0x3F;   // extract ID, drop parity bits

          // if slave response ID is registered, call handler and send response immediately
          if (this->responseHandlers[id].handler != nullptr)
          {
            // get number of response bytes
            this->numData = this->responseHandlers[id].numData;
            
            // call the user-defined handler for this ID
            this->responseHandlers[id].handler(numData, this->bufRx);

            // send slave response (skip echo handling)
            this->pSerial->write(bufRx, numData);
            this->pSerial->write(this->_calculateChecksum(this->numData, this->bufRx));
            
            // revert state machine
            this->state = LIN_Slave_Base::WAIT_FOR_BREAK;

            // optional debug output
            #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 2)
              LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
              LIN_SLAVE_DEBUG_SERIAL.print(": handle slave response PID 0x");
              LIN_SLAVE_DEBUG_SERIAL.println(this->pid, HEX);
            #endif

          } // slave response frame
          
          // if master request ID is registered, get number of data bytes and advance state
          else if (this->requestHandlers[id].handler != nullptr)
          {
            this->numData = this->requestHandlers[id].numData;
            this->state = LIN_Slave_Base::RECEIVING_DATA;
          } // master request frame 
            
          // ID is not registered -> wait for next break
          else
          {
            // reset state machine
            this->state = LIN_Slave_Base::WAIT_FOR_BREAK;

            // optional debug output
            #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 2)
              LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
              LIN_SLAVE_DEBUG_SERIAL.print(": drop frame PID 0x");
              LIN_SLAVE_DEBUG_SERIAL.println(this->pid, HEX);
            #endif

          } // frame not registered

          break; // WAIT_FOR_PID


        // PID has been received for master request frame, receiving data
        case LIN_Slave_Base::RECEIVING_DATA:

          // store received data
          this->bufRx[(this->idxData)++] = byteReceived;
          
          // if frame is completed, advance to checksum
          if (this->idxData >= this->numData)
            this->state = LIN_Slave_Base::CHECKSUM_VERIFY;

          break; // RECEIVING_DATA


        // Data has been received for master request frame, waiting for checksum
        case LIN_Slave_Base::CHECKSUM_VERIFY:

          // calculate checksum for master request frame
          chk_calc = this->_calculateChecksum(this->numData, this->bufRx);
          
          // Checksum valid -> call user-defined callback function for this ID
          if (byteReceived == chk_calc)
          {
            // call user-defined handler
            this->requestHandlers[id].handler(numData, bufRx);

            // optional debug output
            #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 2)
              LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
              LIN_SLAVE_DEBUG_SERIAL.print(": handle master request PID 0x");
              LIN_SLAVE_DEBUG_SERIAL.println(this->pid, HEX);
            #endif

          } 
          
          // checksum error
          else
          {
            // set error
            this->error = (LIN_Slave_Base::error_t) ((int) this->error | (int) LIN_Slave_Base::ERROR_CHK);

            // optional debug output
            #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 1)
              LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
              LIN_SLAVE_DEBUG_SERIAL.print(": CHK error, received 0x");
              LIN_SLAVE_DEBUG_SERIAL.print(byteReceived, HEX);
              LIN_SLAVE_DEBUG_SERIAL.print(", calculated 0x");
              LIN_SLAVE_DEBUG_SERIAL.println(chk_calc, HEX);
            #endif
          }

          // reset state machine
          this->state = LIN_Slave_Base::WAIT_FOR_BREAK;

          break; // CHECKSUM_VERIFY


        // this should never happen -> error
        default:

          // set error
          this->error = (LIN_Slave_Base::error_t) ((int) this->error | (int) LIN_Slave_Base::LIN_Slave_Base::ERROR_STATE);

          // optional debug output
          #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 1)
            LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
            LIN_SLAVE_DEBUG_SERIAL.print(": error: illegal state ");
            LIN_SLAVE_DEBUG_SERIAL.print(this->state);
            LIN_SLAVE_DEBUG_SERIAL.println(", this should never happen...");
          #endif

          // revert state machine
          this->state = LIN_Slave_Base::WAIT_FOR_BREAK;

      } // switch(state)

    } // no BREAK

  } // if byte received

} // LIN_Slave_Base::handler

/*-----------------------------------------------------------------------------
    END OF FILE
-----------------------------------------------------------------------------*/