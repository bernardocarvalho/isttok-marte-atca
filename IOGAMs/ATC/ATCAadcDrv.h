//******************************************************************************
//    MARTe Library
//    $Log: ATCAadcDrv.h,v $
//    Revision 1.32  2009/12/03 14:52:13  ppcc_dev
//    Sleeps, if time is available, before start busy polling.
//    The time to sleep is given by the remaining time until the start of
//    the next pulse minus the worst jitter from a sleep and minus the
//    desired time, before the beginning of the next pulse, that we want to start
//    busy polling.
//    The worst jitter decays to zero in order to try to maintain a good performance
//
//    Revision 1.31  2009/08/07 09:31:47  aneto
//    Allow the autoSoftwareTrigger to work even if the softwareTrigger flag
//    is set to false
//
//    Revision 1.30  2009/05/21 15:18:37  ppcc_dev
//    DigIO does not have outputMap
//
//    Revision 1.29  2009/04/14 09:06:10  aneto
//    Allow the system to auto-trigger after a specified amount of time
//
//    Revision 1.28  2009/04/03 10:02:03  aneto
//    lastCycleUsecTime now is true 64 bits.
//    This uses the information from the headers to increment an internal counter
//
//    Revision 1.27  2009/04/01 15:10:36  aneto
//    Bug in the way the modulus was being calculated for the usec time. The bug was in converting from 64 to 32 bits of lastCycleUsecTime
//
//    Revision 1.26  2009/03/16 11:42:16  aneto
//    Corrected the polling mode in order to allow different acquisition frequencies
//
//    Revision 1.25  2009/03/11 12:31:54  aneto
//    Support an html output with information about the driver
//
//    Revision 1.24  2009/01/26 17:26:20  ppcc_dev
//    Small bugs solved
//
//    Revision 1.23  2009/01/26 09:20:38  aneto
//    linux support
//
//    Revision 1.22  2008/11/28 12:03:13  aneto
//    Added bufferNumber
//
//    Revision 1.21  2008/11/21 14:16:52  ppcc_dev
//    This version works with the new firmware: jet clock+trigger
//
//    Revision 1.19  2008/09/30 11:24:49  rvitelli
//    Added non-synchronous operating mode.
//
//    Revision 1.18  2008/09/22 17:20:36  fpiccolo
//    Solved minor bugs
//
//    Revision 1.17  2008/09/15 16:51:45  ppcc_dev
//    Solved few bugs
//
//    Revision 1.16  2008/09/09 09:29:15  fpiccolo
//    Modified driver structure.
//    Added SingleATCAModule class
//    Added Writing facilities
//
//    Revision 1.15  2008/08/20 16:34:36  ppcc_dev
//    Added PulseStart to reset the internal counter to 0 when using the SoftTrigger option
//
//    Revision 1.14  2008/08/15 10:41:35  fpiccolo
//    Minor stylish modifications.
//    Added TimeModule Interface
//
//    Revision 1.13  2008/08/01 14:09:28  rvitelli
//    First working version
//
//******************************************************************************

#ifndef ATCAADCDRV_H_
#define ATCAADCDRV_H_

#include "System.h"
#include "GenericAcqModule.h"
#include "FString.h"
#include "module/pcieAdc.h"
#include "module/pcieAdcIoctl.h"
#ifdef _LINUX
#include <sys/mman.h>
#endif
#include "WebStatisticGAM.h"

class SingleATCAModule{
private:
    /** Module Identifier */
    int32        moduleIdentifier;

    /** Number of Analogue Input channels for this module (Maximum 32)*/
    int32        numberOfAnalogueInputChannels;

    /** Number of Digital Input channels for this module  (1 or 0)    */
    int32        numberOfDigitalInputChannels;
        
    /** Number of Analogue Output channels                (Maximum 8) */
    int32        numberOfAnalogueOutputChannels;

    /** Number of Digital Output channels                 ()          */
    int32        numberOfDigitalOutputChannels;

    /** Output Map. Used to map the output to a specific physical
        output channel. Channels are identified from 1 to 8.          */
    int32        outputMap[8];


    ////////////////////////////
    // Analogue Input Section //
    ////////////////////////////

    /** If true synchronize on data arrival, if false return latest completed buffer */
    bool             synchronizing;

