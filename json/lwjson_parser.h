/*
 * lwjson_parser.h
 *
 *  Created on: 3 июн. 2025 г.
 *      Author: Админи
 */

#ifndef LWJSON_PARSER_H_
#define LWJSON_PARSER_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


// ----------------------------------------------------------------------------
#define HMI_SETTINGS    "HmiSettings"
// Ключи параметров HMI панели
#define MODBUD_ADDR     "ModbudAddr"
#define START_FRAME_ID  "StartFrameId"
#define CURR_FRAME_ID   "CurrFrameId"
#define FRAME_SEQUENCE  "FrameSequence"
#define SEQUENCE_PERIOD "SequencePeriod"

#define HMI_FRAMES      "HmiFrames"
// Ключи параметров окна
#define MB_REG_NUM      "MbRegNum"
#define MB_REG_TYPE     "MbRegType"
#define PRIMITIVE_TYPE  "PrimitiveType"
#define BACK_GROUND     "BackGround"
#define SIZE            "Size"
#define LAYER_NUM       "LayerNum"
#define COORDINATES_XY  "CoordinatesXY"
#define IMAGE_FILE      "ImageFile"

#define FRAME_PRIMITIVES "FramePrimitives"
// ----------------------------------------------------------------------------
typedef enum
{
  DI      = 0,
  DO      = 1,
  INPUT   = 2,
  HOLDING = 3

} mb_reg_type_t; // тип Modbus регистра
// ----------------------------------------------------------------------------
typedef enum
{
  IMAGE   = 0,
  COLOR   = 1

} background_type_t; // тип подложки элемента
// ----------------------------------------------------------------------------
typedef enum
{
  window = 0,
  lamp,
  button

} element_type_t; // тип элемента element_t
// ----------------------------------------------------------------------------
typedef struct
{
  uint16_t x;
  uint16_t y;

} x_y_pair_t; // размер или координаты
// ----------------------------------------------------------------------------
typedef struct
{
  uint16_t size;
  int * data;

} int_array_t;
// ----------------------------------------------------------------------------
typedef struct
{
  uint16_t          MbRegNum;           // номер Modbus регистра
  uint16_t          MbRegType;          // тип Modbus регистра
  element_type_t    PrimitiveType;        // тип элемента (окно)
  background_type_t BackGround;         // фона заливка цветом или картинка
  x_y_pair_t        Size;               // размер окна
  uint16_t          LayerNum;           // номер слоя окна (всегда 0)
  x_y_pair_t        CoordinatesXY;      // координаты окна (всегда 0,0)
  char              *ImageFileName;         // имя файла картинки
  void              *NextPrimitive;     // (primitive_t*) указатель на следущий элемент
  void              *OwnPrimitivesList; // (primitive_t*) список принадлежащих элементов (окну)

} primitive_t;
// ----------------------------------------------------------------------------
typedef struct
{
  uint16_t    ModbudAddr;      // Modbus адрес устройства
  uint16_t    StartWindId;     // номер окна при старте программы
  uint16_t    CurrWindId;      // номер текущего отображаемого окна
  uint16_t    SequencePeriod;  // период смены экранов
  int_array_t FrameSequence;   // послдовательность смены экранов
  primitive_t *FrameList;      // указатель на массив с окнами

} hmi_settings_t;
// ----------------------------------------------------------------------------
void SettingsParse(hmi_settings_t *hmi_settings, const char * json);
// ----------------------------------------------------------------------------
int SettingsGetFrameCount(hmi_settings_t *hmi_settings);
// ----------------------------------------------------------------------------
const char * SettingsGetFrameFileName(hmi_settings_t *hmi_settings, 
                                                            uint16_t frame_num);
// ----------------------------------------------------------------------------
int SettingsGetFrameX(hmi_settings_t * hmi_settings, uint16_t frame_num);
// ----------------------------------------------------------------------------
int SettingsGetFrameY(hmi_settings_t * hmi_settings, uint16_t frame_num);
// ----------------------------------------------------------------------------
int SettingsGetFramePrimitivesCount(hmi_settings_t *hmi_settings, 
                                                        uint16_t frame_num);
// ----------------------------------------------------------------------------
const char * SettingsGetFramePrimitiveFileName(hmi_settings_t * hmi_settings,
                                                        uint16_t frame_num,
                                                        uint16_t primitive_num);
// ----------------------------------------------------------------------------
int SettingsGetPrinitiveX(hmi_settings_t * hmi_settings, uint16_t frame_num,
                                                        uint16_t primitive_num);
// ----------------------------------------------------------------------------
int SettingsGetPrinitiveY(hmi_settings_t * hmi_settings, uint16_t frame_num,
                                                        uint16_t primitive_num);
// ----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LWJSON_PARSER_H_ */
