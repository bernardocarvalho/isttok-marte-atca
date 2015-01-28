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
#include <time.h>
#include "../pcieAdc.h"
#include "../pcieAdc_ioctl.h"

void cls(void){
	int i = 0;
	for(i=0; i<64; i++){
		printf("\n");
	}
}

static const int DMA_SIZE = 1024;

void sigint_handler(int sig); /* prototype */

int main(int argc, char** argv) {

	int    transferBufferSize        = 0;
	int    totalBytes      = 0;
	int    totalSamples    = 0;
	int    nLoops          = 1;
	int    devFD           = 0;
	int    i               = 0;
	int    j               = 0;
	int    total           = 0;
	int    readBytes       = 0;
	char  *devLoc          = NULL;
	char  *outputFile      = NULL;
	FILE  *outputFileFD    = NULL;
	int   *transferBuffer  = NULL;
	float *acquiredBuffer  = NULL;
	time_t start           = 0;

	if(argc < 5){
		printf("Please run with:\n");
		printf("%s transferBufferSize totalBytes devLoc nLoops [outputFile]\n", argv[0]);
		printf("rBufSize: The size of the buffer which is used to copy the data from the kernel space, must be a multiple of %d\n", DMA_SIZE);
		printf("totalBytes: The number of bytes to read. Must be a multiple of %d\n", DMA_SIZE);
		printf("devLoc: The location of the file device\n");
		printf("nLoops: The number of times the test should be repeated. Must be > 1\n");
		printf("outputFile (optional): Location of the file where the acquired data is saved\n");
		exit(-1);
	}

	cls();

	transferBufferSize = atoi(argv[1]);
	totalBytes         = atoi(argv[2]);
	devLoc             = argv[3];
	nLoops             = atoi(argv[4]);

	if(transferBufferSize == 0 || (transferBufferSize % DMA_SIZE)){
		printf("rBufSize: Must be a multiple of %d\n", DMA_SIZE);
		exit(-1);
	}
	if(totalBytes == 0 || (totalBytes % DMA_SIZE)){
		printf("totalBytes: Must be a multiple of %d\n", DMA_SIZE);
		exit(-1);
	}
	if(nLoops < 1){
		printf("nLoops: must be > 1\n");
	}

	if(argc == 6){
		outputFile = argv[5];
		printf("Acquired data will be saved in: %s\n", outputFile);
	}

	devFD = open(devLoc, O_RDWR);
	if(devFD < 0){
		printf("Failed to open the device file\nExiting\n");
		exit(-1);		
	}

	transferBuffer = (int *)malloc(transferBufferSize);
	if(transferBuffer == NULL){
		printf("Failed to allocate the buffer for the data transfer\nExiting\n");
		exit(-1);
	}
	
	totalSamples = totalBytes / sizeof(float);
	acquiredBuffer = (float *)malloc(totalBytes);
	if(acquiredBuffer == NULL){
		printf("Failed to allocate the buffer for the acquired data\nExiting\n");
		exit(-1);
	}

	totalBytes = 0;
	for(i=0; i<nLoops; i++){
		time(&start);
		while((readBytes != -1) && (total < totalSamples)){
			memset(transferBuffer, 0, transferBufferSize);
			readBytes = read(devFD, ((char *)transferBuffer), transferBufferSize);
			if(readBytes == -1){
				printf("Error reading. Read returned -1!\n");
				break;
			}

			printf(".");
			fflush(stdout);
			totalBytes += readBytes;
        		for (j=0; j<(readBytes / sizeof(int)); j++){
            			//recover the sign
		            	transferBuffer[j] <<= 14;
        	    		transferBuffer[j] >>= 14;
            			acquiredBuffer[total++] = transferBuffer[j] * 32. / 131072;
        		}
			printf("-");
			fflush(stdout);
		}
		printf("\nRead a total of %d bytes => %d samples\n", totalBytes, total);
		printf("Total time = %d seconds\n", (int)(time(NULL) - start));
	}	


	if(outputFile != NULL){
		outputFileFD = fopen(outputFile, "w+");
		if(outputFileFD != NULL){
			printf("Saving acquired data...\n");
			fwrite(acquiredBuffer, sizeof(float), total, outputFileFD);
			fclose(outputFileFD);		
		}else{
			printf("Could not open output file...\n");
		}
	}

	//clean up
	free(acquiredBuffer);
	free(transferBuffer);

	return 0;
}

