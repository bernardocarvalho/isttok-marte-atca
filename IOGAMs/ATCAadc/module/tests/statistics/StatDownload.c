#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdio.h>
#include <termios.h>
#include "StatTest.h"
#include <sys/ioctl.h>
#include <unistd.h>

static const char   *deviceName        = "/dev/atcapcieT0";
static const char   *outputFileName    = "data.out";
static int           fd;
static int           softTrigger       = 1;
static int           extClkTrg         = 1;
static int           numberOfBoards    = 0;
static int           sampleTimeNS      = 500;
static int           acqTotalTimeNS    = 1000000;
static int           acqAllData        = 0;
static int           slotNumsBuffer[12];
static unsigned int  w33Buffer[12];

static void printBitsSimple(int n) {
    unsigned int i;
    i = 1<<(sizeof(n) * 8 - 1);
    while (i > 0) {
        if (n & i){
	    printf("1");
        }
        else{
            printf("0");	    
        }
        i >>= 1;
    }
    printf("\n");
}

void consoleRaw(){
    struct termios tio;
    tcgetattr( 0, &tio );
    tio.c_lflag &= ~ICANON;
    tcsetattr( 0, TCSANOW, &tio );
}

void consoleNoRaw(){
    struct termios tio;

    tcgetattr( 0, &tio );
    tio.c_lflag |= ICANON;
    tcsetattr( 0, TCSANOW, &tio );
} 


static void cls(void){
    int i = 0;
    for(i=0; i<32; i++){
        printf("\n");
    }
}

static void handleNumberBoards(void){
    cls();    
    printf("The number of installed boards is %d\n", numberOfBoards);
    printf("Hit any key\n");
    getchar();
}

static void handleSlotNumbers(void){
    int i = 0;
    cls();
    for(i=0; i<numberOfBoards; i++){
        printf("slotNumsBuffer[%d] = %d\n", i, slotNumsBuffer[i]);
    }
    printf("Hit any key\n");
    getchar();
}

static void handleW33(void){
    int i = 0;
    cls();
    for(i=0; i<numberOfBoards; i++){
        w33Buffer[i] = 0;
    }
    if(ioctl(fd, IOCTL_ENABLE_ACQ) != 0){
	    printf("Failed enabling acquisition...\n");
        return;
    }
    sleep(1);
    if(ioctl(fd, IOCTL_ACQ_W33, &w33Buffer) != 0){
	    printf("Failed acquiring w33...\n");
        return;
    }    
    if(ioctl(fd, IOCTL_DISABLE_ACQ) != 0){
	    printf("Failed disabling acquisition...\n");
        return;
    }
    for(i=0; i<numberOfBoards; i++){        
        printf("Board at slot: %d\n", slotNumsBuffer[i]);
        printf("w33: (%u) = ", w33Buffer[i]);
        printBitsSimple(w33Buffer[i]);        
    }
    printf("Hit any key\n");
    getchar();
}

static void handleSetupDataAcq(void){
    cls();
    char readBuffer[32];
    printf("Insert the sampling time (ns). Current value[%d]\n->", sampleTimeNS);
    memset(readBuffer, 0, 32);
    fgets(readBuffer, 32, stdin);    
    if(readBuffer[0] != '\n'){
        sampleTimeNS = atoi(readBuffer);
    }
    printf("Insert the total acquisition time (ns). Current value[%d]\n->", acqTotalTimeNS);
    memset(readBuffer, 0, 32);
    fgets(readBuffer, 32, stdin);
    if(readBuffer[0] != '\n'){
        acqTotalTimeNS = atoi(readBuffer);
    }
    printf("Use external clock and trigger?. Current value[%d]\n->", extClkTrg);
    memset(readBuffer, 0, 32);
    fgets(readBuffer, 32, stdin);
    if(readBuffer[0] != '\n'){
        extClkTrg = atoi(readBuffer);
    }
    printf("Use soft trigger?. Current value[%d]\n->", softTrigger);
    memset(readBuffer, 0, 32);
    fgets(readBuffer, 32, stdin);
    if(readBuffer[0] != '\n'){
        softTrigger = atoi(readBuffer);
    }
    printf("Acquire all data (1) or only headers and buffers (0)?. Current value[%d]\n->", acqAllData);
    memset(readBuffer, 0, 32);
    fgets(readBuffer, 32, stdin);
    if(readBuffer[0] != '\n'){
        acqAllData = atoi(readBuffer);
    }
    
    printf("Hit any key\n");
    getchar();
}

