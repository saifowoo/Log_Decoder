/**********************************************************************************************************************/
/*                                                                                                                    */
/*  Application : Log Decoder                                                                                         */
/*  Description : Log decoder is a simple console application, that takes a .csv format logfile as an input           */
/*                and provides an output log file also in .csv format, with Payload decoded into meaningful           */
/*                values and additional flags if certains checks are violated for a given frame.                      */
/*                                                                                                                    */
/*  File        : log_decoder.c                                                                                       */
/*                                                                                                                    */
/*  Author      : Saif El-Deen M.                                                                                     */
/*                                                                                                                    */
/*  Date        : 29/05/2022                                                                                          */
/*                                                                                                                    */
/**********************************************************************************************************************/
/* 1 / LogDecoder_u8PosCalcFrameDropCnt                                                                               */
/* 2 / LogDecoder_u8VelCalcFrameDropCnt                                                                               */
/* 3 / LogDecoder_bPosTimeOutStatus                                                                                   */
/* 4 / LogDecoder_bVelTimeOutStatus                                                                                   */
/* 5 / LogDecoder_u8ChecksumStatus                                                                                    */
/* 6 / LogDecoder_strPosFrameDecode                                                                                   */
/* 7 / LogDecoder_strDecodeFrameContent                                                                               */
/* 8 / LogDecoder_vidMainFunction                                                                                     */
/**********************************************************************************************************************/

/**********************************************************************************************************************/
/* INCLUDES                                                                                                           */
/**********************************************************************************************************************/
#include "log_decoder.h"

/**********************************************************************************************************************/
/* LOCAL DEFINES                                                                                                      */
/**********************************************************************************************************************/
#define FALSE                            0U
#define TRUE                             1U
#define STATUS_OK                        1U
#define STATUS_NOK                       0U
#define CHECKSUM_OK                      0U
#define STRING_COMPARE_OK                0U
#define PAYLOAD_BYTES_NUMBER             4U
#define SHIFT_8BITS                      8U
#define SHIFT_16BITS                     16U
#define MASK_1BYTE                       0xFFU
#define MAX_POSITIVE_SIGNED_16BITS       32767U

/**********************************************************************************************************************/
/* LOCAL FUNCTIONS PROTOTYPES                                                                                         */
/**********************************************************************************************************************/
static uint16 LogDecoder_u8PosCalcFrameDropCnt(uint16 u16FrameNB);
static uint16 LogDecoder_u8VelCalcFrameDropCnt(uint16 u16FrameNB);
static boolean LogDecoder_bPosTimeOutStatus(uint16 u16FrameTimestamp);
static boolean LogDecoder_bVelTimeOutStatus(uint16 u16FrameTimestamp);
static boolean LogDecoder_bTimeOutStatus(uint8 u8FrameId, uint16 u16FrameTimestamp);
static boolean LogDecoder_u8ChecksumStatus(uint32 u32PayloadValue, uint8 u8Checksum);
static strDecodedDataType LogDecoder_strPosFrameDecode(uint32 u32PayloadValue);
static strDecodedDataType LogDecoder_strVelFrameDecode(uint32 u32PayloadValue);
static LogDecoder_strOutputDataType LogDecoder_strDecodeFrameContent(LogDecoder_strInputDataType strInputData);

