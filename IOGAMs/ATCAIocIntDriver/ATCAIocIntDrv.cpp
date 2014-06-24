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
 * SVN keywords:
 * $URL$
 * $Id$
 *
**/

#include "ATCAIocIntDrv.h"

#include "Endianity.h"
#include "CDBExtended.h"
#include "FastPollingMutexSem.h"
//#include "atca-ioc-int-lib.h"
//#include "atca-ioc-int.h"
#include "atca-ioc-int-ioctl.h"


//#define _FAKE_DEV

#define DMA_MAX_BYTES (4096 * 32) //PAGE_SIZE // 4096  Linux page size = 2048 samples

#define NCHANNELS 16
#define DMA_SIZE (DMA_MAX_BYTES/16/2/32) //  32 ok

//#define NUM_CHAN_SMP 16// nr of 32bit data channels per sample.

#define SAMP_PER_PACKET (DMA_SIZE/NCHANNELS/sizeof(int32_t)) // 2048

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

struct TimeHeaderStruct{
    unsigned int nSampleNumber; // the sample number since the last t=0
    unsigned int nSampleTime;   // the time since t=0, going to PRE as microseconds 
    unsigned int dummyData[14]; // values from first packet
    unsigned int channelData[16];  // values from last packet, channels 0-14, time on chann 15
};

// Timing ATCAIocInt module
static const int32 timingATCAIocIntDrv = 400;

/**
 * Enable System Acquisition
 */
bool ATCAIocIntDrv::EnableAcquisition(){
  // Initialise lastPacketID equal to 0xFFFFFFFF
  lastPacketID = 0xFFFFFFFF;
  // Set the chip on line

  if (liveness!=-1){
    AssertErrorCondition(Warning, "ATCAIocIntDrv::EnableAcquisition: ATCA device  already alive");
    return False;
  }
  devFd = open(deviceFileName.Buffer(), O_RDWR);
  if(devFd < 1){
    AssertErrorCondition(InitialisationError,"ATCAIocIntDrv::EnableAcquisition: %s: Could not open device node: %s",Name(), deviceFileName.Buffer());
    return False;
  }
  init_device(devFd);

  liveness++;
  return True;
}

/**
 * Disable System Acquisition
 */
bool ATCAIocIntDrv::DisableAcquisition(){

  // close ATCA IOC 
  if (liveness==-1){
    AssertErrorCondition(Warning, "ATCAIocIntDrv::DisableAcquisition: ATCA Board device not open");
    return False;
  }
  int max_buf_count= stop_device(devFd);
  if(max_buf_count > 16){
    AssertErrorCondition(InitialisationError,"ATCAIocIntDrv::DisableAcquisition: %s: DMA buffers overrun: %d: %s",Name(), max_buf_count, deviceFileName.Buffer());
    return False;
  }
  close(devFd);
  liveness--;
  return True;
}

/*
 * Constructors
 */
bool  ATCAIocIntDrv::Init(){
  // Init general parameters
  devFd                                   = 0;
  moduleIdentifier                        = 0;
  numberOfAnalogueInputChannels           = 0;
  numberOfDigitalInputChannels            = 0;
  numberOfAnalogueOutputChannels          = 0;
  numberOfDigitalOutputChannels           = 0;
//moduleType                          = ATCAIOCMODULE_UNDEFINED;
  mux.Create();
  packetByteSize                      = 0;
  // Init receiver parameters
  writeBuffer                         = 0;
  globalReadBuffer                    = 0;
  sizeMismatchErrorCounter            = 0;
  previousPacketTooOldErrorCounter    = 0;
  deviationErrorCounter               = 0;
  lostPacketErrorCounter              = 0;
  lostPacketErrorCounterAux           = 0;
  samePacketErrorCounter              = 0;
  recoveryCounter                     = 0;
  maxDataAgeUsec                      = 20000;
  lastPacketID                        = 0xFFFFFFFF;
  lastPacketUsecTime                  = 0;
  liveness                            = -1;
  keepRunning                         = False;
  //producerUsecPeriod                  = -1;
  //originalNSampleNumber               = 0;
  lastCounter                         = 0;
  cpuMask                             = 0xFFFF;
  receiverThreadPriority              = 0;
  freshPacket                         = False;
  // reset all buffers pointers 
  //for(int i = 0 ; i < nOfDataBuffers ; i++) {
  //  dataBuffer[i] = NULL;
  //}
  return True;
}

