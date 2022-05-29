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
#define ARGUMENTS_NUMBER                3U
#define INPUT_ARGUMENT_NUMBER           1U
#define OUTPUT_ARGUMENT_NUMBER          2U
#define FRAME_ID_POSITION               15U
#define FRAME_ID_VELOCITY               78U
#define POS_TIMESTAMP_PERIODICITY       25U
#define POS_TIMESTAMP_MARGIN            2U
#define VEL_TIMESTAMP_PERIODICITY       50U
#define VEL_TIMESTAMP_MARGIN            3U
#define ELEMENTS_NUM_PER_ROW            5U
#define MAX_CHAR_NUM_PER_LINE           100U
#define HEADER_FOR_INPUT_FILE           "ID,FrameNb,Timestamp,Payload,Checksum"
#define HEADER_FOR_OUTPUT_FILE          "ID,FrameNB,Timestamp,X-Position,Y-Position,X-Velocity,Y-Velocity,ChecksumOK,TimestampOk,FrameDropCnt\n"

/**********************************************************************************************************************/
/* TYPEDEF                                                                                                            */
/**********************************************************************************************************************/
/*---------------------------------- Inputs ----------------------------------*/
typedef struct
{
    uint32 u32Payload;
    uint16 u16FrameNb;
    uint16 u16Timestamp;
    uint8  u8Id;
    uint8  u8Checksum;
}LogDecoder_strInputDataType;
/*---------------------------------- Outputs ---------------------------------*/
typedef struct
{
    float32 f32PosX;
    float32 f32PosY;
    float32 f32VelX;
    float32 f32VelY;
}strDecodedDataType;
typedef struct
{
    strDecodedDataType strDecodedData;
    uint16             u16FrameDropCnt;
    uint16             u16FrameNb;
    uint16             u16Timestamp;
    boolean            bChecksumOK;
    boolean            bTimeoutOK;
    uint8              u8Id;
}LogDecoder_strOutputDataType;

/**********************************************************************************************************************/
/* GLOBAL FUNCTIONS PROTOTYPES                                                                                        */
/**********************************************************************************************************************/
void LogDecoder_vidMainFunction(int s32NumOfArg, char **ptrMainArgs);
int main(int argc, char **argv);

#endif /* LOG_DECODER_H */
/*---------------------------------------------------- end of file ---------------------------------------------------*/