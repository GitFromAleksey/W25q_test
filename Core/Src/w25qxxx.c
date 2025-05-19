
#include <stdio.h>
#include <string.h>
#include "w25qxxx_defs.h"
#include "w25qxxx.h"



bool Is_W25Qxxx_Init = false;
w24qxxx_init_t INIT;

uint8_t   W25Qxxx_Device_ID;
uint16_t  W25Qxxx_Manufacturer_Device_ID;
uint32_t  W25Qxxx_JEDEC_ID;
uint8_t   W25Qxxx_UniqID[8];
uint32_t  W25Qxxx_BlockCount;
uint16_t  W25Qxxx_PageSize;
uint32_t  W25Qxxx_SectorSize;
uint32_t  W25Qxxx_SectorCount;
uint32_t  W25Qxxx_PageCount;
uint32_t  W25Qxxx_BlockSize;
uint32_t  W25Qxxx_CapacityInKiloByte;

#define INIT_CHECK                                   \
                  do                                 \
                  {                                  \
                      if(!Is_W25Qxxx_Init)           \
                          return w24qxxx_init_error; \
                  } while( 0 )
// ----------------------------------------------------------------------------
/** ###########################################################################
  * @brief  W25Qxxx module init function
  * @retval return status
  */
w24qxxx_statusTypeDef W25Qxxx_Init(w24qxxx_init_t * init)
{
  memcpy(&INIT, init, sizeof(w24qxxx_init_t));

  Is_W25Qxxx_Init = true;

  return w24qxxx_OK;
}
// ----------------------------------------------------------------------------
/** ###########################################################################
  * @brief spi transmit and receive
  * @retval return received data [Byte]
  */
static w24qxxx_statusTypeDef W25Qxxx_SPI_RxTx(uint8_t data, uint8_t *pret)
{
  INIT_CHECK;

  uint8_t ret;

  INIT.SPI_TransmitReceive(&data, &ret, 1);

  if(pret != NULL)
    *pret = ret;

  return w24qxxx_OK;
}
// ----------------------------------------------------------------------------
static w24qxxx_statusTypeDef W25Qxxx_SPI_Tx(uint8_t data)
{
  return W25Qxxx_SPI_RxTx(data, NULL);
}
// ----------------------------------------------------------------------------
static w24qxxx_statusTypeDef W25Qxxx_SPI_Rx(uint8_t data, uint8_t *pret)
{
  return W25Qxxx_SPI_RxTx(data, pret);
}
// ----------------------------------------------------------------------------
/** ###########################################################################
  * @brief  ReSet SPI-flash CS pin, spi-flash enter operation state
  *   Used before other operations
  * @param  none
  * @retval none
  */
static w24qxxx_statusTypeDef W25Qxxx_CsEnable(void)
{
  INIT_CHECK;

  INIT.CS_EnableDisable(true);

  return w24qxxx_OK;
}
// ----------------------------------------------------------------------------
/** ###########################################################################
  * @brief  Set SPI-flash CS pin, spi-flash enter disabled state
  *   Used after other operations
  * @param  none
  * @retval none
  */
static w24qxxx_statusTypeDef W25Qxxx_CsDisable(void)
{
  INIT_CHECK;

  INIT.CS_EnableDisable(false);

  return w24qxxx_OK;
}
// ----------------------------------------------------------------------------
/** ###########################################################################
  * @brief detect SR1-bit0 BUSY bit
  *
  */
