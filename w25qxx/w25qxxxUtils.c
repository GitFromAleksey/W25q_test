#include <stdio.h>
#include <string.h>
#include "w25qxxx_defs.h"
#include "w25qxxx.h"

/** ###########################################################################
  * @brief spi transmit and receive
  * @retval return received data [Byte]
  */
w24qxxx_statusTypeDef W25Qxxx_SPI_RxTx(uint8_t data, uint8_t *pret)
{
  INIT_CHECK;

  uint8_t ret;

  INIT.SPI_TransmitReceive(&data, &ret, 1);

  if(pret != NULL)
    *pret = ret;

  return w24qxxx_OK;
}
// ----------------------------------------------------------------------------
w24qxxx_statusTypeDef W25Qxxx_SPI_Tx(uint8_t data)
{
  return W25Qxxx_SPI_RxTx(data, NULL);
}
// ----------------------------------------------------------------------------
w24qxxx_statusTypeDef W25Qxxx_SPI_Rx(uint8_t data, uint8_t *pret)
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
w24qxxx_statusTypeDef W25Qxxx_CsEnable(void)
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
w24qxxx_statusTypeDef W25Qxxx_CsDisable(void)
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
int8_t W25Qxxx_WaitForWriteEnd(void)
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
/** ###########################################################################
  * @brief The Write Enable instruction (Figure 5) sets the Write Enable Latch (WEL) bit in the Status Register to a 1.
  The WEL bit must be set prior to every Page Program, Quad Page Program, Sector Erase, Block Erase,
  Chip Erase, Write Status Register and Erase/Program Security Registers instruction. The Write Enable
  instruction is entered by driving /CS low, shifting the instruction code “06h” into the Data Input (DI) pin on
  the rising edge of CLK, and then driving /CS high.
  * @param  none
  */
void W25Qxxx_Write_Enable(void)
{
  uint8_t ret;

  W25Qxxx_CsEnable();

  W25Qxxx_SPI_RxTx(CMD_Write_Enable, &ret);

  W25Qxxx_CsDisable();
}
// ----------------------------------------------------------------------------
/** ###########################################################################
  * @brief Write Enable for Volatile Status Register (50h)
  * @param  none
  */
void W25Qxxx_Write_Enable_SR(void)
{
  uint8_t ret;

  W25Qxxx_CsEnable();

  W25Qxxx_SPI_RxTx(CMD_Write_Enable_SR, &ret);

  W25Qxxx_CsDisable();
}
// ----------------------------------------------------------------------------
/** ###########################################################################
  * @brief  blockAddr convert to pageAddr
  * @retval return pageAddr
  */
uint32_t W25Qxxx_BlockToPage(uint32_t BlockAddress)
{
  return (BlockAddress * W25Qxxx_BlockSize) / W25Qxxx_PageSize;
}
// ----------------------------------------------------------------------------
/** ###########################################################################
  * @brief  sector convert to pageAddr
  * @retval return pageAddr
  */
uint32_t W25Qxxx_SectorToPage(uint32_t SectorAddress)
{ return (SectorAddress * W25Qxxx_SectorSize) / W25Qxxx_PageSize; }
// ----------------------------------------------------------------------------
