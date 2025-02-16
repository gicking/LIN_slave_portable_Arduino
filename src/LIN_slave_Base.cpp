/**
  \file     LIN_slave_Base.cpp
  \brief    Base class for LIN slave emulation (non-functional)
  \details  This library provides the base class for a slave node emulation of a LIN bus. 
            The base class is non-functional, as it lacks the actual communication interface.
            For an explanation of the LIN bus and protocol e.g. see https://en.wikipedia.org/wiki/Local_Interconnect_Network
  \author   Georg Icking-Konert
*/

// include files
#include <LIN_slave_Base.h>

// warn if debug is active (any debug level)
#if defined(LIN_SLAVE_DEBUG_SERIAL)
  #warning Debug interface is active, see file 'LIN_slave_Base.h'
#endif



/**************************
 * PROTECTED METHODS
**************************/

/**
  \brief      Calculate protected frame ID
  \details    Calculate protected frame ID as described in LIN2.0 spec "2.3.1.3 Protected identifier field"
  \param[in]  ID    frame ID (protected or unprotected)
  \return     Protected frame ID
*/
uint8_t LIN_Slave_Base::_calculatePID(uint8_t ID)
{
  uint8_t  pid;       // protected frame ID
  uint8_t  tmp;       // temporary variable for calculating parity bits

  // protect ID  with parity bits
  pid  = (uint8_t) (ID & 0x3F);                                             // clear upper bits 6 & 7
  tmp  = (uint8_t) ((pid ^ (pid>>1) ^ (pid>>2) ^ (pid>>4)) & 0x01);         // pid[6] = PI0 = ID0^ID1^ID2^ID4
  pid |= (uint8_t) (tmp << 6);
  tmp  = (uint8_t) (~((pid>>1) ^ (pid>>3) ^ (pid>>4) ^ (pid>>5)) & 0x01);   // pid[7] = PI1 = ~(ID1^ID3^ID4^ID5)
  pid |= (uint8_t) (tmp << 7);

  // return protected ID
  return pid;

} // LIN_Slave_Base::_calculatePID()



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

  // optional debug output (debug level 2)
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
  \param[in]  Version     LIN protocol version (default = v2)
  \param[in]  NameLIN     LIN node name (default = "Slave")
  \param[in]  TimeoutRx   timeout [us] for bytes in frame (default = 1500)
  \param[in]  PinTxEN     optional Tx enable pin (high active) e.g. for LIN via RS485 (default = -127/none)
*/
LIN_Slave_Base::LIN_Slave_Base(LIN_Slave_Base::version_t Version, const char NameLIN[], uint32_t TimeoutRx, const int8_t PinTxEN)
{
  // Debug serial initialized in begin() -> no debug output here

  // store parameters in class variables
  this->version = Version;                                    // LIN protocol version (required for checksum)
  memcpy(this->nameLIN, NameLIN, LIN_SLAVE_BUFLEN_NAME);      // node name e.g. for debug
  this->timeoutRx = TimeoutRx;                                // timeout [us] for bytes in frame
  this->pinTxEN = PinTxEN;                                    // optional Tx enable pin for RS485

  // initialize slave node properties
  this->state     = LIN_Slave_Base::STATE_WAIT_FOR_BREAK;     // status of LIN state machine
  this->error     = LIN_Slave_Base::NO_ERROR;                 // last LIN error. Is latched
  for (uint8_t i=0; i<64; i++)
  {
    this->callback[i].type_numData = 0x00;                    // frame type (high nibble) and number of data bytes (low nibble)
    this->callback[i].fct = nullptr;                          // user callback functions (IDs 0x00 - 0x3F)
  }

  // initialize frame properties
  this->pid         = 0x00;                                   // protected frame identifier
  this->id          = 0x00;                                   // unprotected frame identifier
  this->numData     = 0;                                      // number of data bytes in frame
  for (uint8_t i=0; i<9; i++)
    this->bufData[i] = 0x00;                                  // init data bytes (max 8B) + chk
  this->idxData    = 0;                                       // current index in bufData
  this->timeLastRx = 0;                                       // time [ms] of last received byte in frame

  // initialize TxEN pin low (=transmitter off)
  if (this->pinTxEN >= 0)
  {
    digitalWrite(this->pinTxEN, LOW);
    pinMode(this->pinTxEN, OUTPUT);
  }

} // LIN_Slave_Base::LIN_Slave_Base()



