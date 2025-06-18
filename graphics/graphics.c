#include <stdio.h>
#include <string.h>
#include "graphics.h"
#include "fatfs.h"

#define BMP_BYTES_PER_PIXEL  3u
#define RGB_24_TO_565( r, g, b) ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);

static graphics_init_t Init;
// ----------------------------------------------------------------------------
int GraphicsInit(graphics_init_t * init)
{
  memcpy(&Init, init, sizeof(graphics_init_t));

  return 0;
}
// ----------------------------------------------------------------------------
int GraphicsDrawBMP(const char *file_name, uint16_t x_pos, uint16_t y_pos)
{
  FRESULT res;
  FIL fil;        /* File object */
  BITMAP_FILE_HEADER_t *bmp_hdr;

  uint8_t data[sizeof(BITMAP_FILE_HEADER_t)];
  UINT read_bytes;
  int tmp;

  if(file_name == NULL)
  {
    printf("Enter file name please!!!\n");
    return -1;
  }

  printf("Read bmp header: %s\n", file_name);

  res = f_open(&fil, file_name, FA_READ);
  if(res != FR_OK)
  {
    printf("File open error: %s\n", file_name);
    return -1;
  }

  res = f_read(&fil, data, sizeof(data), &read_bytes);
  if(res != FR_OK)
  {
    printf("Read from file error: %s\n", file_name);
    return -1;
  }
  
  bmp_hdr = (BITMAP_FILE_HEADER_t *)data;

  printf("bfType: 0x%X\r\nbfSize: %lu\r\nbfReserved1: 0x%X\r\n\
bfReserved2: 0x%X\r\nbfOffBits: %lu\r\n",
          bmp_hdr->bfType,
          bmp_hdr->bfSize,
          bmp_hdr->bfReserved1,
          bmp_hdr->bfReserved2,
          bmp_hdr->bfOffBits);

  printf("\r\nbiSize: %lu\r\nbiWidth: %lu\r\nbiHeight: %lu\r\nbiPlanes: %u\r\n\
biBitCount: %u\r\nbiCompression: %lu\r\nbiSizeImage: %lu\r\nbiXPelsPerMeter: \
%lu\r\nbiYPelsPerMeter: %lu\r\nbiClrUsed: %lu\r\nbiClrImportant: %lu\r\n",
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

  printf("\r\n");

  uint16_t pixel = 0;
  int padding = (4 - (bmp_hdr->info.biWidth*3 % 4)) % 4;

  Init.setCursorCB(x_pos, y_pos);
  for(int y = 0; y < bmp_hdr->info.biHeight; ++y)
  {

    // перекодировка из RGB888 в RGB565
    for(int x = 0; x < bmp_hdr->info.biWidth; ++x)
    {
      // чтение линии картинки из файла
      f_read(&fil, data, 3, &read_bytes);
      // переводим по 3 байта в uint16_t
      tmp = (x*BMP_BYTES_PER_PIXEL);
      pixel = RGB_24_TO_565(data[2], data[1], data[0]);
      Init.drawPixelCB(x+x_pos, y+y_pos, pixel);
    }
        // пропуск выравивающих (до 4-х) байт
    f_read(&fil, data, padding, &read_bytes);
    // пишем сразу всю строку
//    Init.drawPixelsCB(0+x_pos, y+y_pos, data_pixels, (read_bytes-padding)/3);
  }

//  free(data_line);
  f_close(&fil);
}
// ----------------------------------------------------------------------------
