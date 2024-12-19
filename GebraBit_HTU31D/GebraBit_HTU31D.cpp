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
#include "GebraBit_HTU31D.h"

/*=========================================================================================================================================
 * @param     cmd    Command that will be writen 
 * @return    None
 ========================================================================================================================================*/
void GB_HTU31D_Write_Command( uint8_t cmd)
{
	uint8_t TBuff[1];
	TBuff[0]=cmd;
	Wire.beginTransmission(HTU31D_ADDRESS);  
    Wire.write(TBuff, 1);                    
    Wire.endTransmission(); 
}
/*=========================================================================================================================================
 * @brief     Check CRC
 * @param     HTU31D   HTU31D Struct  CRC_CHECK variable
 * @param     value        Value that must be compare with crc
 * @param     crc          CRC Value
 * @return    Nothing
 ========================================================================================================================================*/
void GB_HTU31D_CRC_Check( GebraBit_HTU31D * HTU31D , uint16_t value, uint8_t crc)
{
	uint32_t polynom = 0x988000; // x^8 + x^5 + x^4 + 1
	uint32_t msb     = 0x800000;
	uint32_t mask    = 0xFF8000;
	uint32_t result  = (uint32_t)value<<8; // Pad with zeros as specified in spec
	
	while( msb != 0x80 ) {
		
		// Check if msb of current value is 1 and apply XOR mask
		if( result & msb )
			result = ((result ^ polynom) & mask) | ( result & ~mask);
			
		// Shift by one
		msb >>= 1;
		mask >>= 1;
		polynom >>=1;
	}
	if( result == crc )
		HTU31D->CRC_CHECK = CRC_OK;
	else
		HTU31D->CRC_CHECK = CRC_ERROR;
} 

/*=========================================================================================================================================
 * @brief     Reset HTU31D
 * @param     HTU31D   HTU31D Struct
 * @return    Nothing
 ========================================================================================================================================*/
void GB_HTU31D_Soft_Reset ( GebraBit_HTU31D * HTU31D )  
{
  GB_HTU31D_Write_Command(HTU31D_RESET);
	delay(15);
	HTU31D->RESET = DONE ;
}

/*=========================================================================================================================================
 * @brief     Enable Or Disable On Chip Heater
 * @param     HTU31D   HTU31D Struct ON_CHIP_HEATER variable
 * @param     heater        Value is from HTU31D_Heater Enume
 * @return    Nothing
 ========================================================================================================================================*/
void GB_HTU31D_On_Chip_Heater ( GebraBit_HTU31D * HTU31D , HTU31D_Heater heater )  
{
 GB_HTU31D_Write_Command(heater);
 HTU31D->ON_CHIP_HEATER= heater ; 
}
/*=========================================================================================================================================
 * @brief     Read Serial Number
 * @param     HTU31D   HTU31D Struct SERIAL_NUMBER variable
 * @return    Nothing
 ========================================================================================================================================*/
void GB_HTU31D_Read_Serial_Number ( GebraBit_HTU31D * HTU31D  ) 
{
 uint8_t data[4];
 Wire.beginTransmission(HTU31D_ADDRESS);
 Wire.write(HTU31D_READ_SERIAL_NUMBER);
 Wire.endTransmission(false);
 delay(15);
 Wire.requestFrom(HTU31D_ADDRESS, 4);
     for (int i = 0; i < 4; i++) {
        if (Wire.available()) {
            data[i] = Wire.read();
        }
    }
 HTU31D->SERIAL_NUMBER =((uint32_t)data[0] << 16) | ((uint32_t)data[1]<< 8) | ((uint32_t)data[2]);
 HTU31D->HTU31D_CRC = data[3] ;
 GB_HTU31D_CRC_Check( HTU31D , (uint16_t)(HTU31D->SERIAL_NUMBER & 0xFF) , HTU31D->HTU31D_CRC) ;
}
/*=========================================================================================================================================
 * @brief     Read HTU31D Diagnostic
 * @param     HTU31D   HTU31D Struct 
 * @return    Nothing
 ========================================================================================================================================*/
