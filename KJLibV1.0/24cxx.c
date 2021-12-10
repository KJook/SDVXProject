#include "24cxx.h"

uint8_t savaData(I2C_HandleTypeDef *hi2c1, uint8_t *data, uint8_t lenth)
{
    uint16_t i;
    for (i = 0; i < lenth; i++)
    {
        if (!HAL_I2C_Mem_Write(hi2c1, ADDR_24LCxx_Write, i, I2C_MEMADD_SIZE_8BIT, &data[i], 1, 0xff))
        {
            return 0;
        }
        HAL_Delay(5);
    }
    return 1;
}

uint8_t readData(I2C_HandleTypeDef *hi2c1, uint8_t *ReadBuffer, uint8_t BufferSize)
{
    if (HAL_I2C_Mem_Read(hi2c1, ADDR_24LCxx_Read, 0, I2C_MEMADD_SIZE_8BIT, ReadBuffer, BufferSize, 0xff))
    {
        return 1;
    }
    return 0;
}
