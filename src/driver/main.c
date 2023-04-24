/**
 *  @file       driver.c
 *  @author     Mark Sherman
 * 
 *  @brief      LCD driver functions
*/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/fs.h>   // file_operations
#include <linux/slab.h>
#include <linux/string.h>

#include "LCDchar.h"

int LCD_major =   0; // use dynamic major
int LCD_minor =   0;

MODULE_AUTHOR("Mark Sherman");
MODULE_LICENSE("Dual BSD/GPL");

struct LCD_dev LCD_device;

int LCD_open(struct inode *inode, struct file *filp)
{
    struct LCD_dev *dev;
    PDEBUG("open");

    dev = container_of(inode->i_cdev, struct LCD_dev, cdev);
    filp->private_data = dev;
    return 0;
}

int LCD_release(struct inode *inode, struct file *filp)
{
    PDEBUG("release");

    filp->private_data = NULL;
    return 0;
}

ssize_t LCD_read(struct file *filp, char __user *buf, size_t count,
                loff_t *f_pos)
{
    /* only writing to the LCD is permitted, read will always return error */
    return -EFAULT;
}

ssize_t LCD_write(struct file *filp, const char __user *buf, size_t count,
                loff_t *f_pos)
{
    struct LCD_dev *dev = filp->private_data;  /* get pointer to our char device */
    ssize_t retval = 0;
    char *input_buffer = NULL;

    PDEBUG("write %zu byte string %s to LCD", count, buf);

/* parameter error handling */
    if(filp == NULL || buf == NULL || f_pos == NULL)
    {
        retval = -EFAULT;
        goto exit;
    }
/* check total buffer size */
    if(count > MAX_MSG_SIZE)
    {
        retval = -EFAULT;
        goto exit;
    }

/* obtain mutex, exit on failure */
    if(mutex_lock_interruptible(&dev->mutex) != 0)
    {
        retval = -ERESTARTSYS;
        goto exit;
    }

/* allocate input buffer to store the entire write */
    input_buffer = (char *)kmalloc(count, GFP_KERNEL);
    if(input_buffer == NULL)
    {
        retval = -ENOMEM;
        goto exit;
    }

/* returns 0 on success, >0 is number of bytes not written */
    if(copy_from_user(input_buffer, buf, count))
    {
        retval = -EFAULT;
        goto free_kmem;
    }

/** 
 * TODO: add size handling for LCD
 *          16 chars per row, 2 rows
 *          figure out how to change cursor position, add llseek??
*/

/* print entered string to LCD */
    LCD_print(input_buffer);

free_kmem:
    kfree(input_buffer);

exit:
/* release mutex */
    mutex_unlock(&dev->mutex);

    return retval;
}

/**
 *  @name   LCD_llseek
 *  @brief  adds lseek functionality
 * 
 *  @param  filp    our device
 *  @param  offset  desired offest for given operation, change f_pos
 *  @param  whence  SEEK operation
 * 
 *  @return new file offset position
 * 
 *  followed general format from http://www.learningaboutelectronics.com/Articles/How-to-implement-the-lseek-file-operation-method-linux-device-driver.php
*/
loff_t LCD_llseek(struct file *filp, loff_t offset, int whence)
{
    loff_t new_f_pos;
    struct LCD_dev *dev = filp->private_data;

    PDEBUG("\nllseek");

    if(mutex_lock_interruptible(&dev->mutex) != 0)
    {
        new_f_pos = -ERESTARTSYS;
        goto exit;
    }

    switch (whence) 
    {
        case SEEK_SET:
            new_f_pos = offset;
            PDEBUG("\nSEEK_SET new f_pos: %lld", new_f_pos);
            break;

        case SEEK_CUR:
            new_f_pos = filp->f_pos + offset;
            PDEBUG("\nSEEK_CUR new f_pos: %lld", new_f_pos);
            break;

        case SEEK_END:
            new_f_pos = MAX_MSG_SIZE - offset;
            PDEBUG("\nSEEK_END new f_pos: %lld", new_f_pos);
            break;

        default:
            return -EINVAL;
    }

/* check that new position doesn't extend past the end of our CB */
    if(new_f_pos > MAX_MSG_SIZE) 
        return -EINVAL;

/* check that we don't back up too far */
    if(new_f_pos < 0) 
        new_f_pos = 0;

/* update actual f_pos with new calculated value */
    filp->f_pos = new_f_pos;
    PDEBUG("\nUpdated f_pos = %lld", filp->f_pos);

exit:
    mutex_unlock(&dev->mutex);

    return new_f_pos;
}

struct file_operations LCD_fops = {
    .owner      = THIS_MODULE,
    .read       = LCD_read,
    .write      = LCD_write,
    .open       = LCD_open,
    .release    = LCD_release,
    .llseek     = LCD_llseek
};

static int LCD_setup_cdev(struct LCD_dev *dev)
{
    int err, devno = MKDEV(LCD_major, LCD_minor);

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
    dev_t dev = 0;
    int result;
    result = alloc_chrdev_region(&dev, LCD_minor, 1,
                                 "LCDchar");
    LCD_major = MAJOR(dev);
    if (result < 0)
    {
        printk(KERN_WARNING "Can't get major %d\n", LCD_major);
        return result;
    }
    memset(&LCD_device, 0, sizeof(struct LCD_dev));

    mutex_init(&LCD_device.mutex);
    LCD_init();

    result = LCD_setup_cdev(&LCD_device);

    if (result)
    {
        unregister_chrdev_region(dev, 1);
    }
    return result;
}

void LCD_cleanup_module(void)
{
    dev_t devno = MKDEV(LCD_major, LCD_minor);

    cdev_del(&LCD_device.cdev);

    mutex_destroy(&LCD_device.mutex);
    unregister_chrdev_region(devno, 1);
}

module_init(LCD_init_module);
module_exit(LCD_cleanup_module);