/********************************************************************************
**                                                                            **
**  FILENAME  : log_decoder.c                                                 **
**                                                                            **
**  DATE      : 2022-05-020                                                   **
**                                                                            **
**  AUTHOR    : Mohamed Tarek Abozaid                                         **
********************************************************************************/
/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include "log_decoder.h"
/**********************************************************************************************************************
 * Type Declarations
 *********************************************************************************************************************/
str_outputFrameData outputFrameData[FRAME_COUNT];
static uint16 vel_prevTimestamp = FALSE;
static uint16 pos_prevTimestamp = FALSE;

static uint16 vel_prevFrameNB = FALSE;
static uint16 pos_prevFrameNB = FALSE;

static uint16 framDrpCnt = FALSE;
/**********************************************************************************************************************
 * Function Declarations
 *********************************************************************************************************************/
static inline boolean posChecTimeOK(uint16 currTimestamp);
static inline boolean velChecTimeOK(uint16 currTimestamp);
static inline boolean u8Checksum8BitsValid (uint32 u32data, uint16 u8Checksum);
static inline void posFrameDecode(str_inputFrameData *recievedFrame, uint16 frameIdx);
static inline void velFrameDecode(str_inputFrameData *recievedFrame, uint16 frameIdx);
static inline void decodeContent(int argc, char *argv[]);
/**********************************************************************************************************************
 * Function Definitions
 *********************************************************************************************************************/
/**
 * @brief Check if the position frame Timestamp is Ok.
 *
 * <b>Reentrant</b>: YES \n
 * <b>Pre-emptible</b>: YES
 *
 * @pre
 * @param[in] uint16
 * @return boolean
 * @post
 */
static inline boolean posChecTimeOK(uint16 currTimestamp)
{
    if (currTimestamp == FALSE)
    {
        return TRUE;
    }
    else
    {
        currTimestamp -= pos_prevTimestamp;
        if ((currTimestamp > POS_TIMESTAMP_NOK_POS) || (currTimestamp < POS_TIMESTAMP_NOK_NEG))
        {
            return FALSE;
        }
        else
        {
            return TRUE;
        }  
    }
}
/**
 * @brief Check if the velocity frame Timestamp is Ok.
 *
 * <b>Reentrant</b>: YES \n
 * <b>Pre-emptible</b>: YES
 *
 * @pre
 * @param[in] uint16
 * @return boolean
 * @post
 */
static inline boolean velChecTimeOK(uint16 currTimestamp)
{
    if (currTimestamp == FALSE)
    {
        return TRUE;
    }
    else
    {
        currTimestamp -= vel_prevTimestamp;
        if ((currTimestamp > VEL_TIMESTAMP_NOK_POS) || (currTimestamp < VEL_TIMESTAMP_NOK_NEG))
        {
            return FALSE;
        }
        else
        {
            return TRUE;
        }  
    }
}
/**
 * @brief 8-bit Checksum on payload data.
 *
 * <b>Reentrant</b>: YES \n
 * <b>Pre-emptible</b>: YES
 *
 * @pre
 * @param[in] uint32, uint16
 * @return boolean
 * @post
 */
static inline boolean u8Checksum8BitsValid (uint32 u32_payloadData, uint16 u16_Checksum)
{
    uint8 u8Valid = FALSE;
    uint8 u8Status = FALSE;
    uint8 u8_idx = FALSE;
    for(; u8_idx<32U ; u8_idx+=8U)
    {
        u8Valid += (uint8)((u32_payloadData >> u8_idx) & (0x000000FF));
    }
    u8Valid = u8Valid + u16_Checksum;
    return (boolean)u8Valid;
}
/**
 * @brief Position frame function to decode the input Frame data.
 *
 * <b>Reentrant</b>: YES \n
 * <b>Pre-emptible</b>: YES
 *
 * @pre
 * @param[in] str_inputFrameData *, uint16
 * @return boolean
 * @post
 */