ATCAIocIntDrv::ATCAIocIntDrv(){
  Init();
}

/*
 * Destructor
 */
ATCAIocIntDrv::~ATCAIocIntDrv(){
  keepRunning = False;
  DisableAcquisition();
  //if(moduleType == ATCAIOCMODULE_RECEIVER) {
    int counter = 0;
    while((!keepRunning) && (counter++ < 100)) SleepMsec(1);
    if(Threads::IsAlive(threadID)) {
      AssertErrorCondition(Warning,"ATCAIocIntDrv::~ATCAIocIntDrv: %s: Had To Kill Thread %d",Name(), threadID);
      Threads::Kill(threadID);
      threadID = 0;
    }
    else{
      AssertErrorCondition(Information,"ATCAIocIntDrv::~ATCAIocIntDrv: %s: Successfully waited for Thread %d to die on its own",Name(), threadID);
    }
    //}
  // Free memory
    for(int i = 0 ; i < nOfDataBuffers ; i++) {
      if(dataBuffer[i] != NULL)free((void *&)dataBuffer[i]);
    }        

}

/**
 * ObjectLoadSetup
 */
bool ATCAIocIntDrv::ObjectLoadSetup(ConfigurationDataBase &info,StreamInterface *err){    
  // Disable previous opened connections
  DisableAcquisition();
  // Parent class Object load setup
  CDBExtended cdb(info);
  if(!GenericAcqModule::ObjectLoadSetup(info,err)){
    AssertErrorCondition(InitialisationError,"ATCAIocIntDrv::ObjectLoadSetup: %s GenericAcqModule::ObjectLoadSetup Failed",Name());
    return False;
  }


  // Read ModuleType IN/OUT
  // FString module;
  // if(!cdb.ReadFString(module,"ModuleType")){
  //   AssertErrorCondition(InitialisationError,"ATCAIocIntDrv::ObjectLoadSetup: %s did not specify ModuleType entry",Name());
  //   return False;
  // }
  // if(module == "InputModule"){
  //   moduleType = ATCAIOCMODULE_RECEIVER;
  // }else if(module == "OutputModule"){
  //   moduleType = ATCAIOCMODULE_TRANSMITTER;
  // }else{
  //   AssertErrorCondition(InitialisationError,"ATCAIocIntDrv::ObjectLoadSetup: %s unknown module type %s",Name(),module.Buffer());
  //   return False;
  // }

  if(!cdb.ReadFString(deviceFileName,"DeviceFileName")){
    AssertErrorCondition(InitialisationError,"ATCAIocIntDrv::ObjectLoadSetup: %s: DeviceFileName has to be specified",Name());
    return False;
  }
  else {
    AssertErrorCondition(Warning, "ATCAIocIntDrv::ObjectLoadSetup: %s, DeviceFileName: %s Opened", Name(), deviceFileName.Buffer());
  }

  cpuMask = 0xFFFF;

  // Read cpu mask
  if(!cdb.ReadInt32(cpuMask, "CpuMask", 0xFFFF)){
    AssertErrorCondition(Warning,"ATCAIocIntDrv::ObjectLoadSetup: %s CpuMask was not specified. Using default: %d",Name(),cpuMask);
  }

  // Based on mudule type, init a recv or a send ATCAIOC channel
  //if(moduleType == ATCAIOCMODULE_RECEIVER){
    /////////////////////////
    // Input Module (recv) //
    /////////////////////////

    // Read MaxDataAgeUsec param
    if (!cdb.ReadInt32(maxDataAgeUsec, "MaxDataAgeUsec", 20000)){
      AssertErrorCondition(Warning,"ATCAIocIntDrv::ObjectLoadSetup: %s did not specify MaxDataAgeUsec entry. Assuming %i usec" ,Name(),maxDataAgeUsec);
      //}
    // Read MacNOfLostPackets
    cdb.ReadInt32(maxNOfLostPackets, "MaxNOfLostPackets", 4);

    if(cdb.ReadInt32(receiverThreadPriority, "ThreadPriority", 0)) {
      if(receiverThreadPriority > 32 || receiverThreadPriority < 0) {
	AssertErrorCondition(InitialisationError, "ATCAIocIntDrv::ObjectLoadSetup: %s ThreadPriority parameter must be <= 32 and >= 0", Name());
	return False;
      }
    } else {
      AssertErrorCondition(Warning, "ATCAIocIntDrv::ObjectLoadSetup: %s ThreadPriority parameter not specified", Name());
    }

    if(!cdb.ReadInt32(numberOfInputChannels, "NumberOfInputs")){
      CStaticAssertErrorCondition(InitialisationError,"SingleATCAModule::ObjectLoadSetup: NumberOfInputs has not been specified.");
      return False;
    }
	
    packetByteSize = 128;
    /// Read the UsecPeriod of the ATCAIocInt packet producer
    //        cdb.ReadInt32(producerUsecPeriod, "ProducerUsecPeriod", -1);

    // Create Data Buffers. Compute total size and allocate storing buffer 
    for(int i=0 ; i < nOfDataBuffers ; i++){
      dataBuffer[i] = (uint32 *)malloc(packetByteSize);//numberOfInputChannels*sizeof(int));
      if(dataBuffer[i] == NULL){
        AssertErrorCondition(InitialisationError,"ATCAIocIntDrv::ObjectLoadSetup: %s ATCAIocInt dataBuffer allocation failed",Name());
        return False;
      }

    // TODO Initialize the triple buffer
      //uint32 *tempData = dataBuffer[i];
      //for(int j = 0 ; j < numberOfInputChannels ; j++) {
      //tempData[j] = 0;
      //}
    
    }
    //    packetByteSize = numberOfInputChannels*sizeof(int32);

  }
  /* else if(moduleType == ATCAIOCMODULE_TRANSMITTER) {
    //////////////////////////
    // Output Module (send) //
    //////////////////////////
    // NOt yet implemented
  }
  */
  if(!EnableAcquisition()) {
    AssertErrorCondition(InitialisationError, "ATCAIocIntDrv::ObjectLoadSetup Failed Enabling Acquisition");
    return False;
  }

  keepRunning = False;
  FString threadName = Name();
  threadName += "ATCAIocIntHandler";
  //  if(moduleType == ATCAIOCMODULE_RECEIVER) {
    threadID = Threads::BeginThread((ThreadFunctionType)ReceiverCallback, (void*)this, THREADS_DEFAULT_STACKSIZE, threadName.Buffer(), XH_NotHandled, cpuMask);
    int counter = 0;
    while((!keepRunning)&&(counter++ < 100)) SleepMsec(1);
    if(!keepRunning) {
      AssertErrorCondition(InitialisationError, "ATCAIocIntDrv::ObjectLoadSetup: ReceiverCallback failed to start");
      return False;
    }
    //}

  // Tell user the initialization phase is done
  AssertErrorCondition(Information,"ATCAIocIntDrv::ObjectLoadSetup:: ATCAIOC Module %s Correctly Initialized - DEVI --> %s",Name(), deviceFileName.Buffer());

  return True;
}

