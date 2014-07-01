/*
 * Copyright 2011 EFDA | European Fusion Development Agreement
 *
 * Licensed under the EUPL, Version 1.1 or - as soon they 
   will be approved by the European Commission - subsequent  
   versions of the EUPL (the "Licence"); 
 * You may not use this work except in compliance with the 
   Licence. 
 * You may obtain a copy of the Licence at: 
 *  
 * http://ec.europa.eu/idabc/eupl
 *
 * Unless required by applicable law or agreed to in 
   writing, software distributed under the Licence is 
   distributed on an "AS IS" basis, 
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
   express or implied. 
 * See the Licence for the specific language governing 
   permissions and limitations under the Licence. 
 *
 * $Id$
 *
**/

#include <stdio.h>
//#include <stdlib.h>
//#include <time.h>
#include <unistd.h>
//#include <errno.h>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <sys/ioctl.h>
#include <fcntl.h>

#include "ATCAIocBoard.h"
#include "atca-ioc-int-ioctl.h"

/***************/
int  ATCAIocBoard::write_reg(int reg, int val){
  int rc, tmp;
  /* Set ADC offsets*/ 
  tmp = reg;
  rc  =  ::ioctl(fD, PCIE_ATCA_IOCS_REG_OFF, &tmp);
  tmp = val;
  rc = ::ioctl(fD, PCIE_ATCA_IOCS_REG_DATA, &tmp);
  
  return rc;
}

int ATCAIocBoard::reset_offsets(){
  int rc;
  /* Reset ADC offsets*/ 
  for (int i=0; i < 64; i++) {
    rc = write_reg(i, 0);
  }
  return rc;
}

int ATCAIocBoard::write_adc_offset(int chan, int val){
  int rc;
  /* Set ADC offsets*/ 
  rc = write_reg(chan,  val);
  return rc;
}

int ATCAIocBoard::write_int_offset(int chan, int val){
  int rc, reg;
  /* Set ADC offsets*/ 
  reg = chan + 32;
  rc = write_reg(reg,  val);
  return rc;
}

bool ATCAIocBoard::dev_open(const char * fileName){
  fD = ::open(fileName, O_RDWR);
  if(fD < 1)
    return false;
  return true;
}

