#include <stdio.h>
#include <string.h>
#include "w25qxxx_defs.h"
#include "w25qxxx.h"


// ----------------------------------------------------------------------------
/** ###########################################################################
  * @brief write one Byte to w25qxxx flash
  * @param pBuffer: [in] input data
  * @param WriteAddr_inBytes: [in] indicate address
  * @retval status 0:passed  1:failed
  */
uint8_t W25Qxxx_WriteByte(const uint8_t byte, uint32_t WriteAddr_inBytes)
{
  if (W25Qxxx_Device_ID == 0)
    return 1; // w25qxx Unknown

  W25Qxxx_WaitForWriteEnd();

  W25Qxxx_Write_Enable();

  W25Qxxx_CsEnable();

  if(W25Qxxx_Device_ID >= W25Q256)
  {
    W25Qxxx_SPI_Tx(CMD_Page_Program_4_Byte_Addr);
    W25Qxxx_SPI_Tx((WriteAddr_inBytes & 0xFF000000) >> 24);
  }
  else
  {
    W25Qxxx_SPI_Tx(CMD_Page_Program);
  }

  W25Qxxx_SPI_Tx((WriteAddr_inBytes & 0xFF0000) >> 16);
  W25Qxxx_SPI_Tx((WriteAddr_inBytes & 0xFF00) >> 8);
  W25Qxxx_SPI_Tx(WriteAddr_inBytes & 0xFF);
  W25Qxxx_SPI_Tx(byte);

  W25Qxxx_CsDisable();

  W25Qxxx_WaitForWriteEnd();

  return 0;
}
// ----------------------------------------------------------------------------
/** ###########################################################################
  * @brief write Byte data to indicate page address
  * @param *pBuffer: [in] Byte data array
  * @param Page_Address: [in] page address (0 - W25Qxxx_PageCount-1)
  * @param OffsetInByte: [in] offset address
  * @retval status 0:passed  1:failed
  */
uint8_t W25Qxxx_WritePage(const uint8_t *pBuffer, uint32_t Page_Address,
                uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_PageSize)
{
  if(W25Qxxx_Device_ID == 0)
    return 1; // w25qxx Unknown

  if(((NumByteToWrite_up_to_PageSize + OffsetInByte) > W25Qxxx_PageSize) || (NumByteToWrite_up_to_PageSize == 0))
    NumByteToWrite_up_to_PageSize = W25Qxxx_PageSize - OffsetInByte;

  W25Qxxx_WaitForWriteEnd();

  W25Qxxx_Write_Enable();

  W25Qxxx_CsEnable();

  Page_Address = (Page_Address * W25Qxxx_PageSize) + OffsetInByte;
  if(W25Qxxx_Device_ID >= W25Q256)
  {
    W25Qxxx_SPI_Tx(CMD_Page_Program_4_Byte_Addr);
    W25Qxxx_SPI_Tx((Page_Address & 0xFF000000) >> 24);
  }
  else
  {
    W25Qxxx_SPI_Tx(CMD_Page_Program);
  }

  W25Qxxx_SPI_Tx((Page_Address & 0xFF0000) >> 16);
  W25Qxxx_SPI_Tx((Page_Address & 0xFF00) >> 8);
  W25Qxxx_SPI_Tx(Page_Address & 0xFF);

  for(int i = 0; i < NumByteToWrite_up_to_PageSize; ++i)
  {
    W25Qxxx_SPI_Tx(pBuffer[i]);
  }

  W25Qxxx_CsDisable();

  W25Qxxx_WaitForWriteEnd();

  return 0;
}
// ----------------------------------------------------------------------------
/** ###########################################################################
  * @brief write Byte data to indicate sector address 4KB Max based on page Write
  * @param *pBuffer: [in] Byte data array
  * @param Page_Address: [in] page address (0 - W25Qxxx_SectorCount-1)
  * @param OffsetInByte: [in] offset byte number
  * @param NumByteToWrite_up_to_SectorSize: [in] Byte data number
  * @retval status 0:passed  1:failed
  */
uint8_t W25Qxxx_WriteSector(const uint8_t *pBuffer, uint32_t Sector_Address,
                uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_SectorSize)
{
  uint32_t StartPage;
  int32_t BytesToWrite;
  uint32_t LocalOffset;

  if(W25Qxxx_Device_ID == 0)
    return 1;  // w25qxx Unknown

  if((NumByteToWrite_up_to_SectorSize > W25Qxxx_SectorSize) || (NumByteToWrite_up_to_SectorSize == 0))
    NumByteToWrite_up_to_SectorSize = W25Qxxx_SectorSize;

  if(OffsetInByte >= W25Qxxx_SectorSize)
    return 1;

  if((OffsetInByte + NumByteToWrite_up_to_SectorSize) > W25Qxxx_SectorSize)
    BytesToWrite = W25Qxxx_SectorSize - OffsetInByte;
  else
    BytesToWrite = NumByteToWrite_up_to_SectorSize;

  StartPage = W25Qxxx_SectorToPage(Sector_Address) + (OffsetInByte / W25Qxxx_PageSize);
  LocalOffset = OffsetInByte % W25Qxxx_PageSize;

  do
  {
    uint8_t res = W25Qxxx_WritePage(pBuffer, StartPage, LocalOffset, BytesToWrite);

    if(res != 0)
      return 1;

    StartPage++;
    BytesToWrite -= W25Qxxx_PageSize - LocalOffset;
    pBuffer += W25Qxxx_PageSize - LocalOffset;
    LocalOffset = 0;

  } while (BytesToWrite > 0);

  return 0;
}
// ----------------------------------------------------------------------------
/** ###########################################################################
  * @brief write Byte data to indicate block address 64KB Max base on page Write
  * @param *pBuffer: [in] Byte data array
  * @param Block_Address: [in] page address (0 - W25Qxxx_BlockCount-1)
  * @param OffsetInByte: [in] offset byte number
  * @param NumByteToWrite_up_to_BlockSize: [in] Byte data number
  * @retval status 0:passed  1:failed
  */
uint8_t W25Qxxx_WriteBlock(const uint8_t *pBuffer, uint32_t Block_Address, 
                uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_BlockSize)
{
  uint32_t StartPage;
  int32_t BytesToWrite;
  uint32_t LocalOffset;

  if(W25Qxxx_Device_ID == 0)
    return 1; // w25qxx Unknown

  if((NumByteToWrite_up_to_BlockSize > W25Qxxx_BlockSize) || (NumByteToWrite_up_to_BlockSize == 0))
    NumByteToWrite_up_to_BlockSize = W25Qxxx_BlockSize;

  if(OffsetInByte >= W25Qxxx_BlockSize)
    return 1;

  if((OffsetInByte + NumByteToWrite_up_to_BlockSize) > W25Qxxx_BlockSize)
    BytesToWrite = W25Qxxx_BlockSize - OffsetInByte;
  else
    BytesToWrite = NumByteToWrite_up_to_BlockSize;

  StartPage = W25Qxxx_BlockToPage(Block_Address) + (OffsetInByte / W25Qxxx_PageSize);
  LocalOffset = OffsetInByte % W25Qxxx_PageSize;

  do
  {
    uint8_t res = W25Qxxx_WritePage(pBuffer, StartPage, LocalOffset, BytesToWrite);

    if(res != 0)
      return 1;

    StartPage++;
    BytesToWrite -= W25Qxxx_PageSize - LocalOffset;
    pBuffer += W25Qxxx_PageSize - LocalOffset;
    LocalOffset = 0;

  } while (BytesToWrite > 0);

  return 0;
}
// ----------------------------------------------------------------------------