/**
 * GetData
 */
int32 ATCAIocIntDrv::GetData(uint32 usecTime, int32 *buffer, int32 bufferNumber) {

  // Check module type
  // if(moduleType!=ATCAIOCMODULE_RECEIVER) {
  //    AssertErrorCondition(FatalError,"ATCAIocIntDrv::GetData: %s is not a receving module on fd %d",Name(), fileDescriptor);
  //return -1;
  //}

  // check if buffer is allocated
  if(buffer == NULL) {
    AssertErrorCondition(FatalError,"ATCAIocIntDrv::GetData: %s. The DDInterface buffer is NULL.",Name());
    return -1;
  }

  // Make sure that, while writeBuffer is being
  // used to update globalReadBuffer, it is not being
  // changed in the receiver thread callback
  while(!mux.FastTryLock());

 // Update readBuffer index
  globalReadBuffer = writeBuffer - 1;
  if(globalReadBuffer < 0) {
    globalReadBuffer = nOfDataBuffers-1;
  }
  // Gets the last acquired data buffer
  uint32 * lastReadBuffer     = dataBuffer[globalReadBuffer];
  TimeHeaderStruct    *header = (TimeHeaderStruct *)lastReadBuffer;
  // Check data age
  uint32 sampleNo = header->nSampleNumber;
  if(freshPacket) {
    if(abs(usecTime-header->nSampleTime) > maxDataAgeUsec) {
      // Packet too old
      // return the last received data and put 0xFFFFFFFF as nSampleNumber
      sampleNo = 0xFFFFFFFF;
      previousPacketTooOldErrorCounter++;
      freshPacket = False;
    }
  } else {
    sampleNo = 0xFFFFFFFF;
    previousPacketTooOldErrorCounter++;
  }
  

  // Give back lock
  mux.FastUnLock();
  
  // Copy the data from the internal buffer to
  // the one passed in GetData
  uint32 *destination = (uint32 *)buffer;
  //uint32 *destinationEnd = (uint32 *)(buffer + packetByteSize/sizeof(int32));
  //  uint32 *destinationEnd = (uint32 *)(buffer + numberOfInputChannels);
  // uint32 *source = lastReadBuffer;
  destination[0] = header->nSampleNumber;// *source++;
  destination[1] = header->nSampleTime;// *source++;

  for(int i = 2 ; i < numberOfInputChannels; i++) {
    destination[i]  =header->channelData[i];   //  dataBuffer[i] = NULL;
  }
    //  while(destination < destinationEnd) {
    //*destination++ = *source++;
    //}

  TimeHeaderStruct *p = (TimeHeaderStruct *)buffer;
  p->nSampleNumber = sampleNo;
  return 1;
}

