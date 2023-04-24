/**
 *  @file       LCD.h
 *  @author     Mark Sherman
 * 
 *  @brief      LCD function and defines for Hitachi HD44780U 16x2 LCD
 *              See data sheet linked below
 *              https://www.sparkfun.com/datasheets/LCD/HD44780.pdf
*/

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <pigpio.h>

/* max LCD message size, 16x2 */
#define MAX_MSG_SIZE    (32)

/* LCD pin definitions */
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
 *  @brief  setup gpio pins and write startup commands to LCD
 * 
 *  @return VOID
*/
void LCD_init();

/**
 *  @name   LCD__toggle_enable
 *  @brief  toggle LCD enable pin to write data on 4-wire bus to LCD
 * 
 *  @return VOID
*/
void LCD_toggle_enable();

/**
 *  @name   LCD_write
 *  @brief  write data to the LCD in 4-wire bus mode
 * 
 *  @param  data    data to write to LCD
 *  @param  mode    where to write data to, command or character register
 * 
 *  @return VOID
*/
void LCD_write(uint16_t data, bool mode);

/**
 *  @name   LCD_print
 *  @brief  print a message to the LCD
 * 
 *  @param  msg message to print to the LCD
 * 
 *  @return VOID
*/
void LCD_print(char msg[MAX_MSG_SIZE]);
