/*
 * LCD_ioctl.h
 *
 *  Created on: Oct 23, 2019
 *      Author: Dan Walkes
 *
 *  @brief Definitins for the ioctl used on LCD char devices for assignment 9
 */

#ifndef LCD_IOCTL_H
#define LCD_IOCTL_H

#ifdef __KERNEL__
#include <asm-generic/ioctl.h>
#include <linux/types.h>
#else
#include <sys/ioctl.h>
#include <stdint.h>
#endif

/**
 * I dont think we need this at all? we just want to clear
 * 
 * A structure to be passed by IOCTL from user space to kernel space, describing the type
 * of seek performed on the LCDchar driver
 */
struct LCD_clear {
    /**
     * The zero referenced write command to seek into
     */
    char *clear_cmd;
};

// Pick an arbitrary unused value from https://github.com/torvalds/linux/blob/master/Documentation/userspace-api/ioctl/ioctl-number.rst
#define LCD_IOC_MAGIC 0x16

/* LCD Instructions */
#define LCD_CLEAR_INS         (0x01)
#define LCD_DISPLAY_OFF_INS   (0x0C)

// Define a write command from the user point of view, use command number 1

/* can last arg be 0? */
#define LCDCHAR_IOCCLEAR _IOWR(LCD_IOC_MAGIC, 1, struct LCD_clear)
/**
 * The maximum number of commands supported, used for bounds checking
 */
#define LCDCHAR_IOC_MAXNR 1

#endif /* LCD_IOCTL_H */