/**
 * WriteData
 */
bool ATCAIocIntDrv::WriteData(uint32 usecTime, const int32 *buffer){
  // check module type
  //  if(moduleType!=ATCAIOCMODULE_TRANSMITTER){
  AssertErrorCondition(FatalError,"ATCAIocIntDrv::WriteData: %s is not a transmitter module ",Name());
  return False;
  // }
  // NOT implemented yet
  // check if buffer is not allocated
  // if(buffer == NULL){
  //   AssertErrorCondition(FatalError,"ATCAIocIntDrv::WriteData: %s. The DDInterface buffer is NULL.",Name());
  //   return False;
  // }
  /*
  // Set the packet content - does endianity swap
  int size = packetByteSize;
  Endianity::MemCopyToMotorola((uint32*)outputPacket, (uint32*)buffer, numberOfOutputChannels);
  // Send packet
  int _ret;
  _ret = send(atmSocket, outputPacket, size, 0);
  if(_ret == -1){
    AssertErrorCondition(FatalError,"ATCAIocIntDrv::WriteData: %s. Send socket error",Name());
    return False;
  }
  */
  return True;
}

/**
 * InputDump
 */ 
bool ATCAIocIntDrv::InputDump(StreamInterface &s) const {
  // Checks for the I/O type
  // if(moduleType != ATCAIOCMODULE_RECEIVER) {
  //   s.Printf("%s is not an Input module\n", Name());
  //   return False;
  // }
  // Prints some usefull informations about the input board
  s.Printf("%s - ATCAIOC board attached at  #s\n", Name(),deviceFileName.Buffer());
  return True;
}

/**
 * OutputDump

bool ATCAIocIntDrv::OutputDump(StreamInterface &s) const{
  // Checks for the I/O type
  if(moduleType != ATCAIOCMODULE_TRANSMITTER){
    s.Printf("%s is not an Output module\n",Name());
    return False;
  }
  // Prints some usefull informations about the input board
  //s.Printf("%s - ATM board attached at VCI #%d\n\n",Name(),vci);
  return True;
}
 */ 

/**
 * GetUsecTime
 */
