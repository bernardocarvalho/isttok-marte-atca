#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/pci.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/cdev.h>
#include <linux/time.h>
#include <linux/ioport.h>
#include <asm/cacheflush.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#include <rtai.h>
#include <rtai_sched.h>
#include <rtai_sem.h>

#include "../../../pcieAdc.h"
#include "StatTest.h"

static int  masterSlotNum   = -1;
static int  numBoards       = 0;
//Use external clock and trigger
static int  extClockTrigger = 0;
static int  softTrigger     = 0;
static int  bufferCopySize  = 0;
static int  totalMemorySize = 0;
//The time between samples (ns)
static int  sampleTime      = 500;
//The total acquisition time (ns)
static int  acqTotalTime    = 1000000;
//The cpu mask
static int  rtCPUMask       = 4;
//The number of times to execute
static int  nloops          = 0;
//1 if all data is to be acquired. Otherwise it only stores headers and footers
static int acqAllData       = 0;
//1 when the acquisition is completed
static int acqCompleted     = 1;

static int *boardsSlotNums;
static char *bufferMemory    = NULL;
static int *boardBufferMemory;

//Device driver interface
static int    minor = 0;
static int    major = 0;
static struct cdev charDev;
#define NAME "atcapcieT"

int charDevOpen(struct inode *inode, struct file *filp);
int charDevRelease(struct inode *inode, struct file *filp);
int charDevRead(struct file *filp, char *buf, size_t count, loff_t *f_pos);
int charDevIoctl(struct inode *inode, struct file *filp, unsigned int fun, unsigned long argp);
/* Structure that declares the usual file */
/* access functions */
static const struct file_operations charDevFileOps = {
   read:     charDevRead,
   open:     charDevOpen,
   release:  charDevRelease,
   ioctl:    charDevIoctl
};

//Acquisition task
static RT_TASK acqTask;
static RT_TASK dacTask;

static double HRTFrequency = 2.833429000;
static int    CPUFrequency = 2833;
module_param(CPUFrequency, int, 0);
MODULE_PARM_DESC(CPUFrequency, "The cpu frequency");

static inline long long int HRTCounter(void) {
    long long int perf;
    unsigned int *pperf = (unsigned int *)&perf;
    asm(
        "rdtsc"
	: "=a"(pperf[0]) , "=d"(pperf[1])
	:
    );
    return perf;
}

static int runDAC         = 0;
static int dacSrcBoardIdx = 1;
static int dacSrcChannel  = 1;
static int dacSlot        = 3;
int dacThread(int ignored){
    int z         = 0;
    int d         = 0;
    int ticksTime = sampleTime * HRTFrequency;
    int value     = 0;
    int t =0;
    double val;
    RTIME  totalSleepTime   = 0;
    printk("Starting DAC thread\n");
    SetATCApcieExternalTriggerAndClock(extClockTrigger);
    EnableATCApcieAcquisition();
    if(softTrigger == 1){
    	SendSoftwareTrigger();
    }
    while(runDAC == 1){
	/*value += 256;
	if(value > 0xFFFF){
	    value = 0;
	}*/
        for(z=0; z<DMA_BUFFS; z++){
	    value = *((int *)(boardBufferMemory[dacSrcBoardIdx * DMA_BUFFS + z] + dacSrcChannel * sizeof(int)));
	    val = value * 1.4901e-8;

	    /*value = ~value;
	    value += 1;*/
	/*    if((value & 0x8000) == 1){
	        value &= 0x7FFF;
	    }else{
	        value |= 0x8000;
	    }*/
	    value = (int)val;
            for(d=0; d<8; d++){	    
	        WriteToDAC(dacSlot, d, value);
            }
	        rt_sleep(nano2count(40000));
	}
	//totalSleepTime = HRTCounter() + ticksTime;
	//while(HRTCounter() < totalSleepTime);
    }
    DisableATCApcieAcquisition();
    printk("DAC thread was stopped\n");
    return 0;
}

