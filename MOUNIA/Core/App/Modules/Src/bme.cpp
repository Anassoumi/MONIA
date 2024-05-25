/*
 * bme.cpp
 *
 *  Created on: May 13, 2024
 *      Author: talib
 */

#include "main.h"
#include <stdio.h>
#include "../Inc/bme.hpp"
#include "i2c.h"

extern I2C_HandleTypeDef hi2c2;
BME_DATA hbme;

BME::BME(I2C_HandleTypeDef *hi2c) :
		hi2c_(hi2c), address_(static_cast<uint8_t>(BME::Register::DEV_ADDRESS)) {

}

HAL_StatusTypeDef BME::writeRegister(uint8_t device_address,
		uint8_t regAddress, uint8_t regData) {
	uint8_t data[2] = { static_cast<uint8_t>(regAddress), regData };

	return HAL_I2C_Master_Transmit(this->hi2c_,
			(uint16_t) (device_address << 1), data, 2, HAL_MAX_DELAY);

}

HAL_StatusTypeDef BME::readRegister(uint8_t device_address, uint8_t register_address, uint8_t *buffer) {
    // Transmit the register address
    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(this->hi2c_, (uint16_t)(device_address << 1),
                                                       &register_address, 1, HAL_MAX_DELAY);
    if (status != HAL_OK) {
        return status;  // Return error if transmission fails
    }

    // Receive the data from the register
    return HAL_I2C_Master_Receive(this->hi2c_, (uint16_t)(device_address << 1),
                                  buffer, 1, HAL_MAX_DELAY);
}


HAL_StatusTypeDef BME::readRegisters(uint8_t device_address,
		uint8_t start_register_address, uint8_t *buffer, uint16_t length) {
	// Transmit register address in write mode
	HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(this->hi2c_,
			(uint16_t) (device_address << 1), &start_register_address, 1,
			HAL_MAX_DELAY);
	if (status != HAL_OK) {
		return status; // Return error status if transmit failed
	}


	// Receive data in read mode
	return HAL_I2C_Master_Receive(this->hi2c_,
			(uint16_t) (device_address << 1) | 1, buffer, length, HAL_MAX_DELAY);
}

uint8_t BME::init() {
	 uint8_t id_buffer { 0 };
	    uint8_t dig_t[6] { 0 };
	    uint8_t dig_p[18] { 0 };
	    uint8_t dig_h1[1] { 0 };
	    uint8_t dig_h2_h6[7] { 0 };

	    // Read temperature calibration data
	    if (this->readRegisters(this->address_, static_cast<uint8_t>(BME::Register::DIG_T1), dig_t, 6) == HAL_OK) {
	        printf("dig data is in the house\n\r");

	        hbme.dig_T1 = (dig_t[0] << 8) | dig_t[1];
	        hbme.dig_T2 = (dig_t[2] << 8) | dig_t[3];
	        hbme.dig_T3 = (dig_t[4] << 8) | dig_t[5];
	    }

	    // Read pressure calibration data
	    if (this->readRegisters(this->address_, static_cast<uint8_t>(BME::Register::DIG_P1), dig_p, 18) == HAL_OK) {
	        hbme.dig_P1 = (dig_p[0] << 8) | dig_p[1];
	        hbme.dig_P2 = (dig_p[2] << 8) | dig_p[3];
	        hbme.dig_P3 = (dig_p[4] << 8) | dig_p[5];
	        hbme.dig_P4 = (dig_p[6] << 8) | dig_p[7];
	        hbme.dig_P5 = (dig_p[8] << 8) | dig_p[9];
	        hbme.dig_P6 = (dig_p[10] << 8) | dig_p[11];
	        hbme.dig_P7 = (dig_p[12] << 8) | dig_p[13];
	        hbme.dig_P8 = (dig_p[14] << 8) | dig_p[15];
	        hbme.dig_P9 = (dig_p[16] << 8) | dig_p[17];
	    }

	    // Read humidity calibration data
	    // Read dig_H1
	    if (this->readRegisters(this->address_, static_cast<uint8_t>(BME::Register::DIG_H1), dig_h1, 1) == HAL_OK) {
	        hbme.dig_H1 = dig_h1[0];
	    }

	    // Read dig_H2 to dig_H6
	    if (this->readRegisters(this->address_, static_cast<uint8_t>(BME::Register::DIG_H2), dig_h2_h6, 7) == HAL_OK) {
	        hbme.dig_H2 = (dig_h2_h6[1] << 8) | dig_h2_h6[0];
	        hbme.dig_H3 = dig_h2_h6[2];
	        hbme.dig_H4 = (dig_h2_h6[3] << 4) | (dig_h2_h6[4] & 0x0F);
	        hbme.dig_H5 = (dig_h2_h6[5] << 4) | (dig_h2_h6[4] >> 4);
	        hbme.dig_H6 = (int8_t)dig_h2_h6[6];
	    }



	if (this->writeRegister(this->address_,static_cast<uint8_t>(BME::Register::CTRL_HUM), 0x04)
			== HAL_OK) {
		printf("ctrl humidity write successful\n\r");
	}
	if (this->writeRegister(this->address_,static_cast<uint8_t>( BME::Register::CTRL_MEAS), 0x93)
			== HAL_OK) //ctrl for temp and pressure
			{
		printf("BME Control sat successfully");
	}

	if (this->readRegister(this->address_,
			static_cast<uint8_t>(BME::Register::ID), &id_buffer) == HAL_OK
			&& id_buffer == 0x60) {
		printf("BME init communication successful \n\r");
		return id_buffer;
	}
	return 0;
}

void BME::reset() {
	this->writeRegister(this->address_,static_cast<uint8_t>(BME::Register::RESET), 0xB6);
}

#include <stdint.h> // For fixed-width integer types

