/******************************************************************************
SparkFun_Alphanumeric_Display.h
SparkFun Alphanumeric Display Library Header File
Madison Chodikov @ SparkFun Electronics
Original Creation Date: July 25, 2019
****ENTER PRODUCT PAGE URL HERE****

This file prototypes the HT16K33 class, implemented in SparkFun_Alphanumeric_Display.cpp.

Development environment specifics:
	IDE: Arduino 1.8.9
	Hardware Platform: Arduino Uno
	Alphanumeric Display Breakout Version: 1.0.0

This code is beerware; if you see me (or any other SparkFun employee) at the
local, and you've found our code helpful, please buy us a round!

Distributed as-is; no warranty is given.
******************************************************************************/
#ifndef __SparkFun_Alphanumeric_Display_H__
#define __SparkFun_Alphanumeric_Display_H__

#include <Wire.h>
#include <Arduino.h>
#include <Stream.h>

#define DEFAULT_ADDRESS 0x70 //Default I2C address when A0, A1 are floating
#define DEV_ID 0x12          //Device ID that I just made up
#define DEFAULT_NOTHING_ATTACHED 0xFF

typedef enum
{
    ALPHA_BLINK_RATE_NOBLINK = 0b00,
    ALPHA_BLINK_RATE_2HZ = 0b01,
    ALPHA_BLINK_RATE_1HZ = 0b10,
    ALPHA_BLINK_RATE_0_5HZ = 0b11,
};

typedef enum
{
    ALPHA_DISPLAY_ON = 0b1,
    ALPHA_DISPLAY_OFF = 0b0,
};

typedef enum
{
    ALPHA_CMD_SYSTEM_SETUP = 0b00100000,
    ALPHA_CMD_DISPLAY_SETUP = 0b10000000,
    ALPHA_CMD_DIMMING_SETUP = 0b11100000,
};

// class HT16K33
class HT16K33 : public Print
{
private:
    TwoWire *_i2cPort;          //The generic connection to user's chosen I2C hardware
    uint8_t _deviceAddressLeft; // Address of primary alphanumeric display
    uint8_t _deviceAddressLeftCenter;
    uint8_t _deviceAddressRightCenter;
    uint8_t _deviceAddressRight;
    uint8_t digitPosition = 0;
    uint8_t sizeOfDisplay = 4;
    bool displayOnOff = 0;                        //Tracks display on/off bit of display setup register
    uint8_t blinkRate = ALPHA_BLINK_RATE_NOBLINK; //Tracks blink bits in display setup register

    //Enough RAM for up to 4 displays on same I2C bus
    uint8_t displayRAM[16 * 4];

public:
    //Device status
    bool begin(uint8_t addressLeft = DEFAULT_ADDRESS,
               uint8_t addressLeftCenter = DEFAULT_NOTHING_ATTACHED,
               uint8_t addressRightCenter = DEFAULT_NOTHING_ATTACHED,
               uint8_t addressRight = DEFAULT_NOTHING_ATTACHED,
               TwoWire &wirePort = Wire); // Sets the address of the device and opens the Wire port for communication
    bool isConnected(uint8_t displayNumber);
    bool initialize();
    bool checkDeviceID(uint8_t displayNumber);
    uint8_t lookUpDisplayAddress(uint8_t displayNumber);

    //Display configuration functions
    bool clearDisplay();
    bool setBrightness(uint8_t duty);
    bool setBrightnessSingle(uint8_t displayNumber, uint8_t duty);
    // uint8_t getBrightness();
    bool setBlinkRate(float rate);
    bool setBlinkRateSingle(uint8_t displayNumber, float rate);
    // uint8_t getBlinkRate();
    bool displayOn();
    bool displayOff();
    bool displayOnSingle(uint8_t displayNumber);
    bool displayOffSingle(uint8_t displayNumber);
    bool setDisplayOnOff(uint8_t displayNumber, bool turnOnDisplay);

    bool enableSystemClock();
    bool disableSystemClock();
    bool enableSystemClockSingle(uint8_t displayNumber);
    bool disableSystemClockSingle(uint8_t displayNumber);

    //Light up functions
    void illuminateSegment(uint8_t segment, uint8_t digit);
    void illuminateChar(uint16_t disp, uint8_t digit);
    void printChar(uint8_t displayChar, uint8_t digit);
    bool updateDisplay();

    // For print
    virtual size_t write(uint8_t);
    virtual size_t write(const uint8_t *buffer, size_t size);
    virtual size_t write(const char *str);

    //I2C abstraction
    bool readRAM(uint8_t address, uint8_t reg, uint8_t *buff, uint8_t buffSize);
    // bool read(uint8_t reg, uint8_t data);
    bool writeRAM(uint8_t address, uint8_t reg, uint8_t *buff, uint8_t buffSize);
    bool writeOne(uint8_t address, uint8_t reg);
    // bool writeRAM(uint8_t reg, uint8_t data);
};

#endif