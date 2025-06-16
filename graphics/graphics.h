#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <stdint.h>
#include "fatfs.h"

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

typedef struct
{
  void (*setCursorCB)(uint16_t x, uint16_t y);
  void (*drawPixelCB)(uint16_t x, uint16_t y, uint16_t color);
} graphics_init_t;

int GraphicsInit(graphics_init_t * init);

int GraphicsDrawBMP(uint8_t *data, uint16_t x, uint16_t y);

#endif /* _GRAPHICS_H_ */
