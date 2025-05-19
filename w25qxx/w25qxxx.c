
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
w24qxxx_statusTypeDef W25Qxxx_Reset(void)
{
  INIT_CHECK;

//  uint8_t ret;

  W25Qxxx_CsEnable();
  W25Qxxx_SPI_Tx(CMD_Enable_Reset);
  W25Qxxx_SPI_Tx(CMD_Reset_Device);
  W25Qxxx_CsDisable();

  printf("W25Qxxx_Reset\r\n");

  return w24qxxx_OK;
}
// ----------------------------------------------------------------------------
w24qxxx_statusTypeDef W25Qxxx_Power_Up(void)
{
  INIT_CHECK;

  uint8_t ret;

  W25Qxxx_Reset();

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
void W25Qxxx_Read_Manu_Dev_ID(void)
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
void W25Qxxx_Read_JEDEC_ID(void)
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
void W25Qxxx_Read_Unique_ID(void)
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
uint8_t W25Qxxx_Read_REG_x(W25Q512_STATUS_REG_NUM_t reg_x)
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


//  uint8_t res;

//  res = W25Qxxx_EraseChip();
//  
//  printf("W25Qxxx_EraseChip: %d\r\n", res);


//// ------------------------------ проверка стирания сктора
//#define SECTOR_SIZE    (W25QXXX_PAGES_IN_SECTOR * W25QXXX_PAGESIZE)
//#define BUF_SIZE    256
//#define SECTOR_NUM  4
//  uint8_t wr_byte_buf[BUF_SIZE];
//  uint16_t cnt    = BUF_SIZE;
//  uint32_t sector = SECTOR_NUM;
//  uint32_t offset = BUF_SIZE;

//  W25Qxxx_ReadSector(wr_byte_buf, sector, offset, cnt);
//  printf("Read sector num: %d, size: %d\r\n", sector, cnt);
//  for(int i = 0; i < cnt; ++i)
//    printf("%X,", wr_byte_buf[i]);
//  printf("\r\n");

//  for(int i = 0; i < cnt; ++i)
//    wr_byte_buf[i] = i;
//  W25Qxxx_WriteSector(wr_byte_buf, sector, offset, cnt);

//  W25Qxxx_ReadSector(wr_byte_buf, sector, offset, cnt);
//  printf("Read sector num: %d, size: %d\r\n", sector, cnt);
//  for(int i = 0; i < cnt; ++i)
//    printf("%X,", wr_byte_buf[i]);
//  printf("\r\n");

//  printf("W25Qxxx_EraseSector\r\n");
//  W25Qxxx_EraseSector(sector);

//  W25Qxxx_ReadSector(wr_byte_buf, sector, offset, cnt);
//  printf("Read sector num: %d, size: %d\r\n", sector, cnt);
//  for(int i = 0; i < cnt; ++i)
//    printf("%X,", wr_byte_buf[i]);
//  printf("\r\n");
//// ------------------------------

//// ------------------------------ проверка стирания блока
//  uint8_t wr_byte_buf[256];
//  uint16_t cnt    = 256;
//  uint32_t block  = 1;
//  uint32_t offset = 0;

//  W25Qxxx_ReadBlock(wr_byte_buf, block, offset, cnt);
//  printf("W25Qxxx_ReadBlock addr: %d\r\n", block);
//  for(int i = 0; i < cnt; ++i)
//    printf("%X,", wr_byte_buf[i]);
//  printf("\r\n");
//  
//  printf("W25Qxxx_WriteBlock: %d\r\n", block);
//  for(int i = 0; i < cnt; ++i)
//    wr_byte_buf[i] = i;
//  printf("\r\n");
//  W25Qxxx_WriteBlock(wr_byte_buf, block, offset, cnt);

//  W25Qxxx_ReadBlock(wr_byte_buf, block, offset, cnt);
//  printf("W25Qxxx_ReadBlock addr: %d\r\n", block);
//  for(int i = 0; i < cnt; ++i)
//    printf("%X,", wr_byte_buf[i]);
//  printf("\r\n");

//  printf("W25Qxxx_EraseBlock: %d\r\n", block);
//  W25Qxxx_EraseBlock(block);

//  W25Qxxx_ReadBlock(wr_byte_buf, block, offset, cnt);
//  printf("W25Qxxx_ReadBlock addr: %d\r\n", block);
//  for(int i = 0; i < cnt; ++i)
//    printf("%X,", wr_byte_buf[i]);
//  printf("\r\n");

//// ------------------------------ проверка стирания блока

}
// ----------------------------------------------------------------------------
