/**
 * ATCA IO CONTROL Integrator
 * Definitions for the Linux Device Driver
 *
 * SVN keywords
 * $Date$
 * $Revision$
 * $URL$
 *
 */
#ifndef _ATCA_IOC_INT_H_
#define _ATCA_IOC_INT_H_

/*To be compatible with C++ compilers*/ 
#ifdef __cplusplus
extern "C"{
#endif

#ifndef __KERNEL__
#define u32 unsigned int
#endif

#define DMA_MAX_BYTES  (4096 * 1024)  //4194304B (4MB)  => 65536 samples /DMA
#define DMA_ACQ_SIZE DMA_MAX_BYTES
#define DMA_INT_SIZE 128

//#define N_SAMP_P_DMA (DMA_SIZE/NUM_CHAN_SMP/4)  // nr samples per buffer (IRQ)
#define DMA_N_CHAN 16 // nr of signal transferred on DMA packet(32 bit)
#define ADC_N_CHAN 16 // nr of adc raw channel transferred (32 bit)
#define N_SAMP_P_DMA  (DMA_ACQ_SIZE/DMA_N_CHAN/4)


#define CHAN_CHOP  14
#define CHOP_BIT_MASK 0x00000001
#define CHOP_POS_VALUE 1
#define CHOP_NEG_VALUE 0

typedef struct _ACQ_DATA_PCKT {
  int rawdata[DMA_N_CHAN];
} ACQ_DATA_PCKT;

typedef struct _INT_DATA_PCKT {
  int rawdata[DMA_N_CHAN];
  float Fintdata[DMA_N_CHAN];
} INT_DATA_PCKT;

//TOD : to be used.
#ifdef __BIG_ENDIAN_BTFLD
 #define BTFLD(a,b) b,a
#else
 #define BTFLD(a,b) a,b
#endif

/*
typedef struct _OFFSET_REGS {
  u32 offset[16];
} OFFSET_REGS;

typedef struct _DRIFT_REGS {
  u32 drift[16];
} DRIFT_REGS;
*/
/*
 * 8 + 24 bit field 
 */
typedef struct _REVID_FLDS {
  u32 RevId:4, TMR:1, HDR:1, DBG:1, reserved:1, none:24;
} REVID_FLDS;

/*
 * 24 + 8 bit field
 */
typedef struct _STATUS_FLDS {
  u32 none:8, rsv0:8, 
    slotID:4, rsv2:2, MASTER:1, ERR0:1,  //rsv1:2, FSH:1, RST:1
    rsv3:2,  FIFE:1, FIFF:1, rsv4:2, DMAC:1, ACQC:1; // msb
} STATUS_FLDS;

typedef struct _STATUS_REG {
  union {
    u32 reg32;
    struct _STALFD {
      u32 revId:8,
	statWrd:24;
    } Str;
    STATUS_FLDS statFlds;
    REVID_FLDS rdIdFlds;
  };
} STATUS_REG;

typedef struct _COMMAND_REG {
  union{
    u32 reg32;
    /*  0, ..... 15
	19 , ...31   */
    struct  {u32 rsv0:10, CHOP_ON:1, CHOP_DEFAULT:1, CHOP_RECONSTRUCT:1, OFFSET_CALC:1, INTEGRAL_CALC:1, DAC_SHIFT:4,
	STREAME:1, rsv1:3, ACQE:1, STRG:1, IRST:1, rsv2:1, DMAE:1, rsv4:1, ERRiE:1, DMAiE:1, ACQiE:1;
    } cmdFlds;
  };    
} COMMAND_REG;


#undef PDEBUG             /* undef it, just in case */
#ifdef ATCA_DEBUG
#  ifdef __KERNEL__
/* This one if debugging is on, and kernel space */
#    define PDEBUG(fmt, args...) printk( KERN_DEBUG "atca_ioc: " fmt, ## args)
#  else
/* This one for user space */
#    define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
#  endif
#else
#  define PDEBUG(fmt, args...) /* not debugging: nothing */
#endif

#undef PDEBUGG
#define PDEBUGG(fmt, args...) /* nothing: it's a placeholder */

#ifdef __cplusplus
}
#endif

	  
#endif /* _ATCA_IOC_INT_H_ */

