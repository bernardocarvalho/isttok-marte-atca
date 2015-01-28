#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
	char   buffer[256];
	float *fbuffer = NULL;
	int    read    = 1;
	FILE  *input   = NULL;
	FILE  *output  = NULL;
	int    i       = 0;
	
	if(argc < 3){
		printf("Usage: %s input.bin output.txt\n", argv[0]);
	}
	
	input = fopen(argv[1], "r");
	if(input == NULL){
		printf("Could not open file: %s, exiting\n", argv[1]);
		exit(-1);
	}
	output = fopen(argv[2], "w+");
	if(output == NULL){
		printf("Could not open file: %s, exiting\n", argv[2]);
		exit(-1);
	}
	
	while(read > 0){
		read = fread(buffer, 1, sizeof(float), input);
		if(read > 0){
			fbuffer = (float *)buffer;
			for(i=0; i<(read / sizeof(float)); i++){
				fprintf(output, "%f\n", fbuffer[i]);	
			}
		}
	}

	fclose(input);
	fclose(output);
	return 0;
}

