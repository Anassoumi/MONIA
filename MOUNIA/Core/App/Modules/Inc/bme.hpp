/*
 * bme.hpp
 *
 *  Created on: May 13, 2024
 *      Author: talib
 */

#pragma once
#include <stdint.h>
#include "../../../Inc/i2c.h"
#define BME_ADDR (0x76<<1)
#define BME_ID 0xD0


typedef struct
{
    uint16_t dig_T1;
    int16_t dig_T2;
    int16_t dig_T3;
    uint16_t dig_P1;
    int16_t dig_P2;
    int16_t dig_P3;
    int16_t dig_P4;
    int16_t dig_P5;
    int16_t dig_P6;
    int16_t dig_P7;
    int16_t dig_P8;
    int16_t dig_P9;
    uint8_t dig_H1;
    int16_t dig_H2;
    uint8_t dig_H3;
    int16_t dig_H4;
    int16_t dig_H5;
    int8_t dig_H6;
    uint16_t temp;
} BME_DATA;



typedef int32_t BME280_S32_t;
typedef uint32_t BME280_U32_t;




#ifdef __cplusplus
 extern "C"
 {
 void BME_entry(void);
  void BME_INIT(void);
 class BME
 {

 public :
 	enum class Register:uint8_t
 	{
 		DEV_ADDRESS=0x76,
 		HUM_LSB=0XFE,
 		HUM_MSB=0xFD,
 		TEMP_XLSB=0xFC,
 		TEMP_LSB=0xFB,
 		TEMP_MSB=0xFA,
 		PRESS_XLSB=0xF9,
 		PRESS_LSB=0xF8,
 		PRESS_MSB=0xF7,
 		CONFIG=0xF5,
		CTRL_MEAS=0xF4,
 		CTRL_HUM=0XF2,
 		/*cALIBRATION DATA*/


		DIG_T1=0x88,
		DIG_P1=0x8E,
		DIG_H1=0xA1,
		DIG_H2=0xE1,

 		//////

 		RESET=0xE0,
 		ID=0xD0,


 	};


 	BME(I2C_HandleTypeDef* hi2c);
 	virtual uint8_t init();
 	void reset(void);
 	BME280_S32_t bme280_compensate(int32_t adc_T);

 	BME280_S32_t read_temp();
 	double read_humidity();
 	double read_pressure();

 	BME280_U32_t bme280_compensate_H_int32(BME280_S32_t adc_H);
 	BME280_U32_t bme280_compensate_P_int32(BME280_S32_t adc_P);



 	virtual HAL_StatusTypeDef readRegister(uint8_t device_address,uint8_t register_address,uint8_t *buffer);
 	virtual HAL_StatusTypeDef writeRegister(uint8_t device_address,uint8_t regAddress,uint8_t regData);
 	virtual HAL_StatusTypeDef readRegisters(uint8_t device_address, uint8_t start_register_address, uint8_t* buffer, uint16_t length);



 private :

 	I2C_HandleTypeDef *hi2c_;
 	uint8_t address_;
 };







 }


#endif




