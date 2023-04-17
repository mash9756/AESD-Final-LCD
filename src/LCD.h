/**
 *  @file       LCD.h
 *  @author     Mark Sherman
 * 
 *  @brief      LCD function and defines for Hitachi HD44780U 16x2 LCD
 *              See data sheet linked below
 *              https://www.sparkfun.com/datasheets/LCD/HD44780.pdf
 * 
 *  @date       4/1/23
 *  @version    1.0
*/

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <pigpio.h>

/* max LCD message size, 16x2 */
#define MAX_MSG_SIZE    (32)

#define RS    (7)
#define E     (8)
#define D4    (25)
#define D5    (24)
#define D6    (23)
#define D7    (18)

/* 37us execution time for commands */
#define CMD_DELAY_uS        (37000)
/* 100us delay for 3rd function set */
#define FUNCSET_DELAY_uS    (100000)
/* 1.52ms delay for cursor return */
#define HOME_DELAY_uS       (1520000)   
/* 15ms powerup delay */
#define POWERUP_DELAY_MS    (15000)
/* 4.1ms delay for function set, round to 5 */
#define FUNCSET_DELAY_MS    (5000)

/* access modes for use with RS */
#define CMD     (0)
#define CHAR    (1)

/* output parameters, LCD is 2 rows of 16 chars */
#define CHAR_PER_ROW    (16)
#define NUM_OF_ROWS     (2)

#define ROW1_ADDR       (0x80)
#define ROW2_ADDR       (0xC0)

/**
 *  @name   LCD_init
 *  @brief  initialize 
*/
void LCD_init();

/**
 * 
*/
void LCD_write(uint16_t data, bool mode);

void LCD_toggle_enable();

/**
 *  @name   LCD_test
 *  @brief  implement simple testing of the LCD interface
 *          write text, move cursor, erase, etc. 
*/
void LCD_test(char msg[MAX_MSG_SIZE]);
