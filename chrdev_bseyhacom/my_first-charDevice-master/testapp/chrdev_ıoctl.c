#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include<sys/ioctl.h>
#include <sys/mman.h>
 
#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)
#define DEVICE_FILENAME "/dev/halit"


int main()
{
    int fd;

    int32_t value, number;

        fd = open(DEVICE_FILENAME, O_RDWR);
        printf("\nDriver Açılıyor\n");

    if(fd < 0) {
                printf("\n..Cannot open device file...\n");
                return 0;
        }

        printf("Göndermek için değer giriniz: \n");
        scanf("%d",&number);
        printf("Driver'a yazılıyor.\n");
        ioctl(fd, WR_VALUE, (int32_t*) &number); 
 
        printf("Driver'dan okunuyor..\n");

        ioctl(fd, RD_VALUE, (int32_t*) &value);

        printf("Okunan değer: %d\n", value);
 
        printf("Driver kapatılıyor \n");

        close(fd);
        
}