int acqThread(int ignored){
    int    i                = 0;
    int    j                = 0;
    int    z                = 0;
    int    d                = 0;
    int    ticksTime        = sampleTime * HRTFrequency;
    RTIME  totalSleepTime   = 0;
    double start            = 0;
    char  *bufferMemoryTemp = NULL;

    nloops = acqTotalTime / sampleTime;

    printk("Going to perform %d loops at %d ns sampling time for a total of %d ns\n", nloops, sampleTime, acqTotalTime);

    if(acqAllData){
        //bufferCopySize = (BUFFER_LENGTH + 2 * HEADER_LENGTH) * sizeof(int);
        bufferCopySize = 3 * sizeof(int);//(BUFFER_LENGTH + 2 * HEADER_LENGTH) * sizeof(int);
    }
    else{
        bufferCopySize = 2 * HEADER_LENGTH * sizeof(int) + 2 * sizeof(int); //+ 6 * sizeof(int);
    }

    //add nloops of acquisition times
    totalMemorySize = numBoards * DMA_BUFFS * bufferCopySize * nloops + nloops * sizeof(int);
    bufferMemory = vmalloc(totalMemorySize);
    memset(bufferMemory, 0, totalMemorySize);
    if(bufferMemory == NULL){
       printk("Failed allocating the internal buffer memory: %d bytes\n", totalMemorySize);
       return -1;
    }else{
       printk("Allocated an internal buffer memory of: %d bytes\n", totalMemorySize);
    }

    SetATCApcieExternalTriggerAndClock(extClockTrigger);

    bufferMemoryTemp = bufferMemory;
    /*for(i=0; i<numBoards; i++){ 
        enableATCApcieAcquisition(boardsSlotNums[i], softTrigger);
    }*/
    EnableATCApcieAcquisition();
    if(softTrigger == 1){
    	SendSoftwareTrigger();
    }

    printk("The ticksTime is = %d\n", ticksTime);
    printk("bufferCopySize = %d\n", bufferCopySize);
    if(ticksTime < 0){
        return -1;
    }
    for(i=0;i<2;i++){
        rt_sleep(nano2count(1e9));
    }
    start = HRTCounter();
    for(i=0; i<nloops; i++){
        j = (int)((HRTCounter() - start) / HRTFrequency);
        memcpy(bufferMemory, &j, sizeof(int));
        bufferMemory += sizeof(int);
          for(j=0; j<numBoards; j++){
            for(z=0; z<DMA_BUFFS; z++){
                if(acqAllData){
                  memcpy(bufferMemory, (void *)boardBufferMemory[j * DMA_BUFFS + z], sizeof(int));    	          
                  bufferMemory += sizeof(int);
                  memcpy(bufferMemory, (void *)boardBufferMemory[j * DMA_BUFFS + z] + sizeof(int), sizeof(int));
                  bufferMemory += sizeof(int);
                  memcpy(bufferMemory, (void *)(boardBufferMemory[j * DMA_BUFFS + z] + sizeof(int) * (BUFFER_LENGTH + HEADER_LENGTH)), sizeof(int));
                  bufferMemory += sizeof(int);
               }
               else{
                  memcpy(bufferMemory, (void *)boardBufferMemory[j * DMA_BUFFS + z], sizeof(int));
		  *((int *)boardBufferMemory[j * DMA_BUFFS + z]) = 10;
                      bufferMemory += sizeof(int);		      
		  memcpy(bufferMemory, (void *)boardBufferMemory[j * DMA_BUFFS + z] + 12 * sizeof(int), sizeof(int));
		  *((int *)boardBufferMemory[j * DMA_BUFFS + z] + 12 * sizeof(int)) = 30;
	              bufferMemory += sizeof(int);
		  memcpy(bufferMemory, (void *)boardBufferMemory[j * DMA_BUFFS + z] + 13 * sizeof(int), sizeof(int));
		  *((int *)boardBufferMemory[j * DMA_BUFFS + z] + 13 * sizeof(int)) = 40;
	              bufferMemory += sizeof(int);
                  memcpy(bufferMemory, (void *)(boardBufferMemory[j * DMA_BUFFS + z] + sizeof(int) * (BUFFER_LENGTH + HEADER_LENGTH)), sizeof(int));
		  *((int *)(boardBufferMemory[j * DMA_BUFFS + z] + sizeof(int) * (BUFFER_LENGTH + HEADER_LENGTH))) = 20;
                      bufferMemory += sizeof(int);

/*
                  memcpy(bufferMemory, (void *)(boardBufferMemory[j * DMA_BUFFS + z] + sizeof(int) * (BUFFER_LENGTH + HEADER_LENGTH + 2)), sizeof(int));
		  *((int *)(boardBufferMemory[j * DMA_BUFFS + z] + sizeof(int) * (BUFFER_LENGTH + HEADER_LENGTH + 2))) = 40;
                      bufferMemory += sizeof(int);

                  memcpy(bufferMemory, (void *)(boardBufferMemory[j * DMA_BUFFS + z] + sizeof(int) * (BUFFER_LENGTH + HEADER_LENGTH + 3)), sizeof(int));
		  *((int *)(boardBufferMemory[j * DMA_BUFFS + z] + sizeof(int) * (BUFFER_LENGTH + HEADER_LENGTH + 3))) = 50;
                      bufferMemory += sizeof(int);

                  memcpy(bufferMemory, (void *)(boardBufferMemory[j * DMA_BUFFS + z] + sizeof(int) * (BUFFER_LENGTH + HEADER_LENGTH + 4)), sizeof(int));
		  *((int *)(boardBufferMemory[j * DMA_BUFFS + z] + sizeof(int) * (BUFFER_LENGTH + HEADER_LENGTH + 4))) = 60;
                      bufferMemory += sizeof(int);

                  memcpy(bufferMemory, (void *)(boardBufferMemory[j * DMA_BUFFS + z] + sizeof(int) * (BUFFER_LENGTH + HEADER_LENGTH + 5)), sizeof(int));
		  *((int *)(boardBufferMemory[j * DMA_BUFFS + z] + sizeof(int) * (BUFFER_LENGTH + HEADER_LENGTH + 5))) = 70;
                      bufferMemory += sizeof(int);

                  memcpy(bufferMemory, (void *)(boardBufferMemory[j * DMA_BUFFS + z] + sizeof(int) * (BUFFER_LENGTH + HEADER_LENGTH + 6)), sizeof(int));
		  *((int *)(boardBufferMemory[j * DMA_BUFFS + z] + sizeof(int) * (BUFFER_LENGTH + HEADER_LENGTH + 6))) = 80;
                      bufferMemory += sizeof(int);*/
               }
           }
        }
        totalSleepTime = HRTCounter() + ticksTime;
        while(HRTCounter() < totalSleepTime);
	//flush_cache_all();
    }

    /*for(i=0; i<numBoards; i++){
        DisableATCApcieAcquisition(boardsSlotNums[i]);
    }*/
    DisableATCApcieAcquisition();
    bufferMemory = bufferMemoryTemp;
    acqCompleted = 1;
    return 0;
}

