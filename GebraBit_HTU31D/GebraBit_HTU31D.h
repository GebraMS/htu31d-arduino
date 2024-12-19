/*
 * ________________________________________________________________________________________________________
 * Copyright (c) 2020 GebraBit Inc. All rights reserved.
 *
 * This software, related documentation and any modifications thereto (collectively �Software�) is subject
 * to GebraBit and its licensors' intellectual property rights under U.S. and international copyright
 * and other intellectual property rights laws. 
 *
 * GebraBit and its licensors retain all intellectual property and proprietary rights in and to the Software
 * and any use, reproduction, disclosure or distribution of the Software without an express license agreement
 * from GebraBit is strictly prohibited.
 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT 
 * NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT IN  
 * NO EVENT SHALL GebraBit BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, 
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THE SOFTWARE.
 * @Author       	: Sepehr Azimi
 * ________________________________________________________________________________________________________
 */
#ifndef	__HTU31D_H__
#define	__HTU31D_H__
#include "Wire.h"
#include "arduino.h"
#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "math.h"
/************************************************
 *              USER REGISTER MAP               *
 ***********************************************/ 	
#define HTU31D_ADDRESS 									  0x40
#define HTU31D_CONVERSION                (0x40)
#define HTU31D_READ_TEMPERATURE_HUMIDITY (0x00)
#define HTU31D_READ_HUMIDITY 						 (0x10)
#define HTU31D_RESET 										 (0x1E)
#define HTU31D_HEATER_ON 								 (0x04)
#define HTU31D_HEATER_OFF 							 (0x02)
#define HTU31D_READ_SERIAL_NUMBER 		   (0x0A)
#define HTU31D_READ_DIAGNOSTIC 		     	 (0x08)
// Processing constants
#define HTU31D_TEMPERATURE_COEFFICIENT	 (float)(-0.15)
#define HTU31D_CONSTANT_A								 (float)(8.1332)
#define HTU31D_CONSTANT_B								 (float)(1762.39)
#define HTU31D_CONSTANT_C								 (float)(235.66)
// Coefficients for temperature computation
#define TEMPERATURE_COEFF_MUL						 (175.72)
#define TEMPERATURE_COEFF_ADD						 (-46.85)
// Coefficients for relative humidity computation
#define HUMIDITY_COEFF_MUL							 (125)
#define HUMIDITY_COEFF_ADD							 (-6)
/*----------------------------------------------*
 *           USER REGISTER MAP End              *
 *----------------------------------------------*/ 
 /************************************************
 *         MSB Bit Start Location Begin         *
 ***********************************************/ 
#define START_MSB_BIT_AT_0                    0
#define START_MSB_BIT_AT_1                    1
#define START_MSB_BIT_AT_2                    2
#define START_MSB_BIT_AT_3                    3
#define START_MSB_BIT_AT_4                    4
#define START_MSB_BIT_AT_5                    5
#define START_MSB_BIT_AT_6                    6
#define START_MSB_BIT_AT_7                    7
/*----------------------------------------------*
 *        MSB Bit Start Location End            *
 *----------------------------------------------*/ 
/************************************************
 *          Bit Field Length Begin              *
 ***********************************************/ 
#define BIT_LENGTH_1                          1
#define BIT_LENGTH_2                          2
#define BIT_LENGTH_3                          3
#define BIT_LENGTH_4                          4
#define BIT_LENGTH_5                          5
#define BIT_LENGTH_6                          6
#define BIT_LENGTH_7                          7
#define BIT_LENGTH_8                          8
/*----------------------------------------------*
 *          Bit Field Length End                *
 *----------------------------------------------*/
#define ADC_RAW_DATA_BUFFER_SIZE              6
/**************************************************
 *     Values For Disable And Enable Functions    *
 **************************************************/ 
typedef enum Ability
{  
	Disable = 0     ,                      
	Enable     
}HTU31D_Ability;    
/**************************************************
 *     Values For ACTIVE And DEACTIVE Functions   *
 **************************************************/ 
typedef enum Action
{  
	DEACTIVE = 0     ,                      
	ACTIVE   = 1    
}HTU31D_Action; 
/**************************************************
 *     					Values For Humidity OSR           *
 **************************************************/ 
typedef enum Humidity_OSR
{
	HUMIDITY_OSR_0  = 0x00 ,
	HUMIDITY_OSR_1  = 0x08 ,
	HUMIDITY_OSR_2  = 0x10 ,
	HUMIDITY_OSR_3  = 0x18
}HTU31D_Humidity_OSR;
/**************************************************
 *     			Values For Temperature OSR            *
 **************************************************/ 
typedef enum Temperature_OSR
{
	TEMPERATURE_OSR_0  = 0x00 ,
	TEMPERATURE_OSR_1  = 0x02 ,
	TEMPERATURE_OSR_2  = 0x04	,
	TEMPERATURE_OSR_3  = 0x06
}HTU31D_Temperature_OSR;
/**************************************************
 *     	Values For Humidity Conversion Time       *
 **************************************************/ 
typedef enum Humidity_Conversion_Time
{
	HUMIDITY_OSR_0_MEASUREMENT_TIME = 1 ,
	HUMIDITY_OSR_1_MEASUREMENT_TIME = 2 ,
	HUMIDITY_OSR_2_MEASUREMENT_TIME = 4 ,
	HUMIDITY_OSR_3_MEASUREMENT_TIME = 8
}HTU31D_Humidity_Conversion_Time;
/**************************************************
 *     	Values For Temperature Conversion Time    *
 **************************************************/
