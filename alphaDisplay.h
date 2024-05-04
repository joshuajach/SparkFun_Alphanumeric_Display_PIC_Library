/* 
 * File:   alphaDisplay.h
 * Author: josh
 *
 * Created on May 2, 2024, 7:20 PM
 */
/******************************************************************************
 * Credits to:
 * SparkFun_Alphanumeric_Display.h
SparkFun Alphanumeric Display Library Header File
Priyanka Makin @ SparkFun Electronics
Original Creation Date: July 25, 2019
https://github.com/sparkfun/SparkFun_Alphanumeric_Display_Arduino_Library

Updated April 30, 2020 by Gaston Williams to add defineChar function

Pickup a board here: https://sparkle.sparkfun.com/sparkle/storefront_products/16391

This file prototypes the HT16K33 class, implemented in SparkFun_Alphanumeric_Display.cpp.

Development environment specifics:
	IDE: Arduino 1.8.9
	Hardware Platform: Arduino Uno
	Alphanumeric Display Breakout Version: 1.0.0

This code is beerware; if you see me (or any other SparkFun employee) at the
local, and you've found our code helpful, please buy us a round!

Distributed as-is; no warranty is given.
******************************************************************************/

#ifndef ALPHADISPLAY_H
#define	ALPHADISPLAY_H
#include "mcc_generated_files/system/system.h"
//#include "/mcc_generated_files/timer/delay.h"

#define DEFAULT_ADDRESS 0x70 
// Define constants for segment bits
#define SEG_A 0x0001
#define SEG_B 0x0002
#define SEG_C 0x0004
#define SEG_D 0x0008
#define SEG_E 0x0010
#define SEG_F 0x0020
#define SEG_G 0x0040
#define SEG_H 0x0080
#define SEG_I 0x0100
#define SEG_J 0x0200
#define SEG_K 0x0400
#define SEG_L 0x0800
#define SEG_M 0x1000
#define SEG_N 0x2000

typedef enum {
    ALPHA_BLINK_RATE_NOBLINK = 0b00,
    ALPHA_BLINK_RATE_2HZ = 0b01,
    ALPHA_BLINK_RATE_1HZ = 0b10,
    ALPHA_BLINK_RATE_0_5HZ = 0b11,
} alpha_blink_rate_t;

typedef enum {
    ALPHA_DISPLAY_ON = 0b1,
    ALPHA_DISPLAY_OFF = 0b0,
} alpha_display_t;

typedef enum {
    ALPHA_DECIMAL_ON = 0b1,
    ALPHA_DECIMAL_OFF = 0b0,
} alpha_decimal_t;

typedef enum {
    ALPHA_COLON_ON = 0b1,
    ALPHA_COLON_OFF = 0b0,
} alpha_colon_t;

typedef enum {
    ALPHA_CMD_SYSTEM_SETUP = 0b00100000,
    ALPHA_CMD_DISPLAY_SETUP = 0b10000000,
    ALPHA_CMD_DIMMING_SETUP = 0b11100000,
} alpha_command_t;

// Structure for defining new character displays

struct CharDef {
    uint8_t position;
    uint16_t segments; //this use to be int16_t
    struct CharDef * next;
};

bool Alpha_Begin(void);
size_t Alpha_Write(const char *, size_t);


#endif	/* ALPHADISPLAY_H */

