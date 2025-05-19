#ifndef _W25QXXX_DEFS_H_
#define _W25QXXX_DEFS_H_

#ifdef __cplusplus
    extern "C" {
#endif

#include <stdint.h>
#include "w25qxxx.h"


#define W25QXXX_PAGESIZE          0x100   // 256  Byte
#define W25QXXX_PAGES_IN_SECTOR   16u
#define W25QXXX_SECTORSIZE        0x1000  // 4096 Byte
#define W25QXXX_SECTORS_IN_BLOCK  16u
#define W25QXXX_BLOCK_SIZE        0x10000 // 64 kB
/**
  * @brief Indicate the equipment used
  */
typedef enum
{
  W25Q10 = 1,
  W25Q20,
  W25Q40,
  W25Q80,
  W25Q16,
  W25Q32,
  W25Q64,
  W25Q128,
  W25Q256,
  W25Q512,

} W25QXXX_ID_t;

/**
  * @brief Indicate device num of blocks
  */
typedef enum
{
  W25Q10_BlockCount  = (uint32_t)2,
  W25Q20_BlockCount  = (uint32_t)4,
  W25Q40_BlockCount  = (uint32_t)8,
  W25Q80_BlockCount  = (uint32_t)16,
  W25Q16_BlockCount  = (uint32_t)32,
  W25Q32_BlockCount  = (uint32_t)64,
  W25Q64_BlockCount  = (uint32_t)128,
  W25Q128_BlockCount = (uint32_t)256,
  W25Q256_BlockCount = (uint32_t)512,
  W25Q512_BlockCount = (uint32_t)1024

} W25QXXX_BlockCount_t;

/**
  * @brief Indicate device JEDEC_ID
  */
typedef enum
{
  W25Q512_JEDEC_ID = (uint8_t)0x20,
  W25Q256_JEDEC_ID = (uint8_t)0x19,
  W25Q128_JEDEC_ID = (uint8_t)0x18,
  W25Q64_JEDEC_ID  = (uint8_t)0x17,
  W25Q32_JEDEC_ID  = (uint8_t)0x16,
  W25Q16_JEDEC_ID  = (uint8_t)0x15,
  W25Q80_JEDEC_ID  = (uint8_t)0x14,
  W25Q40_JEDEC_ID  = (uint8_t)0x13,
  W25Q20_JEDEC_ID  = (uint8_t)0x12,
  W25Q10_JEDEC_ID  = (uint8_t)0x11,

} W25Qxxx_JEDEC_ID_t;

/**
  * @brief Indicate device status register number
  */
typedef enum
{
  W25Q512_STATUS_REG1 = 1,
  W25Q512_STATUS_REG2 = 2,
  W25Q512_STATUS_REG3 = 3

} W25Q512_STATUS_REG_NUM_t;

/**
  * @brief W25Qxxx spi Command type define [uint8_t]
  *   Follow the dataSheet
  */
typedef enum
{

  CMD_DUMMY                       =  (uint8_t)(0x00),

  CMD_Write_Enable                =  (uint8_t)(0x06),
  CMD_Write_Enable_SR             =  (uint8_t)(0x50),
  CMD_Write_Disable               =  (uint8_t)(0x04),
  CMD_Release_Power_Down          =  (uint8_t)(0xAB),
  CMD_Device_ID                   =  (uint8_t)(0xAB),
  CMD_Manufacture_ID              =  (uint8_t)(0x90),
  CMD_JEDEC_ID                    =  (uint8_t)(0x9F),
  CMD_Unique_ID                   =  (uint8_t)(0x4B),

  CMD_Read_Data                   =  (uint8_t)(0x03),
  CMD_Fast_Read                   =  (uint8_t)(0x0B),

  CMD_Page_Program                =  (uint8_t)(0x02),

  CMD_Erase_Sector                =  (uint8_t)(0x20),  //  4KB
  CMD_Erase_Block_32K             =  (uint8_t)(0x52),  // 32KB
  CMD_Erase_Block_64K             =  (uint8_t)(0xD8),  // 64KB
  CMD_Erase_Chip                  =  (uint8_t)(0xC7),  // chip

  CMD_Reg_1_Read                  =  (uint8_t)(0x05),
  CMD_Reg_1_Write                 =  (uint8_t)(0x01),
  CMD_Reg_2_Read                  =  (uint8_t)(0x35),
  CMD_Reg_2_Write                 =  (uint8_t)(0x31),
  CMD_Reg_3_Read                  =  (uint8_t)(0x15),
  CMD_Reg_3_Write                 =  (uint8_t)(0x11),

  CMD_Read_SFDP_Reg               =  (uint8_t)(0x5A),
  CMD_Erase_Security_Reg          =  (uint8_t)(0x44),
  CMD_Program_Security_Reg        =  (uint8_t)(0x42),
  CMD_Read_Security_Reg           =  (uint8_t)(0x48),

  CMD_Global_Block_Lock           =  (uint8_t)(0x7E),
  CMD_Global_Block_Unlock         =  (uint8_t)(0x98),
  CMD_Read_Block_Lock             =  (uint8_t)(0x3D),
  CMD_Individual_Block_Lock       =  (uint8_t)(0x36),
  CMD_Individual_Block_Unlock     =  (uint8_t)(0x39),

  CMD_Erase_Program_Suspend       =  (uint8_t)(0x75),
  CMD_Erase_Program_Resume        =  (uint8_t)(0x7A),
  CMD_Power_Down                  =  (uint8_t)(0xB9),

  CMD_Enable_Reset                =  (uint8_t)(0x66),
  CMD_Reset_Device                =  (uint8_t)(0x99),

  /* 4 Byte Address Mode Instructions <high capacity> */
  CMD_Read_Data_4_Byte_Addr       =  (uint8_t)(0x13),
  CMD_Fast_Read_4_Byte_Addr       =  (uint8_t)(0x0C),

  CMD_Page_Program_4_Byte_Addr    =  (uint8_t)(0x12),

  CMD_Erase_Sector_4_Byte_Addr    =  (uint8_t)(0x21),  // 4KB
  CMD_Erase_Block_64K_4_Byte_Addr =  (uint8_t)(0xDC),  // 64KB

  CMD_Read_Ex_Addr_Reg            =  (uint8_t)(0xC8),
  CMD_Write_Ex_Addr_Reg           =  (uint8_t)(0xC5),

  CMD_Enter_4_Byte_Addr_Mode      =  (uint8_t)(0xB7),
  CMD_Exit_4_Byte_Addr_Mode       =  (uint8_t)(0xE9),

} W25QxxxCMD_TypeDef;


/**
  * @brief W25Qxxx Status Register bit mask
  */
typedef enum
{

  SR1_S0_BUSY =  (uint8_t)(1<<0),  // Erase/Write In Progress (BUSY) – Status Only
  SR1_S1_WEL  =  (uint8_t)(1<<1),  // Write Enable Latch (WEL) – Status Only

  SR2_S9_QE   =  (uint8_t)(1<<1),  // Quad Enable (QE) – Volatile/Non-Volatile Writable
  SR2_S15_SUS =  (uint8_t)(1<<7),  // Erase/Program Suspend Status (SUS) – Status Only

  SR3_S18_WPS =  (uint8_t)(1<<3),  // Write Protect Selection (WPS) – Volatile/Non-Volatile Writable

  /* 4 Byte Address Mode Instructions <high capacity> */
  SR3_S16_ADS =  (uint8_t)(1<<0),  // Current Address Mode (ADS) – Status Only
  SR3_S17_ADP =  (uint8_t)(1<<1),  // Power-Up Address Mode (ADP) – Non-Volatile Writable

  /* high capacity flash  Extended Address Register */
  SR_ADDR_EX_A24_EA0    =  (uint8_t)(1<<0),
  SR_ADDR_EX_A25_EA1    =  (uint8_t)(1<<1),

}W25QxxxSRBitMask_TypeDef;

#define INIT_CHECK                                   \
                  do                                 \
                  {                                  \
                      if(!Is_W25Qxxx_Init)           \
                          return w24qxxx_init_error; \
                  } while( 0 )

extern bool Is_W25Qxxx_Init;
extern w24qxxx_init_t INIT;

extern uint8_t   W25Qxxx_Device_ID;
//extern uint16_t  W25Qxxx_Manufacturer_Device_ID;
//extern uint32_t  W25Qxxx_JEDEC_ID;
//extern uint8_t   W25Qxxx_UniqID[8];
//extern uint32_t  W25Qxxx_BlockCount;
extern uint16_t  W25Qxxx_PageSize;
extern uint32_t  W25Qxxx_SectorSize;
//extern uint32_t  W25Qxxx_SectorCount;
//extern uint32_t  W25Qxxx_PageCount;
extern uint32_t  W25Qxxx_BlockSize;
//extern uint32_t  W25Qxxx_CapacityInKiloByte;

// common functions prototypes for all
w24qxxx_statusTypeDef W25Qxxx_SPI_RxTx(uint8_t data, uint8_t *pret);
w24qxxx_statusTypeDef W25Qxxx_SPI_Tx(uint8_t data);
w24qxxx_statusTypeDef W25Qxxx_SPI_Rx(uint8_t data, uint8_t *pret);
w24qxxx_statusTypeDef W25Qxxx_CsEnable(void);
w24qxxx_statusTypeDef W25Qxxx_CsDisable(void);
int8_t W25Qxxx_WaitForWriteEnd(void);
void W25Qxxx_Write_Enable(void);
void W25Qxxx_Write_Enable_SR(void);
uint32_t W25Qxxx_BlockToPage(uint32_t BlockAddress);
uint32_t W25Qxxx_SectorToPage(uint32_t SectorAddress);

// errase functions prototypes
uint8_t W25Qxxx_EraseChip(void);
uint8_t W25Qxxx_EraseSector(uint32_t SectorAddr);
uint8_t W25Qxxx_EraseBlock(uint32_t BlockAddr);

// read functions prototypes
uint8_t W25Qxxx_ReadByte(uint8_t *pBuffer, uint32_t Bytes_Address);
uint8_t W25Qxxx_ReadBytes(uint8_t *pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead);
uint8_t W25Qxxx_ReadPage(uint8_t *pBuffer, uint32_t Page_Address,
                  uint32_t OffsetInByte, uint32_t NumByteToRead_up_to_PageSize);
uint8_t W25Qxxx_ReadSector(uint8_t *pBuffer, uint32_t Sector_Address, 
                uint32_t OffsetInByte, uint32_t NumByteToRead_up_to_SectorSize);
uint8_t W25Qxxx_ReadBlock(uint8_t *pBuffer, uint32_t Block_Address, 
                  uint32_t OffsetInByte, uint32_t NumByteToRead_up_to_BlockSize);

// write functions prototypes
uint8_t W25Qxxx_WriteByte(const uint8_t byte, uint32_t WriteAddr_inBytes);
uint8_t W25Qxxx_WritePage(const uint8_t *pBuffer, uint32_t Page_Address,
                uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_PageSize);
uint8_t W25Qxxx_WriteSector(const uint8_t *pBuffer, uint32_t Sector_Address,
                uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_SectorSize);
uint8_t W25Qxxx_WriteBlock(const uint8_t *pBuffer, uint32_t Block_Address, 
                uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_BlockSize);

#ifdef __cplusplus
    }
#endif

#endif /* _W25QXXX_DEFS_H_ */
