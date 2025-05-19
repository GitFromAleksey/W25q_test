#include <stdio.h>
#include <string.h>
#include "w25qxxx_defs.h"
#include "w25qxxx.h"

// ----------------------------------------------------------------------------
/** ###########################################################################
  * @brief  read one Byte data from indicate address
  * @param  *pBuffer: [out] receive read byte data
  * @param  Bytes_Address: [in] address 0 ~ (W25Qxxx_CapacityInKiloByte-1)*1024
  * @retval status 0:passed  1:failed
  */
uint8_t W25Qxxx_ReadByte(uint8_t *pBuffer, uint32_t Bytes_Address)
{
  if(W25Qxxx_Device_ID == 0)
    return 1; // w25qxx Unknown

//  if(W25Qxxx_WaitForWriteEnd())
//    return 1;

  W25Qxxx_CsEnable();

  if (W25Qxxx_Device_ID >= W25Q256)
  {
    W25Qxxx_SPI_Tx(CMD_Fast_Read_4_Byte_Addr);
    W25Qxxx_SPI_Tx((Bytes_Address & 0xFF000000) >> 24);
  }
  else
  {
    W25Qxxx_SPI_Tx(CMD_Fast_Read);
  }

  W25Qxxx_SPI_Tx((Bytes_Address & 0xFF0000) >> 16);
  W25Qxxx_SPI_Tx((Bytes_Address & 0xFF00) >> 8);
  W25Qxxx_SPI_Tx(Bytes_Address & 0xFF);
  W25Qxxx_SPI_Tx(CMD_DUMMY);

  W25Qxxx_SPI_Rx(CMD_DUMMY, pBuffer);

  W25Qxxx_CsDisable();

  return 0;
}
// ----------------------------------------------------------------------------
/** ###########################################################################
  * @brief read indicate number bytes
  * @param *pBuffer: [out] receive bytes
  * @param ReadAddr: [in] address 0 ~ (W25Qxxx_CapacityInKiloByte-1)*1024
  * @param NumByteToRead: [in] numbers
  * @retval status 0:passed   1:failed
  */
uint8_t W25Qxxx_ReadBytes(uint8_t *pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead)
{
  if(W25Qxxx_Device_ID == 0)
    return 1;		// w25qxx Unknown

//  if(W25Qxxx_WaitForWriteEnd())
//    return 1;

  W25Qxxx_CsEnable();

  if (W25Qxxx_Device_ID >= W25Q256)
  {
    W25Qxxx_SPI_Tx(CMD_Fast_Read_4_Byte_Addr);
    W25Qxxx_SPI_Tx((ReadAddr & 0xFF000000) >> 24);
  }
  else
  {
    W25Qxxx_SPI_Tx(CMD_Fast_Read);
  }

  W25Qxxx_SPI_Tx((ReadAddr & 0xFF0000) >> 16);
  W25Qxxx_SPI_Tx((ReadAddr & 0xFF00) >> 8);
  W25Qxxx_SPI_Tx(ReadAddr & 0xFF);
  W25Qxxx_SPI_Tx(CMD_DUMMY);

  for(int i = 0; i < NumByteToRead; ++i)
  {
    W25Qxxx_SPI_RxTx(CMD_DUMMY, &pBuffer[i]);
  }
//  if(HAL_SPI_Receive(&hspi_flash, pBuffer, NumByteToRead, SPI_FLASH_TIMEOUT) != HAL_OK)
//  {
//    W25Qxxx_CsDisable();
//    return 1;
//  }

  W25Qxxx_CsDisable();

  return 0;
}
// ----------------------------------------------------------------------------
/** ###########################################################################
  * @brief read a page from indicate page-address
  * @param *pBuffer: [out] receive bytes
  * @param Page_Address: [in] page address (0 - W25Qxxx_PageCount-1)
  * @param OffsetInByte: [in] offset byte number   [0 --- offset ------ 255]
  * @param NumByteToRead_up_to_PageSize: [in] read byte number  max 256Bytes
  * @retval status 0:passed   1:failed
  */
