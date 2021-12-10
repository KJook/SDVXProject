/**
  ******************************************************************************
  * @file    sdvx_hid.h
  * @author  KJ
  * @brief   Header file for the sdvx_hid.c file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 KJ.
  * All rights reserved.</center></h2>
  * 
  *
  ******************************************************************************
  */
#ifndef SDVX_HID_H
#define SDVX_HID_H
#include "stm32f1xx_hal.h"
#define MoveX 2
#define MoveY 3
#define MouseKey 1

//void printKeyboardData(uint8_t * KeyboardData);
void goToSleep(void);
void refreshKeyBoardData(void);


#endif
