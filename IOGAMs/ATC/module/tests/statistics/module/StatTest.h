#ifndef STAT_TEST_H_
#define STAT_TEST_H_

#include <asm/ioctl.h>


#define    STAT_TEST_IOCTL_MAGIC          0xc2

#define    IOCTL_EXT_CLK_TRIGGER          _IOW(STAT_TEST_IOCTL_MAGIC, 0, int)

#define    IOCTL_SOFT_TRIGGER             _IOW(STAT_TEST_IOCTL_MAGIC, 1, int)

#define    IOCTL_SAMPLE_TIME_NS           _IOW(STAT_TEST_IOCTL_MAGIC, 2, int)

#define    IOCTL_ACQ_TOTAL_TIME_NS        _IOW(STAT_TEST_IOCTL_MAGIC, 3, int)

#define    IOCTL_CPU_MASK                 _IOW(STAT_TEST_IOCTL_MAGIC, 4, int)

#define    IOCTL_ACQ_ALL_DATA             _IOW(STAT_TEST_IOCTL_MAGIC, 5, int)

#define    IOCTL_ACQ_COMPLETE             _IOR(STAT_TEST_IOCTL_MAGIC, 6, int)

#define    IOCTL_START_ACQ                _IO(STAT_TEST_IOCTL_MAGIC, 7)

#define    IOCTL_ACQ_W33                  _IOR(STAT_TEST_IOCTL_MAGIC, 8, int)

#define    IOCTL_NUM_BOARDS           _IOR(STAT_TEST_IOCTL_MAGIC, 9, int)

#define    IOCTL_BOARD_SLOTS          _IOR(STAT_TEST_IOCTL_MAGIC, 10, int)

#define    IOCTL_ENABLE_ACQ           _IO(STAT_TEST_IOCTL_MAGIC, 11)

#define    IOCTL_DISABLE_ACQ          _IO(STAT_TEST_IOCTL_MAGIC, 12)

#define    IOCTL_START_DAC_TEST       _IO(STAT_TEST_IOCTL_MAGIC, 13)

#define    IOCTL_STOP_DAC_TEST       _IO(STAT_TEST_IOCTL_MAGIC, 14)

#endif