int ATCAIocBoard::read_packet(void * dataSource, int packetByteSize){
  //    int _ret = board.read_packet(dataSource, packetByteSize);
  return  ::read(fD, dataSource, packetByteSize);
}
int ATCAIocBoard::init_device()
{
  int  tmp, tmp0, rc,i;

  //rc = ioctl(fd, PCIE_ATCA_IOCG_STATUS, &tmp);// Get FPGA STATUS to check if properly initialized (optional)
  //printf("FPGA Status: 0x%.8X\n", tmp);

  rc = ioctl(fD, PCIE_ATCA_IOCG_COUNTER, &tmp0);
  printf("FPGA Counter: 0x%.8X, %d", tmp0, tmp0);
  usleep(10000);
  // sleep(1);
  rc = ioctl(fD, PCIE_ATCA_IOCG_COUNTER, &tmp);
  printf(" +10ms Counter: %d, diff: %d\n", tmp, tmp - tmp0);
  rc = ioctl(fD, PCIE_ATCA_IOCT_CHOP_ON); //Set the Chop on
  rc = ioctl(fD, PCIE_ATCA_IOCT_CHOP_DEFAULT_0); // TODO: Change ioctl name
  //The signal is  to be reconstruted inside the FPGA 
  rc = ioctl(fD, PCIE_ATCA_IOCT_CHOP_RECONSTRUCT_ON);

  tmp = DMA_SIZE;
  rc  =  ioctl(fD, PCIE_ATCA_IOCS_DMA_SIZE, &tmp);
  tmp = DMA_SIZE;
  rc  =  ioctl(fD, PCIE_ATCA_IOCS_DMA_THRES, &tmp);

  rc = ioctl(fD, PCIE_ATCA_IOCT_INTEGRAL_CALC_ON);

  //Set the Chop's period, in this case is 2000 times the period of the acquisition period.
  //(2000) The Chop's frequency will be 1kHz
  tmp = 2000;
  rc  =  ioctl(fD, PCIE_ATCA_IOCS_CHOP_MAX_COUNT, &tmp);
  tmp = 1000;
  rc = ioctl(fD, PCIE_ATCA_IOCS_CHOP_CHANGE_COUNT, &tmp);

  /* Reset ADC offsets*/
  for (i=0; i < 64; i++) {
    tmp = i;
    rc  =  ioctl(fD, PCIE_ATCA_IOCS_REG_OFF, &tmp);
    tmp = 0;
    rc = ioctl(fD, PCIE_ATCA_IOCS_REG_DATA, &tmp);
  }

  /* Set 0, 1 ADC offsets*/
  tmp = 0;
  rc  =  ioctl(fD, PCIE_ATCA_IOCS_REG_OFF, &tmp);
  tmp = -613;
  rc = ioctl(fD, PCIE_ATCA_IOCS_REG_DATA, &tmp);
  tmp = 1;
  rc  =  ioctl(fD, PCIE_ATCA_IOCS_REG_OFF, &tmp);
  tmp = -244;
  rc = ioctl(fD, PCIE_ATCA_IOCS_REG_DATA, &tmp);

  /* Set Integral offsets*/
  tmp = 32;
  rc  =  ioctl(fD, PCIE_ATCA_IOCS_REG_OFF, &tmp);
  tmp = - 12000 ; //- (65536 *  0.2);
  rc = ioctl(fD, PCIE_ATCA_IOCS_REG_DATA, &tmp);

  tmp = 33;
  rc  =  ioctl(fD, PCIE_ATCA_IOCS_REG_OFF, &tmp);
  tmp = -15000;// //- (65536 *  0.2);
  rc = ioctl(fD, PCIE_ATCA_IOCS_REG_DATA, &tmp);

  rc = ioctl(fD, PCIE_ATCA_IOCT_ACQ_ENABLE);  // Arm the FPGA to wait for external trigger

  usleep(10);
  //  rc = ioctl(fD, PCIE_ATCA_IOCG_STATUS, &tmp);
  //printf("FPGA ACQ Status: 0x%.8X\n", tmp);
  //tmp = 0;
  rc = ioctl(fD, PCIE_ATCA_IOCT_STREAM_ENABLE);
  usleep(10);
  //  rc = ioctl(fD, PCIE_ATCA_IOCG_STATUS, &tmp);
  //printf("FPGA STRE Status: 0x%.8X\n", tmp);
  //  rc = ioctl(fD, PCIE_ATCA_IOCT_SOFT_TRIG); // reset time counter on board
  //usleep(10);
  //tmp = 0;
  //rc = ioctl(fD, PCIE_ATCA_IOCG_STATUS, &tmp);
  //printf("FPGA TRG Status: 0x%.8X\n", tmp);

  return rc;
}

int ATCAIocBoard::stop_device(){

  int tmp, rc, max_buf_count;

  // this IOCTL returns the nr of times the driver IRQ handler was called while there was still 1 or more buffers waiting to be read
  max_buf_count = ioctl(fD, PCIE_ATCA_IOCT_ACQ_DISABLE);// Stop streaming and un-arm the FPGA.

  rc = ioctl(fD, PCIE_ATCA_IOCT_STREAM_DISABLE);
  usleep(100);

  rc = ioctl(fD, PCIE_ATCA_IOCT_INTEGRAL_CALC_OFF);

  rc = ioctl(fD, PCIE_ATCA_IOCG_STATUS, &tmp);
  printf("ACQ Stopped, FPGA  Status: 0x%.8X, max buff_count: %d \n", tmp, max_buf_count);
  // close(fD);
  return max_buf_count;
}


bool ATCAIocBoard::soft_trigger(){
    if(ioctl(fD, PCIE_ATCA_IOCT_SOFT_TRIG)>0){ // reset time counter on board
      usleep(10);
      return true;
    }
    return false;
}

void ATCAIocBoard::dev_close(){
  ::close(fD);
  fD = -1;
}