/**
  \brief      Open serial interface
  \details    Open serial interface with specified baudrate. Here dummy!
  \param[in]  Baudrate    communication speed [Baud] (default = 19200)
*/
void LIN_Slave_Base::begin(uint16_t Baudrate)
{
  // For optional debugging
  #if defined(LIN_SLAVE_DEBUG_SERIAL)
    LIN_SLAVE_DEBUG_SERIAL.begin(115200);
    while (!LIN_SLAVE_DEBUG_SERIAL);
  #endif

  // print debug message (debug level 2)
  #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 2)
    LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
    LIN_SLAVE_DEBUG_SERIAL.print(": LIN_Slave_Base::begin(");
    LIN_SLAVE_DEBUG_SERIAL.print((int) Baudrate);
    LIN_SLAVE_DEBUG_SERIAL.println(")");
  #endif

  // store parameters in class variables
  this->baudrate   = Baudrate;                                  // communication baudrate [Baud]

  // initialize slave node properties
  this->error = LIN_Slave_Base::NO_ERROR;                       // last LIN error. Is latched
  this->state = LIN_Slave_Base::STATE_WAIT_FOR_BREAK;           // status of LIN state machine

  // initialize optional TxEN pin to low (=transmitter off)
  if (this->pinTxEN >= 0)
  {
    digitalWrite(this->pinTxEN, LOW);
    pinMode(this->pinTxEN, OUTPUT);
  }

} // LIN_Slave_Base::begin()



/**
  \brief      Close serial interface
  \details    Close serial interface. Here dummy!
*/
void LIN_Slave_Base::end()
{
  // optional debug output (debug level 2)
  #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 2)
    LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
    LIN_SLAVE_DEBUG_SERIAL.println(": LIN_Slave_Base::end()");
  #endif

  // set slave node properties
  this->error = LIN_Slave_Base::NO_ERROR;                     // last LIN error. Is latched
  this->state = LIN_Slave_Base::STATE_OFF;                    // status of LIN state machine

  // optionally disable RS485 transmitter
  _disableTransmitter();

} // LIN_Slave_Base::end()



/**
  \brief      Attach user callback function for master request frame
  \details    Attach user callback function for master request frame. Callback functions are called by handler() after reception of a master request frame
  \param[in]  ID        frame ID (protected or unprotected)
  \param[in]  Fct       user callback function
  \param[in]  NumData   number of frame data bytes
*/
void LIN_Slave_Base::registerMasterRequestHandler(uint8_t ID, LIN_Slave_Base::LinMessageCallback Fct, uint8_t NumData)
{  
  // drop parity bits -> non-protected ID = 0..63
  ID &= 0x3F;

  // register user callback function for master request frame
  this->callback[ID].type_numData = LIN_Slave_Base::MASTER_REQUEST | (NumData & 0x0F);
  this->callback[ID].fct = Fct;

  // optional debug output (debug level 2)
  #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 2)
    LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
    LIN_SLAVE_DEBUG_SERIAL.print(": LIN_Slave_Base::registerMasterRequestHandler()");
    LIN_SLAVE_DEBUG_SERIAL.print(": registered ID 0x");
    LIN_SLAVE_DEBUG_SERIAL.println(ID, HEX);
  #endif

} // LIN_Slave_Base::registerMasterRequestHandler



/**
  \brief      Attach user callback function for slave response frame
  \details    Attach user callback function for slave response frame. Callback functions are called by handler() after reception of a PID
  \param[in]  ID        frame ID (protected or unprotected)
  \param[in]  Fct       user callback function
  \param[in]  NumData   number of frame data bytes
*/
void LIN_Slave_Base::registerSlaveResponseHandler(uint8_t ID, LIN_Slave_Base::LinMessageCallback Fct, uint8_t NumData)
{
  // drop parity bits -> non-protected ID = 0..63
  ID &= 0x3F;

  // register user callback function for slave response frame
  this->callback[ID].type_numData = LIN_Slave_Base::SLAVE_RESPONSE | (NumData & 0x0F);
  this->callback[ID].fct = Fct;

  // optional debug output (debug level 2)
  #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 2)
    LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
    LIN_SLAVE_DEBUG_SERIAL.print(": LIN_Slave_Base::registerSlaveResponseHandler()");
    LIN_SLAVE_DEBUG_SERIAL.print(": registered ID 0x");
    LIN_SLAVE_DEBUG_SERIAL.println(ID, HEX);
  #endif

} // LIN_Slave_Base::registerSlaveResponseHandler



