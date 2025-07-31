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
static int sum = 0;
 
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
    major = register_chrdev(0, "addition", &fops);

    if (major < 0)
        return major;

    pr_info("Major: %d\n", major);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0)
    cls = class_create("addition");
#else
    cls = class_create(THIS_MODULE, "addition");
#endif

    device_create(cls, NULL, MKDEV(major, 0), NULL, "addition");

    return 0;
}

static void __exit chardev_exit(void)
{
    device_destroy(cls, MKDEV(major, 0));
    class_destroy(cls);
    unregister_chrdev(major, "addition");
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
    char str[16];

    // convert 'sum' to a hex string
    {
        int x = sum;
        int i = 0;
        do {
            int digit = x & 0xF;
            char c = digit < 10 ? '0' + digit : 'A' + digit;
            str[i++] = c;

            x >>= 4;
        } while (x > 0);

        str[i] = 0;
    }

    // reverse the string so MSB is first character
    size_t str_len = strlen(str);
    int x = str_len - 1;
    for (int i = 0; i < str_len / 2; i++)
    {
        char tmp = str[i];
        str[i] = str[x - i];
        str[x - i] = tmp;
    }

    // send it out
    if (*f_pos > str_len) return 0;

    if (*f_pos + count > str_len) {
        count = str_len - *f_pos;
    }

    copy_to_user(buf, &str[*f_pos], count);

    *f_pos += count;
    return count;
}

static ssize_t device_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    char *input_str = kmalloc(count, GFP_KERNEL);
    copy_from_user(input_str, &buf[*f_pos], count);

    // convert input string to int
    int input_val = 0, i = 0;

    while (input_str[i] != 0)
    {
        char c = input_str[i];
        int digit_val = 0;

        if (c >= '0' && c <= '9')
        {
            digit_val = c - '0';
        }
        else if (c >= 'A' && c <= 'F')
        {
            digit_val = c - 'A' + 10;
        }
        else break;

        input_val <<= 4;
        input_val |= digit_val;
        i++;
    }

    // add to sum
    sum += input_val << (*f_pos * 4);

    *f_pos += count;
    return count;
}

module_init(chardev_init);
module_exit(chardev_exit);
MODULE_LICENSE("GPL");
