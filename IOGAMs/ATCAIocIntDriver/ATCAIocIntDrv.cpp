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
//#include "atca-ioc-int-ioctl.h"


//#define _FAKE_DEV

//#define NCHANNELS 32


//#define SAMP_PER_PACKET (DMA_SIZE/NCHANNELS/sizeof(int32_t)) // 2048

//#define DMA_SIZE 128 // (DMA_MAX_BYTES/16/2/32) //  32 ok

//int init_device(int fd);
//int stop_device(int fd);


struct PacketStruct{
  unsigned int channelData[30];  // values from last packet receive, channels 0-14, time on channel 15
  unsigned int nSampleNumber;    // the sample number since the last t=0
  unsigned int nSampleTime;      // the time since t=0, going to PRE as microseconds 
  //  unsigned int dummyData[14];    // values from first packet -- to discard
  //unsigned int channelData[16];  // values from last packet receive, channels 0-14, time on channel 15
};

// Timing ATCAIocInt module
static const int32 timingATCAIocIntDrv = 400;

/**
 * Enable System Acquisition
 */
bool ATCAIocIntDrv::EnableAcquisition(){
  // Initialise lastPacketNo equal to 0xFFFFFFFF
  lastPacketNo = 0xFFFFFFFF;

  if (liveness!=-1){
    AssertErrorCondition(Warning, "ATCAIocIntDrv::EnableAcquisition: ATCA device  already alive");
    return False;
  }

#ifndef _FAKE_DEV
  if(board.dev_open(deviceFileName.Buffer())){
    AssertErrorCondition(Information,"ATCAIocIntDrv::EnableAcquisition: %s: Successfully open dev node %s", Name(), deviceFileName.Buffer());

  }
  else {
    AssertErrorCondition(InitialisationError,"ATCAIocIntDrv::EnableAcquisition: %s: Could not open device node: %s",Name(), deviceFileName.Buffer());
    return False;
  }

    /*
  devFd = open(deviceFileName.Buffer(), O_RDWR);
  if(devFd < 1){
    AssertErrorCondition(InitialisationError,"ATCAIocIntDrv::EnableAcquisition: %s: Could not open device node: %s",Name(), deviceFileName.Buffer());
    return False;
  }
  init_device(devFd);
    */
  board.init_device();
#else
  //  devFd = open("/dev/null", O_RDWR);
#endif

  liveness++;
  return True;
}

/**
 * Disable System Acquisition
 */
bool ATCAIocIntDrv::DisableAcquisition(){
  int max_buf_count;
  // close ATCA IOC 
  if (liveness==-1){
    AssertErrorCondition(Warning, "ATCAIocIntDrv::DisableAcquisition: ATCA Board device not open");
    return False;
  }
#ifndef _FAKE_DEV
  max_buf_count = board.stop_device();
#else
  max_buf_count=0;
#endif

  if(max_buf_count > 15){
    AssertErrorCondition(Warning,"ATCAIocIntDrv::DisableAcquisition: %s: DMA buffers overrun: %d on %s",Name(), max_buf_count, deviceFileName.Buffer());
    return False;
  }
  board.dev_close();
  //close(devFd);
  liveness--;
  return True;
}

/*
 * Constructors
 */
