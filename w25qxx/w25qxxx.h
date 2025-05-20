#ifndef _W25QXXX_H_
#define _W25QXXX_H_

// Организация памяти:
// Блоки: по 64кБ(0x10000 - 65536) 32шт(для w25q16). Состоят из секторов.
// Сектор: по 4кБ(0x1000 - 4096) 16шт в блоке
// Страница: по 256Б(0x100) 16шт в секторе



#ifdef __cplusplus
    extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>


typedef enum
{
  w24qxxx_OK         = 0x00U,
  w24qxxx_ERROR      = 0x01U,
  w24qxxx_BUSY       = 0x02U,
  w24qxxx_TIMEOUT    = 0x03U,
  w24qxxx_init_error = 0x04U
} w24qxxx_statusTypeDef;

typedef struct
{
  // spi receive transmit function
  w24qxxx_statusTypeDef (*SPI_TransmitReceive)(const uint8_t *pTxData,
                                                     uint8_t *pRxData,
                                                     uint16_t Size);
  // chip select
  w24qxxx_statusTypeDef (*CS_EnableDisable)(bool enable);
  // write protect
  w24qxxx_statusTypeDef (*WP_EnableDisable)(bool enable);

} w24qxxx_init_t;


w24qxxx_statusTypeDef W25Qxxx_Init(w24qxxx_init_t * init);

w24qxxx_statusTypeDef W25Qxxx_DeviceInit(void);
void Test(void);

int8_t W25Qxxx_GetCapacity(uint32_t *block_num, uint16_t *block_size);
int8_t W25Qxxx_Read_FS(uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
int8_t W25Qxxx_Write_FS(uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);

#ifdef __cplusplus
    }
#endif

#endif /* _W25QXXX_H_ */
