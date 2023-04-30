/**
 *  @file       driver.c
 *  @author     Mark Sherman
 * 
 *  @brief      LCD driver functions
 * 
 * https://embetronicx.com/tutorials/linux/device-drivers/gpio-driver-basic-using-raspberry-pi/
*/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/fs.h>   // file_operations
#include <linux/slab.h>
#include <linux/string.h>

#include <linux/delay.h>
#include <linux/uaccess.h>  //copy_to/from_user()
#include <linux/gpio.h>     //GPIO

#include "LCDchar.h"
#include "LCD_ioctl.h"

int LCD_major =   0; // use dynamic major
int LCD_minor =   0;

MODULE_AUTHOR("Mark Sherman");
MODULE_LICENSE("Dual BSD/GPL");

dev_t device = 0;
struct LCD_dev LCD_device;
static struct class *dev_class;

static bool ioctl_flag = false;

ssize_t LCD_read(struct file *filp, char __user *buf, size_t count,
                loff_t *f_pos)
{
    /* only writing to the LCD is permitted, read will always return error */
    PDEBUG("LCD Read, you shouldn't be here!");
    return -EFAULT;
}

void LCD_toggle_enable(void)
{
    PDEBUG("toggle enable");

    usleep_range(CMD_DELAY_uS, CMD_DELAY_uS + 10);
    gpio_set_value(E, 0);
    usleep_range(CMD_DELAY_uS, CMD_DELAY_uS + 10);
    gpio_set_value(E, 1);
    usleep_range(CMD_DELAY_uS, CMD_DELAY_uS + 10);
}

ssize_t LCD_write(struct file *filp, const char __user *buf, size_t count,
                loff_t *f_pos)
{
    struct LCD_dev *dev = filp->private_data;  /* get pointer to our char device */
    long retval = 0;
    char *input_buffer = NULL;
    size_t i = 0;

    PDEBUG("write %zu byte string %s to LCD", count, buf);

/* parameter error handling */
    if(filp == NULL || buf == NULL)
    {
        PDEBUG("NULL Paramenter found");
        retval = -EFAULT;
        goto exit;
    }
/* check total buffer size */
    if(count > MAX_MSG_SIZE)
    {
        PDEBUG("Message too long");
        retval = -EFAULT;
        goto exit;
    }

/* obtain mutex, exit on failure */
    if(mutex_lock_interruptible(&dev->mutex) != 0)
    {
        PDEBUG("Mutex failed to lock");
        retval = -ERESTARTSYS;
        goto exit;
    }

/* allocate input buffer to store the entire write */
    input_buffer = (char *)kmalloc(count, GFP_KERNEL);
    if(input_buffer == NULL)
    {
        PDEBUG("kmalloc failed");
        retval = -ENOMEM;
        goto exit;
    }

/* returns 0 on success, >0 is number of bytes not written */
    if(ioctl_flag == true)
    {
        PDEBUG("ioctl write, no copy from user");
        input_buffer = &buf;
    }    
    else if(copy_from_user(input_buffer, buf, count))
    {
        PDEBUG("Copy from user failed");
        retval = -EFAULT;
        goto free_kmem;
    }

    for(i = 0; i < count; i++)
    {
    /* ensure data bus is all 0 to start */
        PDEBUG("Zeroing Data Bus");
        gpio_set_value(D4, 0);
        gpio_set_value(D5, 0);
        gpio_set_value(D6, 0);
        gpio_set_value(D7, 0);

    /* write high bits */
        PDEBUG("Writing MSB");
        if((input_buffer[i] & 0x10) == 0x10)
            gpio_set_value(D4, 1);
        if((input_buffer[i] & 0x20) == 0x20)
            gpio_set_value(D5, 1);
        if((input_buffer[i] & 0x40) == 0x40)
            gpio_set_value(D6, 1);
        if((input_buffer[i] & 0x80) == 0x80)
            gpio_set_value(D7, 1);

    /* toggle enable to write first 4 bits */
        LCD_toggle_enable();

    /* ensure data bus is all 0 to start */
        PDEBUG("Zeroing Data Bus");
        gpio_set_value(D4, 0);
        gpio_set_value(D5, 0);
        gpio_set_value(D6, 0);
        gpio_set_value(D7, 0);

    /* write low bits */
        PDEBUG("Writing LSB");
        if((input_buffer[i] & 0x01) == 0x01)
            gpio_set_value(D4, 1);
        if((input_buffer[i] & 0x02) == 0x02)
            gpio_set_value(D5, 1);
        if((input_buffer[i] & 0x04) == 0x04)
            gpio_set_value(D6, 1);
        if((input_buffer[i] & 0x08) == 0x08)
            gpio_set_value(D7, 1);

    /* toggle enable to write first 4 bits */
        LCD_toggle_enable();
    }

    retval = i;
    filp->f_pos = retval;
    PDEBUG("Write Complete! Updated f_pos to %lld", filp->f_pos);

free_kmem:
    PDEBUG("kfree");
    kfree(input_buffer);

exit:
/* release mutex */
    mutex_unlock(&dev->mutex);

    PDEBUG("returning %ld", retval);
    return retval;
}

