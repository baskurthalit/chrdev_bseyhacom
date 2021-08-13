//********************************
// Header file of char dev halit.
//********************************


 
 #include <linux/ioctl.h>
 #include <linux/cdev.h>
 #include <linux/types.h>


#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)
/* magic number, the command id, and the data type,
 * The magic number is a unique number that will allow the driver to detect errors
 * The command id, is a way for dirver to understand what command is needed to be called.*/
#define DRIVER_AUTHOR "Halit"
#define DEVICE_NAME "halit"

#define BUF_LEN		PAGE_SIZE

/*
 * PAGE_SIZE is equal to 4096 bytes for 32 bit systems.
 * PAGE_SIZE is equal to 8192 bytes for 64 bit systems.
 *
 * */

static int device_open(struct inode *inode, struct file *file);
static int device_release(struct inode*, struct file*);
static ssize_t device_read(struct file*, char*, size_t, loff_t*);
static ssize_t device_write(struct file*, const char*, size_t, loff_t*);
static long device_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
static int chrdev_mmap(struct file *filp, struct vm_area_struct *vma);

int32_t value = 0;/*For IOCTL R/W*/
int is_device_open=0;
static dev_t first; 		// for the first device number | Minor
static struct cdev c_dev; 	// for the character device structure
static struct class *cl; 	// for the device class
static char *buf;
struct device *dev;
size_t size;