static inline void posFrameDecode(str_inputFrameData *recievedFrame, uint16 frameIdx)
{

    uint16 u16_localPositionX =FALSE;
    uint16 u16_localPositionY =FALSE;

    /*Copy ID, FrameNB & Timestap from the recieved position frame data*/
    outputFrameData[frameIdx].u8_ID = recievedFrame->u8_ID;
    outputFrameData[frameIdx].u16_frameNB = recievedFrame->u16_frameNB;
    outputFrameData[frameIdx].u16_Timestamp = recievedFrame->u16_Timestamp;
    
    /*Derive the X-position value from the hex payload */
    u16_localPositionX = ((recievedFrame->u32_payload >> 16U));
    outputFrameData[frameIdx].f32_positionX = ((float32)u16_localPositionX/100U);
    /*Derive the Y-position value from the hex payload */
    u16_localPositionY = ((recievedFrame->u32_payload & 0xFFFFU));
    outputFrameData[frameIdx].f32_positionY = (((float32)u16_localPositionY - SINT16_MAX)/1000U);

    /*Leave VelocityX & VelocityY empty for position frame data*/
    outputFrameData[frameIdx].f32_velocityX = FALSE;
    outputFrameData[frameIdx].f32_velocityY = FALSE;
    
    /*Calculate Checksum*/
    outputFrameData[frameIdx].bool_checksumOK = u8Checksum8BitsValid(recievedFrame->u32_payload,recievedFrame->u8_checksum);
    
    /*Check TimeOK status 25ms [+/- 2ms]*/
    outputFrameData[frameIdx].bool_timeoutOK = posChecTimeOK(recievedFrame->u16_Timestamp);
    pos_prevTimestamp = recievedFrame->u16_Timestamp;

    /*Check how many frame dropped */
    FRAME_DROP_CNT((recievedFrame->u16_frameNB),pos_prevFrameNB);
    pos_prevFrameNB = recievedFrame->u16_frameNB;
    outputFrameData[frameIdx].u16_frameDropCnt = framDrpCnt;

    //printf("%d-->%d\n",recievedFrame->frameNB,framDrpCnt);
    //printf("%x-->x=%d   y=%d\n",recievedFrame->payload,outputFrameData[frameIdx].positionX,outputFrameData[frameIdx].positionY); 
}
/**
 * @brief Position frame function to decode the input Frame data.
 *
 * <b>Reentrant</b>: YES \n
 * <b>Pre-emptible</b>: YES
 *
 * @pre
 * @param[in] str_inputFrameData *, uint16
 * @return boolean
 * @post
 */
static inline void velFrameDecode(str_inputFrameData *recievedFrame, uint16 frameIdx)
{
    uint16 u16_localVelocityX =FALSE;
    uint16 u16_localVelocityY =FALSE;

    /*Copy ID, FrameNB & Timestap from the recieved position frame data*/
    outputFrameData[frameIdx].u8_ID = recievedFrame->u8_ID;
    outputFrameData[frameIdx].u16_frameNB = recievedFrame->u16_frameNB;
    outputFrameData[frameIdx].u16_Timestamp = recievedFrame->u16_Timestamp;
    
    /*Leave PositionX & PositionY empty for position frame data*/
    outputFrameData[frameIdx].f32_positionX = FALSE;
    outputFrameData[frameIdx].f32_positionY = FALSE;

    /*Derive the X-Velocity value from the hex payload */
    u16_localVelocityX = ((recievedFrame->u32_payload >> 16U));
    outputFrameData[frameIdx].f32_velocityX = (((float32)u16_localVelocityX - SINT16_MAX)/1000U);
    /*Derive the Y-Velocity value from the hex payload */
    u16_localVelocityY = ((recievedFrame->u32_payload & 0xFFFFU));
    outputFrameData[frameIdx].f32_velocityY = (((float32)u16_localVelocityY - SINT16_MAX)/1000U);

    /*Calculate Checksum*/
    outputFrameData[frameIdx].bool_checksumOK = u8Checksum8BitsValid(recievedFrame->u32_payload,recievedFrame->u8_checksum);
    
    /*Check TimeOK status 50ms [+/- 2ms]*/
    outputFrameData[frameIdx].bool_timeoutOK = velChecTimeOK(recievedFrame->u16_Timestamp);
    vel_prevTimestamp = recievedFrame->u16_Timestamp;

    /*Check how many frame dropped */
    FRAME_DROP_CNT((recievedFrame->u16_frameNB),vel_prevFrameNB);
    vel_prevFrameNB = recievedFrame->u16_frameNB;
    outputFrameData[frameIdx].u16_frameDropCnt = framDrpCnt;

    /*DEBUG */
    //printf("%d-->%d\n",recievedFrame->frameNB,framDrpCnt);
    //printf("%x-->x=%d   y=%d\n",recievedFrame->payload,outputFrameData[frameIdx].positionX,outputFrameData[frameIdx].positionY); 
}
/**
 * @brief Decode function to decide upon the fram ID which decoding logic will be executed.
 *
 * <b>Reentrant</b>: YES \n
 * <b>Pre-emptible</b>: YES
 *
 * @pre
 * @param[in] int *, char *
 * @return void
 * @post
 */
