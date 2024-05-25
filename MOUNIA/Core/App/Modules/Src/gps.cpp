/*
 * gps.cpp
 *
 *  Created on: May 20, 2024
 *      Author: talib
 */

#include "../Inc/gps.hpp"

GPS *GPS::instance=nullptr;

GPS::GPS(UART_HandleTypeDef *huart):huart_{huart}
{
	 instance = this;

	    HAL_UART_Receive_DMA(huart_, storage_buffer.data(), storage_buffer.size());
}


void GPS::get_gps_data()
{
	HAL_UART_Receive_DMA(this->huart_, this->storage_buffer.data(),this->storage_buffer.size());

}

void GPS::receive()
{
	std::fill(storage_buffer.begin(), storage_buffer.end(), 0);  // Using std::fill to clear std::array

	HAL_UART_Receive_DMA(this->huart_, this->storage_buffer.data(),this->storage_buffer.size());



}


//void GPS::process_data()
//{
//	 std::string receivedData(reinterpret_cast<char*>(storage_buffer.data()));
//
//	    // Process and extract full sentences
//	    std::size_t startPos = receivedData.find("$GPGGA,");
//	    if (startPos != std::string::npos) {
//	        std::size_t endPos = receivedData.find("\r\n", startPos);
//	        if (endPos != std::string::npos) {
//	            std::string gpggaSentence = receivedData.substr(startPos, endPos - startPos);
//	            std::cout << gpggaSentence << std::endl;
////	            parseGPGGA(gpggaSentence);
//	        }
//	    }
//}



//uint8_t hours,minutes,seconds{0};
//
//float latitude,longitude,altitude;

void GPS::extract_data(const std::string& gppa_sentence, uint8_t& hours, uint8_t& minutes, uint8_t& seconds, float& latitude, float& longitude, float& altitude){
   std::vector<std::string> fields;
   std::stringstream ss(gppa_sentence);
   std::string field;

   while (std::getline(ss,field,','))
   {
	  fields.push_back(field);
   }

   std::string time_str=fields[1];
   hours=std::stoi(time_str.substr(0,2));
   minutes=std::stoi(time_str.substr(2,2));
   seconds=std::stoi(time_str.substr(4,2));
   latitude=std::stof(fields[2]);
   longitude=std::stof(fields[4]);
   altitude=std::stof(fields[9]);


}


void GPS::process_data()
{
	std::string received_data (reinterpret_cast<char *>(this->storage_buffer.data()));

	//Process and extract full sentence
	std::size_t startPos=received_data.find("$GPGGA,");
	if(startPos != std::string::npos)
	{
		std::size_t endPos=received_data.find("\r\n",startPos);
		if (endPos !=std::string::npos)
		{
			std::string gppa_sentence=received_data.substr(startPos,endPos-startPos);
			this->extract_data(gppa_sentence,hours,minutes,seconds,latitude,longitude,altitude);

		}
	}
}



GPS gps1{&huart3};

#ifdef __cplusplus

extern "C"
{
#endif

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                if (huart->Instance==USART3 && GPS::instance)
    {
            GPS::instance->process_data();
    		GPS::instance->receive();
    }
}


void GPS_entry()
{
gps1.get_gps_data();
HAL_Delay(1000);
}

#ifdef __cplusplus
}
#endif










