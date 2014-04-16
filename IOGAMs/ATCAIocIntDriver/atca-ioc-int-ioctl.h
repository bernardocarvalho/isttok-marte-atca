/**
 * ATCA IO CONTROL Integrator
 * IOCTL Definitions for the Linux Device Driver
 *
 * SVN keywords
 * $Date$
 * $Revision$
 * $URL$
 *
 */

#ifndef _PCIE_ADC_IOCTL_H_
#define _PCIE_ADC_IOCTL_H_

#include "atca-ioc-int.h"

/*
 * IOCTL definitions
 */

#define PCIE_ADC_IOC_MAGIC 'k'  // /* Please use a different 8-bit number in your code */
/*See  /Documentation/ioctl-number.txt*/

/* S means "Set": thru a pointer
 * T means "Tell": directly with the argument value
 * G menas "Get": reply by setting thru a pointer
 * Q means "Qry": response is on the return value
 * X means "eXchange": G and S atomically
 * H means "sHift": T and Q atomically
 */

/**********************************************************************
 *                         IOCTL FUNCTIONS                            *
 *********************************************************************/
#define PCIE_ADC_IOCT_INT_ENABLE   _IO(PCIE_ADC_IOC_MAGIC, 1)
#define PCIE_ADC_IOCT_INT_DISABLE  _IO(PCIE_ADC_IOC_MAGIC, 2)
#define PCIE_ADC_IOCT_ACQ_ENABLE   _IO(PCIE_ADC_IOC_MAGIC, 3)
#define PCIE_ADC_IOCT_ACQ_DISABLE  _IO(PCIE_ADC_IOC_MAGIC, 4)
#define PCIE_ADC_IOCT_DMA_ENABLE   _IO(PCIE_ADC_IOC_MAGIC, 5)
#define PCIE_ADC_IOCT_DMA_DISABLE  _IO(PCIE_ADC_IOC_MAGIC, 6)
#define PCIE_ADC_IOCT_SOFT_TRIG    _IO(PCIE_ADC_IOC_MAGIC, 7)
#define PCIE_ADC_IOCG_STATUS       _IOR(PCIE_ADC_IOC_MAGIC, 8, u_int32_t)
#define PCIE_ADC_IOCG_COUNTER      _IOR(PCIE_ADC_IOC_MAGIC, 9, u_int32_t)
//#define PCIE_ADC_IOCS_RDOFF        _IOW(PCIE_ADC_IOC_MAGIC, 10, u_int32_t)
#define PCIE_ADC_IOCS_RDTMOUT      _IOW(PCIE_ADC_IOC_MAGIC, 11, u_int32_t)
#define PCIE_ADC_IOCT_CLOCKS_SHARED  _IO(PCIE_ADC_IOC_MAGIC, 12)
#define PCIE_ADC_IOCT_CLOCKS_LOCAL   _IO(PCIE_ADC_IOC_MAGIC, 13)
#define PCIE_ADC_IOCT_TRIG_SHARED    _IO(PCIE_ADC_IOC_MAGIC, 14)
#define PCIE_ADC_IOCT_TRIG_LOCAL     _IO(PCIE_ADC_IOC_MAGIC, 15)
#define PCIE_ADC_IOCS_TRIG_DLY      _IOW(PCIE_ADC_IOC_MAGIC, 16, u_int32_t)
#define PCIE_ADC_IOCG_TRIG_DLY      _IOR(PCIE_ADC_IOC_MAGIC, 17, u_int32_t)
#define PCIE_ADC_IOCT_STREAM_ENABLE   _IO(PCIE_ADC_IOC_MAGIC, 18)
#define PCIE_ADC_IOCT_STREAM_DISABLE   _IO(PCIE_ADC_IOC_MAGIC, 19)

#define PCIE_ADC_IOCS_ADC_OFF       _IOW(PCIE_ADC_IOC_MAGIC, 20, OFFSET_REGS)
#define PCIE_ADC_IOCG_ADC_OFF       _IOR(PCIE_ADC_IOC_MAGIC, 21, OFFSET_REGS)
//#define PCIE_ADC_IOCS_CHOP_ON_OFF   _IOW(PCIE_ADC_IOC_MAGIC, 22, u_int32_t)
//#define PCIE_ADC_IOCG_CHOP_ON_OFF   _IOR(PCIE_ADC_IOC_MAGIC, 23, u_int32_t)
////////////////////////////////////////////////////////////////////
#define PCIE_ADC_IOCT_CHOP_ON      _IO(PCIE_ADC_IOC_MAGIC, 22)
#define PCIE_ADC_IOCT_CHOP_OFF     _IO(PCIE_ADC_IOC_MAGIC, 23)
#define PCIE_ADC_IOCT_CHOP_CONSTANT_1   _IO(PCIE_ADC_IOC_MAGIC, 24)
#define PCIE_ADC_IOCT_CHOP_CONSTANT_0   _IO(PCIE_ADC_IOC_MAGIC, 25)
////////////////////////////////////////////////////////////////////
///////////////////////
#define PCIE_ADC_IOCS_CHOP_CONSTANT   _IOW(PCIE_ADC_IOC_MAGIC, 26, u_int32_t)
#define PCIE_ADC_IOCG_CHOP_CONSTANT   _IOR(PCIE_ADC_IOC_MAGIC, 27, u_int32_t)
///////////////////////
#define PCIE_ADC_IOCT_CHOP_RECONSTRUCTED_1   _IO(PCIE_ADC_IOC_MAGIC, 28)
#define PCIE_ADC_IOCT_CHOP_RECONSTRUCTED_0   _IO(PCIE_ADC_IOC_MAGIC, 29)
///////////////////////
#define PCIE_ADC_IOCT_OFFSET_FIRMWARE_CALCULATION_1   _IO(PCIE_ADC_IOC_MAGIC, 30)
#define PCIE_ADC_IOCT_OFFSET_FIRMWARE_CALCULATION_0   _IO(PCIE_ADC_IOC_MAGIC, 31)
///////////////////////
#define PCIE_ADC_IOCG_ADC_POS_INF       _IOR(PCIE_ADC_IOC_MAGIC, 32, u_int32_t)
#define PCIE_ADC_IOCG_ADC_NEG_INF       _IOR(PCIE_ADC_IOC_MAGIC, 33, u_int32_t)
#define PCIE_ADC_IOCG_ADC_OFFSET_INF       _IOR(PCIE_ADC_IOC_MAGIC, 34, u_int32_t)

