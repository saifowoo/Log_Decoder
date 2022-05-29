/**********************************************************************************************************************/
/*                                                                                                                    */
/*  Application : Log Decoder                                                                                         */
/*  Description : Log decoder is a simple console application, that takes a .csv format logfile as an input           */
/*                and provides an output log file also in .csv format, with Payload decoded into meaningful           */
/*                values and additional flags if certains checks are violated for a given frame.                      */
/*                                                                                                                    */
/*  File        : LogDecoder.c                                                                                        */
/*                                                                                                                    */
/*  Author      : Saif El-Deen M.                                                                                     */
/*                                                                                                                    */
/*  Date        : 29/05/2022                                                                                          */
/*                                                                                                                    */
/**********************************************************************************************************************/
/* 1 / LogDecoder_u8CalcFrameDropCnt                                                                                  */
/* 2 / LogDecoder_bTimeOutStatus                                                                                      */
/* 3 / LogDecoder_u8ChecksumStatus                                                                                    */
/* 4 /                                                                                                                */
/**********************************************************************************************************************/

/**********************************************************************************************************************/
/* INCLUDES                                                                                                           */
/**********************************************************************************************************************/
#include "log_decoder.h"

/**********************************************************************************************************************/
/* DEFINES                                                                                                            */
/**********************************************************************************************************************/
#define STATUS_OK                       1U
#define STATUS_NOK                      0U
#define CHECKSUM_OK                     0U
#define STRING_COMPARE_OK               0U
#define PAYLOAD_BYTES_NUMBER            4U
#define SHIFT_8BITS                     8U
#define SHIFT_16BITS                    16U
#define MASK_1BYTE                      0xFFU
#define MAX_POSITIVE_SIGNED_16BITS      32767U
#define ELEMENTS_NUM_PER_ROW            5U
#define ARGUMENTS_NUMBER                3U

/**********************************************************************************************************************/
/* GLOBAL VARIABLES                                                                                                   */
/**********************************************************************************************************************/

/**********************************************************************************************************************/
/* LOCAL VARIABLES                                                                                                    */
/**********************************************************************************************************************/

/**********************************************************************************************************************/
/* LOCAL FUNCTIONS PROTOTYPES                                                                                         */
/**********************************************************************************************************************/

/**********************************************************************************************************************/
/* LOCAL FUNCTIONS DEFINITION                                                                                         */
/**********************************************************************************************************************/

