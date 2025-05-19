#include <stdio.h>
#include <string.h>
#include "w25qxxx_defs.h"
#include "w25qxxx.h"

// ----------------------------------------------------------------------------
/** ###########################################################################
  * @brief  Chip Erase
  * @retval status 0:passed  1:failed
  */
uint8_t W25Qxxx_EraseChip(void)
{
  if (W25Qxxx_Device_ID == 0)
    return 1; // w25qxx Unknown

  W25Qxxx_WaitForWriteEnd();

  W25Qxxx_Write_Enable();

  W25Qxxx_CsEnable();
  W25Qxxx_SPI_Tx(CMD_Erase_Chip);
  W25Qxxx_CsDisable();

  W25Qxxx_WaitForWriteEnd();

  return 0;
}
// ----------------------------------------------------------------------------
/** ###########################################################################
  * @brief  Sector erase 4KB
  * @param  SectorAddr: [in] 0 ~ W25Qxxx_SectorCount-1
  * @retval status 0:passed  1:failed
  */
uint8_t W25Qxxx_EraseSector(uint32_t SectorAddr)
{
  if (W25Qxxx_Device_ID == 0)
    return 1; // w25qxx Unknown

  W25Qxxx_WaitForWriteEnd();

  SectorAddr = SectorAddr * W25Qxxx_SectorSize;
  W25Qxxx_Write_Enable();

  W25Qxxx_CsEnable();

  if (W25Qxxx_Device_ID >= W25Q256)
  {
    W25Qxxx_SPI_Tx(CMD_Erase_Sector_4_Byte_Addr);
    W25Qxxx_SPI_Tx((SectorAddr & 0xFF000000) >> 24);
  }
  else
  {
    W25Qxxx_SPI_Tx(CMD_Erase_Sector);
  }

  W25Qxxx_SPI_Tx((SectorAddr & 0xFF0000) >> 16);
  W25Qxxx_SPI_Tx((SectorAddr & 0xFF00) >> 8);
  W25Qxxx_SPI_Tx( SectorAddr & 0xFF);
  W25Qxxx_CsDisable();

  W25Qxxx_WaitForWriteEnd();

  return 0;
}
// ----------------------------------------------------------------------------
/** ###########################################################################
  * @brief Erase block 64KB
  * @param BlockAddr: [in] 0 ~ W25Qxxx_BlockCount-1
  * @retval status 0:passed  1:failed
  */
uint8_t W25Qxxx_EraseBlock(uint32_t BlockAddr)
{
  if (W25Qxxx_Device_ID == 0)
    return 1; // w25qxx Unknown

  W25Qxxx_WaitForWriteEnd();

  BlockAddr = BlockAddr * W25Qxxx_BlockSize;
  W25Qxxx_Write_Enable();

  W25Qxxx_CsEnable();

  if (W25Qxxx_Device_ID >= W25Q256)
  {
    W25Qxxx_SPI_Tx(CMD_Erase_Block_64K_4_Byte_Addr);
    W25Qxxx_SPI_Tx((BlockAddr & 0xFF000000) >> 24);
  }
  else
  {
    W25Qxxx_SPI_Tx(CMD_Erase_Block_64K);
  }

  W25Qxxx_SPI_Tx((BlockAddr & 0xFF0000) >> 16);
  W25Qxxx_SPI_Tx((BlockAddr & 0xFF00) >> 8);
  W25Qxxx_SPI_Tx(BlockAddr & 0xFF);
  W25Qxxx_CsDisable();

  W25Qxxx_WaitForWriteEnd();

  return 0;
}
// ----------------------------------------------------------------------------
