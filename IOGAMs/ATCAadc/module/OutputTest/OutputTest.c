#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

static const char *mainBoardLocation = "/dev/pcieATCAAdc0";
static int         fileDescriptor    = 0;
static int         moduleIdentifier  = 3;
static float       channelValues[8];

static void writeToDAC(int channel, float value){
    int toWrite[4];
    
    channelValues[channel - 1]  = value;

    toWrite[0] = moduleIdentifier;
    toWrite[1] = channel - 1;
    toWrite[2] = (int)((value + 10) * 65535 / 20);
    toWrite[3] = 0;
    write(fileDescriptor, toWrite, 4 * sizeof(int));
}

static void hitAnyKey(){
    printf("\nHit any key");
    getchar();
    getchar();
}

static void cls(){
    int i;
    for(i=0; i<50; i++){
        printf("\n");
    }
}

int main(int argc, char **argv){
    char  read[4];
    int   i       = 0;
    int   channel = 0;
    float voltage = 0;
    
    cls();
    fileDescriptor = open(mainBoardLocation, O_RDWR); 
    if(fileDescriptor < 1){
        printf("Could not access board\n");
        return -1;
    }

    for(i=0; i<8; i++){
        writeToDAC(i + 1, 0);
    }

    while(1){
        cls();
        printf("To exit press q\n");
        printf("Channel Value [V]\n");
        for(i=0; i<8; i++){
            printf("   %d    %f\n", i + 1, channelValues[i]);
        }        
        printf("Channel (a for all): ");
        scanf("%s", &read[0]);
        if(read[0] == 'q'){
            break;
        }
        if(read[0] != 'a'){
            channel = atoi(read);
            if(channel < 1 || channel > 8){
                printf("\nChannel must be between 1 and 8");
                hitAnyKey();
                continue;
            }
        }
        printf("Enter voltage value: ");
        scanf("%f", &voltage);
        if(voltage < -10 || voltage > 10){
            printf("\nVoltage must be between -10 and 10 V");
            hitAnyKey();
            continue;
        }
      
        if(read[0] != 'a'){
            writeToDAC(channel, voltage);
        }else{
            for(i=1; i<9; i++){
                writeToDAC(i, voltage);
            }
        }
    }
    close(fileDescriptor);


    return 0;
}

