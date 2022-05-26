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
#define VALID_USER_ARGUMENTS       3U
#define POSITON_FRAME_ID           15U
#define VELOCITY_FRAME_ID          78U
#define POS_TIMESTAMP_NOK_POS      27U
#define POS_TIMESTAMP_NOK_NEG      23U
#define VEL_TIMESTAMP_NOK_POS      53U
#define VEL_TIMESTAMP_NOK_NEG      47U
#define FRAME_COUNT                2000U
/**********************************************************************************************************************
 * Preprocessor Directives
 *********************************************************************************************************************/
#define FALSE                 0U
#define TRUE                  1U 
#define NULL_PTR      ((void*)0U)
#define INPUT_FRAME_DATA      5U
#define SINT16_MAX            32767U
#define FRAME_DROP_CNT(x,y)  (((x-y) <= 1) ? 0U : framDrpCnt++)
/**********************************************************************************************************************/
/* TYPEDEF                                                                                                            */
/**********************************************************************************************************************/
/*Struct to save the inpy .csv file's data into it*/
typedef struct strInputFrameData_Tag
{
 uint8 u8_ID;
 uint16 u16_frameNB;
 uint16 u16_Timestamp;
 uint32 u32_payload;
 uint8 u8_checksum;
}str_inputFrameData;
/*Struct to save the calculated output data that will be printed to the output .csv file*/
typedef struct strOutputFrameData_Tag
{
 uint8 u8_ID;
 uint16 u16_frameNB;
 uint16 u16_Timestamp;
 float32 f32_positionX;
 float32 f32_positionY;
 float32 f32_velocityX;
 float32 f32_velocityY;
 boolean bool_checksumOK;
 boolean bool_timeoutOK;
 uint16 u16_frameDropCnt;
}str_outputFrameData;
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
boolean posChecTimeOK(uint16 currTimestamp);
boolean velChecTimeOK(uint16 currTimestamp);
boolean u8Checksum8BitsValid (uint32 u32data, uint16 u8Checksum);
void posFrameDecode(str_inputFrameData *recievedFrame, uint16 frameIdx);
void velFrameDecode(str_inputFrameData *recievedFrame, uint16 frameIdx);
void decodeContent(int argc, char *argv[]);

#endif /* LOG_DECODER_H */
/*---------------------------------------------------- end of file ---------------------------------------------------*/