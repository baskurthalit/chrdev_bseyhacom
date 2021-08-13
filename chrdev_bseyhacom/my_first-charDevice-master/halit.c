#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
//#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/mman.h>
#include <linux/slab.h>
/*Kernel'in kendisine özel olan bazı header dosyaları.*/
#include <asm/io.h>

#include "halit.h"

/*file_operations, linux kerneline ait bir struct yapısıdır. "fs.h" altında bulunur. Burada, file_operations'a ait
bir nesne oluşturup atamalar yaptık.*/

static struct file_operations fops = {
        .owner     = THIS_MODULE,
        .read      = device_read,
        .mmap = chrdev_mmap,
        .write     = device_write,
        .open      = device_open,
        .release   = device_release,
        .unlocked_ioctl = device_ioctl,
};

// Cihaz açılınca çalışacak fonksiyon.
static int device_open(struct inode *inode, struct file *file) {
    buf = (char *)kmalloc(4096, GFP_KERNEL);

    if (is_device_open)
        return -EBUSY;
    is_device_open++;



    /*The module is in use, so it is not reachable*/
    try_module_get(THIS_MODULE);
    pr_info("Driver was opened.  \n");


    return 0;
}

// Cihaz kapatılınca çalışacak fonksiyon.
static int device_release(struct inode *inode, struct file *file) {

    printk("the userspace ınput by buf : %s",buf);

    is_device_open--;
    if(buf){
        kfree(buf);}

    module_put(THIS_MODULE);


    pr_info("Driver was closed \n");
    return 0;
}

//Cihazdan okuma yapmak için.
static ssize_t device_read(struct file *filep, char *buffer, size_t length, loff_t *offset) {
    pr_info("device_read() option is not active. \n");
    return 0;
}

//Cihaza yazma yapmak için.
static ssize_t device_write(struct file *filp, const char *buff, size_t len, loff_t * off) {
    pr_info("device_write() option is not active. \n");
    return -EINVAL;
}



static int chrdev_mmap(struct file *filp, struct vm_area_struct *vma)
{
    /*struct vm_area_struct holds information about a contiguous virtual memory area*/

    pr_info("The chrdev_mmap is called from user space \n");

    /*size_t*/ size = vma->vm_end - vma->vm_start;


	


	/* vma_end(end addres of vma , exclusive - start addres of vma, inclusive)*/
	phys_addr_t offset = (phys_addr_t) vma->vm_pgoff << PAGE_SHIFT;

                        
	                    /*vm_pgoff , sayfa sayısındaki offset adresini gösterir
	                     * vm_area'daki ilk sayfa, device'ın vm_pgoff sayfasıdır.*/
	unsigned long pfn;

	/* Ayarlanan adresde problem var mı ? Kontrolü yapılır.*/
	if (offset >> PAGE_SHIFT != vma->vm_pgoff){
		return -EINVAL;
    }
	/* Offset > PAGE_SIZE veya size > PAGE_SIZE - offset
	 * Fazla büyük adreslerin adreslemesini yapamayız, bunun için kontrol yapılır.
	 * */
	if ((offset > BUF_LEN) || (size > BUF_LEN - offset))
		return -EINVAL;

	/*Sanal kernel adressine ait Physical address almak için yapılan işlem.
	 * Getting physcial addres
	A physical page of memory is identified by the Page Frame Number (PFN)*/
	pfn = virt_to_phys(buf) >> PAGE_SHIFT;

	dev_info(dev, "mmap vma=%lx pfn=%lx size=%lx", vma->vm_start, pfn, size);
    printk("Here is a my buf addres : %p",buf);
   // printk("Here is a my vma->vm_pgoff addres : %lx",vma->vm_pgoff);
    printk("here is vm_start : %lx",vma->vm_start);
    printk("here is vm_end : %lx",vma->vm_end);

	/* Remap-pfn-range will mark the range VM_IO */
  /*map it to user space, remap_pfn_range is a helper function
     * remap_pfn_range() will map a contiguous physical address space into the virtual space represented by vm_area_struct*/
	if (remap_pfn_range(vma, vma->vm_start, pfn, size, vma->vm_page_prot))
		return -EAGAIN;
	return 0;
}

static long device_ioctl(struct file *file,unsigned int cmd, unsigned long arg){

    switch(cmd) {
        case WR_VALUE:
            if( copy_from_user(&value ,(int32_t*) arg, sizeof(value)) )
            {
            /*
            value : gelen değerin kernel'de yazılacağı değişken.
            arg : user space'dan gönderilen verinin kaynak adresi.
            */
                pr_err("Data Write : Err!\n");
            }
            pr_info("Value = %d\n", value);
            break;
        case RD_VALUE:
            if( copy_to_user((int32_t*) arg, &value, sizeof(value)) )
            {
                pr_err("Data Read : Err!\n");
            }
            break;
        default:
            pr_info("Default\n");
            break;
    }
    return 0;
}

/*Modül yüklendiğinde çalıştırılacak.*/

static int __init initHalit(void)
{
    int ret;
    struct device *dev_ret;
    pr_info("Hello, this is kernel. \n");

    if ((ret = alloc_chrdev_region(&first, 0, 1, "cDEV")) < 0)
    {
        /*Kaydedilecek cihaz için yeni bir aralık belirler,
        * 0 veya negatif return döndüğünde bu bir hatadır ve kayıt yapılamamıştır.
        *
        * int alloc_chrdev_region (	dev_t * dev,
                                        unsigned baseminor, // Ilk talep minor numarası
                                           unsigned count, // Toplam talep edilen minor numarası.
                                        const char * name // Sürücünün dizindeki adı.
                                        );
        * */
        return ret;
    }

    if (IS_ERR(cl = class_create(THIS_MODULE, "cDEV")))

        /* Struct class pointer'ı üretir. Üretilen değer device_creat'e parametre olarak geçilir.
         * Burada üretilen pointer, class_destroy çağrıldığında yok edilir.
         * */

    {/*Eğer bir hata olursa...*/
        unregister_chrdev_region(first, 1);
        return PTR_ERR(cl);
    }

    if (IS_ERR(dev_ret = device_create(cl, NULL, first, NULL,  DEVICE_NAME)))
    {
        /* sysfs'de bir struct class üretir. Ve yukarda üretilen class'a (cl) kaydedilir.
        * dev_t 0,0 değilse, cihaz için dev_t'i gösteren bir dev file oluşturulur.
        * */
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return PTR_ERR(dev_ret);

    }

    cdev_init(&c_dev, &fops);
    if ((ret = cdev_add(&c_dev, first, 1)) < 0)

    {   /* Yukarıdaki fonksiyonlarla oluşturulan device'ı sistem'e kaydeder.
        *  Negatif return'ü hatadır.
        *  Hata olduğunda alttaki kodları uygular.
        * */
        device_destroy(cl, first);
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return ret;
    }

    return 0;
}
/*Modül çıkartıldığında çalışacak f.*/
static void __exit cleanUpHalit(void)
{
    pr_info("See u again on the command line. \n");
    cdev_del(&c_dev);
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
}

/*_init, _exit makrolar.*/

module_init(initHalit);
module_exit(cleanUpHalit);
MODULE_LICENSE("GPL");
