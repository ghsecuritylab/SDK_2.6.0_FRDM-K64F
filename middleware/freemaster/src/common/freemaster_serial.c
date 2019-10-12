/*
 * Copyright (c) 2007-2015 Freescale Semiconductor, Inc.
 * Copyright 2018-2019 NXP
 *
 * License: NXP LA_OPT_NXP_Software_License
 *
 * NXP Confidential. This software is owned or controlled by NXP and may
 * only be used strictly in accordance with the applicable license terms.
 * By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that
 * you have read, and that you agree to comply with and are bound by,
 * such license terms.  If you do not agree to be bound by the applicable
 * license terms, then you may not retain, install, activate or otherwise
 * use the software.  This code may only be used in a microprocessor,
 * microcontroller, sensor or digital signal processor ("NXP Product")
 * supplied directly or indirectly from NXP.  See the full NXP Software
 * License Agreement in license/LA_OPT_NXP_Software_License.pdf
 *
 * FreeMASTER Communication Driver - Serial communication
 */

#include "freemaster.h"
#include "freemaster_private.h"

/* Numeric identifier to help pre-processor to identify whether our transport is used or not. */
#define FMSTR_SERIAL_ID 1

#if (FMSTR_MK_IDSTR(FMSTR_TRANSPORT) == FMSTR_SERIAL_ID) && (!(FMSTR_DISABLE))

#include "freemaster_protocol.h"
#include "freemaster_serial.h"
#include "freemaster_utils.h"

/* Serial driver interface structure, defined in freemaster_defcfg.h */
extern const FMSTR_SERIAL_DRV_INTF FMSTR_SERIAL_DRV;

/***********************************
*  local variables
***********************************/

/* FreeMASTER communication buffer (in/out) plus the STS, LEN(LEB) and CRC bytes */
static FMSTR_BCHR fmstr_pCommBuffer[FMSTR_COMM_BUFFER_SIZE+1+4+2];

/* FreeMASTER runtime flags */
/*lint -e{960} using union */
typedef volatile union
{
    FMSTR_FLAGS all;

    struct
    {
        unsigned bTxActive : 1;         /* response is being transmitted */
        unsigned bTxWaitTC : 1;         /* response sent, wait for transmission complete */
        unsigned bTxLastCharSOB   : 1;  /* last transmitted char was equal to SOB  */
        unsigned bRxLastCharSOB   : 1;  /* last received character was SOB */
        unsigned bRxMsgLengthNext : 1;  /* expect the length byte next time */
    } flg;

} FMSTR_SERIAL_FLAGS;

static FMSTR_SERIAL_FLAGS _fmstr_wFlags;

/* receive and transmit buffers and counters */
static FMSTR_SIZE8 fmstr_nTxTodo;       /* transmission to-do counter (0 when tx is idle) */
static FMSTR_SIZE8 fmstr_nRxTodo;       /* reception to-do counter (0 when rx is idle) */
static FMSTR_BPTR  fmstr_pTxBuff;       /* pointer to next byte to transmit */
static FMSTR_BPTR  fmstr_pRxBuff;       /* pointer to next free place in RX buffer */
static FMSTR_BCHR  fmstr_nRxCrc8;       /* checksum of data being received for short messages */

/* Maximal length of message to use 8 bit CRC8. */
#define FMSTR_SHORT_MSG_LEN     128

#if FMSTR_DEBUG_TX
/* The poll counter is used to roughly measure duration of test frame transmission.
 * The test frame will be sent once per N.times this measured period. */
static FMSTR_S32 fmstr_nDebugTxPollCount;
/* the N factor for multiplying the transmission time to get the wait time */
#define FMSTR_DEBUG_TX_POLLCNT_XFACTOR 32
#define FMSTR_DEBUG_TX_POLLCNT_MIN     (-1*0x4000000L)
#endif

/***********************************
*  local function prototypes
***********************************/

static void _FMSTR_Listen(void);
static void _FMSTR_SendError(FMSTR_BCHR nErrCode);
static FMSTR_BOOL _FMSTR_Tx(FMSTR_BCHR* getTxChar);
static FMSTR_BOOL _FMSTR_Rx(FMSTR_BCHR rxChar);