static int8_t W25Qxxx_WaitForWriteEnd(void)
{
  uint8_t reg_res;

  W25Qxxx_CsEnable();

  W25Qxxx_SPI_Tx(CMD_Reg_1_Read);

  do
  {
    W25Qxxx_SPI_Rx(CMD_DUMMY, &reg_res);
  } while ((reg_res & SR1_S0_BUSY) == SR1_S0_BUSY);

  W25Qxxx_CsDisable();

  return 0; // passed return 0
}
// ----------------------------------------------------------------------------
static w24qxxx_statusTypeDef W25Qxxx_Reset(void)
{
  INIT_CHECK;

  uint8_t ret;

  W25Qxxx_CsEnable();
  W25Qxxx_SPI_Tx(CMD_Enable_Reset);
  W25Qxxx_SPI_Tx(CMD_Reset_Device);
  W25Qxxx_CsDisable();

  printf("W25Qxxx_Reset\r\n");

  return w24qxxx_OK;
}
// ----------------------------------------------------------------------------
static w24qxxx_statusTypeDef W25Qxxx_Power_Up(void)
{
  INIT_CHECK;

  uint8_t ret;

  W25Qxxx_CsEnable();

  W25Qxxx_SPI_Rx(CMD_Device_ID, &ret);
  W25Qxxx_SPI_Tx(CMD_DUMMY);
  W25Qxxx_SPI_Tx(CMD_DUMMY);
  W25Qxxx_SPI_Tx(CMD_DUMMY);

  W25Qxxx_SPI_Rx(CMD_DUMMY, &ret);
  W25Qxxx_Device_ID = ret;

  W25Qxxx_CsDisable();

  printf("W25Qxxx_Device_ID = 0x%X\r\n", W25Qxxx_Device_ID);

  return w24qxxx_OK;
}
// ----------------------------------------------------------------------------
/** ###########################################################################
  * @brief get W25Qxxx Manufacturer + Device ID [16-bit]
  */
static void W25Qxxx_Read_Manu_Dev_ID(void)
{
  uint8_t ret;

  W25Qxxx_CsEnable();

  W25Qxxx_SPI_RxTx(CMD_Manufacture_ID, &ret);

  W25Qxxx_SPI_RxTx(CMD_DUMMY, &ret);
  W25Qxxx_SPI_RxTx(CMD_DUMMY, &ret);
  W25Qxxx_SPI_RxTx(CMD_DUMMY, &ret);

  W25Qxxx_SPI_RxTx(CMD_DUMMY, &ret);
  W25Qxxx_Manufacturer_Device_ID = (ret << 8);
  W25Qxxx_SPI_RxTx(CMD_DUMMY, &ret);
  W25Qxxx_Manufacturer_Device_ID |= ret;

  W25Qxxx_CsDisable();

  printf("W25Qxxx_Manufacturer_Device_ID = 0x%X\r\n",
          W25Qxxx_Manufacturer_Device_ID);
}
// ----------------------------------------------------------------------------
/** ###########################################################################
  * @brief get W25Qxxx JEDEC ID [24-bit]
  */
static void W25Qxxx_Read_JEDEC_ID(void)
{
//  uint32_t JEDEC_ID = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;
  uint8_t ret;

  W25Qxxx_CsEnable();

  W25Qxxx_SPI_RxTx(CMD_JEDEC_ID, &ret);
  W25Qxxx_SPI_RxTx(CMD_DUMMY, &ret);
  W25Qxxx_JEDEC_ID = (ret<<16);
  W25Qxxx_SPI_RxTx(CMD_DUMMY, &ret);
  W25Qxxx_JEDEC_ID |= (ret<<8);
  W25Qxxx_SPI_RxTx(CMD_DUMMY, &ret);
  W25Qxxx_JEDEC_ID |= ret;

  W25Qxxx_CsDisable();

  printf("W25Qxxx_JEDEC_ID = 0x%X\r\n",
          W25Qxxx_JEDEC_ID);
}
// ----------------------------------------------------------------------------
/** ###########################################################################
  * @brief  get W25Qxxx Unique ID [64-bit 8Byte-array]
  */
