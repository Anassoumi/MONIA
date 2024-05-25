/*
 * gps.h
 *
 *  Created on: May 20, 2024
 *      Author: talib
 */

#pragma once

#include "../../../Core/Inc/usart.h"
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <sstream>
#ifdef __cplusplus
extern "C" {
class GPS {

private:


	float longitude;
	float latitude;
	float altitude;
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
	UART_HandleTypeDef *huart_;

public:
	static GPS* instance;
	GPS(UART_HandleTypeDef *huart);
	std::array<uint8_t, 700> storage_buffer;

	void receive(void);
	void get_gps_data();
	void process_data(void);
	void extract_data(const std::string& gppa_sentence, uint8_t& hours, uint8_t& minutes, uint8_t& seconds, float& latitude, float& longitude, float& altitude);



};


}

#endif