bool  ATCAIocIntDrv::Init(){
  // Init general parameters
  //devFd                                   = 0;
  moduleIdentifier                        = 0;
  numberOfAnalogueInputChannels           = 0;
  numberOfDigitalInputChannels            = 0;
  numberOfAnalogueOutputChannels          = 0;
  numberOfDigitalOutputChannels           = 0;
  softwareTrigger                         = 0;
  //moduleType                          = ATCAIOCMODULE_UNDEFINED;
  mux.Create();

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
  lastPacketNo                        = 0xFFFFFFFF;
  lastPacketUsecTime                  = 0;
  liveness                            = -1;
  keepRunning                         = False;
  producerUsecPeriod                  = -1;
  originalNSampleNumber               = 0;
  lastCounter                         = 0;
  cpuMask                             = 0xFFFF;
  receiverThreadPriority              = 0;
  freshPacket                         = False;
  // reset all buffers pointers 
  for(int i = 0 ; i < nOfDataBuffers ; i++) 
    dataBuffer[i] = NULL;
  
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

  if(!cdb.ReadFString(deviceFileName,"DeviceFileName")){
    AssertErrorCondition(InitialisationError,"ATCAIocIntDrv::ObjectLoadSetup: %s: DeviceFileName has to be specified",Name());
    return False;
  }
  //  else {
  // AssertErrorCondition(Warning, "ATCAIocIntDrv::ObjectLoadSetup: %s, DeviceFileName: %s Opened", Name(), deviceFileName.Buffer());
  //}

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

    /*    if(!cdb.ReadInt32(numberOf1InputChannels, "NumberOfInputs")){
      CStaticAssertErrorCondition(InitialisationError,"SingleATCAModule::ObjectLoadSetup: NumberOfInputs has not been specified.");
      return False;
    }
    */
    if(!cdb.ReadInt32(numberOfAnalogueInputChannels, "NumberOfAnalogueInputs")){
      CStaticAssertErrorCondition(InitialisationError,"ATCAIocIntDrv::ObjectLoadSetup: NumberOfAnalogueInputs has not been specified.");
      return False;
    }
    adc_offset_vector =  new int[numberOfAnalogueInputChannels];
   if(!cdb.ReadInt32Array(adc_offset_vector, (int *)(& numberOfAnalogueInputChannels), 1, "AdcOffsets"))
      {
	AssertErrorCondition(InitialisationError,"ATCAIocIntDrv::ObjectLoadSetup: %s Could not load AdcOffsets vector", this->Name());
	return False;
      }
    for(int i=0 ; i < numberOfAnalogueInputChannels ; i++)
      printf("adc%d, %d ", i, adc_offset_vector[i] );
    cdb.ReadInt32(softwareTrigger, "SoftwareTrigger", 0);
    /// Read the UsecPeriod of the ATCAIocInt packet producer
    cdb.ReadInt32(producerUsecPeriod, "ProducerUsecPeriod", -1);

    // Create Data Buffers. Compute total size and allocate storing buffer 
    for(int i=0 ; i < nOfDataBuffers ; i++){
      dataBuffer[i] = (uint32 *)malloc(packetByteSize);//numberOfInputChannels*sizeof(int));
      if(dataBuffer[i] == NULL){
        AssertErrorCondition(InitialisationError,"ATCAIocIntDrv::ObjectLoadSetup: %s ATCAIocInt dataBuffer allocation failed",Name());
        return False;
      }
    }
  }

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
  PacketStruct    *pkt = (PacketStruct *)lastReadBuffer;
  // Check data age
  uint32 sampleNo = pkt->nSampleNumber;
  if(freshPacket) {
    if(abs(usecTime - pkt->nSampleTime) > maxDataAgeUsec) {
      // Packet too old
      // return the last received data and put 0xFFFFFFFF as nSampleNumber
      //      printf("old:%u:%u  ", usecTime , pkt->nSampleTime );
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
  uint32 *destinationEnd = (uint32 *)(buffer + packetByteSize/sizeof(int32));
  //  uint32 *destinationEnd = (uint32 *)(buffer + numberOfInputChannels);
  uint32 *source = lastReadBuffer;
  while(destination < destinationEnd) {
    *destination++ = *source++;
  }
  //  destination[0] = pkt->nSampleNumber;// *source++;
  //destination[1] = pkt->nSampleTime;// *source++;

  //  for(int i = 2 ; i < numberOfInputChannels; i++) {
  //destination[i]  = pkt->channelData[i-2];   //  dataBuffer[i] = NULL;
  //}

  PacketStruct *p = (PacketStruct *)buffer;
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

  if(producerUsecPeriod != -1) {
    return ((int64)originalNSampleNumber*(int64)producerUsecPeriod);
  } else {
    return  lastPacketUsecTime;
  }
}

/**
 * PulseStart
 */
bool ATCAIocIntDrv::PulseStart() {
  //rc = 
  if(softwareTrigger!=0){
    board.soft_trigger();
    /*
;
    if(ioctl(devFd, PCIE_ATCA_IOCT_SOFT_TRIG)>0){ // reset time counter on board
      usleep(10);
      return True;
    }
    else
	return False;
    */
  }
  return True;
}

/**
 * ObjectDescription
 */
bool ATCAIocIntDrv::ObjectDescription(StreamInterface &s, bool full, StreamInterface *err){
  s.Printf("%s %s\n",ClassName(),Version());
  // Module name
  s.Printf("Module Name --> %s\n",Name());
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

  int print_n=10;
  //  int timeInit=0;
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
      if(lostPacketErrorCounterAux > 0) {
       	AssertErrorCondition(Warning, "packets lost on  [occured %i times]",  lostPacketErrorCounterAux);
       	lostPacketErrorCounterAux = 0;
      }
      // if(samePacketErrorCounter > 0) {
      // 	AssertErrorCondition(Warning, "nSampleNumber unchanged in  [occured %i times]",  samePacketErrorCounter);
      // 	samePacketErrorCounter = 0;
      // }
      // if(recoveryCounter > 0) {
      // 	AssertErrorCondition(Information, "Re-established correct packets sequence on  [occured %i times]",  recoveryCounter);
      // 	recoveryCounter = 0;
      // }
       if(previousPacketTooOldErrorCounter > 0) {
	 AssertErrorCondition(Warning,"ATCAIocIntDrv::RecCallback: %s:  Data too old [occured %i times]",Name(),
			      previousPacketTooOldErrorCounter);
	 previousPacketTooOldErrorCounter = 0;
       }
      lastCounterTime = currentCounterTime;
    }

#ifndef _FAKE_DEV
    /**
       read data from ATCA card 
       Gets 128 bytes packet
    */ 
    int _ret = board.read_packet(dataSource, packetByteSize);
      //read(devFd, dataSource, packetByteSize);
    if(_ret == -1) {
      AssertErrorCondition(FatalError,"ATCAIocIntDrv::RecCallback: read() error");
      return;
    }
    // Check the buffer length
    if(_ret != packetByteSize) {
      sizeMismatchErrorCounter++;
      continue;
    }
#else
    //Fake read
    SleepSec(1E-4);
    dataSource[30] = lastPacketNo++;
    dataSource[31] = lastPacketUsecTime++;
    dataSource[0] = 11915;
#endif

    // Copy dataSource in the write only buffer; does endianity swap
    Endianity::MemCopyFromIntel((uint32 *)dataBuffer[writeBuffer], (uint32 *)dataSource,
				packetByteSize/sizeof(int32));
    // Checks if packets have been lost
    PacketStruct *pkt = (PacketStruct *)dataBuffer[writeBuffer];

    // Make sure that while writeBuffer is being
    // updated it is not being read elsewhere
    while(!mux.FastTryLock());
    // Update buffer index
    writeBuffer++;
    if(writeBuffer >= nOfDataBuffers) 
      writeBuffer = 0;

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
    */
      // If is the first packet doesn't do any check on nSampleNumber
    if(lastPacketNo != 0xFFFFFFFF) {
      // Checks nSampleNumber
      // Warning if a reset has happened and too much packet had been lost
      // nSampleNumber has been casted to int32 to prevent wrong casting from compiler
      if((int32)(pkt->nSampleNumber) - lastPacketNo < 0) {
	if((int32)(pkt->nSampleNumber) > maxNOfLostPackets) {
	  rolloverErrorCounter++;
	}
      } else { 
	// nSampleNumber has been casted to int32 to prevent wrong casting from compiler
	if(((int32)(pkt->nSampleNumber) - lastPacketNo) > 1) {
	  // Warning if a packet is lost
	  lostPacketErrorCounter++;
	  lostPacketErrorCounterAux++;
	}  else if(((int32)(pkt->nSampleNumber) - lastPacketNo) == 0) {
	  // Warning if nSampleNumber in packet hasn't changed
	   samePacketErrorCounter++;
	} else if(lostPacketErrorCounter > 0) {
	  recoveryCounter++;
	  // Reset error counter
	  lostPacketErrorCounter = 0;
	}
      }
    }

 
    // Update lastPacketNo
    lastPacketNo       = pkt->nSampleNumber;
    if(print_n-- >0 ){
      printf("%d:%d ", lastPacketNo, pkt->nSampleTime);
      //      print_n--;
    }
    if(print_n == 0)
       printf("\n");
    //    lastPacketUsecTime =  pkt->nSampleTime + 1 ;
    //else
    lastPacketUsecTime =  lastPacketNo * 100;

    //lastPacketUsecTime= lastPacketUsecTime + 100;// = pkt->nSampleTime;
    // This line gives error message:
    //       [16:5:22]:localhost:FatalError:tid=0xffc8b700 ():cid=0x0:obj=GLOBAL:TimeInputGAM::Execute: Timeout on Execute
    //    lastPacketUsecTime =  pkt->nSampleTime;
    //    if ((print_n--) > 0) 

    //    if ((print_n--) == 0) 
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

  hStream.Printf("<h1 align=\"center\">%s</h1>\n", Name());
  
  hStream.Printf("<h2 align=\"center\">ATCA device = %s</h2>\n", deviceFileName.Buffer());
  hStream.Printf("<h2 align=\"center\">Analogue Input channels = %d</h2>\n", numberOfAnalogueInputChannels); //numberOfInputChannels
  hStream.Printf("<h2 align=\"center\">MaxDataAgeUsec = %d</h2>\n", maxDataAgeUsec);

  hStream.Printf("<h2 align=\"center\">MaxNOfLostPackets = %d</h2>\n", maxNOfLostPackets);
  hStream.Printf("<h2 align=\"center\">CPU mask = 0x%x</h2>\n", cpuMask);
  hStream.Printf("<h2 align=\"center\">Thread priority = %d</h2>\n", receiverThreadPriority);
  hStream.Printf("<h2 align=\"center\">lastPacketUsecTime = %d</h2>\n", lastPacketUsecTime);
  hStream.Printf("<h2 align=\"center\">producerUsecPeriod = %d</h2>\n", producerUsecPeriod);

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
  hStream.Printf("<td>Packet Number</td>\n");
  for(int32 i = 0 ; i < nOfDataBuffers ; i++) {
    idx = writeBuffer-1-i;
    if(idx < 0) idx = nOfDataBuffers-(int32)fabs(idx);
    if(dataBuffer[idx] != NULL) {
      hStream.Printf("<td>%u</td>", *(dataBuffer[idx]+30));
    }
  }
  hStream.Printf("</tr>\n");

  hStream.Printf("<tr>\n");
  hStream.Printf("<td>PacketUsecTime (usec)</td>\n");
  for(int32 i = 0 ; i < nOfDataBuffers ; i++) {
    idx = writeBuffer-1-i;
    if(idx < 0) idx = nOfDataBuffers-(int32)fabs(idx);
    if(dataBuffer[idx] != NULL) {
      hStream.Printf("<td>%u</td>", *(dataBuffer[idx]+31));
    }
  }
  hStream.Printf("</tr>\n");
    
  hStream.Printf("<tr>\n");
  hStream.Printf("<td>Channel 0</td>\n");
  for(int32 i = 0 ; i < nOfDataBuffers ; i++) {
    idx = writeBuffer-1-i;
    if(idx < 0) idx = nOfDataBuffers-(int32)fabs(idx);
    if(dataBuffer[idx] != NULL) {
      hStream.Printf("<td>%d</td>", *(dataBuffer[idx]));
      //	hStream.Printf("<td>%u</td>", *(dataBuffer[idx]+packetByteSize/sizeof(int32)-1));
    }
  }
  hStream.Printf("</tr>\n");
  hStream.Printf("<tr>\n");

  hStream.Printf("<td>Channel 1</td>\n");
  for(int32 i = 0 ; i < nOfDataBuffers ; i++) {
    idx = writeBuffer-1-i;
    if(idx < 0) idx = nOfDataBuffers-(int32)fabs(idx);
    if(dataBuffer[idx] != NULL) {
      hStream.Printf("<td>%d</td>", *(dataBuffer[idx]+ 1));
    }
  }
  hStream.Printf("</tr>\n");
   
  hStream.Printf("</table>\n");
  hStream.Printf("</body></html>\n");
  
}
OBJECTLOADREGISTER(ATCAIocIntDrv, "$Id$")