    /** Pointers to the DMA memory allocated for data acquisition.
        The number of buffers is fixed to 4.
    */
    int32            *dmaBuffers[DMA_BUFFS];

    /** Current DMA buffer index [0-3].  */
    static int32     currentDMABufferIndex;

    /** The current master header (must be the same in all the boards) */
    static int32     currentMasterHeader;

    /** Estimated time of the next expected arrival in CPU Ticks of the acquired buffer.
        It is computed by adding a delay specified @param periodUsecSleep to
        the time of the previous completed acquisition. The system will sleep
        till this time elapses.
    */
    int64            nextExpectedAcquisitionCPUTicks;

    /** Specifies how long to sleep between acquisitions. It is specified in microseconds
        but it is internally converted in CPU ticks to avoid unecessary computations during
        realtime activities.
    */
    int64            boardInternalCycleTicks;

    /** Amount of time in microseconds after which the data stops waiting for data arrival 
        and reports an acquisition error. 
     */
    int64            dataAcquisitionUsecTimeOut;

    /** Length of a "Short Sleep" in seconds. It is used to monitor
        the data arrival on the master board and specifies a sleep time
        between checks of the data datagram arrival.
    */
    float            datagramArrivalFastMonitorSecSleep;

    /** The number of micro seconds incremented by the board in each cycle. It gives the board acquisition frequency. 
    */
    int32            boardInternalCycleTime;

    /**
     * The statistics info for these channels
     */
    StatSignalInfo  *channelStatistics;
    
    /** Find the currentDMABufferIndex and synchronize on data arrival
        
     */
    int32  CurrentBufferIndex();
    
    /** Find the latest completed buffer without synchronization*/
    int32 GetLatestBufferIndex();
    
public:

    SingleATCAModule();

    /** Initialises the SingleModule Parameter*/
    bool ObjectLoadSetup(ConfigurationDataBase &info,StreamInterface *err = NULL);

    /** Reads  NumberOfInputChannels() from the DMA Buffer.
        The first module must be the master board to assure
        correct data transfer.
    */
    bool GetData(int32 *&buffer);

    bool WriteData(const int32 *&buffer);

    /** Copies the pointers to the DMA Buffers */
#ifdef _LINUX
    bool InstallDMABuffers(int32 *mappedDmaMemoryLocation);
#else
    /** Copies the pointers to the DMA Buffers */
    bool InstallDMABuffers();
#endif
    
    /////////////////
    // Time Module //
    /////////////////

    int64             lastCycleUsecTime;

    int32             packetCounter;
    
    /** Is Master Board */
    bool             isMaster;


    /** Returns the sum of analogue and digital input channels */
    int32 NumberOfInputChannels(){
        return numberOfDigitalInputChannels + numberOfAnalogueInputChannels;
    }

    /** Returns the sum of analogue and digital output channels */
    int32 NumberOfOutputChannels(){
        return numberOfDigitalOutputChannels + numberOfAnalogueOutputChannels;
    }
    
    /** Returns the module Identifier */
    int32  BoardIdentifier(){return moduleIdentifier;}
    
    /**
     * Output an HTML table with the current value in mV of the acquired signals for this board
     */
    virtual bool ProcessHttpMessage(HttpStream &hStream);
    /**
     * Resets the statistics
     */
    bool ResetStatistics();
    /**
     * Polling method
     */
    virtual bool Poll();
    /**
     * Allow sleeping, when enough time is available, before start polling
     */
    bool allowPollSleeping;
    /**
     * Time to sleep before hard polling: pollSleepTime = CycleTime - time left to cycle time - pollSleepTimeWakeBeforeUs - worstPollSleepJitter;
     */
    float pollSleepTime;
    /**
     * Actually we want to start polling some us before reaching the cycle time
     */
    float pollSleepTimeWakeBeforeUs;
    /**
     * The worst jitter calculated in realtime of the actual time slept before polling and time meant to sleep
     */
    float worstPollSleepJitter;
    /**
     * The worst jitter will try to be recovered with a certain rate
     */
    float worstPollSleepJitterDecayRate;
};




OBJECT_DLL(ATCAadcDrv)
/** The high level driver for the ATCA ADC module */
class ATCAadcDrv:public GenericAcqModule{
private:

    /** Number of boards in the crate. Read during Initialisation */
    int32               numberOfBoards;

    /** Pointers to the ATCA modules */
    SingleATCAModule    *modules;