int init_func(void) {
    int    i       = 0;
    dev_t  dev_id;
    printk("Starting atca pcie statistics tests\n");
    numBoards     = GetNumberOfBoards();
    masterSlotNum = GetMasterBoardSlotNumber();
    printk("The number of boards installed on the system is: %d\n", numBoards);
    printk("The master board is in slot: %d\n", masterSlotNum);

    boardsSlotNums = GetBoardsSlotNumbers();
    for(i=0; i<numBoards; i++){
        if(IsRTMPresent(boardsSlotNums[i])){
    	    printk("RTM found in slot: %d\n", boardsSlotNums[i]);
        }
    }

    boardBufferMemory = GetBufferAddress();
    if(boardBufferMemory == NULL){
        printk("Board DMA buffer memory is NULL\n");
	return -1;
    }

    //Dev driver interface
    i = alloc_chrdev_region(&dev_id, 0, 1, NAME);
    if (i < 0) {
        printk("%s: alloc_chrdev_region failed\n", NAME);
        return -1;
    }
    major = MAJOR(dev_id);
    minor = MINOR(dev_id);
    printk("%s: Initialized: Major %d Minor %d\n", NAME, major, minor);
    cdev_init(&charDev, &charDevFileOps);
    charDev.owner = THIS_MODULE;
    charDev.ops   = &charDevFileOps;
    i = cdev_add(&charDev, dev_id, 1);
    if(i < 0){
        printk("%s: cdev_add failed: Major %d Minor %d \n", NAME, major, minor);
	return -1;
    }

    HRTFrequency = (double)(CPUFrequency) / 1000.0;

    rt_set_oneshot_mode();
    start_rt_timer(0);

    return 0;
}

void exit_func(void) {
    dev_t dev_id;
    printk("Exiting statistics pcie statistics\n");
    stop_rt_timer();

    dev_id = MKDEV(major, minor);
    unregister_chrdev_region(dev_id, 1);
    cdev_del(&charDev);
}

int charDevOpen(struct inode *inode, struct file *filp){
    return 0;
}

int charDevRelease(struct inode *inode, struct file *filp){
    return 0;
}

int charDevRead(struct file *filp, char *buf, size_t count, loff_t *f_pos){
    if(count < 0){
        return -1;
    }
    
    if(bufferMemory == NULL){
        return -1;
    }

    if((*f_pos + count) > totalMemorySize){
        count = totalMemorySize - *f_pos;
    }

    if(count > 0){
        copy_to_user(buf, bufferMemory + *f_pos, count);
    }else{
        return 0;
    }

    *f_pos += count;
    return count;
}

