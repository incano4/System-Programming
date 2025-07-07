#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/crc32.h>

#define DEVICE_NAME     "crc_char_device"
#define CLASS_NAME      "crc_class"

static int major_number;
static struct class* crc_class = NULL; // уникальный идентификатор устройства
static struct device* crc_device = NULL;
static struct cdev crc_cdev; // структура символьного устройства

static u32 last_crc = 0; // последнее вычисленное crc

// выводим инфу при открытии /dev/crc_char_device
static int dev_open(struct inode* inodep, struct file* filep) {
    printk(KERN_INFO "crc: Device has been opened\n");
    return 0;
}

// выводим инфу при закрытии /dev/crc_char_device
static int dev_release(struct inode* inodep, struct file* filep) {
    printk(KERN_INFO "crc: Device successfully closed\n");
    return 0;
}


// когда пользователь читает /dev/crc_char_device
static ssize_t dev_read(struct file* filep, char* buffer, size_t len, loff_t* offset) {
    char str[9]; // crc32 = 8 + /0

    if (*offset >= sizeof(str)) // если уже прочитано
        return 0;

    printk(KERN_INFO "crc: Device read\n");        
    sprintf(str, "%x", last_crc); // crc32 -> str
    copy_to_user(buffer, str, sizeof(str)); // copy a block of data into user space
    last_crc = 0;

    *offset += sizeof(str); // увеличиваем смещение
    return sizeof(str);
}

// когда пользователь пишет в /dev/crc_char_device (echo "hello" > /dev/crc_char_device)
static ssize_t dev_write(struct file* filep, const char* buffer, size_t len, loff_t* offset) {
    printk(KERN_INFO "crc: Device write\n");
    
    // if (buffer == NULL || len == 0) {
    //     last_crc = crc32(~0, "0", 0);  // вычисляем crc32 для пустой строки
    //     printk(KERN_INFO "data: (empty)\n");
    //     printk(KERN_INFO "crc32: %x\n", last_crc);
    //     return 0;
    // } else {

    // GFP_KERNEL - память должна быть выделена в контексте ядра с возможностью блокировки
    // исп., когда ядро может временно приостановить выполнение, чтобы выполнить выделение памяти
    char* data = (char*)kmalloc(len, GFP_KERNEL);
    if (!data)
        return -ENOMEM; // ошибка в случае отсутствия памяти

    copy_from_user(data, buffer, len);
    last_crc = crc32(~0, data, len);
    printk(KERN_INFO "data: %s\n", data);
    printk(KERN_INFO "crc32: %x\n", last_crc);
    kfree(data);
    // }
    return len;
}

static struct file_operations fops = {
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};

static int __init crc_init(void) {
    // Dynamically allocate a major number
    if (alloc_chrdev_region(&major_number, 0, 1, DEVICE_NAME) < 0) {
        printk(KERN_ALERT "crc failed to register a major number\n");
        return major_number;
    }
    printk(KERN_INFO "crc: registered correctly with major number %d\n", MAJOR(major_number));

    // Register the device class
    crc_class = class_create(CLASS_NAME);
    if (IS_ERR(crc_class)) {
        unregister_chrdev_region(major_number, 1);
        printk(KERN_ALERT "Failed to register device class\n");
        return PTR_ERR(crc_class);
    }
    printk(KERN_INFO "crc: device class registered correctly\n");

    // Register the device driver
    crc_device = device_create(crc_class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
    if (IS_ERR(crc_device)) {
        class_destroy(crc_class);
        unregister_chrdev_region(major_number, 1);
        printk(KERN_ALERT "Failed to create the device\n");
        return PTR_ERR(crc_device);
    }
    printk(KERN_INFO "crc: device class created correctly\n");

    // Initialize the cdev structure and add it to the kernel
    cdev_init(&crc_cdev, &fops);
    if (cdev_add(&crc_cdev, major_number, 1) < 0) {
        device_destroy(crc_class, MKDEV(major_number, 0));
        class_destroy(crc_class);
        unregister_chrdev_region(major_number, 1);
        printk(KERN_ALERT "Failed to add cdev\n");
        return -1;
    }

    return 0;
}

static void __exit crc_exit(void) {
    cdev_del(&crc_cdev); // удаляем символьное устройство
    device_destroy(crc_class, MKDEV(major_number, 0)); // удаляем устройство из /dev
    class_destroy(crc_class);
    unregister_chrdev_region(major_number, 1);
    printk(KERN_INFO "crc: Goodbye from the LKM!\n");
}

module_init(crc_init);
module_exit(crc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("incano4");
MODULE_DESCRIPTION("Linux char driver");
MODULE_VERSION("1.0");