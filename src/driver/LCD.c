/**
 *  @file       LCD.c
 *  @author     Mark Sherman
 * 
 *  @brief      LCD function implementation
*/

#include "LCD.h"

/**
 *  LCD Pinout
 *      RS - GPIO7
 *          Register Select - used to write to the Data or Instruction register
 *          Hi = DR, PI_LOW = IR
 *      E - GPIO8
 *          Enable - alPI_LOWs writing data to registers
 *      D4 - GPIO25
 *          Data Line 4
 *      D5 - GPIO24
 *          Data Line 5
 *      D6 - GPIO23
 *          Data Line 6
 *      D7 - GPIO18
 *          Data Line 7
*/

/**
 * when and how should we init the pigpio library on startup?
 * should we kill the pigpio pid before loading LCD driver?
 *  try that, init gpio in LCD init then maybe
 * 
 *  maybe we dont need to use the gpioInitialize to start?
 *      could avoid having to kill pigpio pid on start
 *      try removing gpioInitialize and add setup individual pins in LCD_init
 * 
*/

/**
 *  LCD Operation
 *      2 8-bit registers, IR and DR
*/
void LCD_init()
{
    gpioSetMode(RS,     PI_OUTPUT);
    gpioSetMode(E,      PI_OUTPUT);
    gpioSetMode(D4,     PI_OUTPUT);
    gpioSetMode(D5,     PI_OUTPUT);
    gpioSetMode(D6,     PI_OUTPUT);
    gpioSetMode(D7,     PI_OUTPUT);

    gpioDelay(POWERUP_DELAY_MS);
/* set 8-bit mode 3 times on start see data sheet init sequence for detail*/
    LCD_write(0x03, CMD);   
    gpioDelay(FUNCSET_DELAY_MS);
    LCD_write(0x03, CMD);
    gpioDelay(FUNCSET_DELAY_uS);   
    LCD_write(0x03, CMD);
    gpioDelay(CMD_DELAY_uS);
/* set 4-bit mode now */
    LCD_write(0x02, CMD);
    gpioDelay(CMD_DELAY_uS); 
/* Display Off */  
    LCD_write(0x0C, CMD);
    gpioDelay(CMD_DELAY_uS);
/* Display Clear */   
    LCD_write(0x01, CMD);
    gpioDelay(CMD_DELAY_uS);
/* entry mode set */   
    LCD_write(0x06, CMD);
    gpioDelay(CMD_DELAY_uS);
/* 2 rows */   
    LCD_write(0x28, CMD);
    gpioDelay(CMD_DELAY_uS);
}

void LCD_toggle_enable()
{
    gpioDelay(CMD_DELAY_uS);
    gpioWrite(E, PI_LOW);
    gpioDelay(CMD_DELAY_uS);
    gpioWrite(E, PI_HIGH);
    gpioDelay(CMD_DELAY_uS);
}

void LCD_write(uint16_t data, bool mode)
{
/* access IR or DR based on mode */
    gpioWrite(RS, mode);

/* ensure data bus is all 0 to start */
    gpioWrite(D4, PI_LOW);
    gpioWrite(D5, PI_LOW);
    gpioWrite(D6, PI_LOW);
    gpioWrite(D7, PI_LOW);

/* write PI_HIGH bits */
    if((data & 0x10) == 0x10)
        gpioWrite(D4, PI_HIGH);
    if((data & 0x20) == 0x20)
        gpioWrite(D5, PI_HIGH);
    if((data & 0x40) == 0x40)
        gpioWrite(D6, PI_HIGH);
    if((data & 0x80) == 0x80)
        gpioWrite(D7, PI_HIGH);

/* toggle enable to write first 4 bits */
    LCD_toggle_enable();

/* ensure data bus is all 0 to start */
    gpioWrite(D4, PI_LOW);
    gpioWrite(D5, PI_LOW);
    gpioWrite(D6, PI_LOW);
    gpioWrite(D7, PI_LOW);

/* write PI_LOW bits */
    if((data & 0x01) == 0x01)
        gpioWrite(D4, PI_HIGH);
    if((data & 0x02) == 0x02)
        gpioWrite(D5, PI_HIGH);
    if((data & 0x04) == 0x04)
        gpioWrite(D6, PI_HIGH);
    if((data & 0x08) == 0x08)
        gpioWrite(D7, PI_HIGH);

/* toggle enable to write last 4 bits */
    LCD_toggle_enable();
}

void LCD_print(char msg[MAX_MSG_SIZE])
{
    uint8_t i = 0;
    
    printf("\nPrinting %s to LCD...\n\r", msg);
    while(msg[i] != '\0')
        LCD_write(msg[i++], CHAR);
}