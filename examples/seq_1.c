#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
#define HAVE_PROC_OPS
#endif

#define PROC_NAME "sequence"
#define SEQ_LIMIT 20

static void *seq_start(struct seq_file *s, loff_t *pos) {
	static unsigned long cnt = 0;
	
	if (*pos == 0)
		cnt = 0;
	if (cnt >= SEQ_LIMIT)
		return NULL;
	
	return &cnt;
}

static void *seq_next(struct seq_file *s, void *v, loff_t *pos) {
	unsigned long *tmp_v = (unsigned long *)v;
	(*tmp_v)++;
	(*pos)++;

	if (*tmp_v >= SEQ_LIMIT)
		return NULL;
	return v;
}

static void seq_stop(struct seq_file *s, void *v) {	
}

static int seq_show(struct seq_file *s, void *v) {
	loff_t *spos = (loff_t *)v;
	seq_printf(s, "%Ld\n", *spos);
	return 0;
}

static struct seq_operations seq_ops = {
	.start = seq_start,
	.next = seq_next,
	.stop = seq_stop,
	.show = seq_show,
};

static int open(struct inode *inode, struct file *file) {
	return seq_open(file, &seq_ops);
};

#ifdef HAVE_PROC_OPS
static const struct proc_ops file_ops = {
	.proc_open = open,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = seq_release,
};
#else
static const struct file_operations file_ops = {
	.open = open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
};
#endif

static int __init seq_init(void) {
	struct proc_dir_entry *entry;

	entry = proc_create(PROC_NAME, 0, NULL, &file_ops);
	if (entry == NULL) {
		pr_debug("Error initializing /proc/%s\n", PROC_NAME);
		return -ENOMEM;
	}
	pr_info("initialized /proc/%s\n", PROC_NAME);
	return 0;
}

static void __exit seq_exit(void) {
	remove_proc_entry(PROC_NAME, NULL);
	pr_debug("removed /proc/%s\n", PROC_NAME);
}

module_init(seq_init);
module_exit(seq_exit);

MODULE_LICENSE("GPL");