static void handleStartDacTest(void){
    if(ioctl(fd, IOCTL_SAMPLE_TIME_NS, &sampleTimeNS) != 0){
        printf("Failed setting sampleTimeNS\n");
	return;
    }

    if(ioctl(fd, IOCTL_START_DAC_TEST) != 0){
        printf("Failed starting DAC test\n");
        return;
    }
    
}

static void handleStopDacTest(void){
    if(ioctl(fd, IOCTL_STOP_DAC_TEST) != 0){
        printf("Failed stopping DAC test\n");
        return;
    }

}

static void handleStartAcq(void){        
    int acqCompleted   = 0;
    int ret            = 1;
    int i              = 0;
    
    unsigned int       buffer[1024];
    
    cls();
    
    printf("Using sample time ns = %d\n", sampleTimeNS);
    printf("Using total acquisition time ns = %d\n", acqTotalTimeNS);
    if(acqAllData == 0){
        printf("Acquiring only headers and buffers\n");
    }else{
        printf("Acquiring all data\n");
    }
    if(softTrigger == 0){
        printf("No soft trigger\n");
    }else{
        printf("Soft trigger is set\n");
    }
    if(extClkTrg == 0){
        printf("No external clock and trigger\n");
    }else{
        printf("External clock and trigger is set\n");
    }

    if(ioctl(fd, IOCTL_SAMPLE_TIME_NS, &sampleTimeNS) != 0){
        printf("Failed setting sampleTimeNS\n");
        return;
    }
    if(ioctl(fd, IOCTL_ACQ_TOTAL_TIME_NS, &acqTotalTimeNS) != 0){
        printf("Failed setting acqTotalTimeNS\n");
        return;
    }
    if(ioctl(fd, IOCTL_ACQ_ALL_DATA, &acqAllData) != 0){
        printf("Failed setting acqAllData\n");
        return;
    }
    if(ioctl(fd, IOCTL_EXT_CLK_TRIGGER, &extClkTrg) != 0){
        printf("Failed setting acqAllData\n");
        return;
    }
    if(ioctl(fd, IOCTL_SOFT_TRIGGER, &softTrigger) != 0){
        printf("Failed setting acqAllData\n");
        return;
    }
    printf("Starting acquisition\n");
    if(ioctl(fd, IOCTL_START_ACQ) != 0){
        printf("Failed starting acquisition\n");
        return;
    }
    while(acqCompleted == 0){
        sleep(1);
        if(ioctl(fd, IOCTL_ACQ_COMPLETE, &acqCompleted) != 0){
            printf("Failed waiting for acq completed...\n");	   
            return;
        }
    }
    printf("Acquisition completed!\n");
    FILE *outputFile = fopen(outputFileName, "w+");
    if(outputFile == NULL){
        printf("Failed opening the output file: %s\n", strerror(errno));
        return;
    }
    
    while(ret > 0){
        memset(buffer, 0, 4096);
        ret = read(fd, buffer, 4096);
        for(i=0; i<(ret / sizeof(int)); i++){
            fprintf(outputFile, "%u\n", buffer[i]);
        }
    }
    fclose(outputFile);
    printf("Hit any key\n");
    getchar();
}

static int handleMenu(void){
    int c = 0;
    cls();    
    printf("a - Acquire data\n");
    printf("b - Setup internal data acquisition\n");
    printf("d - Start DAC test\n");
    printf("e - Stop DAC test\n");
    printf("n - Show number of boards\n");
    printf("s - Show slot numbers\n");    
    printf("w - Show word 33\n");
    printf("q - quit\n");
    c = getchar();
        
    switch(c){
        case 'q':
            return -1;        
        case 'a':
            handleStartAcq();
            break;
        case 'b':
            handleSetupDataAcq();
            break;
	case 'd':
	    handleStartDacTest();
	    break;
	case 'e':
	    handleStopDacTest();
	    break;
        case 'n':
            handleNumberBoards();
            break;
        case 's':
            handleSlotNumbers();
            break;
        case 'w':
            handleW33();
            break;
    }
    
    return 0;
}

int saveData(void){
    fd = open(deviceName, O_RDWR);
    if(fd < 0){
        printf("Failed opening device driver: %s\n", strerror(errno));
        return fd;
    }
    
    if(ioctl(fd, IOCTL_NUM_BOARDS, &numberOfBoards) != 0){
        printf("Failed getting numberOfBoards\n");
        return -1;
    }
    
    if(ioctl(fd, IOCTL_BOARD_SLOTS, &slotNumsBuffer) != 0){
        printf("Failed getting slotNumsBuffer\n");
        return -1;
    }

    while(handleMenu() == 0);
    
    close(fd);
    return 0;
}

int main(int argc, char *argv[]){
    consoleRaw();
    saveData();
    consoleNoRaw();
    return 0;
}

