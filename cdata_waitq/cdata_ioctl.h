#ifndef __CDATA_IOCTL_H__
#define __CDATA_IOCTL_H__

#include <linux/ioctl.h>

#define DEV_IOCTLID 0xD0 
#define IOCTL_EMPTY _IO(DEV_IOCTLID, 1)
#define IOCTL_SYNC  _IO(DEV_IOCTLID, 2)

#endif /* __CDATA_IOCTL_H__ */
