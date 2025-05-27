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
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart1;

SRAM_HandleTypeDef hsram1;

/* USER CODE BEGIN PV */

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

FRESULT fres;
FATFS fs;
FIL   fp;
DIR   dp;



void CmdMount(const void *param)
{
  fres = f_mount(&fs, "0:", 1);
  if (fres == FR_OK)
    printf("Mount res: %d = FR_OK", fres);
  else
    printf("Mount error res: %d", fres);
}

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
                printf("name: %s;\tsize: %u bytes\r\n", fno.fname, fno.fsize);
                nfile++;
            }
        }
        f_closedir(&dir);
        printf("%d dirs, %d files.\r\n", ndir, nfile);
    } else {
        printf("Failed to open \"%s\". (%u)\r\n", path, res);
    }
}


char * sep = " ";
char * file_name = "FatFsInterface.c";
void CmdReadTextFile(const void *param)
{
  FRESULT res;

  char * argv = (char *)param;
  FIL fil;        /* File object */
  char line[100]; /* Line buffer */

  char * istr;
  istr = strtok(argv, sep);
  if(istr != NULL)
  {

    istr = strtok(NULL, sep);
    if(istr == NULL)
    {
      printf("Enter file name please!!!\n");
      return;
    }
  }

  file_name = istr;

  printf("Read from text file: %s\n", file_name);

  res = f_open(&fil, file_name, FA_READ);

  while(f_gets(line, sizeof(line), &fil))
  {
    printf("%s", line);
  }

  f_close(&fil);
}

