#include <linux/module.h>       // included for all kernel modules
#include <linux/kernel.h>    
#include <linux/random.h>       // make random number
#include <linux/version.h>
#include <linux/types.h>        // dev_t
#include <linux/kdev_t.h>
#include <linux/fs.h>           // support loggin
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>      // copy_to_user func


static dev_t first_dev;         // Global variable for the first device number
static struct cdev c_dev;       // Global variable for the character device structure
static struct class *dv_class;  // Global variable for the device class


static int device_open(struct inode *node, struct file *f);
static int device_close(struct inode *node, struct file *f);
static ssize_t device_read(struct file *f, char *usr_space, size_t len, loff_t *off);

static struct file_operations fops =
{
    .owner = THIS_MODULE,
    .open = device_open,
    .release = device_close,
    .read = device_read,
};


static int device_open(struct inode *node, struct file *f)
{
    printk(KERN_INFO "Driver: open()\n");
    return 0;
}


static int device_close(struct inode *node, struct file *f)
{
    printk(KERN_INFO "Driver: close()\n");
    return 0;
}


static ssize_t device_read(struct file *f, char *usr_space, size_t len, loff_t *off)
{
    int ranNum;
    get_random_bytes(&ranNum, sizeof(int));

    printk(KERN_INFO "Driver: read()\n");

    if (!copy_to_user(usr_space, &ranNum, sizeof(ranNum))) 
    {
        printk(KERN_INFO "randomNumber: return randomized number to the user successfully\n");
        return 0;
    }
    else 
    {
        printk(KERN_INFO "randomNumber: failed to send randomized number to the user\n");
        return -EFAULT;
    }
}


static int __init randomNumber_init(void)
{
    printk(KERN_INFO "randomNumber: ofcd registered");
    if (alloc_chrdev_region(&first_dev, 0, 1, "randomNumber") < 0)
    {
        return -1;
    }

    if ((dv_class = class_create(THIS_MODULE, "randomNumber")) == NULL)
    {
        unregister_chrdev_region(first_dev, 1);
        return -1;
    }

    if (device_create(dv_class, NULL, first_dev, NULL, "randomNumber") == NULL)
    {
        class_destroy(dv_class);
        unregister_chrdev_region(first_dev, 1);
        return -1;
    }

    cdev_init(&c_dev, &fops);
    if (cdev_add(&c_dev, first_dev, 1) == -1)
    {
        device_destroy(dv_class, first_dev);
        class_destroy(dv_class);
        unregister_chrdev_region(first_dev, 1);
        return -1;
    }
    return 0;    
}


static void __exit randomNumber_exit(void)
{
    printk(KERN_INFO "Cleaning up module.\n");
    cdev_del(&c_dev);
    device_destroy(dv_class, first_dev);
    class_destroy(dv_class);
    unregister_chrdev_region(first_dev, 1);
    printk(KERN_INFO "randomNumber: ofcd unregistered");
}


module_init(randomNumber_init);
module_exit(randomNumber_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("@meowmeowplus");
MODULE_DESCRIPTION("A Simple randomNumber Kernel");