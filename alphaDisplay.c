/******************************************************************************
 * Credits to:
 * SparkFun_Alphanumeric_Display.cpp
SparkFun Alphanumeric Display Library Source File
Priyanka Makin @ SparkFun Electronics
Original Creation Date: February 25, 2020
https://github.com/sparkfun/SparkFun_Alphanumeric_Display_Arduino_Library

Updated May 2, 2020 by Gaston Williams to add defineChar function

Pickup a board here: https://sparkle.sparkfun.com/sparkle/storefront_products/16391

This file implements all functions of the HT16K33 class. Functions here range
from printing to one or more Alphanumeric Displays, changing the display settings, and writing/
reading the RAM of the HT16K33.

The Holtek HT16K33 seems to be susceptible to address changes intra-sketch. The ADR pins
are muxed with the ROW and COM drivers so as semgents are turned on/off that affect
the ADR1/ADR0 pins the address has been seen to change. The best way around this is
to do a isConnected check before updateRAM() is sent to the driver IC.

Development environment specifics:
	IDE: Arduino 1.8.9
	Hardware Platform: Arduino Uno
	Alphanumeric Display Breakout Version: 1.0.0

This code is beerware; if you see me (or any other SparkFun employee) at the
local, and you've found our code helpful, please buy us a round!

Distributed as-is; no warranty is given.
******************************************************************************/

#include "alphaDisplay.h"
#include "mcc_generated_files/system/system.h"
#include "mcc_generated_files/timer/delay.h"
#include <string.h>

bool displayOnOff = 0;
uint8_t blinkRate = ALPHA_BLINK_RATE_NOBLINK; // Tracks blink bits in display setup register
uint8_t displayRAM[16];
uint8_t digitPosition = 0;
char displayContent[5];
bool decimalOnOff = 0;
bool colonOnOff = 0;
// Linked List of character definitions
//struct CharDef * pCharDefList = NULL;

#define SFE_ALPHANUM_UNKNOWN_CHAR 95
static const uint16_t alphanumeric_segs[] = {
    // nmlkjihgfedcba
    0b00000000000000, // ' ' (space)
    0b00001000001000, // '!'
    0b00001000000010, // '"'
    0b01001101001110, // '#'
    0b01001101101101, // '$'
    0b10010000100100, // '%'
    0b00110011011001, // '&'
    0b00001000000000, // '''
    0b00000000111001, // '('
    0b00000000001111, // ')'
    0b11111010000000, // '*'
    0b01001101000000, // '+'
    0b10000000000000, // ','
    0b00000101000000, // '-'
    0b00000000000000, // '.'
    0b10010000000000, // '/'
    0b00000000111111, // '0'
    0b00010000000110, // '1'
    0b00000101011011, // '2'
    0b00000101001111, // '3'
    0b00000101100110, // '4'
    0b00000101101101, // '5'
    0b00000101111101, // '6'
    0b01010000000001, // '7'
    0b00000101111111, // '8'
    0b00000101100111, // '9'
    0b00000000000000, // ':'
    0b10001000000000, // ';'
    0b00110000000000, // '<'
    0b00000101001000, // '='
    0b01000010000000, // '>'
    0b01000100000011, // '?'
    0b00001100111011, // '@'
    0b00000101110111, // 'A'
    0b01001100001111, // 'B'
    0b00000000111001, // 'C'
    0b01001000001111, // 'D'
    0b00000101111001, // 'E'
    0b00000101110001, // 'F'
    0b00000100111101, // 'G'
    0b00000101110110, // 'H'
    0b01001000001001, // 'I'
    0b00000000011110, // 'J'
    0b00110001110000, // 'K'
    0b00000000111000, // 'L'
    0b00010010110110, // 'M'
    0b00100010110110, // 'N'
    0b00000000111111, // 'O'
    0b00000101110011, // 'P'
    0b00100000111111, // 'Q'
    0b00100101110011, // 'R'
    0b00000110001101, // 'S'
    0b01001000000001, // 'T'
    0b00000000111110, // 'U'
    0b10010000110000, // 'V'
    0b10100000110110, // 'W'
    0b10110010000000, // 'X'
    0b01010010000000, // 'Y'
    0b10010000001001, // 'Z'
    0b00000000111001, // '['
    0b00100010000000, // '\'
    0b00000000001111, // ']'
    0b10100000000000, // '^'
    0b00000000001000, // '_'
    0b00000010000000, // '`'
    0b00000101011111, // 'a'
    0b00100001111000, // 'b'
    0b00000101011000, // 'c'
    0b10000100001110, // 'd'
    0b00000001111001, // 'e'
    0b00000001110001, // 'f'
    0b00000110001111, // 'g'
    0b00000101110100, // 'h'
    0b01000000000000, // 'i'
    0b00000000001110, // 'j'
    0b01111000000000, // 'k'
    0b01001000000000, // 'l'
    0b01000101010100, // 'm'
    0b00100001010000, // 'n'
    0b00000101011100, // 'o'
    0b00010001110001, // 'p'
    0b00100101100011, // 'q'
    0b00000001010000, // 'r'
    0b00000110001101, // 's'
    0b00000001111000, // 't'
    0b00000000011100, // 'u'
    0b10000000010000, // 'v'
    0b10100000010100, // 'w'
    0b10110010000000, // 'x'
    0b00001100001110, // 'y'
    0b10010000001001, // 'z'
    0b10000011001001, // '{'
    0b01001000000000, // '|'
    0b00110100001001, // '}'
    0b00000101010010, // '~'
    0b11111111111111, // Unknown character (DEL or RUBOUT)
};