/**
 *  @name   LCD_unlocked_ioctl
 *  @brief  add ioctl functionality, supports single command to clear display
 * 
 *  @param  filp    our device
 *  @param  cmd     ioctl command to execute
 *  @param  arg     arguments to pass with cmd
 * 
 *  @return 0 on success, error on failure
 * 
 *  followed general format from https://embetronicx.com/tutorials/linux/device-drivers/ioctl-tutorial-in-linux/
*/
long LCD_unlocked_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    long retval = 0;
    char ins = 0;
    PDEBUG("ioctl");

    switch(cmd)
    {
        case LCDCHAR_IOCCLEAR:
        {
            PDEBUG("LCDCHAR_IOCCLEAR");
            if(copy_from_user(&ins, (const void __user *) arg, sizeof(ins)) != 0)
            {
                PDEBUG("ioctl copy_from_user failed");
                retval = -EFAULT;
                goto exit;
            }

            ioctl_flag = true;
            gpio_set_value(RS, CMD);
            usleep_range(CMD_DELAY_uS, CMD_DELAY_uS + 10);
            LCD_write(filp, &ins, 1, 0);
            usleep_range(CMD_DELAY_uS, CMD_DELAY_uS + 10);
            gpio_set_value(RS, CHAR);
            usleep_range(CMD_DELAY_uS, CMD_DELAY_uS + 10);
            ioctl_flag = false;
            break;
        }
        default:
            PDEBUG("cmd not recognized");
    }

exit:
    PDEBUG("Returning: %ld", retval);
    return retval;
}

int LCD_open(struct inode *inode, struct file *filp)
{
    struct LCD_dev *dev;
    PDEBUG("LCD open");

    dev = container_of(inode->i_cdev, struct LCD_dev, cdev);
    filp->private_data = dev;

    return 0;
}

int LCD_release(struct inode *inode, struct file *filp)
{
    PDEBUG("LCD release");

    filp->private_data = NULL;
    return 0;
}


struct file_operations LCD_fops = 
{
    .owner          = THIS_MODULE,
    .read           = LCD_read,
    .write          = LCD_write,
    .open           = LCD_open,
    .release        = LCD_release,
    .unlocked_ioctl = LCD_unlocked_ioctl
};

static int LCD_setup_cdev(struct LCD_dev *dev)
{
    int err, devno = MKDEV(LCD_major, LCD_minor);

    PDEBUG("LCD Setup cdev");

    cdev_init(&dev->cdev, &LCD_fops);
    dev->cdev.owner = THIS_MODULE;
    dev->cdev.ops = &LCD_fops;
    err = cdev_add (&dev->cdev, devno, 1);
    if (err) {
        printk(KERN_ERR "Error %d adding LCD cdev", err);
    }
    return err;
}

