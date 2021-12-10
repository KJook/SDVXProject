#ifndef __24CXX_H_
#define __24CXX_H_
#include "stm32f1xx_hal.h"

#define ADDR_24LCxx_Write 0xA0
#define ADDR_24LCxx_Read 0xA1

uint8_t savaData(I2C_HandleTypeDef *hi2c1, uint8_t *data, uint8_t lenth);
uint8_t readData(I2C_HandleTypeDef *hi2c1, uint8_t *ReadBuffer, uint8_t BufferSize);

#endif