/***********************************
*  local variables
***********************************/

/* SHORT_INTR receive queue (circular buffer) */
#if FMSTR_SHORT_INTR
static FMSTR_BCHR fmstr_rxBuff[FMSTR_COMM_RQUEUE_SIZE];
static FMSTR_RING_BUFFER fmstr_rxQueue;
#endif

/***********************************
*  local function prototypes
***********************************/

#if FMSTR_SHORT_INTR
static void _FMSTR_RxDequeue(void);
#endif

/* Interface function - Initialization of serial transport */
static FMSTR_BOOL _FMSTR_SerialInit(void);
/* Interface function - Poll function of serial transport */
static void _FMSTR_SerialPoll(void);
/* Interface function - Send Response function of serial transport */
static void _FMSTR_SerialSendResponse(FMSTR_BPTR pResponse, FMSTR_SIZE nLength, FMSTR_U8 statusCode);

/***********************************
*  global variables
***********************************/
/* Interface of this serial driver */
const FMSTR_TRANSPORT_INTF FMSTR_SERIAL =
{
    .Init = _FMSTR_SerialInit,
    .Poll = _FMSTR_SerialPoll,
    .SendResponse = _FMSTR_SerialSendResponse,
};

/*lint -esym(752,_FMSTR_RxQueue) this may be unreferenced in some cases */
/*lint -esym(752,_FMSTR_RxDequeue) this may be unreferenced in some cases */

/*******************************************************************************
*
* @brief    Routine to quick-receive a character (put to a queue only)
*
* This function puts received character into a queue and exits as soon as possible.
*
*******************************************************************************/

#if FMSTR_SHORT_INTR

/*******************************************************************************
*
* @brief    Late processing of queued characters
*
* This function takes the queued characters and calls FMSTR_Rx() for each of them,
* just like as the characters would be received from SCI one by one.
*
*******************************************************************************/

static void _FMSTR_RxDequeue(void)
{
    FMSTR_BCHR nChar = 0U;

    /* get all queued characters */
    while(_FMSTR_RingBuffHasData(&fmstr_rxQueue))
    {
        nChar = _FMSTR_RingBuffGet(&fmstr_rxQueue);

        /* emulate the SCI receive event */
        if(!_fmstr_wFlags.flg.bTxActive)
        {
            (void)_FMSTR_Rx(nChar);
        }
    }
}

#endif /* FMSTR_SHORT_INTR */


/**************************************************************************//*!
*
* @brief    Handle SCI communication (both TX and RX)
*
* This function checks the SCI flags and calls the Rx and/or Tx functions
*
* @note This function can be called either from SCI ISR or from the polling routine
*
******************************************************************************/

void FMSTR_ProcessSerial(void)
{
    FMSTR_BOOL endOfPacket = FMSTR_FALSE;

    /* transmitter active and empty? */
    if (_fmstr_wFlags.flg.bTxActive)
    {
        /* able to accept another character? */
        while(FMSTR_SERIAL_DRV.IsTransmitRegEmpty())
        {
            FMSTR_BCHR ch;
            /* just put the byte into the SCI transmit buffer */
            endOfPacket = _FMSTR_Tx(&ch);
            if(!endOfPacket)
                FMSTR_SERIAL_DRV.PutChar((FMSTR_U8) ch);
            else
                break;
        }

        /* Flush data */
        if(endOfPacket)
            FMSTR_SERIAL_DRV.Flush();

        /* waiting for transmission complete flag? */
        if(_fmstr_wFlags.flg.bTxWaitTC && !FMSTR_SERIAL_DRV.IsTransmitterActive())
        {
            /* after TC, we can switch to listen mode safely */
            _FMSTR_Listen();
        }


//TODO: solve single wire detection
//#if !FMSTR_SCI_TWOWIRE_ONLY
//        /* read-out and ignore any received character (loopback) */
//        if(FMSTR_SERIAL_DRV.IsReceiveRegFull())
//        {
//            /*lint -esym(550, rxChar) */
//            volatile FMSTR_U8 rxChar;
//            rxChar = FMSTR_SERIAL_DRV.GetChar();
//            FMSTR_UNUSED(rxChar);
//        }
//#endif
    }
    /* transmitter not active, able to receive */
    else
    {
        /* data byte received? */
        while (FMSTR_SERIAL_DRV.IsReceiveRegFull())
        {
            FMSTR_BCHR rxChar = 0U;
            rxChar = FMSTR_SERIAL_DRV.GetChar();

#if FMSTR_SHORT_INTR
            _FMSTR_RingBuffPut(&fmstr_rxQueue, rxChar);    //TODO: if queue is lower than received data
#else
            (void)_FMSTR_Rx(rxChar);
#endif
        }
#if FMSTR_DEBUG_TX
        /* time to send another test frame? */
        if(fmstr_bDebugTx && fmstr_nDebugTxPollCount == 0)
        {
            /* yes, start sending it now */
            if(FMSTR_SendTestFrame(&fmstr_pCommBuffer[2]))
            {
                /* measure how long it takes to transmit it */
                fmstr_nDebugTxPollCount = -1;
            }
        }
#endif
    }
}

