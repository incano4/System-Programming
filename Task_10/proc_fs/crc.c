#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/crc32.h>

#define PROC_NAME "crc_entry"
#define MAX_LEN 4096

static u32 last_crc = 0; // последнее вычисленное crc
static struct proc_dir_entry* proc_entry;

// чтение /proc/crc_entry
static ssize_t proc_read(struct file* file, char __user* buffer, size_t len, loff_t* offset) {
    char str[9]; // crc32 = 8 + /0
    int ret;

    if (*offset != 0)
        return 0;

    snprintf(str, sizeof(str), "%x", last_crc); // crc32 -> str
    ret = copy_to_user(buffer, str, sizeof(str));

    if (ret != 0)
        return -EFAULT;

    *offset += sizeof(str); // увеличиваем смещение
    last_crc = ~0;
    printk(KERN_INFO "crc: Read from proc, returned crc32: %s\n", str);
    return sizeof(str);
}

static ssize_t proc_write(struct file* file, const char __user* buffer, size_t len, loff_t* offset) {
    char* data;

    if (len == 0 || len > MAX_LEN)
        return -EINVAL;

    // GFP_KERNEL - память должна быть выделена в контексте ядра с возможностью блокировки
    // исп., когда ядро может временно приостановить выполнение, чтобы выполнить выделение памяти
    data = kmalloc(len, GFP_KERNEL);
    if (!data)
        return -ENOMEM; // ошибка при отсутствии памяти

        // освобождаем память, если не копирование не удалось
    if (copy_from_user(data, buffer, len)) {
        kfree(data);
        return -EFAULT;
    }

    last_crc = crc32(~0, data, len);
    printk(KERN_INFO "crc: Written data: %.*s\n", (int)len, data);
    printk(KERN_INFO "crc: Computed crc32: %x\n", last_crc);

    kfree(data);
    return len;
}

// структура для работы с proc_fs
static const struct proc_ops proc_fops = {
    .proc_read = proc_read,
    .proc_write = proc_write,
};

// инициализация модуля
static int __init crc_init(void) {
    // созд. файл
    proc_entry = proc_create(PROC_NAME, 0666, NULL, &proc_fops);
    if (!proc_entry) {
        printk(KERN_ALERT "crc: Failed to create /proc/%s\n", PROC_NAME);
        return -ENOMEM;
    }

    printk(KERN_INFO "crc: Module loaded, /proc/%s created\n", PROC_NAME);
    return 0;
}

// при удалении модуля
static void __exit crc_exit(void) {
    proc_remove(proc_entry);
    printk(KERN_INFO "crc: Module unloaded, /proc/%s removed\n", PROC_NAME);
}

module_init(crc_init);
module_exit(crc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("incano4");
MODULE_DESCRIPTION("Procfs CRC32 driver");
MODULE_VERSION("1.0");
