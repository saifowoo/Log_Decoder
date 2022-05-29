/**********************************************************************************************************************/
/*                                                                                                                    */
/*  Application : Log Decoder                                                                                         */
/*  Description : Log decoder is a simple console application, that takes a .csv format logfile as an input           */
/*                and provides an output log file also in .csv format, with Payload decoded into meaningful           */
/*                values and additional flags if certains checks are violated for a given frame.                      */
/*                                                                                                                    */
/*  File        : LogDecoder_Types.h                                                                                  */
/*                                                                                                                    */
/*  Author      : Saif El-Deen M.                                                                                     */
/*                                                                                                                    */
/*  Date        : 29/05/2022                                                                                          */
/*                                                                                                                    */
/**********************************************************************************************************************/

#ifndef LOG_DECODER_TYPES_H
#define LOG_DECODER_TYPES_H

/**********************************************************************************************************************/
/* INCLUDES                                                                                                           */
/**********************************************************************************************************************/
#include <stdio.h>
#include <string.h>

/**********************************************************************************************************************/
/* TYPEDEF                                                                                                            */
/**********************************************************************************************************************/
typedef unsigned char       boolean; 
typedef unsigned char       uint8;
typedef signed char         sint8;
typedef unsigned short      uint16;
typedef signed short        sint16;
typedef unsigned long       uint32;
typedef signed long         sint32;
typedef float               float32;

#endif /* LOG_DECODER_TYPES_H */
/*---------------------------------------------------- end of file ---------------------------------------------------*/