/**************************************************************************//*!
*
* @brief    Start listening on a serial line
*
* Reset the receiver machine and start listening on a serial line
*
******************************************************************************/

static void _FMSTR_Listen(void)
{
    fmstr_nRxTodo = 0U;

    /* disable transmitter state machine */
    _fmstr_wFlags.flg.bTxActive = 0U;
    _fmstr_wFlags.flg.bTxWaitTC = 0U;
    /* disable transmitter, enable receiver (enables single-wire connection) */
#if !FMSTR_SCI_TWOWIRE_ONLY
    FMSTR_SERIAL_DRV.EnableTransmit(FMSTR_FALSE);
    FMSTR_SERIAL_DRV.EnableReceive(FMSTR_TRUE);
#endif

    /* disable transmit, enable receive interrupts */
#if FMSTR_SHORT_INTR || FMSTR_LONG_INTR
    FMSTR_SERIAL_DRV.EnableTransmitInterrupt(FMSTR_FALSE);   /* disable SCI transmit interrupt */
    FMSTR_SERIAL_DRV.EnableReceiveInterrupt(FMSTR_TRUE);   /* enable SCI receive interrupt */
#endif /* FMSTR_SHORT_INTR || FMSTR_LONG_INTR */

#if FMSTR_DEBUG_TX
    /* we have just finished the transmission of the test frame, now wait the 32x times the sendtime
       to receive any command from PC (count<0 is measurement, count>0 is waiting, count=0 is send trigger) */
    if(fmstr_nDebugTxPollCount < 0)
        fmstr_nDebugTxPollCount *= -(FMSTR_DEBUG_TX_POLLCNT_XFACTOR);
#endif
}

/**************************************************************************//*!
*
* @brief    Send response of given error code (no data)
*
* @param    nErrCode - error code to be sent
*
******************************************************************************/

static void _FMSTR_SendError(FMSTR_BCHR nErrCode)
{
    /* fill & send single-byte response */
    _FMSTR_SerialSendResponse(&fmstr_pCommBuffer[2], 0U, nErrCode);
}

/**************************************************************************//*!
*
* @brief    Finalize transmit buffer before transmitting
*
* @param    nLength - response length (1 for status + data length)
*
*
* This Function takes the data already prepared in the transmit buffer
* (inlcuding the status byte). It computes the check sum and kicks on tx.
*
******************************************************************************/