/**********************************************************************************************************************/
/* GLOBAL FUNCTIONS                                                                                                   */
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
uint16 LogDecoder_u8PosCalcFrameDropCnt(uint16 u16FrameNB)
{
    static uint16  u16LocPosFrameNbNm1    = FALSE;
    static boolean bLocPosFirstReading    = TRUE;
    static uint16  u16LocPosFrameDropCnt  = FALSE;

    if(bLocPosFirstReading == TRUE)
    {
        u16LocPosFrameDropCnt = FALSE;
        bLocPosFirstReading = FALSE;
    }
    else
    {
        u16LocPosFrameDropCnt = u16LocPosFrameDropCnt + (u16FrameNB - u16LocPosFrameNbNm1 - 1);
    }
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
uint16 LogDecoder_u8VelCalcFrameDropCnt(uint16 u16FrameNB)
{
    static uint16  u16LocVelFrameNbNm1    = FALSE;
    static boolean bLocVelFirstReading    = TRUE;
    static uint16  u16LocVelFrameDropCnt  = FALSE;

    if(bLocVelFirstReading == TRUE)
    {
        u16LocVelFrameDropCnt = FALSE;
        bLocVelFirstReading = FALSE;
    }
    else
    {
        u16LocVelFrameDropCnt = u16LocVelFrameDropCnt + (u16FrameNB - u16LocVelFrameNbNm1 - 1);
    }
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
boolean LogDecoder_bPosTimeOutStatus(uint16 u16FrameTimestamp)
{
    static uint16 u16LocPosTimeStampNm1 = FALSE;
    static boolean bLocPosFirstReading = TRUE;
           boolean bLocTimeOutStatus = STATUS_NOK;

    if(bLocPosFirstReading == TRUE)
    {
        bLocTimeOutStatus = STATUS_OK;
        bLocPosFirstReading = FALSE;
    }
    else
    {
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
boolean LogDecoder_bVelTimeOutStatus(uint16 u16FrameTimestamp)
{
    static uint16 u16LocVelTimeStampNm1 = FALSE;
    static boolean bLocVelFirstReading = TRUE;
           boolean bLocTimeOutStatus = STATUS_NOK;

    if(bLocVelFirstReading == TRUE)
    {
        bLocTimeOutStatus = STATUS_OK;
        bLocVelFirstReading = FALSE;
    }
    else
    {
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
boolean LogDecoder_u8ChecksumStatus(uint32 u32PayloadValue, uint8 u8Checksum)
{
    boolean bLocChecksumstatus = FALSE;
    uint8 u8LocDataSum = FALSE;
    uint8 u8LocChecksumChk = FALSE;
    uint8 u8LocIndex = FALSE;

    for(u8LocIndex = 0; u8LocIndex < PAYLOAD_BYTES_NUMBER ; u8LocIndex++)
    {
        u8LocDataSum = u8LocDataSum + (uint8)((u32PayloadValue >> (u8LocIndex * SHIFT_8BITS)) & (MASK_1BYTE));
    }

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
/* !Description : Transfer the Position raw value to physical value (m/s)                                             */
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
strDecodedDataType LogDecoder_strPosFrameDecode(uint32 u32PayloadValue)
{
    strDecodedDataType strLocDecodedData = {FALSE};
    uint32 u32LocPosPhyValue = FALSE;

    strLocDecodedData.f32VelX = FALSE;
    strLocDecodedData.f32VelY = FALSE;

    u32LocPosPhyValue = (u32PayloadValue >> SHIFT_16BITS);
    strLocDecodedData.f32PosX = (((float32)u32LocPosPhyValue)/100U);

    u32LocPosPhyValue = FALSE;
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
strDecodedDataType LogDecoder_strVelFrameDecode(uint32 u32PayloadValue)
{
    strDecodedDataType strLocDecodedData = {FALSE};
    uint32 u32LocVelPhyValue = FALSE;

    strLocDecodedData.f32PosX = FALSE;
    strLocDecodedData.f32PosY = FALSE;

    u32LocVelPhyValue = (u32PayloadValue >> SHIFT_16BITS);
    strLocDecodedData.f32VelX = (((float32)u32LocVelPhyValue - MAX_POSITIVE_SIGNED_16BITS)/1000U);

    u32LocVelPhyValue = FALSE;
    u32LocVelPhyValue = (u32PayloadValue & 0x0000FFFFU);
    strLocDecodedData.f32VelY = (((float32)u32LocVelPhyValue - MAX_POSITIVE_SIGNED_16BITS)/1000U);

    return strLocDecodedData;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* !FuncName    : COMP_vidInit                                                                                        */
/* !Description :                                                                                                     */
/* !Number      : 1                                                                                                   */
/* !Reference   :                                                                                                     */
/*                                                                                                                    */
/* !Trace_To    :                                                                                                     */
/*                                                                                                                    */
/**********************************************************************************************************************/
/* !LastAuthor  :  Developer                                                                                          */
/**********************************************************************************************************************/
LogDecoder_strOutputDataType LogDecoder_vOutputGeneration(LogDecoder_strInputDataType strInputData)
{
    LogDecoder_strOutputDataType strLocOutputData = {0};
    uint16 u16_localPositionX =FALSE;
    uint16 u16_localPositionY =FALSE;

    strLocOutputData.u8Id = strInputData.u8Id;
    strLocOutputData.u16FrameNb = strInputData.u16FrameNb;
    strLocOutputData.u16Timestamp = strInputData.u16Timestamp;

    switch(strInputData.u8Id)
    {
        case FRAME_ID_POSITION:
            strLocOutputData.u16FrameDropCnt = LogDecoder_u8PosCalcFrameDropCnt(strInputData.u16FrameNb);
            strLocOutputData.bTimeoutOK      = LogDecoder_bPosTimeOutStatus(strInputData.u16Timestamp);
            strLocOutputData.bChecksumOK     = LogDecoder_u8ChecksumStatus(strInputData.u32Payload, strInputData.u8Checksum);
            strLocOutputData.strDecodedData  = LogDecoder_strPosFrameDecode(strInputData.u32Payload);
            break;

        case FRAME_ID_VELOCITY:
            strLocOutputData.u16FrameDropCnt = LogDecoder_u8VelCalcFrameDropCnt(strInputData.u16FrameNb);
            strLocOutputData.bTimeoutOK      = LogDecoder_bVelTimeOutStatus(strInputData.u16Timestamp);
            strLocOutputData.bChecksumOK     = LogDecoder_u8ChecksumStatus(strInputData.u32Payload, strInputData.u8Checksum);
            strLocOutputData.strDecodedData  = LogDecoder_strVelFrameDecode(strInputData.u32Payload);
            break;

        default:
            printf("Invalid Frame ID");
            break;
    }

    return strLocOutputData;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* !FuncName    : COMP_vidInit                                                                                        */
/* !Description :                                                                                                     */
/* !Number      : 1                                                                                                   */
/* !Reference   :                                                                                                     */
/*                                                                                                                    */
/* !Trace_To    :                                                                                                     */
/*                                                                                                                    */
/**********************************************************************************************************************/
/* !LastAuthor  :  Developer                                                                                          */
/**********************************************************************************************************************/
void LogDecoder_vidMainFunction(int argc, char *argv[])
{
    FILE   *LocInputFile = fopen(argv[1],"r");
    FILE   *LocOutputFile = fopen(argv[2],"w");

    string sLocFirstRow[100] = {FALSE};

    LogDecoder_strInputDataType strLocInputData = {0};
    LogDecoder_strOutputDataType strLocOutputData = {0};

    uint8 u8LocElementsNumPerRow = FALSE;
    uint16 u16RowNumber = FALSE;

    fscanf(LocInputFile,"%s",sLocFirstRow);
    if(strcmp(sLocFirstRow, "ID,FrameNb,Timestamp,Payload,Checksum") == STRING_COMPARE_OK)
    {
        fprintf(LocOutputFile,"ID, FrameNB, Timestamp, X-Position, Y-Position, X-Velocity, Y-Velocity, ChecksumOK, TimestampOk, FrameDropCnt\n");

        while (!feof(LocInputFile))
        {
            u8LocElementsNumPerRow = fscanf(LocInputFile, "%d,%d,%d,%x,%x\n", &strLocInputData.u8Id,
                                                                              &strLocInputData.u16FrameNb,
                                                                              &strLocInputData.u16Timestamp,
                                                                              &strLocInputData.u32Payload,
                                                                              &strLocInputData.u8Checksum);
            if (u8LocElementsNumPerRow == ELEMENTS_NUM_PER_ROW)
            {
                strLocOutputData = LogDecoder_vOutputGeneration(strLocInputData);
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
                printf("Missind data in row number %d", (u16RowNumber + 2));
                break;
            }
        }
    }
    else
    {
        printf("First row must be in the following format :\nID,FrameNb,Timestamp,Payload,Checksum");
    }
    
    fclose(LocInputFile);
    fclose(LocOutputFile);
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* !FuncName    : main                                                                                                */
/* !Description : Application main function                                                                           */
/* !Number      : 1                                                                                                   */
/*                                                                                                                    */
/**********************************************************************************************************************/
int main(int argc, char **argv)
{
    
    if (argc != ARGUMENTS_NUMBER)
    {
        printf("Help:\n\t+Please write down ''log_decoder.exe''\n\t+Log decoder shall input data as a .csv file as a first argument'E.g. input_log.csv'\n\t+Log decoder shall output data as a .csv file with the name provided as a second argument'E.g. output_log.csv'\n");
        return 1;
    }
    else
    {
        LogDecoder_vidMainFunction(argc, argv);
    }
    return 0;
}

/*---------------------------------------------------- end of file ---------------------------------------------------*/