void GB_HTU31D_Read_Diagnostic ( GebraBit_HTU31D * HTU31D  ) 
{
 uint8_t data[2];
 Wire.beginTransmission(HTU31D_ADDRESS);
 Wire.write(HTU31D_READ_DIAGNOSTIC);
 Wire.endTransmission(false);
 delay(15);
 Wire.requestFrom(HTU31D_ADDRESS, 2);
     for (int i = 0; i < 2; i++) {
        if (Wire.available()) {
            data[i] = Wire.read();
        }
    }
 HTU31D->DIAGNOSTIC = data[0] ;
 HTU31D->NVM_CRC_ERROR 							 = (HTU31D->DIAGNOSTIC & 0x80)>>7 ;
 HTU31D->HUMIDITY_UNDER_OVER_RUN     = (HTU31D->DIAGNOSTIC & 0x40)>>6 ;
 HTU31D->HUMIDITY_ABOVE_120RH_ERROR  = (HTU31D->DIAGNOSTIC & 0x20)>>5 ;
 HTU31D->HUMIDITY_BELOW_10RH_ERROR   = (HTU31D->DIAGNOSTIC & 0x10)>>4 ;
 HTU31D->TEMPERATURE_UNDER_OVER_RUN  = (HTU31D->DIAGNOSTIC & 0x08)>>3 ;
 HTU31D->TEMPERATURE_ABOVE_150_ERROR = (HTU31D->DIAGNOSTIC & 0x04)>>2 ;
 HTU31D->TEMPERATURE_BELOW_50_ERROR  = (HTU31D->DIAGNOSTIC & 0x02)>>1 ;
 HTU31D->ON_CHIP_HEATER 						 = (HTU31D->DIAGNOSTIC & 0x01)<<((HTU31D->DIAGNOSTIC & 0x01)+1) ;
 HTU31D->HTU31D_CRC = data[1] ;
 GB_HTU31D_CRC_Check( HTU31D , (uint16_t)HTU31D->DIAGNOSTIC , HTU31D->HTU31D_CRC) ;
}
/*=========================================================================================================================================
 * @brief     Set Humidity OSR
 * @param     HTU31D   HTU31D Struct  HUMIDITY_OSR & HUMIDITY_CONVERSION_TIME variable
 * @param     osr        Value is from HTU31D_Humidity_OSR Enume
 * @return    Nothing
 ========================================================================================================================================*/
void GB_HTU31D_Humidity_OSR ( GebraBit_HTU31D * HTU31D , HTU31D_Humidity_OSR osr ) 
{
 HTU31D->HUMIDITY_OSR = osr ;
 switch(HTU31D->HUMIDITY_OSR)
	 {
	  case HUMIDITY_OSR_0:
		HTU31D->HUMIDITY_CONVERSION_TIME = HUMIDITY_OSR_0_MEASUREMENT_TIME ; 
    break;
		case HUMIDITY_OSR_1:
		HTU31D->HUMIDITY_CONVERSION_TIME = HUMIDITY_OSR_1_MEASUREMENT_TIME ; 
    break;	
		case HUMIDITY_OSR_2:
		HTU31D->HUMIDITY_CONVERSION_TIME = HUMIDITY_OSR_2_MEASUREMENT_TIME ; 
    break;	
		case HUMIDITY_OSR_3:
		HTU31D->HUMIDITY_CONVERSION_TIME = HUMIDITY_OSR_3_MEASUREMENT_TIME ;  
    break;	
	 }
}
/*=========================================================================================================================================
 * @brief     Set Temperature OSR
 * @param     HTU31D   HTU31D Struct  TEMPERATURE_OSR & TEMPERATURE_CONVERSION_TIME variable
 * @param     osr        Value is from HTU31D_Temperature_OSR Enume
 * @return    Nothing
 ========================================================================================================================================*/