static void W25Qxxx_Read_Unique_ID(void)
{
  uint8_t ret;

  W25Qxxx_CsEnable();

  W25Qxxx_SPI_RxTx(CMD_Unique_ID, &ret);
  for (uint8_t i = 0; i < 4; i++)
    W25Qxxx_SPI_Tx(CMD_DUMMY);
  for (uint8_t i = 0; i < 8; i++)
  {
    W25Qxxx_SPI_Rx(CMD_DUMMY, &ret);
    W25Qxxx_UniqID[i] = ret;
  }

  W25Qxxx_CsDisable();

  printf("W25Qxxx_UniqID = ");
  for (uint8_t i = 0; i < 8; i++)
    printf("0x%X,", W25Qxxx_UniqID[i]);
  printf("\r\n");
}
// ----------------------------------------------------------------------------
/** ###########################################################################
  * @brief  Read Status Register-1, 2, 3(05h, 35h, 15h)
  * @param  reg_x: [in] 1,2,3
  * @retval retrun SR_x value [Byte]
  */
static uint8_t W25Qxxx_Read_REG_x(W25Q512_STATUS_REG_NUM_t reg_x)
{
  uint8_t res;

  W25Qxxx_CsEnable();

  switch(reg_x)
  {
    case W25Q512_STATUS_REG1:
      W25Qxxx_SPI_Tx(CMD_Reg_1_Read);
      W25Qxxx_SPI_Rx(CMD_DUMMY, &res);
      break;
    case W25Q512_STATUS_REG2:
      W25Qxxx_SPI_Tx(CMD_Reg_2_Read);
      W25Qxxx_SPI_Rx(CMD_DUMMY, &res);
      break;
    case W25Q512_STATUS_REG3:
      W25Qxxx_SPI_Tx(CMD_Reg_3_Read);
      W25Qxxx_SPI_Rx(CMD_DUMMY, &res);
      break;
    default:
      break;
  }

  W25Qxxx_CsDisable();

  return res;
}
// ----------------------------------------------------------------------------
/** ###########################################################################
  * @brief Write Status Register-1, 2, 3 (01h, 31h, 11h)
  * @param reg_x: [in] reg_1,2,3
  * @param data:  [in] input reg_x data
  */
void W25Qxxx_Write_REG_x(uint8_t reg_x, uint8_t data)
{
  W25Qxxx_CsEnable();

  switch (reg_x)
  {
    case 1:
      W25Qxxx_SPI_Tx(CMD_Reg_1_Write);
      W25Qxxx_SPI_Tx(data);
      break;
    case 2:
      W25Qxxx_SPI_Tx(CMD_Reg_2_Write);
      W25Qxxx_SPI_Tx(data);
      break;
    case 3:
      W25Qxxx_SPI_Tx(CMD_Reg_3_Write);
      W25Qxxx_SPI_Tx(data);
      break;
    default:
      break;
  }

  W25Qxxx_CsDisable();
}
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
  * @brief  sector convert to pageAddr
  * @retval return pageAddr
  */
static uint32_t W25Qxxx_SectorToPage(uint32_t SectorAddress)
{ return (SectorAddress * W25Qxxx_SectorSize) / W25Qxxx_PageSize; }
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
  * @brief  blockAddr convert to pageAddr
  * @retval return pageAddr
  */
