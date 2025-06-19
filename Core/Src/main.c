/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "../../w25qxx/w25qxxx.h"
#include "../../console/console.h"
#include "../../ILI9341/ili9341.h"
#include "../../json/lwjson_parser.h"
#include "../../graphics/graphics.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define LDC_HEIGHT        ILI9341_PIXEL_HEIGHT      // 320
#define LDC_WIDTH         ILI9341_PIXEL_WIDTH       // 240
#define LCD_PIXEL_COUNT   (LDC_HEIGHT * LDC_WIDTH)  // 320*240=76800
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart1;

SRAM_HandleTypeDef hsram1;

/* USER CODE BEGIN PV */
FATFS fs;
hmi_settings_t HmiSettings;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_FSMC_Init(void);
/* USER CODE BEGIN PFP */
// ----------------------------------------------------------------------------
int stdout_putchar(int ch);
// ----------------------------------------------------------------------------
int stdin_getchar(void);
// ----------------------------------------------------------------------------
w24qxxx_statusTypeDef SPI_TxRxCB(const uint8_t *pTxData,
                                       uint8_t *pRxData,
                                       uint16_t Size);

w24qxxx_statusTypeDef CS_EnableDisableCB(bool enable);

w24qxxx_statusTypeDef WP_EnableDisable(bool enable);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void CmdMount(const void *param);
// ----------------------------------------------------------------------------
void CmdReadDir(const void *param);
// ----------------------------------------------------------------------------
void CmdReadTextFile(const void *param);
// ----------------------------------------------------------------------------
void CmdReadBinFile(const void *param);
// ----------------------------------------------------------------------------
void CmdReadBmpFile(const void *param);
// ----------------------------------------------------------------------------
void CmdSettingsFile(const void *param);
// ----------------------------------------------------------------------------
void CmdWriteRegister(const void *param);
// ----------------------------------------------------------------------------
void SystemSetup(void);
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
//  MX_USB_DEVICE_Init();
  MX_FATFS_Init();
  MX_FSMC_Init();
  /* USER CODE BEGIN 2 */

  SystemSetup();

  HAL_Delay(500);
  lcdTest();
  CmdMount(NULL);
  CmdSettingsFile(NULL);

  if( HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_RESET )
  {
    HmiSettings.HmiMode = HMI_MODE_RUN;
  }
  else
  {
    HmiSettings.HmiMode = HMI_MODE_USB;
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  int frame_num = 0;
  int primitive_num = 0;
  int frame_count = SettingsGetFrameCount(&HmiSettings);
  uint32_t ticks = HAL_GetTick();

  while (1)
  {
    HAL_Delay(10);
    ConsoleRun();

    if(HmiSettings.HmiMode == HMI_MODE_USB)
      continue;

    if( (HAL_GetTick() - ticks) > 3000 )
    {
      ticks = HAL_GetTick();
      GraphicsDrawBMP(SettingsGetFrameFileName(&HmiSettings, frame_num), 
                                    SettingsGetFrameX(&HmiSettings, frame_num),
                                    SettingsGetFrameY(&HmiSettings, frame_num));


      HAL_Delay(10);
      primitive_num = SettingsGetFramePrimitivesCount(&HmiSettings, frame_num);
//      printf("primitive_num: %u\n", primitive_num);

      while(--primitive_num >= 0)
      {
//        printf("primitive_num: %d\n", primitive_num);

        const char * p_f = SettingsGetFramePrimitiveFileName(&HmiSettings, frame_num, primitive_num);

        GraphicsDrawBMP(SettingsGetFramePrimitiveFileName(&HmiSettings, frame_num, primitive_num),
                        SettingsGetPrinitiveX(&HmiSettings, frame_num, primitive_num),
                        SettingsGetPrinitiveY(&HmiSettings, frame_num, primitive_num));
//        printf("Prim file: %.s\n", 10, p_f);
//        printf("Prim file: %s\n", p_f);
        HAL_Delay(10);
      }

      if(++frame_num >= frame_count)
        frame_num = 0;
    }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED2_Pin|LED3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(F_CS_GPIO_Port, F_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : KEY1_Pin KEY0_Pin */
  GPIO_InitStruct.Pin = KEY1_Pin|KEY0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : LED2_Pin LED3_Pin */
  GPIO_InitStruct.Pin = LED2_Pin|LED3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : F_CS_Pin */
  GPIO_InitStruct.Pin = F_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(F_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LCD_BL_Pin */
  GPIO_InitStruct.Pin = LCD_BL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LCD_BL_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* FSMC initialization function */
static void MX_FSMC_Init(void)
{

  /* USER CODE BEGIN FSMC_Init 0 */

  /* USER CODE END FSMC_Init 0 */

  FSMC_NORSRAM_TimingTypeDef Timing = {0};

  /* USER CODE BEGIN FSMC_Init 1 */

  /* USER CODE END FSMC_Init 1 */

  /** Perform the SRAM1 memory initialization sequence
  */
  hsram1.Instance = FSMC_NORSRAM_DEVICE;
  hsram1.Extended = FSMC_NORSRAM_EXTENDED_DEVICE;
  /* hsram1.Init */
  hsram1.Init.NSBank = FSMC_NORSRAM_BANK1;
  hsram1.Init.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE;
  hsram1.Init.MemoryType = FSMC_MEMORY_TYPE_SRAM;
  hsram1.Init.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16;
  hsram1.Init.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE;
  hsram1.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW;
  hsram1.Init.WrapMode = FSMC_WRAP_MODE_DISABLE;
  hsram1.Init.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;
  hsram1.Init.WriteOperation = FSMC_WRITE_OPERATION_ENABLE;
  hsram1.Init.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;
  hsram1.Init.ExtendedMode = FSMC_EXTENDED_MODE_DISABLE;
  hsram1.Init.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE;
  hsram1.Init.WriteBurst = FSMC_WRITE_BURST_DISABLE;
  hsram1.Init.PageSize = FSMC_PAGE_SIZE_NONE;
  /* Timing */
  Timing.AddressSetupTime = 1;
  Timing.AddressHoldTime = 15;
  Timing.DataSetupTime = 5;
  Timing.BusTurnAroundDuration = 0;
  Timing.CLKDivision = 16;
  Timing.DataLatency = 17;
  Timing.AccessMode = FSMC_ACCESS_MODE_A;
  /* ExtTiming */

  if (HAL_SRAM_Init(&hsram1, &Timing, NULL) != HAL_OK)
  {
    Error_Handler( );
  }

  /* USER CODE BEGIN FSMC_Init 2 */

  /* USER CODE END FSMC_Init 2 */
}

/* USER CODE BEGIN 4 */
// ----------------------------------------------------------------------------
w24qxxx_statusTypeDef SPI_TxRxCB(const uint8_t *pTxData,
                                       uint8_t *pRxData,
                                       uint16_t Size)
{
  HAL_SPI_TransmitReceive(&hspi1, pTxData, pRxData, 1, 100);
  return w24qxxx_OK;
}
// ----------------------------------------------------------------------------
w24qxxx_statusTypeDef CS_EnableDisableCB(bool enable)
{
  HAL_GPIO_WritePin(F_CS_GPIO_Port, F_CS_Pin, (enable)?(GPIO_PIN_RESET):(GPIO_PIN_SET));
  return w24qxxx_OK;
}
// ----------------------------------------------------------------------------
w24qxxx_statusTypeDef WP_EnableDisable(bool enable)
{

  return w24qxxx_OK;
}
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
int stdout_putchar(int ch)
{
  uint8_t tmp = ch;
  if(HAL_UART_Transmit(&huart1, &tmp, 1, 100) == HAL_OK)
    return ch;
  else
    return -1;
}
// ----------------------------------------------------------------------------
int stdin_getchar(void)
{
  uint8_t ch;

  if(HAL_UART_Receive(&huart1, &ch, 1, 100) == HAL_OK)
  {
//    if(ch == 0x72)
//      return -1;
//    else
      return (int)ch;
  }
  else
    return '\n';
}
// ----------------------------------------------------------------------------
void CmdMount(const void *param)
{
  FRESULT fres;

  fres = f_mount(&fs, "0:", 1);

  if (fres == FR_OK)
    printf("Mount res: %d = FR_OK", fres);
  else
    printf("Mount error res: %d", fres);
}
// ----------------------------------------------------------------------------
void CmdReadDir(const void *param)
{
    FRESULT res;
    DIR dir;
    FILINFO fno;
    int nfile, ndir;
    const TCHAR* path = "0:";

    res = f_opendir(&dir, path);                       /* Open the directory */
    if (res == FR_OK)
    {
        nfile = ndir = 0;
        for (;;)
        {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0)
              break;  /* Error or end of dir */
            if (fno.fattrib & AM_DIR)
            {            /* Directory */
                printf("   <DIR>   %s\r\n", fno.fname);
                ndir++;
            }
            else
            {                               /* File */
                printf("name: %s;\tsize: %lu bytes\r\n", fno.fname, fno.fsize);
                nfile++;
            }
        }
        f_closedir(&dir);
        printf("%d dirs, %d files.\r\n", ndir, nfile);
    } else {
        printf("Failed to open \"%s\". (%u)\r\n", path, res);
    }
}
// ----------------------------------------------------------------------------
void CmdReadTextFile(const void *param)
{
  FRESULT res;
  const char * file_name;

  char * argv = (char *)param;
  FIL fil;        /* File object */
  char line[100]; /* Line buffer */

  file_name = ConsoleGetParam(param, 1);
  if(file_name == NULL)
  {
    printf("Enter file name please!!!\n");
    return;
  }

  printf("Read from text file: %s\n", file_name);

  res = f_open(&fil, file_name, FA_READ);

  while(f_gets(line, sizeof(line), &fil))
  {
    printf("%s", line);
  }

  f_close(&fil);
}
// ----------------------------------------------------------------------------
void CmdReadBinFile(const void *param)
{
  FRESULT res;
  const char * file_name;
  FIL fil;        /* File object */
  char data[100]; /* Line buffer */
  UINT read_bytes;

  file_name = ConsoleGetParam(param, 1);
  if(file_name == NULL)
  {
    printf("Enter file name please!!!\n");
    return;
  }

  printf("Read from text file: %s\n", file_name);

  res = f_open(&fil, file_name, FA_READ);

  while(f_read(&fil, data, sizeof(data), &read_bytes) == FR_OK)
  {
    if(read_bytes == 0)
      break;
    for(int i = 0; i < read_bytes; ++i)
      printf("%X,", data[i]);
  }

  printf("\r\n");

  f_close(&fil);
}
// ----------------------------------------------------------------------------
void CmdReadBmpFile(const void *param)
{
  const char * file_name = ConsoleGetParam(param, 1);
  GraphicsDrawBMP(file_name, 0, 0);
}
// ----------------------------------------------------------------------------
void CmdSettingsFile(const void *param)
{
  FRESULT res;
  FIL fil;        /* File object */
  FSIZE_t file_size = 0;
  UINT read_bytes;

  const char * file_name = "settings.json";

  printf("Read settings file: %s\n", file_name);

  res = f_open(&fil, file_name, FA_READ);
  if(res != FR_OK)
  {
    printf("Error open settings file: %s\n", file_name);
    return;
  }

  file_size = f_size(&fil);
  printf("File size: %lu\n", file_size );

  char *json_data = malloc(file_size);
  if(json_data == NULL)
  {
    printf("malloc error\n");
    return;
  }

  res = f_read(&fil, json_data, file_size, &read_bytes);
  if(res == FR_OK)
    printf("Read OK! Read bytes: %u\n", read_bytes);
  else
  {
    printf("Read ERROR!%u\n", res);
    free(json_data);
    f_close(&fil);
    return;
  }

  SettingsParse(&HmiSettings, json_data);

//  for(int i = 0; i < read_bytes; ++i)
//  {
//    printf("%c", json_data[i]);
//  }

  free(json_data);
  f_close(&fil);
}
// ----------------------------------------------------------------------------
void CmdWriteRegister(const void *param)
{
  int frame_num;

  int reg;
  int val;

  const char * reg_str = ConsoleGetParam(param, 1);
  const char * val_str = ConsoleGetParam(param, 2);

  reg = atoi(reg_str);
  if(reg == 0)
    return;
  val = atoi(val_str);
  if(val == 0)
    return;

  printf("CmdWriteRegister. reg_str: %s, val_str: %s\n", reg_str, val_str);
  printf("CmdWriteRegister. reg: %d, val: %d\n", reg, val);

  frame_num = SettingsGetFrameNumByMbReg(&HmiSettings, reg);

  GraphicsDrawBMP(SettingsGetFrameFileName(&HmiSettings, frame_num), 
                              SettingsGetFrameX(&HmiSettings, frame_num),
                              SettingsGetFrameY(&HmiSettings, frame_num));
}
// ----------------------------------------------------------------------------
void StorageSetup(void)
{
  w24qxxx_init_t init;

  init.SPI_TransmitReceive = SPI_TxRxCB;
  init.CS_EnableDisable    = CS_EnableDisableCB;
  init.WP_EnableDisable    = WP_EnableDisable;
  W25Qxxx_Init(&init);

  W25Qxxx_DeviceInit();

  MX_USB_DEVICE_Init();
}
// ----------------------------------------------------------------------------
void ConsoleSetup(void)
{
  ConsoleCommandAdd("mnt",  CmdMount,        "Mount fs.");
  ConsoleCommandAdd("ls",   CmdReadDir,      "Read directory.");
  ConsoleCommandAdd("rft",  CmdReadTextFile, "Read text file.");
  ConsoleCommandAdd("rfb",  CmdReadBinFile,  "Read binary file.");
  ConsoleCommandAdd("rbm",  CmdReadBmpFile,  "Read bmp file.");
  ConsoleCommandAdd("stp",  CmdSettingsFile, "Read settings file.");
  ConsoleCommandAdd("mbw",  CmdWriteRegister,"Write modbus reg, value.");
}
// ----------------------------------------------------------------------------
void LcdSetup(void)
{
  lcdBacklightOff();
  lcdInit();
  lcdBacklightOn();
  lcdSetOrientation(LCD_ORIENTATION_PORTRAIT); // (LCD_ORIENTATION_LANDSCAPE);
}
// ----------------------------------------------------------------------------
void GraphicsSetup(void)
{
  graphics_init_t ginit;

  ginit.lcdFrameArray     = NULL; // LcdFrameArray;
  ginit.lcdFrameArraySize = 0; // LCD_PIXEL_COUNT;

  ginit.drawPixelCB   = lcdDrawPixel;
  ginit.drawPixelsCB  = lcdDrawPixels;
  ginit.setCursorCB   = lcdSetCursor;

  GraphicsInit(&ginit);
}
// ----------------------------------------------------------------------------
void SystemSetup(void)
{
  StorageSetup();
  ConsoleSetup();
  LcdSetup();
  GraphicsSetup();
}
// ----------------------------------------------------------------------------
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