void GB_HTU31D_Temperature_OSR ( GebraBit_HTU31D * HTU31D , HTU31D_Temperature_OSR osr ) 
{
 HTU31D->TEMPERATURE_OSR = osr ;
 switch(HTU31D->TEMPERATURE_OSR)
	 {
	  case TEMPERATURE_OSR_0:
		HTU31D->TEMPERATURE_CONVERSION_TIME = TEMPERATURE_OSR_0_MEASUREMENT_TIME ; 
    break;
		case TEMPERATURE_OSR_1:
		HTU31D->TEMPERATURE_CONVERSION_TIME = TEMPERATURE_OSR_1_MEASUREMENT_TIME ; 
    break;	
		case TEMPERATURE_OSR_2:
		HTU31D->TEMPERATURE_CONVERSION_TIME = TEMPERATURE_OSR_2_MEASUREMENT_TIME ; 
    break;	
		case TEMPERATURE_OSR_3:
		HTU31D->TEMPERATURE_CONVERSION_TIME = TEMPERATURE_OSR_3_MEASUREMENT_TIME ;  
    break;	
	 }
}
/*
M403Z 
*/
/*=========================================================================================================================================
 * @brief     initialize HTU31D
 * @param     HTU31D     HTU31D Struct 
 * @return    Nothing
 ========================================================================================================================================*/ 
void GB_HTU31D_initialize( GebraBit_HTU31D * HTU31D )
{
  GB_HTU31D_Soft_Reset   ( HTU31D ) ;
  GB_HTU31D_Read_Serial_Number ( HTU31D ) ;
	GB_HTU31D_Read_Diagnostic ( HTU31D ) ;
}
/*=========================================================================================================================================
 * @brief     Configure HTU31D
 * @param     HTU31D  Configure HTU31D 
 * @return    Nothing
 ========================================================================================================================================*/ 
void GB_HTU31D_Configuration(GebraBit_HTU31D * HTU31D)
{
	GB_HTU31D_On_Chip_Heater( HTU31D , HTU31D_HEATER_OFF ) ;
  GB_HTU31D_Humidity_OSR ( HTU31D , HUMIDITY_OSR_1 ) ; 
	GB_HTU31D_Temperature_OSR ( HTU31D , TEMPERATURE_OSR_2 ) ; 
} 
/*=========================================================================================================================================
 * @brief     Start Conversion
 * @param     HTU31D   HTU31D Struct
 * @return    Nothing
 ========================================================================================================================================*/
void GB_HTU31D_Start_Conversion ( GebraBit_HTU31D * HTU31D ) 
{
 GB_HTU31D_Write_Command(HTU31D_CONVERSION|HTU31D->HUMIDITY_OSR|HTU31D->TEMPERATURE_OSR);
 delay(HTU31D->HUMIDITY_CONVERSION_TIME+HTU31D->TEMPERATURE_CONVERSION_TIME);
}
/*=========================================================================================================================================
 * @brief     Read HTU31D ADC Temperature , Humidity Raw Data
 * @param     HTU31D   HTU31D Struct RAW_HUMIDITY variable and RAW_TEMPERATURE variable
 * @return    Nothing
 ========================================================================================================================================*/
void GB_HTU31D_Read_Raw_Temperature_Humidity ( GebraBit_HTU31D * HTU31D ) 
{
  Wire.beginTransmission(HTU31D_ADDRESS);
  Wire.write(HTU31D_READ_TEMPERATURE_HUMIDITY);
  Wire.endTransmission(false);
  delay(15);
  Wire.requestFrom(HTU31D_ADDRESS, ADC_RAW_DATA_BUFFER_SIZE);
     for (int i = 0; i < ADC_RAW_DATA_BUFFER_SIZE; i++) {
        if (Wire.available()) {
            HTU31D->ADC_RAW_DATA[i] = Wire.read();
        }
    }
  HTU31D->RAW_TEMPERATURE = ((uint16_t)HTU31D->ADC_RAW_DATA[0]<< 8) | ((uint16_t)HTU31D->ADC_RAW_DATA[1]);
  HTU31D->HTU31D_CRC = HTU31D->ADC_RAW_DATA[2] ;
  GB_HTU31D_CRC_Check( HTU31D , HTU31D->RAW_TEMPERATURE , HTU31D->HTU31D_CRC) ;	
  HTU31D->RAW_HUMIDITY    = ((uint16_t)HTU31D->ADC_RAW_DATA[3]<< 8) | ((uint16_t)HTU31D->ADC_RAW_DATA[4]); 
  HTU31D->HTU31D_CRC = HTU31D->ADC_RAW_DATA[5] ;
  GB_HTU31D_CRC_Check( HTU31D , HTU31D->RAW_HUMIDITY , HTU31D->HTU31D_CRC) ;
  delay(400);
}
/*=========================================================================================================================================
 * @brief     Calculate Temperature
 * @param     HTU31D   HTU31D Struct TEMPERATURE variable
 * @return    Nothing
 ========================================================================================================================================*/
