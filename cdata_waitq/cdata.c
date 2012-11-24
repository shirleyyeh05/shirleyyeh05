#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/wait.h>
#include <linux/smp_lock.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include "cdata_ioctl.h"

#ifdef CONFIG_SMP
#define __SMP__
#endif

#define	CDATA_MAJOR 121
#define BUFSIZE	    1024

/*
#define DEV_IOCTLID 0xD0 
#define IOCTL_EMPTY _IO(DEV_IOCTLID, 0)
#define IOCTL_SYNC  _IO(DEV_IOCTLID, 1)
*/

static DECLARE_MUTEX(cdata_sem);

struct cdata_t {
	char data[BUFSIZE];
        int  index;
	wait_queue_head_t	wait;
};

static int cdata_open(struct inode *inode, struct file *filp)
{
	/*
	int minor;

	minor = MINOR(inode->i_rdev);
	printk(KERN_ALERT "cdata: in cdata_open(minor = %d)\n", minor);
        */
	struct cdata_t *cdata;

	printk(KERN_ALERT "cdata: in cdata_open(filp = %p)\n", filp);

        cdata = (struct cdata_t *) kmalloc(sizeof(struct cdata_t), GFP_KERNEL);
        cdata->index = 0;
	init_waitqueue_head(&cdata->wait);

	filp->private_data = (void *) cdata;	
	
	return 0;
}

static ssize_t card_write(struct file *filp, const char *buf, size_t count)
{
	int i;
	struct cdata_t *cdata = (struct cdata_t *)filp->private_data;
	DECLARE_WAITQUEUE(wait, current);

	if (cdata == NULL)
		return -EFAULT;

	down(&cdata_sem);   // avoid reentrance, use mutex	

	for (i = 0; i < count; i++){
		if (cdata->index >= BUFSIZE){
			// enter into "sleep" mode (waiting queue)
			current->state = TASK_UNINTERRUPTIBLE;   
			add_wait_queue(&cdata->wait, &wait); 
			// do context switch to other process (the process in ready queue)			
			schedule();
			// it might have some handler (IRQ hander/ IO Interrupt) to wake it up and turn it into running state
			current->state = TASK_RUNNING;
			remove_wait_queue(&cdata->wait, &wait);
  			return -EFAULT;
		}
		if (copy_from_user(&cdata->data[cdata->index++], &buf[i], 1))
			return -EFAULT;
	}
	up(&cdata_sem);   // avoid reentrance, use mutex

	return 0;
}

static int cdata_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct cdata_t *cdata = (struct cdata_t *)filp->private_data;
	printk(KERN_ALERT "cdata: in cdata_ioctl()\n");	

	switch (cmd) {
	case IOCTL_EMPTY:
		printk(KERN_ALERT "cdata: in ioctl: IOCTL_EMPTY\n");
		memset(cdata->data, 0, sizeof(cdata->data));
		break;
	case IOCTL_SYNC:
		printk(KERN_ALERT "cdata: in ioctl: IOCTL_SYNC\n");
		printk(KERN_ALERT "cdata: in ioctl: data=%s\n", cdata->data);
		break;
	default:
		return -ENOTTY;
	}

	return 0;
}

static ssize_t cdata_read(struct file *filp, char *buf, size_t size, loff_t *off)
{
	printk(KERN_ALERT "cdata: in cdata_read()\n");
        return 0;
}

static ssize_t cdata_write(struct file *filp, const char *buf, 
				size_t size, loff_t *off)
{
	printk(KERN_ALERT "cdata_write: %s\n", buf);
        card_write(filp, buf, size);

	return 0;
}

static int cdata_release(struct inode *inode, struct file *filp)
{
	printk(KERN_ALERT "cdata: in cdata_release()\n");

	if (filp->private_data)
        	kfree(filp->private_data);

	return 0;
}

static struct file_operations cdata_fops = {	
	owner:		THIS_MODULE,
	open:		cdata_open,
	release:	cdata_release,
	ioctl:		cdata_ioctl,
	read:		cdata_read,
	write:		cdata_write,
};

int my_init_module(void)
{
	register_chrdev(CDATA_MAJOR, "cdata", &cdata_fops);
	printk(KERN_ALERT "cdata module: registered.\n");

	return 0;
}

void my_cleanup_module(void)
{
	unregister_chrdev(CDATA_MAJOR, "cdata");
	printk(KERN_ALERT "cdata module: unregisterd.\n");
}

module_init(my_init_module);
module_exit(my_cleanup_module);

MODULE_LICENSE("GPL");