typedef enum Temperature_Conversion_Time 
{
	TEMPERATURE_OSR_0_MEASUREMENT_TIME = 2 ,
	TEMPERATURE_OSR_1_MEASUREMENT_TIME = 4 ,
	TEMPERATURE_OSR_2_MEASUREMENT_TIME = 7 ,
	TEMPERATURE_OSR_3_MEASUREMENT_TIME = 13
}HTU31D_Temperature_Conversion_Time;
/*************************************************
 *           Values For Reset Process             *
 **************************************************/ 
typedef enum Reset_Status
{  
	FAILED = 0     ,                      
	DONE     
}HTU31D_Reset_Status;
/*************************************************
 *           			Values For Heater              *
 **************************************************/ 
typedef enum Heater 
{  
	HEATER_ON  = HTU31D_HEATER_ON     ,                      
	HEATER_OFF = HTU31D_HEATER_OFF      
}HTU31D_Heater;
/*************************************************
 *           Values For CRC Status               *
 **************************************************/ 
typedef enum CRC_Status 
{  
	CRC_ERROR = 0     ,                      
	CRC_OK     
}HTU31D_CRC_Status;

 /*************************************************
 *  Defining HTU31D Register & Data As Struct   *
 **************************************************/
typedef	struct HTU31D
{
	  uint8_t                       	   Register_Cache;
		HTU31D_Reset_Status					       RESET;
	  uint32_t                           SERIAL_NUMBER;
	  uint8_t														 DIAGNOSTIC;
	  HTU31D_Action											 NVM_CRC_ERROR;
	  HTU31D_Action                      HUMIDITY_UNDER_OVER_RUN;
	  HTU31D_Action											 HUMIDITY_ABOVE_120RH_ERROR;
	  HTU31D_Action											 HUMIDITY_BELOW_10RH_ERROR ;//-10
	  HTU31D_Action                      TEMPERATURE_UNDER_OVER_RUN;
	  HTU31D_Action											 TEMPERATURE_ABOVE_150_ERROR;
	  HTU31D_Action											 TEMPERATURE_BELOW_50_ERROR ;//-50
	  HTU31D_Heater								       ON_CHIP_HEATER;
	  HTU31D_Humidity_OSR                HUMIDITY_OSR;
	  HTU31D_Temperature_OSR						 TEMPERATURE_OSR;
	  HTU31D_Humidity_Conversion_Time    HUMIDITY_CONVERSION_TIME;
	  HTU31D_Temperature_Conversion_Time TEMPERATURE_CONVERSION_TIME;  
		uint8_t                            ADC_RAW_DATA[ADC_RAW_DATA_BUFFER_SIZE];
		uint16_t                           RAW_TEMPERATURE;
		uint16_t													 RAW_HUMIDITY;
		uint8_t 													 HTU31D_CRC;
		HTU31D_CRC_Status 								 CRC_CHECK;
    float 														 TEMPERATURE;
		float 														 HUMIDITY;
//	  double														 PARTIAL_PRESSURE;
//		double 														 DEW_POINT;
}GebraBit_HTU31D;
/*
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: */
/********************************************************
 *  Declare Read&Write HTU31D Register Values Functions *
 ********************************************************/
extern void GB_HTU31D_Write_Command( uint8_t cmd);
/********************************************************
 *       Declare HTU31D Configuration Functions         *
 ********************************************************/
extern void GB_HTU31D_Soft_Reset ( GebraBit_HTU31D * HTU31D )  ;
extern void GB_HTU31D_CRC_Check( GebraBit_HTU31D * HTU31D , uint16_t value, uint8_t crc) ;
extern void GB_HTU31D_On_Chip_Heater ( GebraBit_HTU31D * HTU31D , HTU31D_Heater heater )   ;
extern void GB_HTU31D_Read_Serial_Number ( GebraBit_HTU31D * HTU31D  )    ;
extern void GB_HTU31D_Read_Diagnostic ( GebraBit_HTU31D * HTU31D  )   ;
extern void GB_HTU31D_Humidity_OSR ( GebraBit_HTU31D * HTU31D , HTU31D_Humidity_OSR osr )  ;
extern void GB_HTU31D_Temperature_OSR ( GebraBit_HTU31D * HTU31D , HTU31D_Temperature_OSR osr )  ;
extern void GB_HTU31D_Configuration(GebraBit_HTU31D * HTU31D)  ;
extern void GB_HTU31D_Start_Conversion ( GebraBit_HTU31D * HTU31D   )   ;
extern void GB_HTU31D_Read_Raw_Temperature_Humidity( GebraBit_HTU31D * HTU31D )  ;
extern void GB_HTU31D_Temperature ( GebraBit_HTU31D * HTU31D )  ;
extern void GB_HTU31D_Humidity ( GebraBit_HTU31D * HTU31D )   ;
extern void GB_HTU31D_Dew_Point( GebraBit_HTU31D * HTU31D  ) ;
extern void GB_HTU31D_initialize( GebraBit_HTU31D * HTU31D )  ;
extern void GB_HTU31D_Get_Data(GebraBit_HTU31D * HTU31D);
#endif