static void _FMSTR_SerialSendResponse(FMSTR_BPTR pResponse, FMSTR_SIZE nLength, FMSTR_U8 statusCode)
{
    FMSTR_SIZE8 i;
    FMSTR_U8 c;

    if(nLength > 254 || pResponse != &fmstr_pCommBuffer[2])
    {
        /* The Serial driver doesn't support bigger responses than 254 bytes, change the response to status error */
        statusCode = FMSTR_STC_RSPBUFFOVF;
        nLength = 0;
    }

    /* remember the buffer to be sent */
    fmstr_pTxBuff = fmstr_pCommBuffer;
    /* send the message with status, length and the checksum and the SOB */
    fmstr_nTxTodo = (FMSTR_SIZE8) (nLength + 3);

    if(statusCode & FMSTR_STSF_VARLEN)
    {
        fmstr_pCommBuffer[0] = statusCode;
        fmstr_pCommBuffer[1] = nLength;
    }
    else
    {
        fmstr_pCommBuffer[1] = statusCode;
        fmstr_pTxBuff++;
        fmstr_nTxTodo--;
    }

    /* Initialize CRC algorithms */
    FMSTR_Crc8Init(&fmstr_nRxCrc8);

    /* status byte and data are already there, compute checksum only     */
    pResponse = fmstr_pTxBuff;
    for (i=0U; i<(fmstr_nTxTodo - 1); i++)
    {
        pResponse = FMSTR_ValueFromBuffer8(&c, pResponse);
        /* add character to checksum */
        FMSTR_Crc8AddByte(&fmstr_nRxCrc8, c);
    }

    /* store checksum after the message */
    pResponse = FMSTR_ValueToBuffer8(pResponse, fmstr_nRxCrc8);

    /* now transmitting the response */
    _fmstr_wFlags.flg.bTxActive = 1U;
    _fmstr_wFlags.flg.bTxWaitTC = 0U;

    /* do not replicate the initial SOB  */
    _fmstr_wFlags.flg.bTxLastCharSOB = 0U;

    /* disable receiver, enable transmitter (single-wire communication) */
#if !FMSTR_SCI_TWOWIRE_ONLY
    FMSTR_SERIAL_DRV.EnableReceive(FMSTR_FALSE);
    FMSTR_SERIAL_DRV.EnableTransmit(FMSTR_TRUE);
#endif
    /* kick on the SCI transmission (also clears TX Empty flag on some platforms) */
    (void)FMSTR_SERIAL_DRV.IsTransmitRegEmpty();
    FMSTR_SERIAL_DRV.PutChar(FMSTR_SOB);

    /* TX interrupt enable, RX interrupt disable */
#if (FMSTR_LONG_INTR) || (FMSTR_SHORT_INTR)
    FMSTR_SERIAL_DRV.EnableReceiveInterrupt(FMSTR_FALSE);   /* disable SCI receive interrupt */
    FMSTR_SERIAL_DRV.EnableTransmitInterrupt(FMSTR_TRUE);   /* enable SCI transmit interrupt */
#endif /* FMSTR_LONG_INTR || FMSTR_SHORT_INTR */
}

/**************************************************************************//*!
*
* @brief    Output buffer transmission
*
* @param  getTxChar Return character to be transmitted
*
* @return Non-zero when transmission is complete
*
* get ready buffer(prepare data to send)
*
******************************************************************************/

static FMSTR_BOOL _FMSTR_Tx(FMSTR_BCHR* getTxChar)
{
    if (fmstr_nTxTodo)
    {
        /* fetch & send character ready to transmit */
        /*lint -e{534} ignoring return value */
        (void)FMSTR_ValueFromBuffer8(getTxChar, fmstr_pTxBuff);

        /* first, handle the replicated SOB characters */
        if (*getTxChar == FMSTR_SOB)
        {
            _fmstr_wFlags.flg.bTxLastCharSOB ^= 1U;
            if ((_fmstr_wFlags.flg.bTxLastCharSOB))
            {
                /* yes, repeat the SOB next time */
                return FMSTR_FALSE;
            }
        }
        
        /* no, advance tx buffer pointer */
        fmstr_nTxTodo--;
        fmstr_pTxBuff = FMSTR_SkipInBuffer(fmstr_pTxBuff, 1U);
        return FMSTR_FALSE;
    }

    /* when SCI TX buffering is enabled, we must first wait until all
       characters are physically transmitted (before disabling transmitter) */

    /* waiting for transmission complete flag? */
    if(!FMSTR_SERIAL_DRV.IsTransmitterActive())
    {
        /* after TC, we can switch to listen mode safely */
        _FMSTR_Listen();
    }
    else
    {
        _fmstr_wFlags.flg.bTxWaitTC = 1;
    }
    /* wait for SCI TC interrupt */
// TODO What is this??
    //    #if FMSTR_SHORT_INTR || FMSTR_LONG_INTR
//    FMSTR_SCI_ETCI();
//    #endif

    return FMSTR_TRUE;
}


