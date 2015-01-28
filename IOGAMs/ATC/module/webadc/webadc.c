/*
 * This file is an example of how to embed web-server functionality
 * into existing application.
 * Compilation line:
 * cc example.c shttpd.c -DEMBEDDED
 */

#include <sys/types.h>
#include <sys/select.h>
#define ALIAS_URI "/my_etc"
#define ALIAS_DIR "/etc/"

#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "shttpd.h"
#include "pcieAdc.h"
#include "pcieAdc_ioctl.h"

static char devName[32];
static int last4Bits[16];
static int MAX_CHANNELS = 32;

static void createGNUPlotFile1(int channel){
	char txt[256];
	char filename[32];
        memset(filename, 0, 32);
        sprintf(filename, "chart_%d.gnuplot", channel);
	sprintf(txt, "set terminal png\nset title \"Results for channel %d\"\nset output \"signal_%d.png\"\nplot \"signal_%d.data\" with lines\n", channel, channel, channel);
	FILE *f = fopen(filename, "w+");
	fprintf(f, "%s", txt);
	fclose(f);
}

static void createGNUPlotFile2(int channel){
	char txt[256];
	char filename[32];
        memset(filename, 0, 32);
        sprintf(filename, "chartDer_%d.gnuplot", channel);
	sprintf(txt, "set terminal png\nset title \"Derivate results for channel %d\"\nset output \"signalDer_%d.png\"\nplot \"signalDer_%d.data\" with lines\n", channel, channel, channel);
	FILE *f = fopen(filename, "w+");
	fprintf(f, "%s", txt);
	fclose(f);
}

static void createGNUPlotFile3(int channel){
	char txt[256];
	char filename[32];
        memset(filename, 0, 32);
        sprintf(filename, "charterror_%d.gnuplot", channel);
	sprintf(txt, "set terminal png\nset title \"Last 4 bits variation for channel %d\"\nset output \"signalError_%d.png\"\nplot \"signalError_%d.data\" with histograms\n", channel, channel, channel);
	FILE *f = fopen(filename, "w+");
	fprintf(f, "%s", txt);
	fclose(f);
}

static int save_all_data(const char* deviceName, int nSamples, int bufferSize, int channel){
	int fd = open(deviceName, O_RDWR);
	char filename[32];
	memset(filename, 0, 32);
	sprintf(filename, "signal_%d.data", channel);
	FILE *dataFile = fopen(filename, "w+");
	memset(filename, 0, 32);
	sprintf(filename, "signal_raw_%d.data", channel);
	FILE *dataFileRaw = fopen(filename, "w+");
	memset(filename, 0, 32);
	sprintf(filename, "signalError_%d.data", channel);
	FILE *dataFileError = fopen(filename, "w+");
	memset(filename, 0, 32);
	sprintf(filename, "signalDer_%d.data", channel);
	FILE *dataFileDer = fopen(filename, "w+");

	int *buffer = (int *)malloc(nSamples * sizeof(int));
	int *copyBuffer = (int *)malloc(bufferSize);
	int ret = ioctl(fd, PCIE_ADC_IOCT_INT_ENABLE);
	int j = 0;
	int inc = bufferSize / sizeof(int);
	float value = 0;
	float oldValue = 0;
	time_t start;
	time_t end;
	int total_time;

	memset(last4Bits, 0, sizeof(int) * 16);

	time(&start);
	for(j=0; j<nSamples; j+=inc){
		memset(copyBuffer, 0, bufferSize);
		ret = read(fd, copyBuffer, bufferSize);
		memcpy(&buffer[j], copyBuffer, ret);
	}
	time(&end);
	total_time = end - start;
	
	for(j=0; j<nSamples; j++){
		fprintf(dataFileRaw, "0x%x\n", buffer[j]);
		last4Bits[buffer[j] & 0xf]++;
		//recover the sign
		buffer[j] <<= 14;
		buffer[j] >>= 14;
		value = buffer[j] * 32. / 131072;
		fprintf(dataFile, "%f\n", value);
		fprintf(dataFileDer, "%f\n", (value - oldValue));
		oldValue = value;
	}

	for(j=0; j<16; j++){
		fprintf(dataFileError, "%d\n", last4Bits[j]);
	}

	close(fd);
	fclose(dataFile);
	fclose(dataFileError);
	fclose(dataFileRaw);
	fclose(dataFileDer);
	createGNUPlotFile1(channel);
	createGNUPlotFile2(channel);
	createGNUPlotFile3(channel);
	memset(filename, 0, 32);
        sprintf(filename, "gnuplot chart_%d.gnuplot", channel);
	system(filename);
	memset(filename, 0, 32);
        sprintf(filename, "gnuplot chartDer_%d.gnuplot", channel);
	system(filename);
	memset(filename, 0, 32);
        sprintf(filename, "gnuplot charterror_%d.gnuplot", channel);
	system(filename);
	free(copyBuffer);
	free(buffer);
	return total_time;
}