int LCD_init_module(void)
{
    int result;
    bool invalid = false;

    PDEBUG("LCD Init Module");

    result = alloc_chrdev_region(&device, LCD_minor, 1, "LCDchar");
    LCD_major = MAJOR(device);
    if (result < 0)
    {
        printk(KERN_WARNING "Can't get major %d\n", LCD_major);
        return result;
    }
    memset(&LCD_device, 0, sizeof(struct LCD_dev));

    mutex_init(&LCD_device.mutex);

    result = LCD_setup_cdev(&LCD_device);
    if (result)
        unregister_chrdev_region(device, 1);

/* Creating struct class */
    if(IS_ERR(dev_class = class_create(THIS_MODULE,"LCD_class")))
    {
        PDEBUG("Cannot create the struct class\n");
        goto class_invalid;
    }

/** Creating device 
 * TODO:    Might be redundant to alloc_chrdev_regiod? 
 *          seems like echo to both LCDchar and LCD_device work
*/
    if(IS_ERR(device_create(dev_class, NULL, device, NULL, "LCD_device")))
    {
        PDEBUG( "Cannot create the Device \n");
        goto device_invalid;
    }

/* Check each GPIO for validity */
    if(gpio_is_valid(RS) == false)
    {
        PDEBUG("GPIO %d is not valid\n", RS);
        invalid = true;
    }

    if(gpio_is_valid(E) == false)
    {
        PDEBUG("GPIO %d is not valid\n", E);
        invalid = true;
    }

    if(gpio_is_valid(D4) == false)
    {
        PDEBUG("GPIO %d is not valid\n", D4);
        invalid = true;
    }

    if(gpio_is_valid(D5) == false)
    {
        PDEBUG("GPIO %d is not valid\n", D5);
        invalid = true;
    }

    if(gpio_is_valid(D6) == false)
    {
        PDEBUG("GPIO %d is not valid\n", D6);
        invalid = true;
    }

    if(gpio_is_valid(D7) == false)
    {
        PDEBUG("GPIO %d is not valid\n", D7);
        invalid = true;
    }

    if(invalid)
        goto gpio_invalid;

/* request control of LCD GPIO pins */
    if(gpio_request(RS,"RS") < 0)
    {
        PDEBUG("ERROR: GPIO %d request\n", RS);
        goto gpio_invalid;
    }
    if(gpio_request(E,"E") < 0)
    {
        PDEBUG("ERROR: GPIO %d request\n", E);
        goto gpio_invalid;
    }
    if(gpio_request(D4,"D4") < 0)
    {
        PDEBUG("ERROR: GPIO %d request\n", D4);
        goto gpio_invalid;
    }
    if(gpio_request(D5,"D5") < 0)
    {
        PDEBUG("ERROR: GPIO %d request\n", D5);
        goto gpio_invalid;
    }
    if(gpio_request(D6,"D6") < 0)
    {
        PDEBUG("ERROR: GPIO %d request\n", D6);
        goto gpio_invalid;
    }
    if(gpio_request(D7,"D7") < 0)
    {
        PDEBUG("ERROR: GPIO %d request\n", D7);
        goto gpio_invalid;
    }

    PDEBUG("Setting GPIO directions");
    gpio_direction_output(RS, 0);
    gpio_direction_output(E,  0);
    gpio_direction_output(D4, 0);
    gpio_direction_output(D5, 0);
    gpio_direction_output(D6, 0);
    gpio_direction_output(D7, 0);

/** TODO: Remove LCD data bus pins from export for safety */
    PDEBUG("Exporting GPIO for user access");
    gpio_export(RS, false);
    gpio_export(E, false);
    gpio_export(D4, false);
    gpio_export(D5, false);
    gpio_export(D6, false);
    gpio_export(D7, false);

    PDEBUG("Init Complete!");
    return result;

gpio_invalid:
    gpio_free(RS);
    gpio_free(E);
    gpio_free(D4);
    gpio_free(D5);
    gpio_free(D6);
    gpio_free(D7);

device_invalid:
    device_destroy(dev_class, device);

class_invalid:
    class_destroy(dev_class);

    PDEBUG("Init Failed, returning fault");
    return -EFAULT;
}

void LCD_cleanup_module(void)
{
    dev_t devno = MKDEV(LCD_major, LCD_minor);

    PDEBUG("LCD Cleanup Module");

    gpio_unexport(RS);
    gpio_unexport(E);
    gpio_unexport(D4);
    gpio_unexport(D5);
    gpio_unexport(D6);
    gpio_unexport(D7);

    gpio_free(RS);
    gpio_free(E);
    gpio_free(D4);
    gpio_free(D5);
    gpio_free(D6);
    gpio_free(D7);

    device_destroy(dev_class, device);
    class_destroy(dev_class);

    cdev_del(&LCD_device.cdev);

    mutex_destroy(&LCD_device.mutex);
    unregister_chrdev_region(devno, 1);
}

module_init(LCD_init_module);
module_exit(LCD_cleanup_module);