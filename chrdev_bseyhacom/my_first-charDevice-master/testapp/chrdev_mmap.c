/*
 * chrdev mmap() testing program
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
/*
void clearBuffer() {
    int ch;
    while((ch = getchar()) != '\n' && ch != EOF);
}
fgets için clearBuffer dı, fakat artık kullanıma gerek kalmadı . */
int main()
{
	int mmapFile;
	char *addr;
	long len;
	char *ptr;
	int i;
	//int ret;
	#define DEVICE_FILENAME "/dev/halit"

	printf("\nEnter a len value : \n");

	scanf("%ld",&len);

	mmapFile = open(DEVICE_FILENAME, O_RDWR);

	if (mmapFile) {

		printf("file  opened\n");

	}
	else {
	perror("open");
		exit(EXIT_FAILURE);
		}
	addr = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, mmapFile, 0);

	printf("got address=%p and len=%ld\n", addr, len);
	printf("--------------------------\n");
		if (addr == MAP_FAILED) {
		perror("mmap");
		exit(EXIT_FAILURE);
}

    //addr = (char *) addr;
    char temp;

    printf("\n Lütfen metni giriniz: ");

    scanf("%c",&temp); // temp statement to clear buffer, after INT ınput.

    fgets(addr,len,stdin);    /*scanf("%[^\\]",ptr); "its old v of fgets,"*/
    addr[strlen(addr)] = '\0';


    for(int i=0;i<len;i++){
        printf("%c", addr[i]);
    }

    munmap(addr, len);
	close(mmapFile);

	return 0;
}
