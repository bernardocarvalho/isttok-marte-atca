//******************************************************************************
//    $Log: pcieAdc_ioctl.h,v $
//    Revision 1.1  2008/11/28 12:04:00  aneto
//    A fake atca low level driver
//
//    Revision 1.3  2008/06/06 11:15:49  aneto
//    Work in progress
//
//    Revision 1.2  2008/06/05 10:07:02  rvitelli
//    Added Bitfields
//
//    Revision 1.1  2008/06/05 07:59:57  rvitelli
//    First ATCA module cleanup
//
//
//******************************************************************************

#ifndef _PCIE_ADC_IOCTL_H_
#define _PCIE_ADC_IOCTL_H_


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
#define PCIE_ADC_IOCT_INT_ENABLE _IO(PCIE_ADC_IOC_MAGIC, 1)
#define PCIE_ADC_IOCT_INT_DISABLE _IO(PCIE_ADC_IOC_MAGIC, 2)
#define PCIE_ADC_IOC_MAXNR   2

/*
#define PCIE_ADC_IOCS_CONFIG_ACQ _IOW(PCIE_ADC_IOC_MAGIC, 2, u_int32_t)
//#define PCIE_ADC_IOCT_DSP_HOLD _IO(PCIE_ADC_IOC_MAGIC, 1)
//#define PCIE_ADC_IOCS_BAR1_WREG _IOW(PCIE_ADC_IOC_MAGIC, 5, uint32_t)
//#define PCIE_ADC_IOCG_BAR1_RREG _IOR(PCIE_ADC_IOC_MAGIC, 6, uint32_t)
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
