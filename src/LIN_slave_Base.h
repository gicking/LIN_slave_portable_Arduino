/**
  \file     LIN_slave_Base.h
  \brief    Base class for LIN slave emulation (non-functional)
  \details  This library provides the base class for a slave node emulation of a LIN bus. 
            The base class is non-functional, as it lacks the actual communication interface.
            For an explanation of the LIN bus and protocol e.g. see https://en.wikipedia.org/wiki/Local_Interconnect_Network
  \author   Georg Icking-Konert
*/

/*-----------------------------------------------------------------------------
  MODULE DEFINITION FOR MULTIPLE INCLUSION
-----------------------------------------------------------------------------*/
#ifndef _LIN_SLAVE_BASE_H_
#define _LIN_SLAVE_BASE_H_


/*-----------------------------------------------------------------------------
  GLOBAL DEFINES
-----------------------------------------------------------------------------*/

// misc parameters
#define LIN_SLAVE_BUFLEN_NAME   30            //!< max. length of node name
#define LIN_SLAVE_RX_TIMEOUT    10            //!< max. time [ms] between bytes in frame

// optional LIN debug output. For AVR must use NeoSerialx to avoid linker conflict
//#define LIN_SLAVE_DEBUG_SERIAL  Serial        //!< serial interface used for debug output. Comment out for none
//#define LIN_SLAVE_DEBUG_SERIAL  NeoSerial     //!< serial interface used for debug output. Comment out for none
#define LIN_SLAVE_DEBUG_LEVEL   3             //!< debug verbosity 0..3 (1=errors only, 3=verbose)


/*-----------------------------------------------------------------------------
  INCLUDE FILES
-----------------------------------------------------------------------------*/

#include <Arduino.h>


/*-----------------------------------------------------------------------------
  GLOBAL CLASS
-----------------------------------------------------------------------------*/

/**
  \brief  LIN slave node base class

  \details LIN slave node base class. From this class the actual LIN classes for a Serialx are derived.
*/
class LIN_Slave_Base
{
  // PUBLIC TYPEDEFS
  public:

    /// LIN protocol version 
    typedef enum
    {
      LIN_V1 = 1,                               //!< LIN protocol version 1.x
      LIN_V2 = 2                                //!< LIN protocol version 2.x
    } version_t;


    /// LIN state machine states
    typedef enum
    { 
      WAIT_FOR_BREAK  = 0x00,                   //!< waiting for sync break
      WAIT_FOR_SYNC   = 0x01,                   //!< waiting for sync field
      WAIT_FOR_PID    = 0x02,                   //!< waiting for frame PID
      RECEIVING_DATA  = 0x03,                   //!< receiving data
      CHECKSUM_VERIFY = 0x04                    //!< waiting for checksum
    } state_t;


    /// LIN error codes. Use bitmasks, as error is latched
    typedef enum
    {
      NO_ERROR      = 0x00,                     //!< no error
      ERROR_STATE   = 0x01,                     //!< error in LIN state machine
      ERROR_BREAK   = 0x02,                     //!< error in BREAK (not 0x00) 
      ERROR_SYNC    = 0x04,                     //!< error in SYNC (not 0x55) 
      ERROR_TIMEOUT = 0x08,                     //!< frame timeout error
      ERROR_CHK     = 0x10,                     //!< LIN checksum error
      ERROR_MISC    = 0x80                      //!< misc error, should not occur
    } error_t;


    /// Pointer to frame handler
    typedef void (*LinMessageHandler)(uint8_t numData, uint8_t* data);

    /// User-defined frame handler with data length
    typedef struct
    {
      LinMessageHandler handler;
      uint8_t           numData;
    } handler_t;



  // PROTECTED VARIABLES
  protected:

    // node properties
    Stream                *pSerial;                   //!< pointer to serial I/F
    uint16_t              baudrate;                   //!< communication baudrate [Baud]
    LIN_Slave_Base::version_t  version;               //!< LIN protocol version
    LIN_Slave_Base::state_t    state;                 //!< status of LIN state machine
    LIN_Slave_Base::error_t    error;                 //!< error state. Is latched until cleared
    bool                  flagBreak;                  //!< flag for BREAK detected. Needs to be set in Rx-ISR 
    LIN_Slave_Base::handler_t  requestHandlers[64];   //!< handlers for master request IDs 0x00..0x3F
    LIN_Slave_Base::handler_t  responseHandlers[64];  //!< handlers for slave response IDs 0x00..0x3F

    // frame properties
    uint8_t               pid;                        //!< protected frame identifier
    uint8_t               id;                         //!< unprotected frame identifier
    uint8_t               numData;                    //!< number of data bytes in frame
    uint8_t               bufRx[8];                   //!< buffer for data bytes (max 8B)
    uint8_t               idxData;                    //!< current index in bufRx
    uint32_t              timeLastRx;                 //!< time [ms] of last received byte in frame


  // PUBLIC VARIABLES
  public:

    char                  nameLIN[LIN_SLAVE_BUFLEN_NAME];    //!< LIN node name, e.g. for debug


  // PROTECTED METHODS
  protected:
  
    /// @brief Calculate LIN frame checksum
    uint8_t _calculateChecksum(uint8_t NumData, uint8_t Data[]);
    
    /// @brief Get break detection flag. Is hardware dependent
    virtual bool _getBreakFlag(void);

    /// @brief Clear break detection flag. Is hardware dependent
    virtual void _resetBreakFlag(void);


  // PUBLIC METHODS
  public:
  
    /// @brief LIN master node constructor
    LIN_Slave_Base(LIN_Slave_Base::version_t Version, const char NameLIN[]);
    
    /// @brief Open serial interface
    virtual void begin(uint16_t Baudrate);
    
    /// @brief Close serial interface
    virtual void end(void);


    /// @brief Clear error of LIN state machine
    inline void resetError(void) { this->error = LIN_Slave_Base::NO_ERROR; }
    
    /// @brief Getter for LIN state machine error
    inline LIN_Slave_Base::error_t getError(void) { return this->error; }


    /// @brief Attach callback function for master request frame
    void registerMasterRequestHandler(uint8_t ID, LinMessageHandler Handler, uint8_t NumData);

    /// @brief Attach callback function for slave response frame
    void registerSlaveResponseHandler(uint8_t ID, LinMessageHandler Handler, uint8_t NumData);


    /// @brief Handle LIN protocol and call user-defined frame handlers
    virtual void handler(void);

}; // class LIN_Slave_Base

/*-----------------------------------------------------------------------------
    END OF MODULE DEFINITION FOR MULTIPLE INLUSION
-----------------------------------------------------------------------------*/
#endif // _LIN_SLAVE_BASE_H_
