#include "FatFsInterface.h"
#include "w25qxxx.h"

static volatile DSTATUS Stat = STA_NOINIT;

// ----------------------------------------------------------------------------
const SPIFLASHDiskio_drvTypeDef  SPI_Flash_Driver =
{
  W25Qxxx_diskio_initialize,
  W25Qxxx_diskio_status,
  W25Qxxx_diskio_read,
  W25Qxxx_diskio_write,
  W25Qxxx_diskio_ioctl,
};
// ----------------------------------------------------------------------------
/*!< Initialize Disk Drive */
DSTATUS W25Qxxx_diskio_initialize(void)
{
  Stat = 0;
  return Stat;
}
// ----------------------------------------------------------------------------
/*!< Get Disk Status */
DSTATUS W25Qxxx_diskio_status(void)
{
  Stat = STA_NOINIT;
  Stat &= ~STA_NOINIT;

  return Stat;
}
// ----------------------------------------------------------------------------
/*!< Read Sector(s) */
DRESULT W25Qxxx_diskio_read(BYTE *buff, DWORD sector, UINT count)
{
  DRESULT res = RES_ERROR;

  W25Qxxx_Read_FS(buff, sector, count);
  res = RES_OK;

  return res;
}
// ----------------------------------------------------------------------------
/*!< Write Sector(s) when _USE_WRITE = 0 */
DRESULT W25Qxxx_diskio_write(const BYTE *buff, DWORD sector, UINT count)
{
  DRESULT res = RES_ERROR;

  W25Qxxx_Write_FS((BYTE *)buff, sector, count);

  res = RES_OK;

  return res;
}
// ----------------------------------------------------------------------------
/*!< I/O control operation when _USE_IOCTL = 1 */
DRESULT W25Qxxx_diskio_ioctl(BYTE cmd, void *buff)
{
  DRESULT res = RES_ERROR;
  uint32_t block_num;
  uint16_t block_size;

//  if(Stat & STA_NOINIT)
//    return RES_NOTRDY;

  switch(cmd)
  {
    /* Make sure that no pending write process */
    case CTRL_SYNC:
      res = RES_OK;
      break;
    /* Get number of sectors on the disk (DWORD) */
    case GET_SECTOR_COUNT :
      W25Qxxx_GetCapacity(&block_num, &block_size);
      *(DWORD*)buff = block_num;
      res = RES_OK;
      break;
    /* Get R/W sector size (WORD) */
    case GET_SECTOR_SIZE:
      W25Qxxx_GetCapacity(&block_num, &block_size);
      *(WORD*)buff = block_size;
      res = RES_OK;
      break;
    /* Get erase block size in unit of sector (DWORD) */
    case GET_BLOCK_SIZE:
      *(DWORD*)buff = 1;
      res = RES_OK;
      break;
    default:
      res = RES_PARERR;
  }

  return res;
}
// ----------------------------------------------------------------------------