/**
  \brief      Handle LIN protocol and call user-defined frame callback functions
  \details    Handle LIN protocol and call user-defined frame callback functions, both for slave request and slave response frames
*/
void LIN_Slave_Base::handler()
{
  uint8_t   chk_calc;

  // on receive timeout [us] within frame reset state machine
  if (!(this->state | (LIN_Slave_Base::STATE_OFF | LIN_Slave_Base::STATE_WAIT_FOR_BREAK | LIN_Slave_Base::STATE_DONE)) && 
    ((micros() - this->timeLastRx) > this->timeoutRx))
  {
    // set error and abort frame
    this->error = (LIN_Slave_Base::error_t) ((int) this->error | (int) LIN_Slave_Base::ERROR_TIMEOUT);
    this->state = LIN_Slave_Base::STATE_DONE;

    // flush receive buffer
    while (this->available())
      this->_serialRead();

    // optionally disable RS485 transmitter
    _disableTransmitter();

    // optional debug output (debug level 1)
    #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 1)
      LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
      LIN_SLAVE_DEBUG_SERIAL.print(": LIN_Slave_Base::handler()");
      LIN_SLAVE_DEBUG_SERIAL.print(": error: frame timeout after ");
      LIN_SLAVE_DEBUG_SERIAL.print((long) (micros() - this->timeLastRx));
      LIN_SLAVE_DEBUG_SERIAL.println("us");
    #endif

  } // if frame receive timeout


  // detected LIN BREAK (=0x00 with framing error or inter-frame pause detected)
  // Note: received BREAK byte is consumed by child class to support also sync on SYNC byte. 
  if (this->_getBreakFlag() == true)
  {
    // clear BREAK flag again
    this->_resetBreakFlag();
    
    // start frame reception. Note: 0x00 already checked by derived class
    this->state = LIN_Slave_Base::STATE_WAIT_FOR_SYNC;

    // optionally disable RS485 transmitter
    _disableTransmitter();

    // optional debug output (debug level 3)
    #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 3)
      LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
      LIN_SLAVE_DEBUG_SERIAL.print(": LIN_Slave_Base::handler()");
      LIN_SLAVE_DEBUG_SERIAL.println(": BREAK detected ");
    #endif

  } // if BREAK detected


  // A byte was received -> handle it
  if (this->available())
  {
    // read received byte and reset timeout timer
    uint8_t byteReceived = this->_serialRead();
    this->timeLastRx = micros();

    // optional debug output (debug level 3)
    #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 3)
      LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
      LIN_SLAVE_DEBUG_SERIAL.print(": LIN_Slave_Base::handler()");
      if (this->_getBreakFlag() == true)
        LIN_SLAVE_DEBUG_SERIAL.print(": BRK, Rx=0x");
      else
        LIN_SLAVE_DEBUG_SERIAL.print(": Rx=0x");
      LIN_SLAVE_DEBUG_SERIAL.println(byteReceived, HEX);
    #endif

    // handle byte
    switch (this->state)
    {
      // LIN interface disabled, do nothing
      case LIN_Slave_Base::STATE_OFF:
        break;

      // just to avoid compiler warning, do nothing. Break state is handled above
      case LIN_Slave_Base::STATE_WAIT_FOR_BREAK:
        break;
      
      // master request frame is finished, do nothing
      case LIN_Slave_Base::STATE_DONE:
        break;

      // break has been received, waiting for sync field
      case LIN_Slave_Base::STATE_WAIT_FOR_SYNC:
        
        // valid SYNC (=0x55) -> wait for ID
        if (byteReceived == 0x55)
        {
          this->idxData = 0;
          this->state = LIN_Slave_Base::STATE_WAIT_FOR_PID;
        } 

        // invalid SYNC (!=0x55) -> error
        else
        {
          // set error and abort frame
          this->error = (LIN_Slave_Base::error_t) ((int) this->error | (int) LIN_Slave_Base::ERROR_SYNC);
          this->state = LIN_Slave_Base::STATE_DONE;

          // optionally disable RS485 transmitter
          _disableTransmitter();

          // optional debug output (debug level 1)
          #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 1)
            LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
            LIN_SLAVE_DEBUG_SERIAL.print(": LIN_Slave_Base::handler()");
            LIN_SLAVE_DEBUG_SERIAL.print(": SYNC error, received 0x");
            LIN_SLAVE_DEBUG_SERIAL.println(byteReceived, HEX);
          #endif

        } // invalid SYNC

        break; // STATE_WAIT_FOR_SYNC


      // sync field has been received, waiting for protected ID
      case LIN_Slave_Base::STATE_WAIT_FOR_PID:

        this->pid = byteReceived;          // received (protected) ID
        this->id  = byteReceived & 0x3F;   // extract ID, drop parity bits

        // check PID parity bits 7+8
        if (this->pid != this->_calculatePID(this->id))
        {
          // set error and abort frame
          this->error = (LIN_Slave_Base::error_t) ((int) this->error | (int) LIN_Slave_Base::ERROR_PID);
          this->state = LIN_Slave_Base::STATE_DONE;

          // optionally disable RS485 transmitter
          _disableTransmitter();

          // optional debug output (debug level 1)
          #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 1)
            LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
            LIN_SLAVE_DEBUG_SERIAL.print(": LIN_Slave_Base::handler()");
            LIN_SLAVE_DEBUG_SERIAL.print(": PID parity error, received 0x");
            LIN_SLAVE_DEBUG_SERIAL.print(this->pid, HEX);
            LIN_SLAVE_DEBUG_SERIAL.print(", calculated 0x");
            LIN_SLAVE_DEBUG_SERIAL.println(this->_calculatePID(this->id), HEX);
          #endif
          
        } // PID error

        // if slave response ID is registered, call callback function and send response
        else if ((this->callback[id].fct != nullptr) && (this->callback[id].type_numData & LIN_Slave_Base::SLAVE_RESPONSE))
        {
          // get type (high nibble) and number of response bytes (low nibble) from callback array
          this->type = (LIN_Slave_Base::frame_t) (this->callback[id].type_numData & 0xF0);
          this->numData = this->callback[id].type_numData & 0x0F;
          
          // call the user-defined callback function for this ID
          this->callback[id].fct(numData, this->bufData);

          // attach frame checksum
          bufData[numData] = this->_calculateChecksum(this->numData, this->bufData);

          // optionally enable RS485 transmitter
          _enableTransmitter();

          // send slave response (data+chk)
          this->_serialWrite(bufData, numData+1);

          // advance state to receiving echo
          this->state = LIN_Slave_Base::STATE_RECEIVING_ECHO;

          // optional debug output (debug level 2)
          #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 2)
            LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
            LIN_SLAVE_DEBUG_SERIAL.print(": LIN_Slave_Base::handler()");
            LIN_SLAVE_DEBUG_SERIAL.print(": handle slave response PID 0x");
            LIN_SLAVE_DEBUG_SERIAL.println(this->pid, HEX);
          #endif

        } // if slave response frame
        
        // if master request ID is registered, get number of data bytes and advance state
        else if ((this->callback[id].fct != nullptr) && (this->callback[id].type_numData & LIN_Slave_Base::MASTER_REQUEST))
        {
          // get type (high nibble) and number of response bytes (low nibble) from callback array
          this->type = (LIN_Slave_Base::frame_t) (this->callback[id].type_numData & 0xF0);
          this->numData = this->callback[id].type_numData & 0x0F;
          
          // advance state to receiving data
          this->state = LIN_Slave_Base::STATE_RECEIVING_DATA;
        
        } // if master request frame 
          
        // ID is not registered -> wait for next break
        else
        {
          // optional debug output (debug level 2)
          #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 2)
            LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
            LIN_SLAVE_DEBUG_SERIAL.print(": LIN_Slave_Base::handler()");
            LIN_SLAVE_DEBUG_SERIAL.print(": drop frame PID 0x");
            LIN_SLAVE_DEBUG_SERIAL.println(this->pid, HEX);
          #endif

          // reset state machine
          this->state = LIN_Slave_Base::STATE_WAIT_FOR_BREAK;

        } // if frame not registered

        break; // STATE_WAIT_FOR_PID


      // receive master request data
      case LIN_Slave_Base::STATE_RECEIVING_DATA:

        // check received data
        this->bufData[(this->idxData)++] = byteReceived;
        
        // if data is finished, advance to checksum check
        if (this->idxData >= this->numData)
          this->state = LIN_Slave_Base::STATE_WAIT_FOR_CHK;

        break; // STATE_RECEIVING_DATA


      // receive slave response echo
      case LIN_Slave_Base::STATE_RECEIVING_ECHO:

        // compare received echo to sent data
        if (this->bufData[(this->idxData)++] != byteReceived)
        {
          // set error and abort frame
          this->error = (LIN_Slave_Base::error_t) ((int) this->error | (int) LIN_Slave_Base::ERROR_ECHO);
          this->state = LIN_Slave_Base::STATE_DONE;

          // optionally disable RS485 transmitter
          _disableTransmitter();

          // optional debug output (debug level 1)
          #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 1)
            LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
            LIN_SLAVE_DEBUG_SERIAL.print(": LIN_Slave_Base::handler()");
            LIN_SLAVE_DEBUG_SERIAL.print(": echo error, received 0x");
            LIN_SLAVE_DEBUG_SERIAL.print(byteReceived, HEX);
            LIN_SLAVE_DEBUG_SERIAL.print(", expected 0x");
            LIN_SLAVE_DEBUG_SERIAL.println(this->bufData[(this->idxData)-1], HEX);
          #endif

        } // if echo error

        // if data is finished, finish frame
        else if (this->idxData >= this->numData+1)
        {
          this->state = LIN_Slave_Base::STATE_DONE;

          // optionally disable RS485 transmitter
          _disableTransmitter();
        }

        break; // STATE_RECEIVING_ECHO


      // Data has been received for master request frame, waiting for checksum
      case LIN_Slave_Base::STATE_WAIT_FOR_CHK:

        // calculate checksum for master request frame
        chk_calc = this->_calculateChecksum(this->numData, this->bufData);
        
        // Checksum valid -> call user-defined callback function for this ID
        if (byteReceived == chk_calc)
        {
          // call user-defined master request callback function. Only reachable if callback has been registered
          this->callback[id].fct(numData, bufData);

          // optional debug output (debug level 2)
          #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 2)
            LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
            LIN_SLAVE_DEBUG_SERIAL.print(": LIN_Slave_Base::handler()");
            LIN_SLAVE_DEBUG_SERIAL.print(": handle master request PID 0x");
            LIN_SLAVE_DEBUG_SERIAL.println(this->pid, HEX);
          #endif

        } // if checksum ok
        
        // checksum error
        else
        {
          // set error
          this->error = (LIN_Slave_Base::error_t) ((int) this->error | (int) LIN_Slave_Base::ERROR_CHK);

          // optional debug output (debug level 1)
          #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 1)
            LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
            LIN_SLAVE_DEBUG_SERIAL.print(": LIN_Slave_Base::handler()");
            LIN_SLAVE_DEBUG_SERIAL.print(": CHK error, received 0x");
            LIN_SLAVE_DEBUG_SERIAL.print(byteReceived, HEX);
            LIN_SLAVE_DEBUG_SERIAL.print(", calculated 0x");
            LIN_SLAVE_DEBUG_SERIAL.println(chk_calc, HEX);
          #endif

        } // if checksum error

        // frame is finished
        this->state = LIN_Slave_Base::STATE_DONE;

        // optionally disable RS485 transmitter
        _disableTransmitter();

        break; // STATE_WAIT_FOR_CHK


      // this should never happen -> error
      default:

        // set error and abort frame
        this->error = (LIN_Slave_Base::error_t) ((int) this->error | (int) LIN_Slave_Base::LIN_Slave_Base::ERROR_STATE);
        this->state = LIN_Slave_Base::STATE_DONE;

        // optionally disable RS485 transmitter
        _disableTransmitter();

        // optional debug output (debug level 1)
        #if defined(LIN_SLAVE_DEBUG_SERIAL) && (LIN_SLAVE_DEBUG_LEVEL >= 1)
          LIN_SLAVE_DEBUG_SERIAL.print(this->nameLIN);
          LIN_SLAVE_DEBUG_SERIAL.print(": LIN_Slave_Base::handler()");
          LIN_SLAVE_DEBUG_SERIAL.print(": error: illegal state ");
          LIN_SLAVE_DEBUG_SERIAL.print(this->state);
          LIN_SLAVE_DEBUG_SERIAL.println(", this should never happen...");
        #endif

    } // switch(state)

  } // if byte received

} // LIN_Slave_Base::handler

/*-----------------------------------------------------------------------------
    END OF FILE
-----------------------------------------------------------------------------*/