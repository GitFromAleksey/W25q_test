#ifndef _FAT_FS_INTERFACE_H_
#define _FAT_FS_INTERFACE_H_


#ifdef __cplusplus
    extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "diskio.h"

typedef struct
{
  DSTATUS (*disk_initialize) (void);                     /*!< Initialize Disk Drive                     */
  DSTATUS (*disk_status)     (void);                     /*!< Get Disk Status                           */
  DRESULT (*disk_read)       (BYTE*, DWORD, UINT);       /*!< Read Sector(s)                            */
  DRESULT (*disk_write)      (const BYTE*, DWORD, UINT); /*!< Write Sector(s) when _USE_WRITE = 0       */
  DRESULT (*disk_ioctl)      (BYTE, void*);              /*!< I/O control operation when _USE_IOCTL = 1 */

}SPIFLASHDiskio_drvTypeDef;

/*!< Initialize Disk Drive */
DSTATUS W25Qxxx_diskio_initialize(void);
/*!< Get Disk Status */
DSTATUS W25Qxxx_diskio_status(void);
/*!< Read Sector(s) */
DRESULT W25Qxxx_diskio_read(BYTE *buff, DWORD sector, UINT count);
/*!< Write Sector(s) when _USE_WRITE = 0 */
DRESULT W25Qxxx_diskio_write(const BYTE *buff, DWORD sector, UINT count);
/*!< I/O control operation when _USE_IOCTL = 1 */
DRESULT W25Qxxx_diskio_ioctl(BYTE cmd, void *buff);

extern const SPIFLASHDiskio_drvTypeDef  SPI_Flash_Driver;

#ifdef __cplusplus
    }
#endif

#endif /* _FAT_FS_INTERFACE_H_ */
