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
int GraphicsDrawBMP(const char *file_name, uint16_t x, uint16_t y)
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
// выделение памяти для чтения линий из файла
  uint8_t *data_line = malloc( (bmp_hdr->info.biWidth*3) + padding);
// будем использовать выделенную память и для хранения пикселей
  uint16_t *data_pixels = (uint16_t *)data_line;
  if(data_line == NULL)
    return -1;

  Init.setCursorCB(0, 0);
  for(int y = 0; y < bmp_hdr->info.biHeight; ++y)
  {
    // чтение линии картинки из файла
    f_read(&fil, data_line, (bmp_hdr->info.biWidth*3) + padding, &read_bytes);
    // перекодировка из RGB888 в RGB565
    for(int x = 0; x < read_bytes-padding; ++x)
    {
      // переводим по 3 байта в uint16_t
      tmp = (x*BMP_BYTES_PER_PIXEL);
      pixel = RGB_24_TO_565(data_line[tmp+2], data_line[tmp+1], data_line[tmp]);
      data_pixels[x] = pixel;
    }
    // пишем сразу всю строку
    Init.drawPixelsCB(0, y, data_pixels, (read_bytes-padding)/3);
  }

  free(data_line);
  f_close(&fil);
}
// ----------------------------------------------------------------------------
