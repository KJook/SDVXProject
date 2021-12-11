#include "sdvx_hid.h"
#include "gpio.h"
#include "24cxx.h"
#include "i2c.h"
#include "usb_device.h"
#include "tim.h"

#ifdef __DEBUGMODE__
#include <stdio.h>

//KeyboardData[0] 报告id
//KeyboardData[1] 特殊按键
//KeyboardData[2] 保留
//KeyboardData[3-11] 9无冲键

void printKeyboardData(uint8_t *KeyboardData)
{
    uint8_t i;
    printf("打印键盘Report状态\r\n");
    for (i = 0; i < 12; i++)
    {
        printf("0x%02x  ", KeyboardData[i]);
    }
    printf("\r\n");
}
#endif

uint8_t KeyboardCustomData[9] = {4, 5, 6, 7, 8, 9, 10, 11, 12};
//存放自定义按键
uint8_t EncoderMouseStep = 5;
//存放鼠标移动步长
uint8_t readBuffer[128];
//储存器读取缓存
extern USBD_HandleTypeDef hUsbDeviceFS;
//usb处理设备
uint8_t KeyboardData[12] = {1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//主要的键盘报文数据
uint8_t KMouseData[5] = {2, 0, 0, 0, 0};
//主要的鼠标报文数据

uint32_t timeTick = 0;
//计数变量，用于进入休眠模式
uint32_t timeMax = 500;
//最大时间，超过该时间进入休眠

//键盘数据初始化，用来读取储存器中的用户自定义数据
void initSDVX(void)
{
    uint8_t i;
    readData(&hi2c1, readBuffer, 128);
    //前9位[0:8]存放用户自定义按键
    for (i = 0; i < 9; i++)
    {
        KeyboardCustomData[i] = readBuffer[i];
    }

    //第10位[9]存放鼠标移动步长
    EncoderMouseStep = readBuffer[9];
}

//主要的键盘监听函数
void listenSDVXData()
{
    // k1 下拉中断
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0))
    {
        KeyboardData[3] = KeyboardCustomData[0];
        timeTick = 0;
    }
    else
    {
        KeyboardData[3] = 0x00;
    }
    //k2 上拉
    if (!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11))
    {

        timeTick = 0;
        KeyboardData[4] = KeyboardCustomData[1];
    }
    else
    {
        KeyboardData[4] = 0x00;
    }
    //k3
    if (!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10))
    {
        timeTick = 0;
        KeyboardData[5] = KeyboardCustomData[2];
    }
    else
    {
        KeyboardData[5] = 0x00;
    }
    //k4
    if (!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1))
    {
        timeTick = 0;
        KeyboardData[6] = KeyboardCustomData[3];
    }
    else
    {
        KeyboardData[6] = 0x00;
    }
    //k5
    if (!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15))
    {
        timeTick = 0;
        KeyboardData[7] = KeyboardCustomData[4];
    }
    else
    {
        KeyboardData[7] = 0x00;
    }
    //k6
    if (!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3))
    {
        timeTick = 0;
        KeyboardData[8] = KeyboardCustomData[5];
    }
    else
    {
        KeyboardData[8] = 0x00;
    }
    //k7
    if (!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4))
    {
        timeTick = 0;
        KeyboardData[9] = KeyboardCustomData[6];
    }
    else
    {
        KeyboardData[9] = 0x00;
    }
    //k8
    if (!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5))
    {
        timeTick = 0;
        KeyboardData[10] = KeyboardCustomData[7];
    }
    else
    {
        KeyboardData[10] = 0x00;
    }
    //k9
    if (!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3))
    {
        //HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        timeTick = 0;
        KeyboardData[11] = KeyboardCustomData[8];
    }
    else
    {
        KeyboardData[11] = 0x00;
    }
    if (!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15))
    {
        timeTick = 0;
        KMouseData[1] = 1;
    }
    else
    {
        KMouseData[1] = 0;
    }
    if (!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12))
    {
        timeTick = 0;
        KMouseData[1] = 2;
    }
    else
    {
        KMouseData[1] = 0;
    }
}

//进入休眠模式函数
void goToSleep(void)
{

    HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);
    /* 清除所有WAKEUP标志 */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

    /* 使能PA0 WAKEUP标志 */
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);

    /* 进入待机模式 */
    HAL_PWR_EnterSTANDBYMode();
}

//定时器中断函数
/*
*tim3每间隔1ms触发一次，用于回报数据，回报率1000hz
*tim4每间隔1s触发一次，用于进入休眠
*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim3)
    {
        //回报键盘和鼠标的数据
        USBD_HID_SendReport(&hUsbDeviceFS, &KeyboardData, sizeof(KeyboardData));
        USBD_HID_SendReport(&hUsbDeviceFS, &KMouseData, sizeof(KMouseData));
    }
    if (htim == &htim4)
    {
        //睡眠模式计数
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    }
}


//外部中断处理函数
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    //编码器中断
    if (GPIO_Pin == GPIO_PIN_14)
    {
        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13))
        {
            KMouseData[MoveX] = EncoderMouseStep;
        }
        else
        {
            KMouseData[MoveX] = -EncoderMouseStep;
        }
        USBD_HID_SendReport(&hUsbDeviceFS, &KMouseData, sizeof(KMouseData));
    }
    else if (GPIO_Pin == GPIO_PIN_10)
    {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9))
        {
            KMouseData[MoveY] = EncoderMouseStep;
        }
        else
        {
            KMouseData[MoveY] = -EncoderMouseStep;
        }
        USBD_HID_SendReport(&hUsbDeviceFS, &KMouseData, sizeof(KMouseData));
    }
}