bool enableSystemClock() {
    uint8_t data = ALPHA_CMD_SYSTEM_SETUP | 1;
    bool status = I2C1_Host_Write(DEFAULT_ADDRESS, &data, 1);
    DELAY_milliseconds(10); // Allow display to start
    return (status);
}

bool setBrightness(uint8_t level) {
    level = (level <= 15) ? level : 1;
    uint8_t data = ALPHA_CMD_DIMMING_SETUP | level;
    return I2C1_Host_Write(DEFAULT_ADDRESS, &data, 1);
}

bool setBlinkRate(float rate) {
    if (rate == 2.0) {
        blinkRate = ALPHA_BLINK_RATE_2HZ;
    } else if (rate == 1.0) {
        blinkRate = ALPHA_BLINK_RATE_1HZ;
    } else if (rate == 0.5) {
        blinkRate = ALPHA_BLINK_RATE_0_5HZ;
    }//default to no blink
    else {
        blinkRate = ALPHA_BLINK_RATE_NOBLINK;
    }

    uint8_t data = ALPHA_CMD_DISPLAY_SETUP | (uint8_t) (blinkRate << 1);
    return I2C1_Host_Write(DEFAULT_ADDRESS, &data, 1);
}

bool setDisplayOnOff(bool turnOnDisplay) {
    if (turnOnDisplay) {
        displayOnOff = ALPHA_DISPLAY_ON;
    } else {
        displayOnOff = ALPHA_DISPLAY_OFF;
    }
    uint8_t dataToWrite = ALPHA_CMD_DISPLAY_SETUP | (uint8_t) (blinkRate << 1) | displayOnOff;
    return I2C1_Host_Write(DEFAULT_ADDRESS, &dataToWrite, 1);
}

bool initialize() {
    // Turn on system clock of all displays
    if (enableSystemClock() == false) {
        return false;
    }

    // Set brightness of all displays to full brightness
    if (setBrightness(15) == false) {
        return false;
    }

    // Turn blinking off for all displays
    if (setBlinkRate(ALPHA_BLINK_RATE_NOBLINK) == false) {
        return false;
    }

    // Turn on all displays
    if (setDisplayOnOff(true) == false) {
        return false;
    }

    return true;
}

bool isConnected() {
    uint8_t triesBeforeGiveup = 5;

    for (uint8_t x = 0; x < triesBeforeGiveup; x++) {
        //implement this if their are issues connecting with the display occasionally
        //_i2cPort->beginTransmission(lookUpDisplayAddress(displayNumber));
        //if (_i2cPort->endTransmission() == 0)
        //{
        return true;
        //}
        //delay(100);
    }
    return false;
}

void shiftArray(uint8_t *original, uint8_t *shifted) {
    shifted[0] = 0; // Or any other desired value for the first element
    memcpy(shifted + 1, original, 16 * sizeof (uint8_t));
}

bool updateDisplay() {
    uint8_t msg[17];
    shiftArray(displayRAM, msg);
    return I2C1_Host_Write(DEFAULT_ADDRESS, msg, 16);
}

bool clear() {
    for (uint8_t i = 0; i < 16; i++)
        displayRAM[i] = 0;
    digitPosition = 0;

    return (updateDisplay());
}

bool Alpha_Begin(void) {

    if (isConnected() == false) {
        return false;
    }
    DELAY_milliseconds(100);

    if (initialize() == false) {
        return false;
    }

    if (clear() == false) // Clear all displays
    {
        return false;
    }
    DELAY_milliseconds(100);
    displayContent[4] = '\0'; // Terminate the array because we are doing direct prints

    return true;
}
// Set or clear the decimal on/off bit