int64 ATCAIocIntDrv::GetUsecTime(){

  // Check module type
  // if (moduleType!=ATCAIOCMODULE_RECEIVER){
  //   AssertErrorCondition(FatalError,"GetUsecTime:This method can only be called an input ATCAIocInt channel");
  //   return 0xFFFFFFFF;
  // }

  if(producerUsecPeriod != -1) {
    return ((int64)originalNSampleNumber*(int64)producerUsecPeriod);
  } else {
    return lastPacketUsecTime;
  }
}

/**
 * ObjectDescription
 */
bool ATCAIocIntDrv::ObjectDescription(StreamInterface &s, bool full, StreamInterface *err){
  s.Printf("%s %s\n",ClassName(),Version());
  // Module name
  s.Printf("Module Name --> %s\n",Name());
  // VCI Parameters
  //s.Printf("VCI No                   = %d\n",vci);
//  s.Printf("MaxDataAgeUsec           = %d\n",maxDataAgeUsec);
  s.Printf("MaxNOfLostPackets        = %d\n",maxNOfLostPackets);
  // VCI Type
  // switch (moduleType){
  // case 0:
  //   s.Printf("ATCA Type --> RECEIVER");
  //   break;
  // case 1:
  //   s.Printf("ATCAType --> TRANSMITTER");
  //   break;
  // default:
  //   s.Printf("ATACType --> UNDEFINED");
  // }
  //  if(vci == timingATCAIocIntDrv) s.Printf("\nThis is even a Time Module");
  return True;
}

/**
 * Receiver CallBack
 */
void ReceiverCallback(void* userData){
  ATCAIocIntDrv *p = (ATCAIocIntDrv*)userData;   
  p->RecCallback(userData);
}


/**
 * RecCallback
 */
