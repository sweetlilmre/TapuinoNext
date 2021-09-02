#pragma once
#define ROTARY_ENCODER_A_PIN 25
#define ROTARY_ENCODER_B_PIN 33
#define ROTARY_ENCODER_BUTTON_PIN 32
#define C64_SENSE_PIN 16
#define C64_MOTOR_PIN 17
#define C64_READ_PIN 5
#define C64_WRITE_PIN 18

#define I2C_DISP_ROWS 2
#define I2C_DISP_COLS 16

#define I2C_DISP_TYPE_HD44780 0
#define I2C_DISP_TYPE_SSD1306 1

#define LCD_SSD1306_128x64

#include "config-user.h"

#ifndef I2C_DISP_ADDR
#error Please define "I2C_DISP_ADDR" in config-user.h. Create this file if it does not exist!
#endif

#ifndef I2C_DISP_TYPE
#error Please define "I2C_DISP_TYPE" as "I2C_DISP_TYPE_HD44780" or "I2C_DISP_TYPE_SSD1306" in config-user.h. Create this file if it does not exist!
#endif