uint8_t W25Qxxx_ReadPage(uint8_t *pBuffer, uint32_t Page_Address,
                  uint32_t OffsetInByte, uint32_t NumByteToRead_up_to_PageSize)
{
  if (W25Qxxx_Device_ID == 0)
    return 1; // w25qxx Unknown

  if ((NumByteToRead_up_to_PageSize > W25Qxxx_PageSize) || (NumByteToRead_up_to_PageSize == 0))
    NumByteToRead_up_to_PageSize = W25Qxxx_PageSize;
  if ((OffsetInByte + NumByteToRead_up_to_PageSize) > W25Qxxx_PageSize)
    NumByteToRead_up_to_PageSize = W25Qxxx_PageSize - OffsetInByte;

  Page_Address = Page_Address * W25Qxxx_PageSize + OffsetInByte;

  W25Qxxx_CsEnable();

  if (W25Qxxx_Device_ID >= W25Q256)
  {
    W25Qxxx_SPI_Tx(CMD_Fast_Read_4_Byte_Addr);
    W25Qxxx_SPI_Tx((Page_Address & 0xFF000000) >> 24);
  }
  else
  {
    W25Qxxx_SPI_Tx(CMD_Fast_Read);
  }

  W25Qxxx_SPI_Tx((Page_Address & 0xFF0000) >> 16);
  W25Qxxx_SPI_Tx((Page_Address & 0xFF00) >> 8);
  W25Qxxx_SPI_Tx(Page_Address & 0xFF);
  W25Qxxx_SPI_Tx(CMD_DUMMY);

  for(int i = 0; i < NumByteToRead_up_to_PageSize; ++i)
  {
    W25Qxxx_SPI_RxTx(CMD_DUMMY, &pBuffer[i]);
  }

  W25Qxxx_CsDisable();

  return 0;
}
// ----------------------------------------------------------------------------
/** ###########################################################################
  * @brief read a sector from indicate sector-address
  * @param *pBuffer: [out] receive bytes
  * @param Sector_Address: [in] sector address (0 - W25Qxxx_SectorCount-1)
  * @param OffsetInByte: [in] offset byte number
  * @param NumByteToRead_up_to_SectorSize: [in] read byte number  max 4096Bytes
  * @retval status 0:passed   1:failed
  */
uint8_t W25Qxxx_ReadSector(uint8_t *pBuffer, uint32_t Sector_Address, 
                uint32_t OffsetInByte, uint32_t NumByteToRead_up_to_SectorSize)
{
  if(W25Qxxx_Device_ID == 0)
    return 1; // w25qxx Unknown

  if((NumByteToRead_up_to_SectorSize > W25Qxxx_SectorSize) || (NumByteToRead_up_to_SectorSize == 0))
    NumByteToRead_up_to_SectorSize = W25Qxxx_SectorSize;

  if(OffsetInByte >= W25Qxxx_SectorSize)
    return 1;

  uint32_t StartPage;
  int32_t  BytesToRead;
  uint32_t LocalOffset;

  if((OffsetInByte + NumByteToRead_up_to_SectorSize) > W25Qxxx_SectorSize)
    BytesToRead = W25Qxxx_SectorSize - OffsetInByte;
  else
    BytesToRead = NumByteToRead_up_to_SectorSize;

  StartPage = W25Qxxx_SectorToPage(Sector_Address) + (OffsetInByte / W25Qxxx_PageSize);
  LocalOffset = OffsetInByte % W25Qxxx_PageSize;

  do
  {
    W25Qxxx_ReadPage(pBuffer, StartPage, LocalOffset, BytesToRead);
    StartPage++;
    BytesToRead -= W25Qxxx_PageSize - LocalOffset;
    pBuffer     += W25Qxxx_PageSize - LocalOffset;
    LocalOffset = 0;
  } while (BytesToRead > 0);

  return 0;
}
// ----------------------------------------------------------------------------
/** ###########################################################################
  * @brief read a block bytes data from block-address
  * @param *pBuffer: [out] receive bytes
  * @param Block_Address: [in] sector address (0 - W25Qxxx_BLockCount-1)
  * @param OffsetInByte: [in] offset byte number
  * @param NumByteToRead_up_to_BlockSize: [in] read byte number  max 64KiBytes
  * @retval status 0:passed   1:failed
  */
uint8_t W25Qxxx_ReadBlock(uint8_t *pBuffer, uint32_t Block_Address, 
                  uint32_t OffsetInByte, uint32_t NumByteToRead_up_to_BlockSize)
{
  uint32_t StartPage;
  int32_t  BytesToRead;
  uint32_t LocalOffset;

  if(W25Qxxx_Device_ID == 0)
    return 1; // w25qxx Unknown

  if((NumByteToRead_up_to_BlockSize > W25Qxxx_BlockSize) || 
                                          (NumByteToRead_up_to_BlockSize == 0))
    NumByteToRead_up_to_BlockSize = W25Qxxx_BlockSize;

  if(OffsetInByte >= W25Qxxx_BlockSize)
    return 1;

  if((OffsetInByte + NumByteToRead_up_to_BlockSize) > W25Qxxx_BlockSize)
    BytesToRead = W25Qxxx_BlockSize - OffsetInByte;
  else
    BytesToRead = NumByteToRead_up_to_BlockSize;

  StartPage   = W25Qxxx_BlockToPage(Block_Address) + (OffsetInByte / W25Qxxx_PageSize);
  LocalOffset = OffsetInByte % W25Qxxx_PageSize;

  do
  {
    W25Qxxx_ReadPage(pBuffer, StartPage, LocalOffset, BytesToRead);
    StartPage++;
    BytesToRead -= W25Qxxx_PageSize - LocalOffset;
    pBuffer += W25Qxxx_PageSize - LocalOffset;
    LocalOffset = 0;
  } while (BytesToRead > 0);

  return 0;
}
// ----------------------------------------------------------------------------