/**********************************************************************************************************************/
/* LOCAL FUNCTIONS DEFINITION                                                                                         */
/**********************************************************************************************************************/
/**********************************************************************************************************************/
/*                                                                                                                    */
/* !FuncName    : LogDecoder_u8PosCalcFrameDropCnt                                                                    */
/* !Description : Calculate the cumulative number of droped Position frames                                           */
/*                                                                                                                    */
/* !Inputs      : u16FrameNB                    !Comment : Counter of frames for a Position ID                        */
/*                                              !Range   : [0, 65535]                                                 */
/* !Outputs     : u16LocRetFrameDropCnt         !Comment : Return the cumulative number of droped frames              */
/*                                              !Range   : [0, 65535]                                                 */
/* !Number      : 1                                                                                                   */
/*                                                                                                                    */
/**********************************************************************************************************************/
static uint16 LogDecoder_u8PosCalcFrameDropCnt(uint16 u16FrameNB)
{
    static uint16  u16LocPosFrameNbNm1    = FALSE;
    static boolean bLocPosFirstReading    = TRUE;
    static uint16  u16LocPosFrameDropCnt  = FALSE;

    /* Check if it's the first frame recieved                                                     */
    if(bLocPosFirstReading == TRUE)
    {
        /* Clear the first frame recieved flag and reset the counter                              */
        u16LocPosFrameDropCnt = FALSE;
        bLocPosFirstReading = FALSE;
    }
    else
    {
        /* Set the Frame Drop Counter to value equal to the difference between the previous frame */
        /* and the current frame minus 1. because for example :                                   */
        /* -> If (current = 2 ) - (Previous = 1) - (1) = (0) Then, no droped frame                */
        /* -> If (current = 4 ) - (Previous = 1) - (1) = (2) Then, we have two dropped frames     */
        u16LocPosFrameDropCnt = u16LocPosFrameDropCnt + (u16FrameNB - u16LocPosFrameNbNm1 - 1);
    }
    /* Set prevoius fram equal to the current frame for the next iteration                        */
    u16LocPosFrameNbNm1 = u16FrameNB;

    return u16LocPosFrameDropCnt;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* !FuncName    : LogDecoder_u8VelCalcFrameDropCnt                                                                    */
/* !Description : Calculate the cumulative number of droped Velocity frames                                           */
/*                                                                                                                    */
/* !Inputs      : u16FrameNB                    !Comment : Counter of frames for a Velocity ID                        */
/*                                              !Range   : [0, 65535]                                                 */
/* !Outputs     : u16LocVelFrameDropCnt         !Comment : Return the cumulative number of droped frames              */
/*                                              !Range   : [0, 65535]                                                 */
/* !Number      : 2                                                                                                   */
/*                                                                                                                    */
/**********************************************************************************************************************/
static uint16 LogDecoder_u8VelCalcFrameDropCnt(uint16 u16FrameNB)
{
    static uint16  u16LocVelFrameNbNm1    = FALSE;
    static boolean bLocVelFirstReading    = TRUE;
    static uint16  u16LocVelFrameDropCnt  = FALSE;

    /* Check if it's the first frame recieved                                                     */
    if(bLocVelFirstReading == TRUE)
    {
        /* Clear the first frame recieved flag and reset the counter                              */
        u16LocVelFrameDropCnt = FALSE;
        bLocVelFirstReading = FALSE;
    }
    else
    {
        /* Set the Frame Drop Counter to value equal to the difference between the previous frame */
        /* and the current frame minus 1. because for example :                                   */
        /* -> If (current = 2 ) - (Previous = 1) - (1) = (0) Then, no droped frame                */
        /* -> If (current = 4 ) - (Previous = 1) - (1) = (2) Then, we have two dropped frames     */
        u16LocVelFrameDropCnt = u16LocVelFrameDropCnt + (u16FrameNB - u16LocVelFrameNbNm1 - 1);
    }
    /* Set prevoius frame equal to the current frame for the next iteration                       */
    u16LocVelFrameNbNm1 = u16FrameNB;

    return u16LocVelFrameDropCnt;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* !FuncName    : LogDecoder_bPosTimeOutStatus                                                                        */
/* !Description : Check the timeout status for the current Position frame is OK or NOK                                */
/*                                                                                                                    */
/* !Inputs      : u16FrameTimestamp             !Comment : Timestamp when the frame was received in (ms)              */
/*                                              !Range   : [0, 65535]                                                 */
/* !Outputs     : bLocTimeOutStatus             !Comment : Return the TimeoutStatus for the given timestamp           */
/*                                              !Range   : STATUS_OK,                                                 */
/*                                                         STATUS_NOK                                                 */
/* !Number      : 3                                                                                                   */
/*                                                                                                                    */
/**********************************************************************************************************************/
static boolean LogDecoder_bPosTimeOutStatus(uint16 u16FrameTimestamp)
{
    static uint16 u16LocPosTimeStampNm1 = FALSE;
    static boolean bLocPosFirstReading = TRUE;
           boolean bLocTimeOutStatus = STATUS_NOK;

    /* Check if it's the first frame recieved                                                     */
    if(bLocPosFirstReading == TRUE)
    {
        /* Clear the first frame recieved flag and reset the return OK                            */
        bLocTimeOutStatus = STATUS_OK;
        bLocPosFirstReading = FALSE;
    }
    else
    {
        /* Check if the timestamp is equal to the cycle time +/- range (25 +/- 2 ms)              */
        if(  ((u16FrameTimestamp - u16LocPosTimeStampNm1) > (POS_TIMESTAMP_PERIODICITY + POS_TIMESTAMP_MARGIN))
          || ((u16FrameTimestamp - u16LocPosTimeStampNm1) < (POS_TIMESTAMP_PERIODICITY - POS_TIMESTAMP_MARGIN)) )
        {
            bLocTimeOutStatus = STATUS_NOK;
        }
        else
        {
            bLocTimeOutStatus = STATUS_OK;
        }
    }
    /* Set prevoius frame equal to the current frame for the next iteration                       */
    u16LocPosTimeStampNm1 = u16FrameTimestamp;

    return bLocTimeOutStatus;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* !FuncName    : LogDecoder_bVelTimeOutStatus                                                                        */
/* !Description : Check the timeout status for the current Velocity frame is OK or NOK                                */
/*                                                                                                                    */
/* !Inputs      : u16FrameTimestamp             !Comment : Timestamp when the frame was received in (ms)              */
/*                                              !Range   : [0, 65535]                                                 */
/* !Outputs     : bLocTimeOutStatus             !Comment : Return the TimeoutStatus for the given timestamp           */
/*                                              !Range   : STATUS_OK,                                                 */
/*                                                         STATUS_NOK                                                 */
/* !Number      : 4                                                                                                   */
/*                                                                                                                    */
/**********************************************************************************************************************/
static boolean LogDecoder_bVelTimeOutStatus(uint16 u16FrameTimestamp)
{
    static uint16 u16LocVelTimeStampNm1 = FALSE;
    static boolean bLocVelFirstReading = TRUE;
           boolean bLocTimeOutStatus = STATUS_NOK;

    /* Check if it's the first frame recieved                                                    */
    if(bLocVelFirstReading == TRUE)
    {
        /* Clear the first frame recieved flag and reset the return OK                           */
        bLocTimeOutStatus = STATUS_OK;
        bLocVelFirstReading = FALSE;
    }
    else
    {
        /* Check if the timestamp is equal to the cycle time +/- range (50 +/- 3 ms)              */
        if(  ((u16FrameTimestamp - u16LocVelTimeStampNm1) > (VEL_TIMESTAMP_PERIODICITY + VEL_TIMESTAMP_MARGIN))
          || ((u16FrameTimestamp - u16LocVelTimeStampNm1) < (VEL_TIMESTAMP_PERIODICITY - VEL_TIMESTAMP_MARGIN)) )
        {
            bLocTimeOutStatus = STATUS_NOK;
        }
        else
        {
            bLocTimeOutStatus = STATUS_OK;
        }
    }
    /* Set prevoius frame equal to the current frame for the next iteration                       */
    u16LocVelTimeStampNm1 = u16FrameTimestamp;

    return bLocTimeOutStatus;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* !FuncName    : LogDecoder_u8ChecksumStatus                                                                         */
/* !Description : Check the checksum status for the coded data is Ok or NOK                                           */
/*                                                                                                                    */
/* !Inputs      : u32PayloadValue               !Comment : Hex coded data in big endian format                        */
/*                                              !Range   : [0x0000, 0xFFFF]                                           */
/*                u8Checksum                    !Comment : Sum complement checksum of four bytes Data (1Byte)         */
/*                                              !Range   : [0x00, 0xFF]                                               */
/* !Outputs     : bLocChecksumstatus            !Comment : Retun the status of the checksum                           */
/*                                              !Range   : STATUS_OK,                                                 */
/*                                                         STATUS_NOK                                                 */
/* !Number      : 5                                                                                                   */
/*                                                                                                                    */
/**********************************************************************************************************************/
static boolean LogDecoder_u8ChecksumStatus(uint32 u32PayloadValue, uint8 u8Checksum)
{
    boolean bLocChecksumstatus = FALSE;
    uint8 u8LocDataSum = FALSE;
    uint8 u8LocChecksumChk = FALSE;
    uint8 u8LocIndex = FALSE;

    for(u8LocIndex = 0; u8LocIndex < PAYLOAD_BYTES_NUMBER ; u8LocIndex++)
    {
        /* Add all the bytes of payload data together (Byte1 + Byte2 + ...) and take only the     */
        /* first byte of the sum                                                                  */
        u8LocDataSum = u8LocDataSum + (uint8)((u32PayloadValue >> (u8LocIndex * SHIFT_8BITS)) & (MASK_1BYTE));
    }
    /* Add the sum of payload bytes to the checksum and take the first byte only. if it's equal   */
    /* to zero then return OK otherwise return NOK                                                */
    u8LocChecksumChk = (uint8)(u8LocDataSum + u8Checksum);
    if(u8LocChecksumChk == CHECKSUM_OK)
    {
        bLocChecksumstatus = TRUE;
    }
    else
    {
        bLocChecksumstatus = FALSE;
    }

    return bLocChecksumstatus;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* !FuncName    : LogDecoder_strPosFrameDecode                                                                        */
/* !Description : Transfer the Position raw value to physical value (m)                                               */
/*                                                                                                                    */
/* !Inputs      : u32PayloadValue               !Comment : Hex coded data in big endian format                        */
/*                                              !Range   : [0x0000, 0xFFFF]                                           */
/* !Outputs     : strLocDecodedData             !Comment : Retun the status of the checksum                           */
/*                                              !Range   : f32PosX -> [0.000000, 655.349976]                          */
/*                                                         f32PosY -> [-32.766998, 32.768002]                         */
/*                                                         f32VelX -> 0.000000                                        */
/*                                                         f32VelY -> 0.000000                                        */
/* !Number      : 6                                                                                                   */
/*                                                                                                                    */
/**********************************************************************************************************************/
static strDecodedDataType LogDecoder_strPosFrameDecode(uint32 u32PayloadValue)
{
    strDecodedDataType strLocDecodedData = {FALSE};
    uint32 u32LocPosPhyValue = FALSE;
    /*Set the Velocity to zero                                                                    */
    strLocDecodedData.f32VelX = FALSE;
    strLocDecodedData.f32VelY = FALSE;

    /*Take the first 2Bytes and divide it by 100 to have PositionX  in (m)                        */
    u32LocPosPhyValue = (u32PayloadValue >> SHIFT_16BITS);
    strLocDecodedData.f32PosX = (((float32)u32LocPosPhyValue)/100U);

    u32LocPosPhyValue = FALSE;
    /*Take the Last 2Bytes and divide it by 1000 to have PositionY  in (m)                        */
    u32LocPosPhyValue = (u32PayloadValue & 0x0000FFFFU);
    strLocDecodedData.f32PosY = (((float32)u32LocPosPhyValue - MAX_POSITIVE_SIGNED_16BITS)/1000U);

    return strLocDecodedData;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* !FuncName    : LogDecoder_strVelFrameDecode                                                                        */
/* !Description : Transfer the Velocity raw value to physical value (m/s)                                             */
/*                                                                                                                    */
/* !Inputs      : u32PayloadValue               !Comment : Hex coded data in big endian format                        */
/*                                              !Range   : [0x0000, 0xFFFF]                                           */
/* !Outputs     : strLocDecodedData             !Comment : Retun the status of the checksum                           */
/*                                              !Range   : f32PosX -> 0.000000                                        */
/*                                                         f32PosY -> 0.000000                                        */
/*                                                         f32VelX -> [-32.766998, 32.768002]                         */
/*                                                         f32VelY -> [-32.766998, 32.768002]                         */
/* !Number      : 7                                                                                                   */
/*                                                                                                                    */
/**********************************************************************************************************************/
static strDecodedDataType LogDecoder_strVelFrameDecode(uint32 u32PayloadValue)
{
    strDecodedDataType strLocDecodedData = {FALSE};
    uint32 u32LocVelPhyValue = FALSE;

    /*Set the Position to zero                                                                    */
    strLocDecodedData.f32PosX = FALSE;
    strLocDecodedData.f32PosY = FALSE;

    /*Take the first 2Bytes and divide it by 1000 to have VelocityX  in (m/s)                     */
    u32LocVelPhyValue = (u32PayloadValue >> SHIFT_16BITS);
    strLocDecodedData.f32VelX = (((float32)u32LocVelPhyValue - MAX_POSITIVE_SIGNED_16BITS)/1000U);

    u32LocVelPhyValue = FALSE;
    /*Take the Last 2Bytes and divide it by 1000 to have VelocityY  in (m)                        */
    u32LocVelPhyValue = (u32PayloadValue & 0x0000FFFFU);
    strLocDecodedData.f32VelY = (((float32)u32LocVelPhyValue - MAX_POSITIVE_SIGNED_16BITS)/1000U);

    return strLocDecodedData;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* !FuncName    : LogDecoder_strDecodeFrameContent                                                                    */
/* !Description : Decode the input frame content to the required output                                               */
/*                                                                                                                    */
/* !Inputs      : strInputData                  !Comment : Input frame content                                        */
/*                                              !Range   : u32Payload,                                                */
/*                                                         u16FrameNb,                                                */
/*                                                         u16Timestamp,                                              */
/*                                                         u8Id,                                                      */
/*                                                         u8Checksum                                                 */
/* !Outputs     : strLocDecodedData             !Comment : Retun the status of the checksum                           */
/*                                              !Range   : strDecodedData,                                            */
/*                                                         u16FrameDropCnt,                                           */
/*                                                         u16FrameNb,                                                */
/*                                                         u16Timestamp,                                              */
/*                                                         bChecksumOK,                                               */
/*                                                         bTimeoutOK,                                                */
/*                                                         u8Id                                                       */
/* !Number      : 8                                                                                                   */
/*                                                                                                                    */
/**********************************************************************************************************************/
static LogDecoder_strOutputDataType LogDecoder_strDecodeFrameContent(LogDecoder_strInputDataType strInputData)
{
    LogDecoder_strOutputDataType strLocOutputData = {0};
    uint16 u16_localPositionX =FALSE;
    uint16 u16_localPositionY =FALSE;

    /*Copy ID,FrameNb and Timestamp  to the output*/
    strLocOutputData.u8Id = strInputData.u8Id;
    strLocOutputData.u16FrameNb = strInputData.u16FrameNb;
    strLocOutputData.u16Timestamp = strInputData.u16Timestamp;

    switch(strInputData.u8Id)
    {
        case FRAME_ID_POSITION:
            /* Call the Position internal functions */
            strLocOutputData.u16FrameDropCnt = LogDecoder_u8PosCalcFrameDropCnt(strInputData.u16FrameNb);
            strLocOutputData.bTimeoutOK      = LogDecoder_bPosTimeOutStatus(strInputData.u16Timestamp);
            strLocOutputData.bChecksumOK     = LogDecoder_u8ChecksumStatus(strInputData.u32Payload, strInputData.u8Checksum);
            strLocOutputData.strDecodedData  = LogDecoder_strPosFrameDecode(strInputData.u32Payload);
            break;

        case FRAME_ID_VELOCITY:
            /* Call the Velocity internal functions */
            strLocOutputData.u16FrameDropCnt = LogDecoder_u8VelCalcFrameDropCnt(strInputData.u16FrameNb);
            strLocOutputData.bTimeoutOK      = LogDecoder_bVelTimeOutStatus(strInputData.u16Timestamp);
            strLocOutputData.bChecksumOK     = LogDecoder_u8ChecksumStatus(strInputData.u32Payload, strInputData.u8Checksum);
            strLocOutputData.strDecodedData  = LogDecoder_strVelFrameDecode(strInputData.u32Payload);
            break;

        default:
            /* If the frame ID not equal to Position or Velocity, print Warning message*/
            printf("Invalid Frame ID");
            break;
    }

    return strLocOutputData;
}

/**********************************************************************************************************************/
/* GLOBAL FUNCTIONS                                                                                                   */
/**********************************************************************************************************************/
/**********************************************************************************************************************/
/*                                                                                                                    */
/* !FuncName    : LogDecoder_vidMainFunction                                                                          */
/* !Description : Read Inputs from .csv file and call internal functions and write the .csv output file               */
/*                                                                                                                    */
/* !Inputs      : s32NumOfArg                   !Comment : Number of main arguments                                   */
/*                                              !Range   :                                                            */
/*                ptrMainArgs                   !Comment : main function given arguments                              */
/*                                              !Range   :                                                            */
/* !Number      : 7                                                                                                   */
/*                                                                                                                    */
/**********************************************************************************************************************/
void LogDecoder_vidMainFunction(int s32NumOfArg, char **ptrMainArgs)
{
    /* Open the Input .csv file with read access                                                  */
    FILE   *LocInputFile = fopen(ptrMainArgs[INPUT_ARGUMENT_NUMBER],"r");
    /* Open the Input .csv file with write access                                                 */
    FILE   *LocOutputFile = fopen(ptrMainArgs[OUTPUT_ARGUMENT_NUMBER],"w");

    uint32 u32Id = FALSE;
    uint32 u32FrameNb = FALSE;
    uint32 u32Timestamp = FALSE;
    uint32 u32Payload = FALSE;
    uint32 u32Checksum = FALSE;

    uint8 sLocFirstRow[MAX_CHAR_NUM_PER_LINE] = {FALSE};

    LogDecoder_strInputDataType strLocInputData = {FALSE};
    LogDecoder_strOutputDataType strLocOutputData = {FALSE};

    uint8 u8LocElementsNumPerRow = FALSE;
    uint16 u16RowNumber = FALSE;

    /* Check if the number of arguments is equal to the expected number                           */
    if (s32NumOfArg != ARGUMENTS_NUMBER)
    {
        printf("Help Info:\n"
            "\t- The first command shall be .exe file (for example: log_decoder.exe)\n"
            "\t- The second command shall be .csv input file (for example: input_log.csv)\n"
            "\t- The third command shall be .csv input file (for example: output_log.csv)\n"
            "\t- All comands shall be delimited by whitespace (for example: log_decoder.exe input_log.csv output_log.csv)");
    }
    else
    {
        /* Scan and check the first row format is the same expected format                        */
        fscanf(LocInputFile,"%s",sLocFirstRow);
        if(strcmp(sLocFirstRow, HEADER_FOR_INPUT_FILE) == STRING_COMPARE_OK)
        {
            fprintf(LocOutputFile, HEADER_FOR_OUTPUT_FILE);

            while (!feof(LocInputFile))
            {
                u8LocElementsNumPerRow = fscanf(LocInputFile, "%u,%u,%u,%x,%x\n", &u32Id,
                                                                                  &u32FrameNb,
                                                                                  &u32Timestamp,
                                                                                  &u32Payload,
                                                                                  &u32Checksum);

                strLocInputData.u8Id         = (uint8)u32Id;
                strLocInputData.u16FrameNb   = (uint16)u32FrameNb;
                strLocInputData.u16Timestamp = (uint16)u32Timestamp;
                strLocInputData.u32Payload   = u32Payload;
                strLocInputData.u8Checksum   = (uint8)u32Checksum;

                /* Check if the number of elements per raw is equal to the expected number        */
                if (u8LocElementsNumPerRow == ELEMENTS_NUM_PER_ROW)
                {
                    strLocOutputData = LogDecoder_strDecodeFrameContent(strLocInputData);
                    fprintf(LocOutputFile,"%d, %d, %d, %.2f, %.3f, %.3f, %.3f, %d, %d, %d\n", strLocOutputData.u8Id, 
                                                                                              strLocOutputData.u16FrameNb,
                                                                                              strLocOutputData.u16Timestamp,
                                                                                              strLocOutputData.strDecodedData.f32PosX,
                                                                                              strLocOutputData.strDecodedData.f32PosY,
                                                                                              strLocOutputData.strDecodedData.f32VelX,
                                                                                              strLocOutputData.strDecodedData.f32VelY,
                                                                                              strLocOutputData.bChecksumOK,
                                                                                              strLocOutputData.bTimeoutOK,
                                                                                              strLocOutputData.u16FrameDropCnt);
                    u16RowNumber++;
                }
                else
                {
                    printf("Missing data in row number %d", (u16RowNumber + 2));
                    break;
                }
            }
        }
        else
        {
            printf("First row must be in the following format :\n"
                "ID,FrameNb,Timestamp,Payload,Checksum");
        }

        /* Close input and output Files */
        fclose(LocInputFile);
        fclose(LocOutputFile);
    }
}

/**********************************************************************************************************************/
/* APPLICATION MAIN FUNCTION                                                                                          */
/**********************************************************************************************************************/
int main(int argc, char **argv)
{
    LogDecoder_vidMainFunction(argc, argv);
    return 0;
}

/*---------------------------------------------------- end of file ---------------------------------------------------*/