static void show_data(struct shttpd_arg *arg){
	char readBuffer[24];
	int numberOfSamples = 2048;
	int channel = 0;
	int bufferSize = 64;
	int showDer = 0;
	int show4Bits = 0;
	static int running = 0;
	const char* query_string = shttpd_get_env(arg, "QUERY_STRING");
	int i = 0;
	char *checked1 = "Checked";
	char *checked2 = "Checked";

	if(query_string != NULL){
		memset(readBuffer, 0, 24);
		(void) shttpd_get_var("nsamples", query_string, strlen(query_string), readBuffer, sizeof(readBuffer));
		if(readBuffer[0] != '\0'){
			numberOfSamples = atol(readBuffer);
		}
		memset(readBuffer, 0, 24);	
		(void) shttpd_get_var("bsize", query_string, strlen(query_string), readBuffer, sizeof(readBuffer));
		if(readBuffer[0] != '\0'){
			bufferSize = atol(readBuffer);
		}
		memset(readBuffer, 0, 24);	
		(void) shttpd_get_var("running", query_string, strlen(query_string), readBuffer, sizeof(readBuffer));
		if(readBuffer[0] != '\0' && running != 2){
			running = atol(readBuffer);
		}
		memset(readBuffer, 0, 24);
		(void) shttpd_get_var("channel", query_string, strlen(query_string), readBuffer, sizeof(readBuffer));
		if(readBuffer[0] != '\0'){			
			channel = atol(readBuffer);
			if(channel > 31 || channel < -1){
				shttpd_printf(arg, "<p><b>Invalid channel: %d</b></p>\n", channel);
				shttpd_printf(arg, "<p><b>-1&lt;channel&lt;32</b></p>\n");
				shttpd_printf(arg, "%s", "<p><a href=\"./adc\">Back</b></p>\n");
				arg->flags |= SHTTPD_END_OF_OUTPUT;
				channel = 0;
				running = 0;
				return;				
			}
		}
		memset(readBuffer, 0, 24);
		(void) shttpd_get_var("derok", query_string, strlen(query_string), readBuffer, sizeof(readBuffer));
		if(readBuffer[0] != '\0'){
			showDer = atol(readBuffer);
		}
		memset(readBuffer, 0, 24);
		(void) shttpd_get_var("last4ok", query_string, strlen(query_string), readBuffer, sizeof(readBuffer));
		if(readBuffer[0] != '\0'){
			show4Bits = atol(readBuffer);
		}

		if(showDer != 1){
			checked1 = "";
		}
		else{
			checked1 = "checked";
		}

		if(show4Bits != 1){
			checked2 = "";
		}
		else{
			checked2 = "checked";
		}
		sprintf(devName, "/dev/pcie%d", channel);
	}
        	
	if(running < 2){
		shttpd_printf(arg, "%s", "HTTP/1.1 200 OK\r\n");
	        shttpd_printf(arg, "%s", "Content-Type: text/html\r\n\r\n");
		shttpd_printf(arg, "%s", "<html><body>\r\n\r\n");
	}
	if(running == 1){
		shttpd_printf(arg, "%s", "<b>Please wait...Acquiring data from the board...</b>\n");
		running++;
		return;
	}
	shttpd_printf(arg, "<form method=\"GET\">Number of samples: "
		           "<input type=\"text\" name=\"nsamples\" value=\"%d\"/></br>"
			   "Channel number:"
		           "<input type=\"text\" name=\"channel\" value=\"%d\"/></br>"
			   "Buffer size:"
		           "<input type=\"text\" name=\"bsize\" value=\"%d\"/></br>"
		           "Show derivative results:"			
                           "<input type=\"checkbox\" name=\"derok\" value=\"1\" %s/></br>"
			   "Show last 4 bits results:"
		           "<input type=\"checkbox\" name=\"last4ok\" value=\"1\" %s/></br>"
			   "<input type=\"hidden\" name=\"running\" value=\"1\">"
                	   "<input type=\"submit\" "
		           "value=\"Start\"></form>", numberOfSamples, channel, bufferSize, checked1, checked2);
	if(running == 2){
		while(numberOfSamples < (bufferSize / 4))
			numberOfSamples++;
		while((numberOfSamples % (bufferSize / 4)) != 0)
			numberOfSamples++;

		for(i = 0; i<MAX_CHANNELS; i++){
			if(channel != -1){
				i = channel;
			}
			printf("Counter = %d\n", i);
			sprintf(devName, "/dev/pcie%d", i);
			int tottime = save_all_data(devName, numberOfSamples, bufferSize, i);
	
			shttpd_printf(arg, "<p>To read %d samples from channel %d took me %d seconds\n</p>", numberOfSamples, i, tottime);
			shttpd_printf(arg, "<img src=\"signal_%d.png\"/>\r\n\r\n", i);
			if(showDer == 1){
				shttpd_printf(arg, "<img src=\"signalDer_%d.png\"/>\r\n\r\n", i);
			}
			if(show4Bits == 1){
				shttpd_printf(arg, "<img src=\"signalError_%d.png\"/>\r\n\r\n", i);
			}
			shttpd_printf(arg, "<p><a href=\"signal_%d.data\">signal_%d.data</a></p>\r\n\r\n", i, i);
			shttpd_printf(arg, "<p><a href=\"signalDer_%d.data\">signalDer_%d.data</a></p>\r\n\r\n", i, i);
			shttpd_printf(arg, "<p><a href=\"signal_raw_%d.data\">signal_raw_%d.data</a></p>\r\n\r\n", i, i);
			if(channel != -1){
				break;
			}
		}
	}
        shttpd_printf(arg, "%s", "</body></html>\r\n\r\n");
	arg->flags |= SHTTPD_END_OF_OUTPUT;
	running = 0;
}

int main(int argc, char *argv[])
{
	struct shttpd_ctx	*ctx;

	/* Get rid of warnings */
	argc = argc;
	argv = argv;

	signal(SIGPIPE, SIG_IGN);

	/*
	 * Initialize SHTTPD context.
	 * Attach folder c:\ to the URL /my_c  (for windows), and
	 * /etc/ to URL /my_etc (for UNIX). These are Apache-like aliases.
	 * Set WWW root to current directory.
	 * Start listening on ports 8080 and 8081
	 */
	ctx = shttpd_init(NULL,
		"aliases", ALIAS_URI "=" ALIAS_DIR,
		"document_root", ".", NULL);

	shttpd_register_uri(ctx, "/adc", &show_data, NULL);

	shttpd_listen(ctx, 9000, 0);

	/* Serve connections infinitely until someone kills us */
	for (;;)
		shttpd_poll(ctx, 1000);

	/* Probably unreached, because we will be killed by a signal */
	shttpd_fini(ctx);

	return (EXIT_SUCCESS);
}
