/**
 *  @file       LCD.c
 *  @author     Mark Sherman
 * 
 *  @brief      LCD function implementation
 *  @date       4/1/23
 *  @version    1.0
*/

#include "LCD.h"

/**
 *  LCD Pinout
 *      RS - GPIO7
 *          Register Select - used to write to the Data or Instruction register
 *          Hi = DR, Low = IR
 *      E - GPIO8
 *          Enable - allows writing data to registers
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
 *  LCD Operation
 *      2 8-bit registers, IR and DR
*/
void LCD_init()
{
    delay(POWERUP_DELAY_MS);
/* set 8-bit mode 3 times on start see data sheet init sequence for detail*/
    LCD_write(0x03, CMD);   
    delay(FUNCSET_DELAY_MS);
    LCD_write(0x03, CMD);
    delay(FUNCSET_DELAY_uS);   
    LCD_write(0x03, CMD);
    delayMicroseconds(CMD_DELAY_uS);
/* set 4-bit mode now */
    LCD_write(0x02, CMD);
    delayMicroseconds(CMD_DELAY_uS); 
/* Display Off */  
    LCD_write(0x0C, CMD);
    delayMicroseconds(CMD_DELAY_uS);
/* Display Clear */   
    LCD_write(0x01, CMD);
    delayMicroseconds(CMD_DELAY_uS);
/* entry mode set */   
    LCD_write(0x06, CMD);
    delayMicroseconds(CMD_DELAY_uS);
/* 2 rows */   
    LCD_write(0x28, CMD);
    delayMicroseconds(CMD_DELAY_uS);
}

void LCD_toggle_enable()
{
    delayMicroseconds(CMD_DELAY_uS);
    digitalWrite(E, LOW);
    delayMicroseconds(CMD_DELAY_uS);
    digitalWrite(E, HIGH);
    delayMicroseconds(CMD_DELAY_uS);
}

void LCD_write(uint16_t data, bool mode)
{
/* access IR or DR based on mode */
    digitalWrite(RS, mode);

/* ensure data bus is all 0 to start */
    digitalWrite(D4, LOW);
    digitalWrite(D5, LOW);
    digitalWrite(D6, LOW);
    digitalWrite(D7, LOW);

/* write high bits */
    if((data & 0x10) == 0x10)
        digitalWrite(D4, HIGH);
    if((data & 0x20) == 0x20)
        digitalWrite(D5, HIGH);
    if((data & 0x40) == 0x40)
        digitalWrite(D6, HIGH);
    if((data & 0x80) == 0x80)
        digitalWrite(D7, HIGH);

/* toggle enable to write first 4 bits */
    LCD_toggle_enable();

/* ensure data bus is all 0 to start */
    digitalWrite(D4, LOW);
    digitalWrite(D5, LOW);
    digitalWrite(D6, LOW);
    digitalWrite(D7, LOW);

/* write low bits */
    if((data & 0x01) == 0x01)
        digitalWrite(D4, HIGH);
    if((data & 0x02) == 0x02)
        digitalWrite(D5, HIGH);
    if((data & 0x04) == 0x04)
        digitalWrite(D6, HIGH);
    if((data & 0x08) == 0x08)
        digitalWrite(D7, HIGH);

/* toggle enable to write last 4 bits */
    LCD_toggle_enable();
}

void LCD_test(char msg[MAX_MSG_SIZE])
{
    uint8_t i = 0;
    
    printf("\nPrinting %s to LCD...\n\r", msg);
    while(msg[i] != '\0')
        LCD_write(msg[i++], CHAR);
}