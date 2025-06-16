#include <stdio.h>
#include <string.h>
#include "graphics.h"

static graphics_init_t Init;
// ----------------------------------------------------------------------------
int GraphicsInit(graphics_init_t * init)
{
  memcpy(&Init, init, sizeof(graphics_init_t));

  return 0;
}
// ----------------------------------------------------------------------------
int GraphicsDrawBMP(uint8_t *data, uint16_t x, uint16_t y)
{
  BITMAP_FILE_HEADER_t *bmp_hdr;

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

  uint16_t pixel = 0;
  int padding = (4 - (bmp_hdr->info.biWidth*3 % 4)) % 4;
  Init.setCursorCB(0, 0);
  for(int y = 0; y < bmp_hdr->info.biHeight; ++y)
  {
    for(int x = 0; x < bmp_hdr->info.biWidth; ++x)
    {
//      f_read(&fil, data, 3, &read_bytes);

      pixel  = 0;
      pixel |= data[0]>>3;        // COLOR_BLUE (uint16_t)(0x001F)  // 0000 0000 0001 1111
      pixel |= (data[1]>>2)<<5;   // COLOR_GREEN (uint16_t)(0x07E0) // 0000 0111 1110 0000
      pixel |= (data[2]>>3)<<11;  // COLOR_RED (uint16_t)(0xF800)   // 1111 1000 0000 0000

      Init.drawPixelCB(y, x, pixel);
    }
    // пропуск выравивающих (до 4-х) байт
//    f_read(&fil, data, padding, &read_bytes);
  }

//  f_close(&fil);
  return 0;
}
// ----------------------------------------------------------------------------
