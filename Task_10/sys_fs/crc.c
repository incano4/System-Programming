#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/crc32.h>
#include <linux/stat.h>

static struct kobject *crc_kobj; // указатель на sysfs-объект
static u32 last_crc = 0;

static ssize_t crc_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    printk(KERN_INFO "crc_sysfs: reading value\n");
    ssize_t count = sprintf(buf, "%0x\n", last_crc); // crc -> str
    last_crc = ~0;
    return count;
}

static ssize_t crc_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
    printk(KERN_INFO "crc_sysfs: writing value: %s", buf);
    // printk(KERN_INFO "crc_size: %zx", count);
    
    last_crc = crc32(~0, buf, strnlen(buf, count));
    printk(KERN_INFO "crc_sysfs: calculated crc32 = %0x\n", last_crc);
    return count;
}

// задаем атрибуты файла: имя, режим доступа, функции чтения и записи
static struct kobj_attribute crc_attr = __ATTR(crc_val, S_IWUSR | S_IRUGO, crc_show, crc_store);
// S_IRUGO для чтения всем пользователям
// S_IWUSR для записи  владельцу

// инициализация модуля
static int __init crc_sysfs_init(void) {
    printk(KERN_INFO "crc_sysfs: Initializing module\n");

    crc_kobj = kobject_create_and_add("crc_sysfs", kernel_kobj);
    if (!crc_kobj)
        return -ENOMEM;

    if (sysfs_create_file(crc_kobj, &crc_attr.attr)) {
        kobject_put(crc_kobj);
        return -EINVAL;
    }

    printk(KERN_INFO "crc_sysfs: Module loaded. Use /sys/kernel/crc_sysfs/crc_val\n");
    return 0;
}

static void __exit crc_sysfs_exit(void) {
    sysfs_remove_file(crc_kobj, &crc_attr.attr);
    kobject_put(crc_kobj);

    printk(KERN_INFO "crc_sysfs: Module unloaded\n");
}

module_init(crc_sysfs_init);
module_exit(crc_sysfs_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("incano4");
MODULE_DESCRIPTION("Kernel module with sysfs interface for CRC32");
MODULE_VERSION("1.0");