/**************************************************************************//*!
*
* @brief  Handle received character
*
* @param  rxChar  The character to be processed
*
* Handle the character received and -if the message is complete- call the
* protocol decode routine.
*
******************************************************************************/

static FMSTR_BOOL _FMSTR_Rx(FMSTR_BCHR rxChar)
{
    FMSTR_SERIAL_FLAGS * pflg = &_fmstr_wFlags;
    /* first, handle the replicated SOB characters */
    if(rxChar == FMSTR_SOB)
    {
        pflg->flg.bRxLastCharSOB ^= 1;
        if(pflg->flg.bRxLastCharSOB)
        {
            /* this is either the first byte of replicated SOB or a  */
            /* real Start-of-Block mark - we will decide next time in FMSTR_Rx */
            return FMSTR_FALSE;
        }
    }

    /* we have got a common character preceded by the SOB -  */
    /* this is the command code! */
    if(pflg->flg.bRxLastCharSOB)
    {
        /* reset receiving process */
        fmstr_pRxBuff = fmstr_pCommBuffer;

        FMSTR_Crc8Init(&fmstr_nRxCrc8);
        FMSTR_Crc8AddByte(&fmstr_nRxCrc8, rxChar);

        *(fmstr_pRxBuff++) = rxChar;
        fmstr_nRxTodo = 0;

        /* if the standard command was received, the message length will come in next byte */
        pflg->flg.bRxMsgLengthNext = 1U;

        /* command code stored & processed */
        pflg->flg.bRxLastCharSOB = 0U;
        return FMSTR_FALSE;
    }

    /* we are waiting for the length byte */
    if(pflg->flg.bRxMsgLengthNext)
    {
        /* total data length and the checksum */
        fmstr_nRxTodo = rxChar + 1;
        FMSTR_Crc8AddByte(&fmstr_nRxCrc8, rxChar);

        *(fmstr_pRxBuff++) = rxChar;

        /* now read the data bytes */
        pflg->flg.bRxMsgLengthNext = 0U;

        return FMSTR_FALSE;
    }

    /* waiting for a data byte? */
    if(fmstr_nRxTodo)
    {
        /* decrease number of expected bytes */
        fmstr_nRxTodo--;
        /* was it the last byte of the message (checksum)? */
        if(!fmstr_nRxTodo)
        {
            /* receive buffer overflow? */
            if(fmstr_pRxBuff == NULL)
            {
                _FMSTR_SendError(FMSTR_STC_CMDTOOLONG);
            }
            /* checksum error? */
            else if(fmstr_nRxCrc8 != rxChar)
            {
                _FMSTR_SendError(FMSTR_STC_CMDCSERR);
            }
            /* message is okay */
            else
            {
                FMSTR_BPTR pMessageIO = fmstr_pCommBuffer;
                FMSTR_U8 nCmd, nSize;

                /* command code comes first in the message */
                /*lint -e{534} return value is not used */
                pMessageIO = FMSTR_ValueFromBuffer8(&nCmd, pMessageIO);
                /* lenght of command follows */
                /*lint -e{534} return value is not used */
                pMessageIO = FMSTR_ValueFromBuffer8(&nSize, pMessageIO);

                /* do decode now! */
                FMSTR_ProtocolDecoder(pMessageIO, nSize, nCmd);
            }

            return FMSTR_TRUE;
        }
        /* not the last character yet */
        else
        {
            /* add this byte to checksum */
            FMSTR_Crc8AddByte(&fmstr_nRxCrc8, rxChar);

            /* is there still a space in the buffer? */
            if(fmstr_pRxBuff)
            {
                /*lint -e{946} pointer arithmetic is okay here (same array) */
                if(fmstr_pRxBuff < (fmstr_pCommBuffer + FMSTR_COMM_BUFFER_SIZE))
                {
                    /* store byte  */
                    *fmstr_pRxBuff++ = rxChar;
                }
                /* buffer is full! */
                else
                {
                    /* NULL rx pointer means buffer overflow - but we still need */
                    /* to receive all message characters (for the single-wire mode) */
                    /* so keep "receiving" - but throw away all characters from now */
                    fmstr_pRxBuff = NULL;
                }
            }
        }
    }
    
    return FMSTR_FALSE;
}