#define PCIE_ADC_IOCG_COUNTER_OFFSET_P       _IOR(PCIE_ADC_IOC_MAGIC, 35, u_int32_t)
#define PCIE_ADC_IOCG_COUNTER_OFFSET_N       _IOR(PCIE_ADC_IOC_MAGIC, 36, u_int32_t)
#define PCIE_ADC_IOCG_COUNTER_OFFSET_I       _IOR(PCIE_ADC_IOC_MAGIC, 37, u_int32_t)

#define PCIE_ADC_IOCT_INTEGRAL_CALCULATION_1   _IO(PCIE_ADC_IOC_MAGIC, 38)
#define PCIE_ADC_IOCT_INTEGRAL_CALCULATION_0   _IO(PCIE_ADC_IOC_MAGIC, 39)

#define PCIE_ADC_IOCS_COUNTER_INTEGRAL   _IOW(PCIE_ADC_IOC_MAGIC, 40, u_int32_t)
#define PCIE_ADC_IOCG_COUNTER_INTEGRAL   _IOR(PCIE_ADC_IOC_MAGIC, 41, u_int32_t)

#define PCIE_ADC_IOCS_INTEGRAL_DRIFT       _IOW(PCIE_ADC_IOC_MAGIC, 42, OFFSET_REGS)
#define PCIE_ADC_IOCG_INTEGRAL_DRIFT       _IOR(PCIE_ADC_IOC_MAGIC, 43, OFFSET_REGS)


//#define PCIE_ADC_IOC_MAXNR   21
//#define PCIE_ADC_IOC_MAXNR   25
//#define PCIE_ADC_IOC_MAXNR   27
//#define PCIE_ADC_IOC_MAXNR   29
//#define PCIE_ADC_IOC_MAXNR   31
//#define PCIE_ADC_IOC_MAXNR   34
//#define PCIE_ADC_IOC_MAXNR   37
//#define PCIE_ADC_IOC_MAXNR   39
#define PCIE_ADC_IOC_MAXNR   43


/**
#define PCIE_ADC_IOCS_CONFIG_ACQ _IOW(PCIE_ADC_IOC_MAGIC, 2, u_int32_t)
#define PCIE_ADC_IOCT_DSP_HOLD _IO(PCIE_ADC_IOC_MAGIC, 1)
#define PCIE_ADC_IOCS_BAR1_WREG _IOW(PCIE_ADC_IOC_MAGIC, 5, uint32_t)
#define PCIE_ADC_IOCG_BAR1_RREG _IOR(PCIE_ADC_IOC_MAGIC, 6, uint32_t)
#define PCIE_ADC_IOCG_IRQ_CNT _IOR(PCIE_ADC_IOC_MAGIC, 7, u_int32_t)
#define PCIE_ADC_IOCT_SOFT_TRIG _IO(PCIE_ADC_IOC_MAGIC, 8)
#define PCIE_ADC_IOCT_DMA_ENABLE _IO(PCIE_ADC_IOC_MAGIC, 9)
#define PCIE_ADC_IOCT_DMA_DISABLE _IO(PCIE_ADC_IOC_MAGIC, 10)
#define PCIE_ADC_IOCS_TMOUT      _IOW(PCIE_ADC_IOC_MAGIC, 11, u_int32_t)
#define PCIE_ADC_IOCS_CHAN       _IOW(PCIE_ADC_IOC_MAGIC, 12, u_int32_t)

#define PCIE_ADC_IOCT_DSP_HOLD    _IO(PCIE_ADC_IOC_MAGIC, 20)
#define PCIE_ADC_IOCT_DSP_FREE    _IO(PCIE_ADC_IOC_MAGIC, 21)
#define PCIE_ADC_IOCT_DSP_RESET    _IO(PCIE_ADC_IOC_MAGIC, 22)
#define PCIE_ADC_IOCG_STATUS      _IOR(PCIE_ADC_IOC_MAGIC, 23, status_port_t)


//#define PCIE_ADC_IOC_RESET _IO(PCIE_ADC_IOC_MAGIC, 0)

//#define PCIE_ADC_IOCG_BRD_INFO  _IOR(PCIE_ADC_IOC_MAGIC, 1, BOARD_INFO)

//#define PCIE_ADC_IOCS_CR1 _IOW(PCIE_ADC_IOC_MAGIC, 2, u_int8_t)
//#define PCIE_ADC_IOCG_CR1 _IOR(PCIE_ADC_IOC_MAGIC, 3, u_int8_t)
#define PCIE_ADC_IOCS_CONFIG_ACQ _IOW(PCIE_ADC_IOC_MAGIC, 2, u_int32_t)
*/
#endif /* _PCIE_ADC_IOCTL_H_ */
