/********************************************************************************
**                                                                            **
**  FILENAME  : log_decoder.h                                                 **
**                                                                            **
**  DATE      : 2022-05-020                                                   **
**                                                                            **
**  AUTHOR    : Mohamed Tarek Abozaid                                         **
********************************************************************************/
#ifndef LOG_DECODER_H
#define LOG_DECODER_H
/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include <stdio.h>
/**********************************************************************************************************************
 * Global Type Definitions
 *********************************************************************************************************************/
typedef unsigned char       boolean; 
typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned long       uint32;
typedef short               sint16;
typedef float               float32;
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
/**********************************************************************************************************************
 * Type Declarations
 *********************************************************************************************************************/
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
/**********************************************************************************************************************
 * Function Declarations
 *********************************************************************************************************************/
int main(int argc, char **argv);

#endif  /* SCR_WAKE_DEVELOPMENT */