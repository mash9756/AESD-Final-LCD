/**
 *  @file       LCDchar.h
 *  @author     Mark Sherman
 * 
 *  @brief      LCD character device structure defintion
*/

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
#define CMD_DELAY_uS        (37) 
/* 100us delay for 3rd function set */
#define FUNCSET_DELAY_uS    (100) 
/* 1.52ms delay for cursor return */
#define HOME_DELAY_MS       (2000)   
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

#ifndef AESD_CHAR_DRIVER_AESDCHAR_H_
#define AESD_CHAR_DRIVER_AESDCHAR_H_

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