/*
 * tcs34.hpp
 *
 *  Created on: May 19, 2024
 *      Author: talib
 */

#pragma once


#ifdef __cplusplus
extern "C"
{
#include <cstdint>
constexpr uint8_t TCS34_ADDR =(0x29);


#include "bme.hpp"

void entry_TC34();
class TCS34: public BME {

private:

	static uint16_t BRIGHTNESS;

	enum class REGISTER : uint8_t {
		ENABLE = 0x00,
		ID=0x12,
		CDATA=0x14,
		CDATAH=0x15,

	};

	void readColour();

public:
	TCS34(I2C_HandleTypeDef *hi2c);
	uint8_t init() override;
	double convertToPercent();
//	HAL_StatusTypeDef readRegister(uint8_t device_address,
//			uint8_t register_address, uint8_t *buffer) override;
//	HAL_StatusTypeDef writeRegister(uint8_t device_address, Register regAddress,
//			uint8_t regData) override;
//	HAL_StatusTypeDef readRegisters(uint8_t device_address,
//			uint8_t start_register_address, uint8_t *buffer, uint16_t length) override;

};
}

#endif