void CmdReadBinFile(const void *param)
{
  FRESULT res;

  char * argv = (char *)param;
  FIL fil;        /* File object */
  char data[100]; /* Line buffer */
  UINT read_bytes;

  char * istr;
  istr = strtok(argv, sep);
  if(istr != NULL)
  {

    istr = strtok(NULL, sep);
    if(istr == NULL)
    {
      printf("Enter file name please!!!\n");
      return;
    }
  }

  file_name = istr;

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

#pragma pack(push, 1)
typedef struct
{
  DWORD biSize;         // Размер структуры.
  LONG biWidth;         // Ширина изображения в пикселах
  LONG biHeight;        // высота изображения в пикселах
  WORD biPlanes;        // Количество плоскостей
  WORD biBitCount;      // Глубина цвета в битах на пиксель
  DWORD biCompression;  // Тип сжатия
  DWORD biSizeImage;    // Размер изображения в байтах
  LONG biXPelsPerMeter; // Горизонтальное разрешение
  LONG biYPelsPerMeter; // вертикальное разрешение
  DWORD biClrUsed;      // Количество используемых цветов кодовой таблицы
  DWORD biClrImportant; // Количество основных цветов

} BITMAP_INFO_HEADER_t; // структура информационного заголовка

typedef struct
{
  WORD bfType;                // Тип файла. Должен быть "BM".
  DWORD bfSize;               // Размер файла в байтах.
  WORD bfReserved1;           // Зарезервированные поля.
  WORD bfReserved2;           // Зарезервированные поля.
  DWORD bfOffBits;            // Смещение битового массива относительно начала файла
  BITMAP_INFO_HEADER_t info;  // структура информационного заголовка
//  uint8_t data;
} BITMAP_FILE_HEADER_t;       // структура заголовка файла 
#pragma pack(pop)



void CmdReadBmpFile(const void *param)
{
  FRESULT res;
  
  BITMAP_FILE_HEADER_t *bmp_hdr;

  char * argv = (char *)param;
  FIL fil;        /* File object */
  char data[sizeof(BITMAP_FILE_HEADER_t)]; /* Line buffer */
  uint8_t *p_bi_data;
  UINT read_bytes;

  char * istr;
  istr = strtok(argv, sep);
  if(istr != NULL)
  {

    istr = strtok(NULL, sep);
    if(istr == NULL)
    {
      printf("Enter file name please!!!\n");
      return;
    }
  }

  file_name = istr;

  printf("Read from text file: %s\n", file_name);

  res = f_open(&fil, file_name, FA_READ);

  if(f_read(&fil, data, sizeof(data), &read_bytes) == FR_OK)
  {
    bmp_hdr = (BITMAP_FILE_HEADER_t *)data;

    printf("bfType: 0x%X\r\nbfSize: %u\r\nbfReserved1: 0x%X\r\n\
bfReserved2: 0x%X\r\nbfOffBits: %u\r\n",
            bmp_hdr->bfType,
            bmp_hdr->bfSize,
            bmp_hdr->bfReserved1,
            bmp_hdr->bfReserved2,
            bmp_hdr->bfOffBits);

    printf("\r\nbiSize: %u\r\nbiWidth: %u\r\nbiHeight: %u\r\nbiPlanes: %u\r\n\
biBitCount: %u\r\nbiCompression: %u\r\nbiSizeImage: %u\r\n,biXPelsPerMeter: \
%u\r\nbiYPelsPerMeter: %u\r\nbiClrUsed: %u\r\nbiClrImportant: %u\r\n",
            bmp_hdr->info.biSize,
            bmp_hdr->info.biWidth,
            bmp_hdr->info.biHeight,
            bmp_hdr->info.biPlanes,
            bmp_hdr->info.biBitCount,
            bmp_hdr->info.biCompression,
            bmp_hdr->info.biSizeImage,
            bmp_hdr->info.biXPelsPerMeter,
            bmp_hdr->info.biYPelsPerMeter,
            bmp_hdr->info.biClrUsed,
            bmp_hdr->info.biClrImportant
            );

    printf("sizeof(BITMAP_FILE_HEADER_t): %u\r\n", sizeof(BITMAP_FILE_HEADER_t));

//    p_bi_data = &bmp_hdr->data;


//    lcdSetCursor(0, 0);
//    for(int i = bmp_hdr->bfOffBits; i < sizeof(data); ++i)
//    {
//      f_read(&fil, data, 3, &read_bytes);
//      lcdDrawPixel(0, i-bmp_hdr->bfOffBits, data[i]);
//      printf("%X(%X);", data[i], *(p_bi_data++));
//    }
  }

  printf("\r\n");

  uint16_t pixel = 0;
  lcdSetCursor(0, 0);
  for(int y = 0; y < bmp_hdr->info.biHeight; ++y)
  {
    for(int x = 0; x < bmp_hdr->info.biWidth; ++x)
    {
    //void lcdDrawPixel(uint16_t x, uint16_t y, uint16_t color)

      f_read(&fil, data, 3, &read_bytes);
//      pixel  = (data[0] & 0x1F);      // COLOR_BLUE (uint16_t)(0x001F) // 0000 0000 0001 1111
//      pixel |= (data[1] & 0x1F)<<11;  // COLOR_RED (uint16_t)(0xF800) // 1111 1000 0000 0000
//      pixel |= (data[2] & 0x3F)<<5;   // COLOR_GREEN (uint16_t)(0x07E0) // 0000 0111 1110 0000

      pixel  = 0;

      pixel |= data[0]>>3;      // COLOR_BLUE (uint16_t)(0x001F) // 0000 0000 0001 1111
      pixel |= (data[1]>>2)<<5;   // COLOR_GREEN (uint16_t)(0x07E0) // 0000 0111 1110 0000
      pixel |= (data[2]>>3)<<11;  // COLOR_RED (uint16_t)(0xF800) // 1111 1000 0000 0000

//      pixel |= (data[0] & 0xF8);      // COLOR_BLUE (uint16_t)(0x001F) // 0000 0000 0001 1111
//      pixel |= ((data[2] & 0xF8)>>3)<<11;  // COLOR_RED (uint16_t)(0xF800) // 1111 1000 0000 0000
//      pixel |= ((data[1] & 0xFC)>>2)<<5;   // COLOR_GREEN (uint16_t)(0x07E0) // 0000 0111 1110 0000
      lcdDrawPixel(y, x, pixel);
    }
  }

  f_close(&fil);
}
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
  w24qxxx_init_t init;
  
  init.SPI_TransmitReceive = SPI_TxRxCB;
  init.CS_EnableDisable    = CS_EnableDisableCB;
  init.WP_EnableDisable    = WP_EnableDisable;
  W25Qxxx_Init(&init);

  W25Qxxx_DeviceInit();

  ConsoleCommandAdd("mnt",  CmdMount,        "Mount fs.");
  ConsoleCommandAdd("ls",   CmdReadDir,      "Read directory.");
  ConsoleCommandAdd("rft",  CmdReadTextFile, "Read text file.");
  ConsoleCommandAdd("rfb",  CmdReadBinFile,  "Read binary file.");
  ConsoleCommandAdd("rbm",  CmdReadBmpFile,  "Read bmp file.");

  MX_USB_DEVICE_Init();

  lcdBacklightOn();
  lcdInit();
  lcdSetOrientation(LCD_ORIENTATION_LANDSCAPE);
  
//  lcdFillRGB(COLOR_BLACK);
//  HAL_Delay(500);
//  lcdFillRGB(COLOR_BLUE);
//  HAL_Delay(500);
//  lcdFillRGB(COLOR_RED);
//  HAL_Delay(500);
//  lcdFillRGB(COLOR_GREEN);
//  HAL_Delay(500);
//  lcdFillRGB(COLOR_CYAN);
//  HAL_Delay(500);
//  lcdFillRGB(COLOR_MAGENTA);
//  HAL_Delay(500);
//  lcdFillRGB(COLOR_YELLOW);
//  HAL_Delay(500);
//  lcdFillRGB(COLOR_WHITE);
  HAL_Delay(500);
  lcdTest();
  CmdMount(NULL);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    HAL_Delay(10);
    ConsoleRun();

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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED2_Pin|LED3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(F_CS_GPIO_Port, F_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_RESET);

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