int charDevIoctl(struct inode *inode, struct file *filp, unsigned int fun, unsigned long arg){
    u32  value;
    u32  i;
    u32  w33[12];
    u32  sn[12];
    int *boardSlots = GetBoardsSlotNumbers();
    if(_IOC_TYPE(fun) != STAT_TEST_IOCTL_MAGIC)
        return -ENOTTY;
    if (_IOC_DIR(fun) & _IOC_READ){
        if(!access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(fun))){
            return -EFAULT;
	}
    }
    else if (_IOC_DIR(fun) & _IOC_WRITE){
        if(!access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(fun))){
            return -EFAULT;
	}
    }

    switch(fun){
	case IOCTL_EXT_CLK_TRIGGER:
	    if(__get_user(value, (int __user *)arg) == 0){
	        extClockTrigger = value;
	    }
	    break;
	case IOCTL_SOFT_TRIGGER:
	    if(__get_user(value, (int __user *)arg) == 0){
	        softTrigger = value;
	    }
	    break;
	case IOCTL_SAMPLE_TIME_NS:
	    if(__get_user(value, (int __user *)arg) == 0){
	        sampleTime = value;
	    }
	    break;
	case IOCTL_ACQ_TOTAL_TIME_NS:
	    if(__get_user(value, (int __user *)arg) == 0){
	        acqTotalTime = value;
	    }
	    break;
	case IOCTL_CPU_MASK:
	    if(__get_user(value, (int __user *)arg) == 0){
	        rtCPUMask = value;
	    }
	    break;
	case IOCTL_ACQ_ALL_DATA:
	    if(__get_user(value, (int __user *)arg) == 0){
	        acqAllData = value;
	    }
	   break;
    case IOCTL_ACQ_COMPLETE:
        if(copy_to_user((void __user *)arg, &acqCompleted, sizeof(u32)) != 0){
	        return -EFAULT;
        }
	    break;
    case IOCTL_ENABLE_ACQ:
        /*for(i=0; i<numBoards; i++){ 
            enableATCApcieAcquisition(boardsSlotNums[i], softTrigger);
        } */
	EnableATCApcieAcquisition();
        break;
    case IOCTL_DISABLE_ACQ:
        /*for(i=0; i<numBoards; i++){ 
            disableATCApcieAcquisition(boardsSlotNums[i]);
        } */
	DisableATCApcieAcquisition();
        break;
	case IOCTL_ACQ_W33:
        for(i=0; i<numBoards; i++){
            w33[i] = *((int *)(GetBoardBufferAddress(boardSlots[i])[0] + 33 * sizeof(int)));
        }
        if(copy_to_user((void __user *)arg, &w33, numBoards * sizeof(u32)) != 0){
	        return -EFAULT;
        }
	    break;
    case IOCTL_BOARD_SLOTS:
	    for(i=0; i<numBoards; i++){
	        sn[i] = boardSlots[i];
	    }
        if(copy_to_user((void __user *)arg, &sn, numBoards * sizeof(u32)) != 0){
	        return -EFAULT;
        }
        break;
	case IOCTL_NUM_BOARDS:
        if(copy_to_user((void __user *)arg, &numBoards, sizeof(u32)) != 0){
                return -EFAULT;
        }
	    break;
	case IOCTL_START_ACQ:
        if(bufferMemory != NULL){
            vfree(bufferMemory);
        }
	    acqCompleted = 0;
        rt_task_init(&acqTask, acqThread, NULL, 32000, RT_SCHED_HIGHEST_PRIORITY, 1, 0);
        rt_set_runnable_on_cpus(&acqTask, rtCPUMask);
        rt_task_resume(&acqTask);
        break;
	case IOCTL_START_DAC_TEST:
	runDAC = 1;
        /*for(i=0; i<8; i++){
	    WriteToDAC(dacSlot, i, i*1000);
        }*/
	
        rt_task_init(&dacTask, dacThread, NULL, 32000, RT_SCHED_HIGHEST_PRIORITY, 1, 0);
	rt_set_runnable_on_cpus(&dacTask, rtCPUMask);
	rt_task_resume(&dacTask);
	break;
	case IOCTL_STOP_DAC_TEST:
	runDAC = 0;
	break;
        default:
            return -ENOTTY; 
    }

    return 0;
}

module_init(init_func);
module_exit(exit_func);

