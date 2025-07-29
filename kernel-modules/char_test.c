#include <linux/atomic.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <linux/string.h>
#include <linux/io.h>
 
#include <asm/errno.h>

static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char __user *, size_t, loff_t *);

static int major;
static char string[64];
 
enum {
    CDEV_NOT_USED = 0,
    CDEV_EXCLUSIVE_OPEN = 1,
};

static atomic_t already_open = ATOMIC_INIT(CDEV_NOT_USED);
 
static struct class *cls;
 
static struct file_operations fops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release,
};

static int __init chardev_init(void)
{
    major = register_chrdev(0, "char_test", &fops);

    if (major < 0)
        return major;

    pr_info("Major: %d\n", major);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0)
    cls = class_create("char_test");
#else
    cls = class_create(THIS_MODULE, "char_test");
#endif

    device_create(cls, NULL, MKDEV(major, 0), NULL, "char_test");
    pr_info("/dev/char_test\n");

    strcpy(string, "Hello, world!\n");

    return 0;
}

static void __exit chardev_exit(void)
{
    device_destroy(cls, MKDEV(major, 0));
    class_destroy(cls);
    unregister_chrdev(major, "char_test");
}

static int device_open(struct inode *inode, struct file *file)
{
    if (atomic_cmpxchg(&already_open, CDEV_NOT_USED, CDEV_EXCLUSIVE_OPEN))
        return -EBUSY;

    try_module_get(THIS_MODULE);

    return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
    atomic_set(&already_open, CDEV_NOT_USED);
    module_put(THIS_MODULE);
    
    return 0;
}

static ssize_t device_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    size_t n_bytes = strlen(string) + 1; // string content + 0 byte

    if (*f_pos > n_bytes) return 0;

    count = n_bytes - *f_pos;
    copy_to_user(buf, &string[*f_pos], count);

    *f_pos += count;

    return count;
}

static ssize_t device_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    pr_info("device_write(filp, buf, count=%d, *f_pos=%d): ", count, *f_pos);

    if (*f_pos < 64)
    {
        count = min(count, 64) - *f_pos;

        memset(string, 0, 64);
        copy_from_user(string, &buf[*f_pos], count);

        string[63] = 0; // ensure at least one 0 exists
    }

    pr_info("count = %d\n", count);

    *f_pos += count;
    return count;
}

module_init(chardev_init);
module_exit(chardev_exit);
MODULE_LICENSE("GPL");