static uint32_t W25Qxxx_BlockToPage(uint32_t BlockAddress)
{
	return (BlockAddress * W25Qxxx_BlockSize) / W25Qxxx_PageSize;
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
/** ###########################################################################
  * @brief W25Qxxx Init global static variable
  * @retval status 0:passed  1:failed
  */
w24qxxx_statusTypeDef W25Qxxx_DeviceInit(void)
{
  char * dev_type_desc;
//	while (HAL_GetTick() < 20)
//		HAL_Delay(1);
  W25Qxxx_CsDisable();
//	HAL_Delay(20);

  W25Qxxx_Power_Up();
  W25Qxxx_Read_Manu_Dev_ID();
  W25Qxxx_Read_JEDEC_ID();

  switch (W25Qxxx_JEDEC_ID & 0x000000FF)
  {
    case W25Q512_JEDEC_ID:
      W25Qxxx_Device_ID  = W25Q512;
      W25Qxxx_BlockCount = W25Q512_BlockCount;
      dev_type_desc = "W25Q512";
      break;
    case W25Q256_JEDEC_ID:
      W25Qxxx_Device_ID  = W25Q256;
      W25Qxxx_BlockCount = W25Q256_BlockCount;
      dev_type_desc = "W25Q256";
      break;
    case W25Q128_JEDEC_ID:
      W25Qxxx_Device_ID  = W25Q128;
      W25Qxxx_BlockCount = W25Q128_BlockCount;
      dev_type_desc = "W25Q128";
      break;
    case W25Q64_JEDEC_ID:
      W25Qxxx_Device_ID  = W25Q64;
      W25Qxxx_BlockCount = W25Q64_BlockCount;
      dev_type_desc = "W25Q64";
      break;
    case W25Q32_JEDEC_ID:
      W25Qxxx_Device_ID  = W25Q32;
      W25Qxxx_BlockCount = W25Q32_BlockCount;
      dev_type_desc = "W25Q32";
      break;
    case W25Q16_JEDEC_ID:
      W25Qxxx_Device_ID  = W25Q16;
      W25Qxxx_BlockCount = W25Q16_BlockCount;
      dev_type_desc = "W25Q16";
      break;
    case W25Q80_JEDEC_ID:
      W25Qxxx_Device_ID  = W25Q80;
      W25Qxxx_BlockCount = W25Q80_BlockCount;
      dev_type_desc = "W25Q80";
      break;
    case W25Q40_JEDEC_ID:
      W25Qxxx_Device_ID  = W25Q40;
      W25Qxxx_BlockCount = W25Q40_BlockCount;
      dev_type_desc = "W25Q40";
      break;
    case W25Q20_JEDEC_ID:
      W25Qxxx_Device_ID  = W25Q20;
      W25Qxxx_BlockCount = W25Q20_BlockCount;
      dev_type_desc = "W25Q20";
      break;
    case W25Q10_JEDEC_ID:
      W25Qxxx_Device_ID  = W25Q10;
      W25Qxxx_BlockCount = W25Q10_BlockCount;
      dev_type_desc = "W25Q10";
      break;
    default:
      printf("Not W25Qxxx device detected.\r\n");
      return w24qxxx_ERROR;
  }

  W25Qxxx_PageSize            = W25QXXX_PAGESIZE;    // 256  Byte
  W25Qxxx_SectorSize          = W25QXXX_SECTORSIZE;  // 4096 Byte
  W25Qxxx_SectorCount         = W25Qxxx_BlockCount * W25QXXX_SECTORS_IN_BLOCK;
  W25Qxxx_PageCount           = (W25Qxxx_SectorCount * W25Qxxx_SectorSize) / W25Qxxx_PageSize;
  W25Qxxx_BlockSize           = W25Qxxx_SectorSize * W25QXXX_SECTORS_IN_BLOCK;
  W25Qxxx_CapacityInKiloByte  = (W25Qxxx_SectorCount * W25Qxxx_SectorSize) / 1024;
  W25Qxxx_Read_Unique_ID();

  printf("Device \"%s\" detected.\r\n", dev_type_desc);

  printf("UniqID: ");
  for (uint8_t i = 0; i < 8; i++)
    printf("%X:", W25Qxxx_UniqID[i]);
  printf("\r\n");

  printf("Capacity: %d kB\r\nBlockSize: %d B\r\nSectorCount: %d\r\n",
          W25Qxxx_CapacityInKiloByte,
          W25Qxxx_BlockSize,
          W25Qxxx_SectorCount
          );

  uint8_t regVal = W25Qxxx_Read_REG_x(W25Q512_STATUS_REG1);

  if ((regVal & SR1_S0_BUSY) == SR1_S0_BUSY)
    return w24qxxx_ERROR;

  return w24qxxx_OK;
}
// ----------------------------------------------------------------------------
/** ###########################################################################
  * @brief The Write Enable instruction (Figure 5) sets the Write Enable Latch (WEL) bit in the Status Register to a 1.
	The WEL bit must be set prior to every Page Program, Quad Page Program, Sector Erase, Block Erase,
	Chip Erase, Write Status Register and Erase/Program Security Registers instruction. The Write Enable
	instruction is entered by driving /CS low, shifting the instruction code “06h” into the Data Input (DI) pin on
	the rising edge of CLK, and then driving /CS high.
  * @param  none
  */
static void W25Qxxx_Write_Enable(void)
{
  uint8_t ret;

  W25Qxxx_CsEnable();

  W25Qxxx_SPI_RxTx(CMD_Write_Enable, &ret);

  W25Qxxx_CsDisable();
}
// ----------------------------------------------------------------------------
/** ############################################################################################
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
/** ###########################################################################
  * @brief Write Enable for Volatile Status Register (50h)
  * @param  none
  */
static void W25Qxxx_Write_Enable_SR(void)
{
  uint8_t ret;

  W25Qxxx_CsEnable();

  W25Qxxx_SPI_RxTx(CMD_Write_Enable_SR, &ret);

  W25Qxxx_CsDisable();
}
// ----------------------------------------------------------------------------
void Test(void)
{
//  static uint8_t Buffer[W25QXXX_PAGESIZE];

//  for(uint32_t block_num = 0; block_num < W25Qxxx_BlockCount; ++block_num)
//  {
//    printf("\r\nBlock num: %d\r\n", block_num);
//    for(uint32_t sector_num = 0; sector_num < W25QXXX_SECTORS_IN_BLOCK; ++sector_num )
//    {
//      printf("\r\nSector num: %d\r\n", sector_num);
//      for(uint32_t page_num = 0; page_num < W25QXXX_PAGES_IN_SECTOR; ++page_num )
//      {
//        printf("\r\nPage num: %d\r\n", page_num);
//        uint32_t start_addr = (block_num * W25QXXX_BLOCK_SIZE);
//        start_addr += (sector_num * W25QXXX_SECTORSIZE);
//        start_addr += (page_num * W25QXXX_PAGESIZE);
//        for(uint32_t byte_num = start_addr; byte_num < (start_addr+W25QXXX_PAGESIZE); ++byte_num )
//        {
//          W25Qxxx_ReadByte(&Buffer[0], byte_num);
//          printf("%X,", Buffer[0]);
//        }
//      }
//    }
//  }


//  printf("\r\n");
//  W25Qxxx_ReadBytes(Buffer, 0, W25QXXX_PAGESIZE);
//  for(int i = 0; i < W25QXXX_PAGESIZE; ++i)
//  {
//    printf("%X,", Buffer[i]);
//  }
//  printf("\r\n");
//  W25Qxxx_ReadBytes(Buffer, W25QXXX_PAGESIZE*W25QXXX_PAGES_IN_SECTOR, W25QXXX_PAGESIZE);
//  for(int i = 0; i < W25QXXX_PAGESIZE; ++i)
//  {
//    printf("%X,", Buffer[i]);
//  }
//  printf("\r\n---");

//  uint32_t cnt = 5;
//  printf("\r\n");
//  W25Qxxx_ReadPage(Buffer, 1, W25QXXX_PAGESIZE-cnt, cnt);
//  for(int i = 0; i < cnt; ++i)
//  {
//    printf("%X,", Buffer[i]);
//  }
//  printf("\r\n---");
//  
  
//  uint32_t cnt = 10;
//  printf("\r\n");
//  W25Qxxx_ReadSector(Buffer, (4*16)+5, 0, cnt);
//  for(int i = 0; i < cnt; ++i)
//  {
//    printf("%X,", Buffer[i]);
//  }
//  printf("\r\n---");

//  uint32_t cnt = 10;
//  printf("\r\n");
//  W25Qxxx_ReadBlock(Buffer, 4, (5*16*256), cnt);
//  for(int i = 0; i < cnt; ++i)
//  {
//    printf("%X,", Buffer[i]);
//  }
//  printf("\r\n---");

//  uint32_t addr = 256*17+4;
//  uint8_t wr_byte = 0x6;
//  uint8_t rd_byte = 0;

//  W25Qxxx_ReadByte(&rd_byte, addr);
//  printf("rd_byte:%X; addr: %X\r\n", rd_byte, addr);

//  W25Qxxx_WriteByte(wr_byte, addr);

//  W25Qxxx_ReadByte(&rd_byte, addr);
//  printf("wr_byte: %X - rd_byte:%X\r\n", wr_byte, rd_byte);

//  uint8_t wr_byte_buf[W25QXXX_PAGESIZE];
//  uint8_t rd_byte_buf[W25QXXX_PAGESIZE];
//  uint16_t cnt = W25QXXX_PAGESIZE;
//  uint32_t page = 18;
//  uint32_t offset = 0;

//  W25Qxxx_ReadPage(rd_byte_buf, page, offset, cnt);
//  for(int i = 0; i < cnt; ++i)
//  {
//    printf("%X,", rd_byte_buf[i]);
//  }
//  printf("\r\n");


//  for(int i = 0; i < cnt; ++i)
//  {
//    wr_byte_buf[i] = i;
//  }
//  W25Qxxx_WritePage(wr_byte_buf, page, offset, cnt);

//  W25Qxxx_ReadPage(rd_byte_buf, page, offset, cnt);
//  for(int i = 0; i < cnt; ++i)
//  {
//    printf("%X,", wr_byte_buf[i]);
//  }
//  printf("\r\n");
//  for(int i = 0; i < cnt; ++i)
//  {
//    printf("%X,", rd_byte_buf[i]);
//  }
//  printf("\r\n");

//#define SECTOR_SIZE    (W25QXXX_PAGES_IN_SECTOR * W25QXXX_PAGESIZE)
//  uint8_t wr_byte_buf[256];
//  uint16_t cnt = 256;
//  uint32_t sector = 4;
//  uint32_t offset = 256;

//  W25Qxxx_ReadSector(wr_byte_buf, sector, offset, cnt);
//  printf("\r\n");
//  for(int i = 0; i < cnt; ++i)
//    printf("%X,", wr_byte_buf[i]);
//  printf("\r\n");

//  printf("\r\n");
//  for(int i = 0; i < cnt; ++i)
//  {
//    wr_byte_buf[i] = i;
//    printf("%X,", wr_byte_buf[i]);
//  }
//  printf("\r\n");
//  W25Qxxx_WriteSector(wr_byte_buf, sector, offset, cnt);

//  W25Qxxx_ReadSector(wr_byte_buf, sector, offset, cnt);
//  printf("\r\n");
//  for(int i = 0; i < cnt; ++i)
//    printf("%X,", wr_byte_buf[i]);
//  printf("\r\n");


//  uint8_t wr_byte_buf[256];
//  uint16_t cnt = 256;
//  uint32_t block = 1;
//  uint32_t offset = 0;

//  W25Qxxx_ReadBlock(wr_byte_buf, block, offset, cnt);
//  printf("\r\n");
//  for(int i = 0; i < cnt; ++i)
//    printf("%X,", wr_byte_buf[i]);
//  printf("\r\n");
//  
//  printf("\r\n");
//  for(int i = 0; i < cnt; ++i)
//  {
//    wr_byte_buf[i] = i;
//    printf("%X,", wr_byte_buf[i]);
//  }
//  printf("\r\n");
//  W25Qxxx_WriteBlock(wr_byte_buf, block, offset, cnt);


//  W25Qxxx_ReadBlock(wr_byte_buf, block, offset, cnt);
//  printf("\r\n");
//  for(int i = 0; i < cnt; ++i)
//    printf("%X,", wr_byte_buf[i]);
//  printf("\r\n");

}
// ----------------------------------------------------------------------------
