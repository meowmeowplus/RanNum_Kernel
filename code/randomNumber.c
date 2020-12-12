#include <linux/module.h>    // included for all kernel modules
#include <linux/kernel.h>    // included for KERN_INFO    // included for __init and __exit macros
#include <linux/random.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>


static dev_t first; // Global variable for the first device number
static struct cdev c_dev; // Global variable for the character device structure
static struct class *cl; // Global variable for the device class



static int my_open(struct inode *i, struct file *f)
{
    printk(KERN_INFO "Driver: open()\n");
    return 0;
}

static int my_close(struct inode *i, struct file *f)
{
    printk(KERN_INFO "Driver: close()\n");
    return 0;
}

static ssize_t my_read(struct file *f, char *usr_space, size_t len, loff_t *off)
{
    int ranNum;
    get_random_bytes(&ranNum, sizeof(int));
    printk(KERN_INFO "Random: %d\n", ranNum);

    printk(KERN_INFO "Driver: read()\n");
    if (!copy_to_user(usr_space, &ranNum, sizeof(ranNum))) {
        printk(KERN_INFO "RC: return randomized number to the user\n");
        return 0;
    }
        // else return error: Bad address
    else {
        printk(KERN_INFO "RC: failed to send randomized number to the user\n");
        return -EFAULT;
    }
}

static struct file_operations pugs_fops =
{
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_close,
    .read = my_read,
};

static int __init hello_init(void)
{
    printk(KERN_INFO "RN: ofcd registered");
    if (alloc_chrdev_region(&first, 0, 1, "randomNumber") < 0)
    {
        return -1;
    }
    if ((cl = class_create(THIS_MODULE, "randomNumber")) == NULL)
    {
        unregister_chrdev_region(first, 1);
        return -1;
    }
    if (device_create(cl, NULL, first, NULL, "randomNumber") == NULL)
    {
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return -1;
    }
    cdev_init(&c_dev, &pugs_fops);
    if (cdev_add(&c_dev, first, 1) == -1)
    {
        device_destroy(cl, first);
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return -1;
    }
    return 0;    // Non-zero return means that the module couldn't be loaded.
}

static void __exit hello_cleanup(void)
{
    printk(KERN_INFO "Cleaning up module.\n");
    cdev_del(&c_dev);
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    printk(KERN_INFO "RN: ofcd unregistered");
}

module_init(hello_init);
module_exit(hello_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lakshmanan");
MODULE_DESCRIPTION("A Simple Hello World module");