    /** Last cycle usec time */
    int64               lastCycleUsecTime;
    
    /** */
    bool                synchronizing;

    /** Software triggered acquisition. 
        0, which is the default value, means hardware trigger. 
    */
    int32               softwareTrigger;

	/** The master board index
     */
    int32               masterBoardIdx;

    /** If set to true the a software trigger will be sent every
     * AutoSoftwareTriggerAfterUs microseconds. For this to be 
     * true the value of AutoSoftwareTriggerAfterUs in the configuration file
     * must be > 1
     */
    bool                autoSoftwareTrigger;
    int32               autoSoftwareTriggerAfterUs;

#ifdef _LINUX
    /** Used only in Linux. The mmapped memory location.*/
    int32               *mappedDmaMemoryLocation;
    int32                mappedDmaMemorySize;
#endif
    /** The css for this page
     */
    const char *css;

public:

#ifdef _LINUX
    /**The page size*/
    static int32 pageSize;
    
    /**The file descriptor to access the driver*/
    static int32 fileDescriptor;
#endif
    
    // (De)Constructor
    ATCAadcDrv();

    virtual ~ATCAadcDrv(){
        if(modules != NULL) delete[] modules;
#ifdef _LINUX
	munmap(mappedDmaMemoryLocation, mappedDmaMemorySize);
        close(fileDescriptor);
#endif
    }
		
    // Standard GAM methods
    /* Load setup from CDB.
       This IOGAM peculiar parameters are PeriodSleep_usec and FastSleep_usec. 
       @param info: CDB from which load data
       @param err: not used 
       @returns true if all ok*/
    virtual bool ObjectLoadSetup(ConfigurationDataBase &info,StreamInterface *err);
		
    /* Print internal GAM informations 
       @param s: StreamInterface in which print infos
       @param full: not used
       @param err: not used 
       @returns true if all ok*/
    virtual bool ObjectDescription(StreamInterface &s,bool full = False, StreamInterface *err=NULL);
		
    /* Saves the data into the DDB
       @param usecTime: not used
       @param buffer: pointer to the data buffer to be filled
       @returns 1 if all ok*/
    int32 GetData(uint32 usecTime, int32 *buffer, int32 bufferNum = 0);
		
    bool WriteData(uint32 usecTime, const int32 *buffer);
		
    // Set board used as input
    virtual bool SetInputBoardInUse(bool on = False){
        if(inputBoardInUse && on){
            AssertErrorCondition(InitialisationError, "ATCAadcDrv::SetInputBoardInUse: Board %s is already in use", Name());
            return False;
        }
        inputBoardInUse  = on;
        return True;
    }

    virtual bool SetOutputBoardInUse(bool on = False){
        if(outputBoardInUse && on){
            AssertErrorCondition(InitialisationError, "ATCAadcDrv::SetOutputBoardInUse: Board %s is already in use", Name());
            return False;
        }
        outputBoardInUse = on;
        return True;
    }

    virtual bool EnableAcquisition();

    virtual bool DisableAcquisition();

    //////////////////////
    // From Time Module //
    //////////////////////
    
    // Get the Time
    int64 GetUsecTime(){return lastCycleUsecTime;}

    bool SoftwareTrigger(){    
        if(modules == NULL)return False;
#ifdef _RTAI
        SendSoftwareTrigger();
#elif defined(_LINUX)
        int ret = ioctl(fileDescriptor, PCIE_ATCA_ADC_IOCT_SEND_SOFT_TRG);
        if(ret != 0){
            AssertErrorCondition(InitialisationError,"ATCAadcDrv::PulseStart: Could send software trigger. ioctl returned : %d",ret);
            return False;
        }
#endif
        return True;
    }


    //////////////////////////////////
    // Simulation Purpose Functions //
    //////////////////////////////////

    bool PulseStart(){    
        if(modules == NULL)return False;
        if(softwareTrigger == 1){
            return SoftwareTrigger();
        }
        return True;
    }

    /**
     * Output an HTML page with the current value in mV of the acquired signals
     */
    virtual bool ProcessHttpMessage(HttpStream &hStream);
    /**
     * Polling method
     */
    virtual bool Poll();
private:

    OBJECT_DLL_STUFF(ATCAadcDrv);
};
#endif /*ATCAADCDRV_H_*/