bool setDecimalOnOff(bool turnOnDecimal, bool updateNow) {
    uint8_t adr = 0x03;
    uint8_t dat;

    if (turnOnDecimal == true) {
        decimalOnOff = ALPHA_DECIMAL_ON;
        dat = 0x01;
    } else {
        decimalOnOff = ALPHA_DECIMAL_OFF;
        dat = 0x00;
    }

    displayRAM[adr] &= 0xFE;
    displayRAM[adr] |= dat;

    if (updateNow) {
        return updateDisplay();
    } else {
        return true;
    }
}
// Set or clear the colon on/off bit

bool setColonOnOff(bool turnOnColon, bool updateNow) {
    uint8_t adr = 0x01;
    uint8_t dat;

    if (turnOnColon == true) {
        colonOnOff = ALPHA_COLON_ON;
        dat = 0x01;
    } else {
        colonOnOff = ALPHA_COLON_OFF;
        dat = 0x00;
    }

    displayRAM[adr] &= 0xFE;
    displayRAM[adr] |= dat;

    if (updateNow) {
        return updateDisplay();
    } else {
        return true;
    }
}

uint16_t getSegmentsToTurnOn(uint16_t charPos) {
    uint16_t segments = 0;

    segments = alphanumeric_segs[charPos];

    return segments;
}
// Given a segment and a digit, set the matching bit within the RAM of the Holtek RAM set

void illuminateSegment(uint8_t segment, uint8_t digit) {
    uint8_t com;
    uint8_t row;

    com = segment - 'A'; // Convert the segment letter back to a number

    if (com > 6)
        com -= 7;
    if (segment == 'I')
        com = 0;
    if (segment == 'H')
        com = 1;

    row = digit % 4; // Convert digit (1 to 16) back to a relative position on a given digit on display
    if (segment > 'G')
        row += 4;

    //uint8_t offset = digit / 4 * 16;
    uint8_t adr = com * 2; // + offset;

    // Determine the address
    if (row > 7)
        adr++;

    // Determine the data bit
    if (row > 7)
        row -= 8;
    uint8_t dat = 0b1 << row;

    displayRAM[adr] = displayRAM[adr] | dat;
}
// Given a binary set of segments and a digit, store this data into the RAM array

void illuminateChar(uint16_t segmentsToTurnOn, uint8_t digit) {
    for (uint8_t i = 0; i < 14; i++) // There are 14 segments on this display
    {
        if ((segmentsToTurnOn >> i) & 0b1)
            illuminateSegment('A' + i, digit); // Convert the segment number to a letter
    }
}
// Print a character, for a given digit, on display

void printChar(uint8_t displayChar, uint8_t digit) {
    // moved alphanumeric_segs array to PROGMEM Josh????
    uint16_t characterPosition = 65532;

    // space
    if (displayChar == ' ' || displayChar == 0)
        characterPosition = 0;
        // Printable Symbols -- Between first character ! and last character ~
    else if (displayChar >= '!' && displayChar <= '~') {
        characterPosition = displayChar - '!' + 1;
    }

    // Take care of special characters by turning correct segment on
    if (characterPosition == 14) // '.'
        setDecimalOnOff(true, false);
    if (characterPosition == 26) // ':'
        setColonOnOff(true, false);
    if (characterPosition == 65532) // unknown character
        characterPosition = SFE_ALPHANUM_UNKNOWN_CHAR;

    uint16_t segmentsToTurnOn = getSegmentsToTurnOn(characterPosition);

    illuminateChar(segmentsToTurnOn, digit);
}

/*
 * Write a character buffer to the display.
 * Required for overloading the Print function.
 */
size_t Alpha_Write(const char *buffer, size_t size) {
    char buff;
    size = size > 4 ? 4 : size;

    // Clear the displayRAM array
    for (uint8_t i = 0; i < 16; i++)
        displayRAM[i] = 0;

    digitPosition = 0;
    size_t stringIndex = 0;

    while (stringIndex < size && digitPosition < (4)) {
        buff = buffer[stringIndex];
        // For special characters like '.' or ':', do not increment the digitPosition
        if (buff == '.')
            printChar('.', 0);
        else if (buff == ':')
            printChar(':', 0);
        else {
            printChar(buff, digitPosition);
            displayContent[digitPosition] = buff; // Record to internal array

            digitPosition++;
        }
        stringIndex++;
    }
    updateDisplay(); // Send RAM buffer over I2C bus
    return stringIndex;
}