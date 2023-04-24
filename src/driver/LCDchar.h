/**
 *  @file       LCDchar.h
 *  @author     Mark Sherman
 * 
 *  @brief      LCD character device structure defintion
*/

#ifndef AESD_CHAR_DRIVER_AESDCHAR_H_
#define AESD_CHAR_DRIVER_AESDCHAR_H_

#include "LCD.h"

#define AESD_DEBUG 1

#undef PDEBUG             /* undef it, just in case */
#ifdef AESD_DEBUG
#  ifdef __KERNEL__
     /* This one if debugging is on, and kernel space */
#    define PDEBUG(fmt, args...) printk( KERN_DEBUG "aesdchar: " fmt, ## args)
#  else
     /* This one for user space */
#    define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
#  endif
#else
#  define PDEBUG(fmt, args...) /* not debugging: nothing */
#endif

struct LCD_dev
{
    struct mutex mutex; /* locking primitive               */
    struct cdev cdev;   /* Char device structure           */
};


#endif /* AESD_CHAR_DRIVER_AESDCHAR_H_ */