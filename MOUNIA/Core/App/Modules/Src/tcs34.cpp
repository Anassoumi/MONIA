#include "../Inc/tcs34.hpp"
#include <stdio.h>

// Constructor initializing base class with I2C handler
TCS34::TCS34(I2C_HandleTypeDef *hi2c) : BME(hi2c) {}

uint16_t brightness_value{0};


uint8_t TCS34::init() {
    uint8_t id_buffer = 0;

    uint8_t cmd = 0x80;  // CMD bit must be set to 1 (0x80)

    // Activate the sensor via ENABLE register
    if (this->writeRegister(TCS34_ADDR, static_cast<uint8_t>(TCS34::REGISTER::ENABLE), 0x03) != HAL_OK) {
         printf("Failed to initialize\n");
        return 1;  // Return an error code
    }

    printf("Successfully initialized\n");

    // Read ID from the sensor to confirm the sensor type
    if (this->readRegister(TCS34_ADDR, cmd | static_cast<uint8_t>(TCS34::REGISTER::ID), &id_buffer) != HAL_OK) {
        printf("Failed to read ID\n");
        return 1;
    }

    if (id_buffer == 0x44 || id_buffer == 0x4D) {
        printf("TCS34 detected\n");
    } else {
        printf("TCS34 not detected\n");
        return 1;
    }




    return 0;  // Success
}



double TCS34::convertToPercent()
{
	 uint8_t brightness_buffer_low = 0;
	    uint8_t brightness_buffer_high = 0;
	    uint8_t cmd = 0x80;  // CMD bit must be set to 1 (0x80)
	  // Read brightness values
	    if (this->readRegister(TCS34_ADDR, cmd | static_cast<uint8_t>(TCS34::REGISTER::CDATA), &brightness_buffer_low) != HAL_OK ||
	        this->readRegister(TCS34_ADDR, cmd | static_cast<uint8_t>(TCS34::REGISTER::CDATAH), &brightness_buffer_high) != HAL_OK) {
	        printf("Failed to read brightness data\n");
	        return 1;
	    }

	    // Combine low and high brightness bytes
	    brightness_value = (brightness_buffer_high << 8) | brightness_buffer_low;
	    double brightness = (static_cast<double>(brightness_value)/65535)*100;


	    	return brightness;
}

double brightness{0};
// Entry function as per C++ linkage specifications
extern "C"{

TCS34 sensor2(&hi2c6);
void tcs_init()
{
	sensor2.init();
}


void entry_TC34()
{



    brightness_value=sensor2.convertToPercent();


}

}
