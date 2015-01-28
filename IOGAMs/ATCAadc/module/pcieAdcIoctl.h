#ifndef _PCIE_ATCA_ADC_IOCTL_H_
#define _PCIE_ATCA_ADC_IOCTL_H_

/*
 * IOCTL definitions
 */
#define PCIE_ATCA_ADC_IOCT_MAGIC 0xEA  // /* Please use a different 8-bit number in your code */
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
#define PCIE_ATCA_ADC_IOCT_ACQ_ENABLE         _IO(PCIE_ATCA_ADC_IOCT_MAGIC, 1)
#define PCIE_ATCA_ADC_IOCT_ACQ_DISABLE        _IO(PCIE_ATCA_ADC_IOCT_MAGIC, 2)
#define PCIE_ATCA_ADC_IOCT_NUM_BOARDS         _IOR(PCIE_ATCA_ADC_IOCT_MAGIC, 3, u_int32_t)
#define PCIE_ATCA_ADC_IOCT_MASTER_SLOT_NUM    _IOR(PCIE_ATCA_ADC_IOCT_MAGIC, 4, u_int32_t)
#define PCIE_ATCA_ADC_IOCT_SET_EXT_CLK_TRG    _IOW(PCIE_ATCA_ADC_IOCT_MAGIC, 5, u_int32_t)
#define PCIE_ATCA_ADC_IOCT_IS_RTM_PRESENT     _IOWR(PCIE_ATCA_ADC_IOCT_MAGIC, 6, u_int32_t)
#define PCIE_ATCA_ADC_IOCT_SEND_SOFT_TRG      _IO(PCIE_ATCA_ADC_IOCT_MAGIC, 7)
#define PCIE_ATCA_ADC_IOCT_GET_BOARD_SLOT_NS  _IOR(PCIE_ATCA_ADC_IOCT_MAGIC, 8, u_int32_t)
#define PCIE_ATCA_ADC_IOCT_ACQ_DEBUG          _IO(PCIE_ATCA_ADC_IOCT_MAGIC, 9)
#define PCIE_ATCA_ADC_IOCT_N_IN_ANA_CHANNELS  _IOWR(PCIE_ATCA_ADC_IOCT_MAGIC, 10, u_int32_t)
#define PCIE_ATCA_ADC_IOCT_N_IN_DIG_CHANNELS  _IOWR(PCIE_ATCA_ADC_IOCT_MAGIC, 11, u_int32_t)
#define PCIE_ATCA_ADC_IOCT_N_OUT_ANA_CHANNELS _IOWR(PCIE_ATCA_ADC_IOCT_MAGIC, 12, u_int32_t)
#define PCIE_ATCA_ADC_IOCT_N_OUT_DIG_CHANNELS _IOWR(PCIE_ATCA_ADC_IOCT_MAGIC, 13, u_int32_t)
#define PCIE_ATCA_ADC_IOCT_MAXNR   13

#endif /* _PCIE_ATCA_ADC_IOCTL_H_ */