// Define BME280_S32_t as int32_t for clarity and consistency
// Placeholder for your BME_DATA structure and any other necessary includes

BME280_S32_t t_fine { 0 };
BME280_S32_t T;

BME280_S32_t BME::bme280_compensate(int32_t adc_T) {
    BME280_S32_t var1, var2;

    var1 = ((((adc_T >> 3) - ((BME280_S32_t) hbme.dig_T1 << 1)))
            * ((BME280_S32_t) hbme.dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((BME280_S32_t) hbme.dig_T1))
            * ((adc_T >> 4) - ((BME280_S32_t) hbme.dig_T1))) >> 12)
            * ((BME280_S32_t) hbme.dig_T3)) >> 14;

    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    hbme.temp = T / 100;

    return T;
}

BME280_U32_t BME::bme280_compensate_H_int32(BME280_S32_t adc_H) {
    BME280_S32_t v_x1_u32r;

    v_x1_u32r = (t_fine - ((BME280_S32_t)76800));

    v_x1_u32r = (((((adc_H << 14) - (((BME280_S32_t)hbme.dig_H4) << 20) - (((BME280_S32_t)hbme.dig_H5) * v_x1_u32r)) +
                  ((BME280_S32_t)16384)) >> 15) * (((((((v_x1_u32r * ((BME280_S32_t)hbme.dig_H6)) >> 10) *
                  (((v_x1_u32r * ((BME280_S32_t)hbme.dig_H3)) >> 11) + ((BME280_S32_t)32768))) >> 10) +
                  ((BME280_S32_t)2097152)) * ((BME280_S32_t)hbme.dig_H2) + 8192) >> 14));

    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((BME280_S32_t)hbme.dig_H1)) >> 4));

    v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
    v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);

    return (BME280_U32_t)(v_x1_u32r >> 12);
}

BME280_U32_t BME::bme280_compensate_P_int32(BME280_S32_t adc_P) {
    BME280_S32_t var1, var2;
    BME280_U32_t p;

    var1 = (((BME280_S32_t)t_fine) >> 1) - (BME280_S32_t)64000;
    var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * ((BME280_S32_t)hbme.dig_P6);
    var2 = var2 + ((var1 * ((BME280_S32_t)hbme.dig_P5)) << 1);
    var2 = (var2 >> 2) + (((BME280_S32_t)hbme.dig_P4) << 16);
    var1 = (((hbme.dig_P3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3) + ((((BME280_S32_t)hbme.dig_P2) * var1) >> 1)) >> 18;
    var1 = ((((32768 + var1)) * ((BME280_S32_t)hbme.dig_P1)) >> 15);

    if (var1 == 0) {
        return 0; // avoid exception caused by division by zero
    }

    p = (((BME280_U32_t)(((BME280_S32_t)1048576) - adc_P) - (var2 >> 12))) * 3125;
    if (p < 0x80000000) {
        p = (p << 1) / ((BME280_U32_t)var1);
    } else {
        p = (p / (BME280_U32_t)var1) * 2;
    }
    var1 = (((BME280_S32_t)hbme.dig_P9) * ((BME280_S32_t)(((p >> 3) * (p >> 3)) >> 13))) >> 12;
    var2 = (((BME280_S32_t)(p >> 2)) * ((BME280_S32_t)hbme.dig_P8)) >> 13;
    p = (BME280_U32_t)((BME280_S32_t)p + ((var1 + var2 + hbme.dig_P7) >> 4));

    return p;
}


uint32_t temp_raw = 0;

BME280_S32_t BME::read_temp() {
    uint8_t temp_data[3] = { 0 };

    // Read 3 bytes starting from TEMP_MSB register
    if (this->readRegisters(this->address_, static_cast<uint8_t>(BME::Register::TEMP_MSB), temp_data, 3) != HAL_OK) {
        // Handle error
        printf("Failed to read temperature data\n\r");
        return 0;
    }

    // Combine the bytes to form the temperature value
    temp_raw = (temp_data[0] << 12) | (temp_data[1] << 4) | (temp_data[2] >> 4);

    return bme280_compensate(static_cast<BME280_S32_t>(temp_raw));
}

double BME::read_humidity() {
    uint8_t humidity_data[2] = { 0 };

    // Read 2 bytes starting from HUM_MSB register
    if (this->readRegisters(this->address_, static_cast<uint8_t>(BME::Register::HUM_MSB), humidity_data, 2) != HAL_OK) {
        // Handle error
        printf("Failed to read humidity data\n\r");
        return 0;
    }

    // Combine the bytes to form the humidity value
    uint32_t humidity_raw = (humidity_data[0] << 8) | humidity_data[1];

    return (double)bme280_compensate_H_int32(static_cast<BME280_S32_t>(humidity_raw)) / 1024.0;
}

double BME::read_pressure() {
    uint8_t press_data[3] = { 0 };

    // Read 3 bytes starting from PRESS_MSB register
    if (this->readRegisters(this->address_, static_cast<uint8_t>(BME::Register::PRESS_MSB), press_data, 3) != HAL_OK) {
        // Handle error
        printf("Failed to read pressure data\n\r");
        return 0;
    }

    // Combine the bytes to form the pressure value
    uint32_t press_raw = (press_data[0] << 12) | (press_data[1] << 4) | (press_data[2] >> 4);

    return (double)bme280_compensate_P_int32(static_cast<BME280_S32_t>(press_raw)) / 100.0;
}

BME sensor1(&hi2c6);

void BME_INIT() {
    if (sensor1.init() == 0) {
        printf("BME initialization failed\n");
    }
}

double t, h, p;

void BME_entry() {
    t = sensor1.read_temp();
    p = sensor1.read_pressure();
    h = sensor1.read_humidity();
}