/**************************************************************************//*!
*
* @brief    Serial communication initialization
*
******************************************************************************/

static FMSTR_BOOL _FMSTR_SerialInit(void)
{
    /* initialize all state variables */
    _fmstr_wFlags.all = 0U;
    fmstr_nTxTodo = 0U;

    /* Check the interface if it's valid */   //TODO this could be conditional to save some Flash
    if(!FMSTR_SERIAL_DRV.Init)
        return FMSTR_FALSE;
    if(!FMSTR_SERIAL_DRV.EnableTransmit)
        return FMSTR_FALSE;
    if(!FMSTR_SERIAL_DRV.EnableReceive)
        return FMSTR_FALSE;
    if(!FMSTR_SERIAL_DRV.EnableTransmitInterrupt)
        return FMSTR_FALSE;
    if(!FMSTR_SERIAL_DRV.EnableReceiveInterrupt)
        return FMSTR_FALSE;
    if(!FMSTR_SERIAL_DRV.IsTransmitRegEmpty)
        return FMSTR_FALSE;
    if(!FMSTR_SERIAL_DRV.IsReceiveRegFull)
        return FMSTR_FALSE;
    if(!FMSTR_SERIAL_DRV.IsTransmitterActive)
        return FMSTR_FALSE;
    if(!FMSTR_SERIAL_DRV.PutChar)
        return FMSTR_FALSE;
    if(!FMSTR_SERIAL_DRV.GetChar)
        return FMSTR_FALSE;
    if(!FMSTR_SERIAL_DRV.Flush)
        return FMSTR_FALSE;

    /* Call initialization of serial driver */
    FMSTR_SERIAL_DRV.Init();

    /* Initialize SCI and JTAG interface */
#if FMSTR_SCI_TWOWIRE_ONLY
    /* to enable TX and RX together in FreeMASTER initialization */
    FMSTR_SERIAL_DRV.EnableReceive(FMSTR_TRUE);   /* enable SCI receive interrupt */
    FMSTR_SERIAL_DRV.EnableTransmit(FMSTR_TRUE);   /* enable SCI transmit interrupt */
#endif

#if FMSTR_SHORT_INTR
    _FMSTR_RingBuffCreate(&fmstr_rxQueue, fmstr_rxBuff, FMSTR_COMM_RQUEUE_SIZE);
#endif

#if FMSTR_DEBUG_TX
    /* this zero will initiate the test frame transmission
     * as soon as possible during Listen */
    fmstr_nDebugTxPollCount = 0;
#endif

    /* start listening for commands */
    _FMSTR_Listen();
    return FMSTR_TRUE;
}


/*******************************************************************************
*
* @brief    API: Serial Transport "Polling" call from the application main loop
*
* This function either handles all the SCI communication (polling-only mode =
* FMSTR_POLL_DRIVEN) or decodes messages received on the background by SCI interrupt
* (short-interrupt mode = FMSTR_SHORT_INTR).
*
*
*******************************************************************************/

static void _FMSTR_SerialPoll(void)
{

#if FMSTR_POLL_DRIVEN

    /* polled SCI mode */
    FMSTR_ProcessSerial();

#elif FMSTR_SHORT_INTR
    /* process queued SCI characters */
    _FMSTR_RxDequeue();
#endif

#if FMSTR_DEBUG_TX
    /* down-counting the polls for heuristic time measurement */
    if(fmstr_nDebugTxPollCount != 0 && fmstr_nDebugTxPollCount > FMSTR_DEBUG_TX_POLLCNT_MIN)
        fmstr_nDebugTxPollCount--;
#endif
}

#endif /* !(FMSTR_DISABLE) */
