#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/crc32.h>

#define DEVICE_NAME     "crc_char_device"
#define CLASS_NAME      "crc_class"

#define CRC_MAX_LEN	256
#define CRC_IOCTL_SET	_IOW('d', 0, char[CRC_MAX_LEN])
#define CRC_IOCTL_GET	_IOR('d', 1, u32)

static int major_number;
static struct class* crc_class = NULL;
static struct device* crc_device = NULL;
static struct cdev crc_cdev;

static u32 last_crc = 0;

// file operations
static int dev_open(struct inode* inodep, struct file* filep) {
	printk(KERN_INFO "crc: Device has been opened\n");
	return 0;
}

static int dev_release(struct inode* inodep, struct file* filep) {
	printk(KERN_INFO "crc: Device successfully closed\n");
	return 0;
}

static ssize_t dev_read(struct file* filep, char* buffer, size_t len, loff_t* offset) {
	char str[9];

	if (*offset >= sizeof(str))
		return 0;

	printk(KERN_INFO "crc: Device read\n");        
	sprintf(str, "%x", last_crc);
	copy_to_user(buffer, str, sizeof(str));

	*offset += sizeof(str);
	return sizeof(str);
}

static ssize_t dev_write(struct file* filep, const char* buffer, size_t len, loff_t* offset) {
	printk(KERN_INFO "crc: Device write\n");
	char* data = (char*)kmalloc(len, GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	copy_from_user(data, buffer, len);
	last_crc = crc32_le(~0, data, len);
	printk(KERN_INFO "data: %s\n", data);
	printk(KERN_INFO "crc32: %x\n", last_crc);
	kfree(data);

	return len;
}

static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
	char str[CRC_MAX_LEN] = { 0 };
	size_t str_len = 0;

	printk(KERN_INFO "crc: Device ioctl\n");

	if (cmd == CRC_IOCTL_SET) {
		if (copy_from_user(str, (char*)arg, CRC_MAX_LEN)) {
			printk(KERN_WARNING "crc: copy_from_user failed\n");
			return -EFAULT;
		}

		str_len = strnlen(str, CRC_MAX_LEN);
		printk(KERN_INFO "crc: input len %zu\n", str_len);
		last_crc = crc32_le(~0, str, str_len);
		printk(KERN_INFO "crc32: %x\n", last_crc);
	} else if (cmd == CRC_IOCTL_GET) {
		if (copy_to_user((u32 *)arg, &last_crc, sizeof(u32))) {
			printk(KERN_WARNING "crc: copy_to_user failed\n");
			return -EFAULT;
		}
	} else {
		printk(KERN_WARNING "crc: Unknown ioctl command: 0x%x\n", cmd);
		return -ENOTTY;
	}

	return 0;
    }

static struct file_operations fops = {
	.open = dev_open,
	.read = dev_read,
	.write = dev_write,
	.release = dev_release,
	.unlocked_ioctl = dev_ioctl,
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
	cdev_del(&crc_cdev);
	device_destroy(crc_class, MKDEV(major_number, 0));
	class_destroy(crc_class);
	unregister_chrdev_region(major_number, 1);
	printk(KERN_INFO "crc: Goodbye from the LKM!\n");
}

module_init(crc_init);
module_exit(crc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("incano4");
MODULE_DESCRIPTION("A crc32 Linux driver");
MODULE_VERSION("0.1");