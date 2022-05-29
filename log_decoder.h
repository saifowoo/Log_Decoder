/**********************************************************************************************************************/
/*                                                                                                                    */
/*  Application : Log Decoder                                                                                         */
/*  Description : Log decoder is a simple console application, that takes a .csv format logfile as an input           */
/*                and provides an output log file also in .csv format, with Payload decoded into meaningful           */
/*                values and additional flags if certains checks are violated for a given frame.                      */
/*                                                                                                                    */
/*  File        : LOG_DECODER_H.h                                                                                     */
/*                                                                                                                    */
/*  Author      : Saif El-Deen M.                                                                                     */
/*                                                                                                                    */
/*  Date        : 29/05/2022                                                                                          */
/*                                                                                                                    */
/**********************************************************************************************************************/

#ifndef LOG_DECODER_H
#define LOG_DECODER_H

/**********************************************************************************************************************/
/* INCLUDES                                                                                                           */
/**********************************************************************************************************************/
#include "log_decoder_Types.h"

/**********************************************************************************************************************/
/* DEFINES                                                                                                            */
/**********************************************************************************************************************/
/**********************************************************************************************************************
 * User Configurable Defines
 *********************************************************************************************************************/ 
#define FRAME_ID_POSITION               15U
#define FRAME_ID_VELOCITY               78U
#define POS_TIMESTAMP_PERIODICITY       25U
#define POS_TIMESTAMP_MARGIN            2U
#define VEL_TIMESTAMP_PERIODICITY       50U
#define VEL_TIMESTAMP_MARGIN            3U
#define FRAME_COUNT                2000U
#define VALID_USER_ARGUMENTS       3U
/**********************************************************************************************************************
 * Preprocessor Directives
 *********************************************************************************************************************/
#define FALSE                 0U
#define TRUE                  1U 
#define NULL_PTR      ((void*)0U)
/**********************************************************************************************************************/
/* TYPEDEF                                                                                                            */
/**********************************************************************************************************************/
typedef struct
{
    float32 f32PosX;
    float32 f32PosY;
    float32 f32VelX;
    float32 f32VelY;
}strDecodedDataType;
/*---------------------------------- Inputs ----------------------------------*/
typedef struct
{
    uint32 u32Payload;
    uint16 u16FrameNb;
    uint16 u16Timestamp;
    uint8 u8Id;
    uint8 u8Checksum;
}LogDecoder_strInputDataType;
/*---------------------------------- Outputs ---------------------------------*/
typedef struct
{
    strDecodedDataType strDecodedData;
    uint16 u16FrameDropCnt;
    uint16 u16FrameNb;
    uint16 u16Timestamp;
    boolean bChecksumOK;
    boolean bTimeoutOK;
    uint8 u8Id;
}LogDecoder_strOutputDataType;
/*----------------------------------------------------------------------------*/
/**********************************************************************************************************************/
/* GLOBAL VARIABLES                                                                                                   */
/**********************************************************************************************************************/

/**********************************************************************************************************************/
/* MACRO FUNCTIONS                                                                                                    */
/**********************************************************************************************************************/
/**********************************************************************************************************************/
/*                                                                                                                    */
/* !FuncName    :                                                                                                     */
/* !Description :                                                                                                     */
/* !Number      : 1                                                                                                   */
/* !Reference   :                                                                                                     */
/*                                                                                                                    */
/* !Trace_To    :                                                                                                     */
/*                                                                                                                    */
/**********************************************************************************************************************/
/* !LastAuthor  :  Developer                                                                                          */
/**********************************************************************************************************************/

/**********************************************************************************************************************/
/* INTERNAL MACRO FUNCTIONS                                                                                           */
/**********************************************************************************************************************/

/**********************************************************************************************************************/
/* GLOBAL FUNCTIONS PROTOTYPES                                                                                        */
/**********************************************************************************************************************/
uint16 LogDecoder_u8PosCalcFrameDropCnt(uint16 u16FrameNB);
uint16 LogDecoder_u8VelCalcFrameDropCnt(uint16 u16FrameNB);
boolean LogDecoder_bPosTimeOutStatus(uint16 u16FrameTimestamp);
boolean LogDecoder_bVelTimeOutStatus(uint16 u16FrameTimestamp);
boolean LogDecoder_bTimeOutStatus(uint8 u8FrameId, uint16 u16FrameTimestamp);
boolean LogDecoder_u8ChecksumStatus(uint32 u32PayloadValue, uint8 u8Checksum);
strDecodedDataType LogDecoder_strPosFrameDecode(uint32 u32PayloadValue);
strDecodedDataType LogDecoder_strVelFrameDecode(uint32 u32PayloadValue);
LogDecoder_strOutputDataType LogDecoder_vOutputGeneration(LogDecoder_strInputDataType strInputData);
void LogDecoder_vidMainFunction(int argc, char *argv[]);

#endif /* LOG_DECODER_H */
/*---------------------------------------------------- end of file ---------------------------------------------------*/