static inline void decodeContent(int argc, char *argv[])
{
    FILE *inputFile = fopen(argv[1],"r");
    FILE *outputFile = fopen(argv[2],"w");
    str_inputFrameData str_inputFrameData[FRAME_COUNT];
    uint16 read = FALSE;
    uint16 records = FALSE;
    /*Save csv file's data to framdata*/
    fscanf(inputFile,"ID,FrameNb,Timestamp,Payload,Checksum");
    do
    {
        read = fscanf(inputFile,
                        "%d,%d,%d,%x,%x\n",
                        &str_inputFrameData[records].u8_ID,
                        &str_inputFrameData[records].u16_frameNB,
                        &str_inputFrameData[records].u16_Timestamp,
                        &str_inputFrameData[records].u32_payload,
                        &str_inputFrameData[records].u8_checksum);
        if (read == INPUT_FRAME_DATA) records++;

    } while (!feof(inputFile));
    fclose(inputFile);

    for (uint16 u8_counter = FALSE; u8_counter < records; u8_counter++)
    {
        switch (str_inputFrameData[u8_counter].u8_ID)
        {
        case POSITON_FRAME_ID:
            posFrameDecode(&str_inputFrameData[u8_counter],u8_counter);
            break;
        case VELOCITY_FRAME_ID:
            velFrameDecode(&str_inputFrameData[u8_counter],u8_counter);
            break;
        default:
            /* Do Nothing */
            break;
        }   
    }
    fprintf(outputFile,"ID, FrameNB, Timestamp, X-Position, Y-Position, X-Velocity, Y-Velocity, ChecksumOK, TimestampOk, FrameDropCnt\n");
    for (uint16 i = 0; i < records; i++)
    fprintf(outputFile,"%d, %d, %d, %.2f, %.3f, %.3f, %.3f, %d, %d, %d\n",
                        outputFrameData[i].u8_ID, 
                        outputFrameData[i].u16_frameNB,
                        outputFrameData[i].u16_Timestamp,
                        outputFrameData[i].f32_positionX,
                        outputFrameData[i].f32_positionY,
                        outputFrameData[i].f32_velocityX,
                        outputFrameData[i].f32_velocityY,
                        outputFrameData[i].bool_checksumOK,
                        outputFrameData[i].bool_timeoutOK,
                        outputFrameData[i].u16_frameDropCnt);
    fclose(outputFile);
}
/**
 * @brief lod_decoder main function.
 *
 * <b>Reentrant</b>: YES \n
 * <b>Pre-emptible</b>: YES
 *
 * @pre
 * @param[in] int, char **
 * @return int
 * @post
 */
int main(int argc, char **argv)
{
    
    if (argc != VALID_USER_ARGUMENTS)
    {
        printf("Help:\n\t+Please write down ''log_decoder.exe''\n\t+Log decoder shall input data as a .csv file as a first argument'E.g. input_log.csv'\n\t+Log decoder shall output data as a .csv file with the name provided as a second argument'E.g. output_log.csv'\n");
        return 1;
    }
    else
    {
        decodeContent(argc, argv);
    }
    return 0;
}