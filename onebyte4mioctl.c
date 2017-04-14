#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/ioctl.h>

#define MAJOR_NUMBER 65
#define DEVICE_SIZE 4*1024*1024
#define ONEBYTE4M_IOC_TYPE 'k'
#define ONEBYTE4M_IOC_HELLO _IO(ONEBYTE4M_IOC_TYPE, 1)
#define ONEBYTE4M_IOC_WRITE _IOW(ONEBYTE4M_IOC_TYPE, 2, char* )
#define ONEBYTE4M_IOC_READ _IOR(ONEBYTE4M_IOC_TYPE, 3, char* )
#define ONEBYTE4M_IOC_WR _IOWR(ONEBYTE4M_IOC_TYPE, 4, char* )

/* forward declarations */

int onebyte4m_open(struct inode *inode, struct file *filep);
int onebyte4m_release(struct inode *inode, struct file *filep);
ssize_t onebyte4m_read(struct file *filep, char *buf, size_t count, loff_t *f_pos);
ssize_t onebyte4m_write(struct file *filep, const char *buf, size_t count, loff_t *f_pos);
loff_t onebyte4m_llseek(struct file *filep, loff_t offset, int whence);
long onebyte4m_ioctl(struct file* filep, unsigned int cmd, unsigned long arg);
static void onebyte4m_exit(void);

/* definition of file operation structure */
struct file_operations onebyte4m_fops = {
 read: onebyte4m_read,
 write: onebyte4m_write,
 open: onebyte4m_open,
 llseek: onebyte4m_llseek,
 release: onebyte4m_release,
 unlocked_ioctl: onebyte4m_ioctl
};

char *onebyte4m_data = NULL;
size_t cur = 0;
char dev_msg[50];

int onebyte4m_open(struct inode *inode, struct file *filep)
{
  return 0; // always successful
}

int onebyte4m_release(struct inode *inode, struct file *filep)
{
  return 0; // always successful
}

ssize_t onebyte4m_read(struct file *filep, char *buf, size_t count, loff_t *f_pos)
{
  /* please complete the function on your own */
  if ( *f_pos > cur ) return -1;
  if ( count + (*f_pos) > cur) count = cur - *f_pos;
  copy_to_user(buf,onebyte4m_data + *f_pos, count);
  *f_pos += count;
  return count;
}

ssize_t onebyte4m_write(struct file *filep, const char *buf, size_t count, loff_t *f_pos)
{
  /* please complete the function on your own */
  if (*f_pos > DEVICE_SIZE) return -ENOSPC;
  if (count + (*f_pos) > DEVICE_SIZE) count = DEVICE_SIZE - *f_pos;
  copy_from_user(onebyte4m_data + *f_pos, buf, count);
  *f_pos += count;
  cur = *f_pos;
  if (cur == DEVICE_SIZE) return -ENOSPC;
  return count;
}

loff_t onebyte4m_llseek(struct file* filep, loff_t offset, int whence)
{
  loff_t new_position = 0;
  switch (whence)
    {
    case 0: new_position = offset;
      break;
    case 1: new_position = filep->f_pos + offset;
      break;
    case 2: new_position = DEVICE_SIZE + offset;
      break;
    default: return -EINVAL;
    }
  if (new_position < 0) return -EINVAL;
  else if (new_position > DEVICE_SIZE) new_position = DEVICE_SIZE;
  filep->f_pos = new_position;
  return new_position;
}

long onebyte4m_ioctl(struct file* filep, unsigned int cmd, unsigned long arg)
{
  ssize_t val = 0;
  char tmp[50];
  if (_IOC_TYPE(cmd) != ONEBYTE4M_IOC_TYPE) return -ENOTTY;
  switch (cmd)
    {
    case ONEBYTE4M_IOC_HELLO:
      printk(KERN_WARNING "hello\n");
      break;
    case ONEBYTE4M_IOC_WRITE:
      if (copy_from_user(dev_msg, (char *)arg, sizeof(dev_msg)))
	val = -EACCES;
      printk(KERN_INFO " \"%s\"\n", dev_msg);
      break;
    case ONEBYTE4M_IOC_READ:
      if (copy_to_user((char *)arg, dev_msg, sizeof(dev_msg)))
	val = -EACCES;
      break;
    case ONEBYTE4M_IOC_WR:
      memcpy(tmp, dev_msg, sizeof(dev_msg));
      val = copy_from_user(dev_msg, (char *)arg, sizeof(dev_msg));
      if (val == 0)
	val = copy_to_user((char *)arg, tmp, sizeof(dev_msg));
      printk(KERN_INFO "new content \"%s\"\n", dev_msg);
      break;
    default:
      val = -ENOTTY;
      break;
    }
  return val;
}
  
static int onebyte4m_init(void)
{
  int result;
  // register the device
  result = register_chrdev(MAJOR_NUMBER, "onebyte4m", &onebyte4m_fops);
  if (result < 0) {
    return result;
  }
  // allocate one byte of memory for storage
  // kmalloc is just like malloc, the second parameter is
  // the type of memory to be allocated.
  // To release the memory allocated by kmalloc, use kfree.
  onebyte4m_data = kmalloc(DEVICE_SIZE, GFP_KERNEL);
  if (!onebyte4m_data) {
    onebyte4m_exit();
    // cannot allocate memory
    // return no memory error, negative signify a failure

    return -ENOMEM;
  }
  // initialize the value to be X
  *onebyte4m_data = 'X';
  printk(KERN_ALERT "This is a onebyte4m device module\n");
  return 0;
}

static void onebyte4m_exit(void)
{
  // if the pointer is pointing to something
  if (onebyte4m_data) {
    // free the memory and assign the pointer to NULL
    kfree(onebyte4m_data);
    onebyte4m_data = NULL;
  }
  // unregister the device
  unregister_chrdev(MAJOR_NUMBER, "onebyte4m");
  printk(KERN_ALERT "Onebyte4m device module is unloaded\n");
}

MODULE_LICENSE("GPL");
module_init(onebyte4m_init);
module_exit(onebyte4m_exit);