void GB_HTU31D_Temperature ( GebraBit_HTU31D * HTU31D  ) 
{
	HTU31D->TEMPERATURE = ((float)HTU31D->RAW_TEMPERATURE / 65535.0f) * 165.0f - 40.0f;	
}
/*=========================================================================================================================================
 * @brief     Calculate Humidity
 * @param     HTU31D   HTU31D Struct HUMIDITY variable
 * @return    Nothing
 ========================================================================================================================================*/
void GB_HTU31D_Humidity ( GebraBit_HTU31D * HTU31D  )  
{
	HTU31D->HUMIDITY   =  ((float)HTU31D->RAW_HUMIDITY / 65535.0f) * 100.0f;	
}
/*=========================================================================================================================================
 * @brief     Calculate Dew Point
 * @param     HTU31D   HTU31D Struct DEW_POINT variable and PARTIAL_PRESSURE variable
 * @return    Nothing
 ========================================================================================================================================*/
void GB_HTU31D_Dew_Point( GebraBit_HTU31D * HTU31D  ) 
{
	double partial_pressure;
	double dew_point;
	
	// Missing power of 10
//	HTU31D->PARTIAL_PRESSURE = pow( 10, HTU31D_CONSTANT_A - HTU31D_CONSTANT_B / (HTU31D->TEMPERATURE + HTU31D_CONSTANT_C) );
//	HTU31D->DEW_POINT        = - HTU31D_CONSTANT_B / (log10( HTU31D->COMPANSATED_HUMIDITY * partial_pressure / 100) - HTU31D_CONSTANT_A) - HTU31D_CONSTANT_C;
	
}
/*=========================================================================================================================================
 * @brief     Get Data  
 * @param     HTU31D       GebraBit_HTU31D Staruct
 * @return    Nothing
 ========================================================================================================================================*/ 
void GB_HTU31D_Get_Data(GebraBit_HTU31D * HTU31D)
{
  	GB_HTU31D_Start_Conversion( HTU31D );
	GB_HTU31D_Read_Raw_Temperature_Humidity(HTU31D);
	GB_HTU31D_Read_Diagnostic ( HTU31D ) ;
	GB_HTU31D_Temperature( HTU31D  );
	GB_HTU31D_Humidity( HTU31D  );
//	GB_HTU31D_Dew_Point( HTU31D  );
}
/*----------------------------------------------------------------------------------------------------------------------------------------*
 *                                                                      End                                                               *
 *----------------------------------------------------------------------------------------------------------------------------------------*/
/*
M403Z 
*/
//  GB_HTU31D_Read_User_Register(&data);
//	GB_HTU31D_Write_User_Register(0x83);
//	GB_HTU31D_Read_User_Register_Bits(START_MSB_BIT_AT_7, BIT_LENGTH_4, &HTU31D_Module.Register_Cache);
//	GB_HTU31D_Read_User_Register_Bits(START_MSB_BIT_AT_3, BIT_LENGTH_4, &HTU31D_Module.Register_Cache);
//	GB_HTU31D_Write_User_Register_Bits(START_MSB_BIT_AT_7, BIT_LENGTH_4, 0);
//	GB_HTU31D_Write_User_Register_Bits(START_MSB_BIT_AT_3, BIT_LENGTH_4, 3);
//	GB_HTU31D_Read_User_Register(&data);