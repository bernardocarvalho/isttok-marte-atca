/**
 * ATCA IO CONTROL Integrator
 * Definitions for the Linux Device Driver
 *
 * SVN keywords
 * $Date: 2013-05-27 14:06:26 +0100 (Mon, 27 May 2013) $
 * $Revision: 4894 $
 * $URL: http://metis.ipfn.ist.utl.pt/svn/cdaq/ATCA/ATCA-IO-CONTROL/IPP/ADC_INTEGRATORS/Software/trunk/include/atca-ioc-int.h $
 *
 */

#ifndef _ATCA_IOC_INT_H_
#define _ATCA_IOC_INT_H_

#include "atca-ioc-int-ioctl.h"

#ifdef __KERNEL__
//#include <linux/module.h>
#else
#define u32 unsigned int
//#define dma_addr_t u32
#endif

//#include "atca-ioc-int-ioctl.h"

/* board configurable parameters */
//#define BAR1_RVAL 0xB
//  + (0x20 << 4)) // 256 Dwords = 1KB
// #define DMA_NBYTES PAGE_SIZE // 4096  Linux page size

#define DMA_NBYTES PAGE_SIZE // 128k 


#define DMA_SIZE 4194304 // 1024 * PAGE_SIZE
#define NUM_CHAN_SMP 16// nr of 32bit data channels per sample.
#define NUM_CHAN_SAV 16 // nr of channel to save

#define DECIMATE 100

#define DMA_BUFFS   16    //Number of DMA Buffs
#define GFPORDER 10 // 2^5 = 32, 32*4k = 128k / gfpo 8 = 1Mb / gfpo 10 = 4 Mb
#define CHANNELS 32

//#define CHAR_DEVICE_NAME      "atcaAdcStream"

#define MAX_BOARDS	12

#ifndef __cplusplus
//TOD : to be used.
#ifdef __BIG_ENDIAN_BTFLD
 #define BTFLD(a,b) b,a
#else
 #define BTFLD(a,b) a,b
#endif

/*
//Not realy much use, since I/O must be done through ioread, iowrite etc..
typedef struct _PCIEADC_REG {
	u32 BTFLD(BTFLD(BTFLD(BTFLD(counter_en:1, int_mode:1), int_status:1), DMA_en:1), reserved:28);
	u32 DMA_addr;
	u32 DMA_size;
	u32 BTFLD(output:1, _reserved:31);
} PCIEADC_REG;


*/

typedef struct _OFFSET_REGS {
  u32 offset[16];
} OFFSET_REGS;

typedef struct _DRIFT_REGS {
  u32 drift[16];
} DRIFT_REGS;

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
    //    struct  { u32 rsv0:19, STREAME:1,  ACQS:1, ACQT:1, ACQK:1, ACQE:1, STRG:1, TRGS:1, rsv1:1, 
    //	DMAE:1, rsv4:1, ERRiE:1, DMAiE:1, ACQiE:1;
      /////////////////////////////
    /*
    struct  { u32 rsv0:10, CHOP_ON_OFF:1, CHOP_VALUE:1, CHOP_RECONSTRUCTED:1, rsv1:6 , STREAME:1,  ACQS:1, ACQT:1, ACQK:1, ACQE:1, STRG:1, TRGS:1, rsv2:1, 
	DMAE:1, rsv4:1, ERRiE:1, DMAiE:1, ACQiE:1;
    */
      /////////////////////////////
      /////////////////////////////
    /*
    struct  { u32 rsv0:10, CHOP_ON_OFF:1, CHOP_VALUE:1, CHOP_RECONSTRUCTED:1, OFFSET_FIRMWARE_CALCULATION:1, rsv1:5 , STREAME:1,  ACQS:1, 
	ACQT:1, ACQK:1, ACQE:1, STRG:1, TRGS:1, rsv2:1, DMAE:1, rsv4:1, ERRiE:1, DMAiE:1, ACQiE:1;
    */
      /////////////////////////////
      /////////////////////////////
    struct  { u32 rsv0:10, CHOP_ON_OFF:1, CHOP_VALUE:1, CHOP_RECONSTRUCTED:1, OFFSET_FIRMWARE_CALCULATION:1, INTEGRAL_CALCULATION:1, rsv1:4,
	STREAME:1,  ACQS:1, ACQT:1, ACQK:1, ACQE:1, STRG:1, TRGS:1, rsv2:1, DMAE:1, rsv4:1, ERRiE:1, DMAiE:1, ACQiE:1;

      /////////////////////////////

      //    struct  { u32 rsv0:23, ACQE:1, rsv1:3, DMAE:1, rsv4:1, ERRiE:1, DMAiE:1, ACQiE:1;
    } cmdFlds;
  };    
} COMMAND_REG;


#endif //__cplusplus
	  
#endif /* _ATCA_IOC_INT_H_ */

