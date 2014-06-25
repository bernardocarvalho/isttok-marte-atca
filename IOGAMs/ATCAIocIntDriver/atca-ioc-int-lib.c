#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
//#include <linux/types.h>
//#include <math.h>
//#include <signal.h>
#include <string.h>
#include <sys/time.h>

//#include "atca-ioc-int.h"
#include "atca-ioc-int-ioctl.h"

#define DMA_SIZE 128 // (DMA_MAX_BYTES/16/2/32) //  32 ok

/***************/
int init_device(int fd)
{
  int  tmp, tmp0, rc,i;
  //OFFSET_REGS off_s;

  //  usleep(100);
  //rc = ioctl(fd, PCIE_ATCA_IOCG_STATUS, &tmp);// Get FPGA STATUS to check if properly initialized (optional)
  //printf("FPGA Status: 0x%.8X\n", tmp);

  rc = ioctl(fd, PCIE_ATCA_IOCG_COUNTER, &tmp0);
  printf("FPGA Counter: 0x%.8X, %d", tmp0, tmp0);
  usleep(10000);
  // sleep(1);
  rc = ioctl(fd, PCIE_ATCA_IOCG_COUNTER, &tmp);
  printf(" +10ms Counter: %d, diff: %d\n", tmp, tmp - tmp0);
  rc = ioctl(fd, PCIE_ATCA_IOCT_CHOP_ON); //Set the Chop on
  rc = ioctl(fd, PCIE_ATCA_IOCT_CHOP_DEFAULT_0); // TODO: Change ioctl name
  //The signal is  to be reconstruted inside the FPGA 
  rc = ioctl(fd, PCIE_ATCA_IOCT_CHOP_RECONSTRUCT_ON);

  tmp = DMA_SIZE;
  rc  =  ioctl(fd, PCIE_ATCA_IOCS_DMA_SIZE, &tmp);
  rc = ioctl(fd, PCIE_ATCA_IOCT_INTEGRAL_CALC_ON);

  //Set the Chop's period, in this case is 2000 times the period of the acquisition period.
  //(2000) The Chop's frequency will be 1kHz
  tmp = 2000;
  rc  =  ioctl(fd, PCIE_ATCA_IOCS_CHOP_MAX_COUNT, &tmp);
  tmp = 1000;
  rc = ioctl(fd, PCIE_ATCA_IOCS_CHOP_CHANGE_COUNT, &tmp);

  /* Reset ADC offsets*/
  for (i=0; i < 64; i++) {
    tmp = i;
    rc  =  ioctl(fd, PCIE_ATCA_IOCS_REG_OFF, &tmp);
    tmp = 0;
    rc = ioctl(fd, PCIE_ATCA_IOCS_REG_DATA, &tmp);
  }

  /* Set 0, 1 ADC offsets*/
  tmp = 0;
  rc  =  ioctl(fd, PCIE_ATCA_IOCS_REG_OFF, &tmp);
  tmp = -613;
  rc = ioctl(fd, PCIE_ATCA_IOCS_REG_DATA, &tmp);
  tmp = 1;
  rc  =  ioctl(fd, PCIE_ATCA_IOCS_REG_OFF, &tmp);
  tmp = -244;
  rc = ioctl(fd, PCIE_ATCA_IOCS_REG_DATA, &tmp);

  /* Set Integral offsets*/
  tmp = 32;
  rc  =  ioctl(fd, PCIE_ATCA_IOCS_REG_OFF, &tmp);
  tmp = - 12000 ; //- (65536 *  0.2);
  rc = ioctl(fd, PCIE_ATCA_IOCS_REG_DATA, &tmp);

  tmp = 33;
  rc  =  ioctl(fd, PCIE_ATCA_IOCS_REG_OFF, &tmp);
  tmp = -15000;// //- (65536 *  0.2);
  rc = ioctl(fd, PCIE_ATCA_IOCS_REG_DATA, &tmp);

  rc = ioctl(fd, PCIE_ATCA_IOCT_ACQ_ENABLE);  // Arm the FPGA to wait for external trigger
  usleep(10);
  //  rc = ioctl(fd, PCIE_ATCA_IOCG_STATUS, &tmp);
  //printf("FPGA ACQ Status: 0x%.8X\n", tmp);
  //tmp = 0;

  rc = ioctl(fd, PCIE_ATCA_IOCT_STREAM_ENABLE);
  //  rc = ioctl(fd, PCIE_ATCA_IOCG_STATUS, &tmp);
  //printf("FPGA STRE Status: 0x%.8X\n", tmp);
  rc = ioctl(fd, PCIE_ATCA_IOCT_SOFT_TRIG);
  usleep(10);
  //tmp = 0;
  //rc = ioctl(fd, PCIE_ATCA_IOCG_STATUS, &tmp);
  //printf("FPGA TRG Status: 0x%.8X\n", tmp);

  return rc;
}

int stop_device(int fd){

  int tmp, rc, max_buf_count;

  // this IOCTL returns the nr of times the driver IRQ handler was called while there was still 1 or more buffers waiting to be read
  max_buf_count = ioctl(fd, PCIE_ATCA_IOCT_ACQ_DISABLE);// Stop streaming and un-arm the FPGA.

  rc = ioctl(fd, PCIE_ATCA_IOCT_STREAM_DISABLE);
  usleep(100);

  rc = ioctl(fd, PCIE_ATCA_IOCT_INTEGRAL_CALC_OFF);

  rc = ioctl(fd, PCIE_ATCA_IOCG_STATUS, &tmp);
  printf("ACQ Stopped, FPGA  Status: 0x%.8X, max buff_count: %d \n", tmp, max_buf_count);
  // close(fd);
  return max_buf_count;
}