void ATCAIocIntDrv::RecCallback(void* arg){

  // Set Thread priority
  if(receiverThreadPriority) {
    Threads::SetRealTimeClass();
    Threads::SetPriorityLevel(receiverThreadPriority);
  }

  // Allocate
  int32 *dataSource;
  if((dataSource = (int32 *)malloc(packetByteSize)) == NULL) {
    AssertErrorCondition(FatalError, "ATCAIocIntDrv::RecCallback: unable to allocate buffer on receiver thread exiting");
    return;
  }
  keepRunning = True;	

  int64 lastCounterTime = 0;
  int64 oneMinCounterTime = HRT::HRTFrequency()*60;
  while(keepRunning) {

    // Print statistics of errors and warnings every minute
    int64 currentCounterTime = HRT::HRTCounter();
    if(currentCounterTime - lastCounterTime > oneMinCounterTime) {
      if(sizeMismatchErrorCounter > 0) {
	AssertErrorCondition(FatalError, "ATCAIocIntDrv::RecCallback: ATCA Wrong packet size  [occured %i times]", 
			     sizeMismatchErrorCounter);
	sizeMismatchErrorCounter = 0;
      }
      // if(deviationErrorCounter > 0) {
      // 	AssertErrorCondition(Warning, "ATCAIocIntDrv::RecCallback: %s: Data arrival period mismatch with specified producer period [occured %i times]", Name(),  deviationErrorCounter);
      // 	deviationErrorCounter = 0;
      // }
      // if(rolloverErrorCounter > 0) {
      // 	AssertErrorCondition(Warning,"Lost more than %d packets after a reset on [occured %i times]",maxNOfLostPackets, rolloverErrorCounter);
      // 	rolloverErrorCounter = 0;
      // }
      // if(lostPacketErrorCounterAux > 0) {
      // 	AssertErrorCondition(Warning, "packets lost on  [occured %i times]",  lostPacketErrorCounterAux);
      // 	lostPacketErrorCounterAux = 0;
      // }
      // if(samePacketErrorCounter > 0) {
      // 	AssertErrorCondition(Warning, "nSampleNumber unchanged in  [occured %i times]",  samePacketErrorCounter);
      // 	samePacketErrorCounter = 0;
      // }
      // if(recoveryCounter > 0) {
      // 	AssertErrorCondition(Information, "Re-established correct packets sequence on  [occured %i times]",  recoveryCounter);
      // 	recoveryCounter = 0;
      // }
      // if(previousPacketTooOldErrorCounter > 0) {
      // 	AssertErrorCondition(Warning,"ATCAIocIntDrv::GetData: %s:  Data too old [occured %i times]",Name(),
      // 			     previousPacketTooOldErrorCounter);
      // 	previousPacketTooOldErrorCounter = 0;
      // }
      lastCounterTime = currentCounterTime;
    }

#ifndef _FAKE_DEV

    //read data from ATCA card
        int _ret = read(devFd, dataSource, packetByteSize);

    if(_ret == -1) {
      AssertErrorCondition(FatalError,"ATCAIocIntDrv::RecCallback: read() error");
      return;
    }

    // Check the buffer length
    if(_ret != packetByteSize) {
      sizeMismatchErrorCounter++;
      continue;
    }
    dataSource[0] = lastPacketID;
    dataSource[1] = dataSource[31]/2; // Time counter information from board in 0.5 us resolut.

#else
    //Fake read
    SleepSec(1E-3);
    dataSource[0] = lastPacketID;
    dataSource[1] = lastPacketUsecTime++;
    dataSource[2] = 11915;
#endif

    // Copy dataSource in the write only buffer; does endianity swap
    Endianity::MemCopyFromIntel((uint32 *)dataBuffer[writeBuffer], (uint32 *)dataSource,
				packetByteSize/sizeof(int32));
    //printf("%d %d\n", writeBuffer, dataBuffer[0][0]);
    // Checks if packets have been lost
    TimeHeaderStruct *header = (TimeHeaderStruct *)dataBuffer[writeBuffer];

    // Make sure that while writeBuffer is being
    // updated it is not being read elsewhere
    while(!mux.FastTryLock());
    // Update buffer index
    writeBuffer++;
    if(writeBuffer >= nOfDataBuffers) 
      writeBuffer = 0;
    //}
    freshPacket = True;
    // Unlock resource
    mux.FastUnLock();
    /*
    if(producerUsecPeriod != -1) {
      int64 counter = HRT::HRTCounter();
      /// Allow for a 10% deviation from the specified producer usec period
      if(abs((uint32)((counter-lastCounter)*HRT::HRTPeriod()*1000000)-(uint32)((header->nSampleNumber-originalNSampleNumber)*producerUsecPeriod)) > 0.1*producerUsecPeriod) {
	deviationErrorCounter++;
      }
      originalNSampleNumber = header->nSampleNumber;
      lastCounter = counter;
    }

    // If is the first packet doesn't do any check on nSampleNumber
    if(lastPacketID != 0xFFFFFFFF) {
      // Checks nSampleNumber
      // Warning if a reset has happened and too much packet had been lost
      // nSampleNumber has been casted to int32 to prevent wrong casting from compiler
      if((int32)(header->nSampleNumber)-lastPacketID < 0) {
	if((int32)(header->nSampleNumber) > maxNOfLostPackets) {
	  rolloverErrorCounter++;
	}
      } else {
	// nSampleNumber has been casted to int32 to prevent wrong casting from compiler
	if(((int32)(header->nSampleNumber)-lastPacketID) > 1) {
	  // Warning if a packet is lost
	  lostPacketErrorCounter++;
	  lostPacketErrorCounterAux++;
	} else if(((int32)(header->nSampleNumber)-lastPacketID) == 0) {
	  // Warning if nSampleNumber in packet hasn't changed
	  samePacketErrorCounter++;
	} else if(lostPacketErrorCounter > 0) {
	  recoveryCounter++;
	  // Reset error counter
	  lostPacketErrorCounter = 0;
	}
      }
    }
    */
    // Update lastPacketID
    //    lastPacketID       = header->nSampleNumber;
    //lastPacketUsecTime = header->nSampleTime;

    // Update 
    lastPacketID++;
    lastPacketUsecTime = header->nSampleTime;
    // If the module is the timingATCAIocIntDrv call the Trigger() method of
    // the time service object
    for(int i = 0 ; i < nOfTriggeringServices ; i++) {
      triggerService[i].Trigger();
    }

  }
  keepRunning = True;	
}

