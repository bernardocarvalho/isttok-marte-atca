#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <signal.h>
#include "../pcieAdc.h"
#include "../pcieAdc_ioctl.h"

#define MAX_BUFFER    16
char DEVNAME[] = "/dev/pcie0";

void sigint_handler(int sig); /* prototype */

int run=1;

static int error[256];

int main(int argc, char** argv) {
	int buffer[MAX_BUFFER];
	int ret;
	int i;
	int counter = 0;
	char * devn;
	int temp = 0;
	float value;
	int flags = O_RDWR;

	if(argc <2)
		devn = DEVNAME;
	else
		devn = argv[1];
	/* set up the handler */
        if (signal(SIGINT, sigint_handler) == SIG_ERR) {
		perror("signal");
		exit(1);
        }
	int fd = open(devn,flags);
	if (fd==-1){ 
	    fprintf (stderr,"Error: cannot open device %s \n",argv[1]);
	    fprintf (stderr," errno = %i\n",errno);
	    exit(EXIT_FAILURE);
	}
	ret = ioctl(fd, PCIE_ADC_IOCT_INT_ENABLE); /* Not actually implemented, but...*/
	if (ret == -1){
	    fprintf (stderr,"Error: cannot ioctl device %s \n",argv[1]);
	    fprintf (stderr," errno = %i\n",errno);
	    close(fd);
	    exit(EXIT_FAILURE);
	}

        
	while (run!=0) {
		memset(&buffer, 0, (sizeof(int)*MAX_BUFFER));
		ret = read(fd, &buffer, sizeof(int)*MAX_BUFFER);		
	        for (i=0; i<MAX_BUFFER; i++){
	//	    printf("%012d = 0x%8x", counter, buffer[i]);
        	    //recover the sign
	            temp = (buffer[i]&0xffffffff) << 14;
        	    temp >>= 14;
	            value = temp * 32. / 131072;
//        	    printf("%f\n", value);
//        	    printf("0x%x\n", buffer[i]);
		    error[(buffer[i] & 0xff)]++;
	        }			
	}
	close(fd);

	return 0;
}

/* this is the handler */
void sigint_handler(int sig)
{
  run=0;
  int i=0;
  for(i=0;i<255;i++){
  	printf("%d\n", error[i]);
  }
//  printf("INT received!\n");
}