bool ATCAIocIntDrv::ProcessHttpMessage(HttpStream &hStream) {
  hStream.SSPrintf("OutputHttpOtions.Content-Type","text/html");
  hStream.keepAlive = False;
  //copy to the client
  hStream.WriteReplyHeader(False);

  hStream.Printf("<html><head><title>LinuxATCAIocIntDrv</title></head>\n");
  hStream.Printf("<body>\n");
  /*
  hStream.Printf("<h1 align=\"center\">%s</h1>\n", Name());
  if(moduleType == ATMMODULE_RECEIVER) {
    hStream.Printf("<h2 align=\"center\">Type = %s</h2>\n", "Receiver");
  } else if(moduleType == ATMMODULE_TRANSMITTER) {
    hStream.Printf("<h2 align=\"center\">Type = %s</h2>\n", "Transmitter");
  } else {
    hStream.Printf("<h2 align=\"center\">Type = %s</h2>\n", "Undefined");
  }
  hStream.Printf("<h2 align=\"center\"> %d</h2>\n", vci);
  if(moduleType == ATMMODULE_RECEIVER) {
    hStream.Printf("<h2 align=\"center\">Input channels = %d</h2>\n", numberOfInputChannels);
    hStream.Printf("<h2 align=\"center\">MaxDataAgeUsec = %d</h2>\n", maxDataAgeUsec);
    hStream.Printf("<h2 align=\"center\">MaxNOfLostPackets = %d</h2>\n", maxNOfLostPackets);
    hStream.Printf("<h2 align=\"center\">CPU mask = 0x%x</h2>\n", cpuMask);
    hStream.Printf("<h2 align=\"center\">Thread priority = %d</h2>\n", receiverThreadPriority);
  } else if(moduleType == ATMMODULE_TRANSMITTER) {
    hStream.Printf("<h2 align=\"center\">Output channels = %d</h2>\n", numberOfOutputChannels);
  }

  if(moduleType == ATMMODULE_RECEIVER) {
    // Data table 
    hStream.Printf("<table border=\"1\" align=\"center\">\n");
    hStream.Printf("<tr>\n");
    hStream.Printf("<th></th>\n");
    hStream.Printf("<th>Buffer(k)</th>\n");
    hStream.Printf("<th>Buffer(k-1)</th>\n");
    hStream.Printf("<th>Buffer(k-2)</th>\n");
    hStream.Printf("</tr>\n");

    int32 idx;

    hStream.Printf("<tr>\n");
    hStream.Printf("<td>Sample Number</td>\n");
    for(int32 i = 0 ; i < nOfDataBuffers ; i++) {
      idx = writeBuffer-1-i;
      if(idx < 0) idx = nOfDataBuffers-(int32)fabs(idx);
      if(dataBuffer[idx] != NULL) {
	hStream.Printf("<td>%u</td>", *(dataBuffer[idx]+0));
      }
    }
    hStream.Printf("</tr>\n");

    hStream.Printf("<tr>\n");
    hStream.Printf("<td>Sample Time (usec)</td>\n");
    for(int32 i = 0 ; i < nOfDataBuffers ; i++) {
      idx = writeBuffer-1-i;
      if(idx < 0) idx = nOfDataBuffers-(int32)fabs(idx);
      if(dataBuffer[idx] != NULL) {
	hStream.Printf("<td>%u</td>", *(dataBuffer[idx]+1));
      }
    }
    hStream.Printf("</tr>\n");

    hStream.Printf("<tr>\n");
    hStream.Printf("<td>Packet Id</td>\n");
    for(int32 i = 0 ; i < nOfDataBuffers ; i++) {
      idx = writeBuffer-1-i;
      if(idx < 0) idx = nOfDataBuffers-(int32)fabs(idx);
      if(dataBuffer[idx] != NULL) {
	hStream.Printf("<td>%u</td>", *(dataBuffer[idx]+packetByteSize/sizeof(int32)-1));
      }
    }
    hStream.Printf("</tr>\n");
  }
*/
  hStream.Printf("</table>\n");
  hStream.Printf("</body></html>\n");

}
OBJECTLOADREGISTER(ATCAIocIntDrv, "$Id$")
