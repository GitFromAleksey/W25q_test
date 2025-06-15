#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "lwjson_parser.h"
#include "../lwjson/include/lwjson.h"

#ifdef __cplusplus
extern "C" {
#endif



// ----------------------------------------------------------------------------
const char * lwjson_type_s[] =
{
  "LWJSON_TYPE_STRING",
  "LWJSON_TYPE_NUM_INT",
  "LWJSON_TYPE_NUM_REAL",
  "LWJSON_TYPE_OBJECT",
  "LWJSON_TYPE_ARRAY",
  "LWJSON_TYPE_TRUE",
  "LWJSON_TYPE_FALSE",
  "LWJSON_TYPE_NULL"
};
// ----------------------------------------------------------------------------
const char * primitiveType[] = { "window", "lamp", "button" };
/* LwJSON instance and tokens */
static lwjson_token_t tokens[128];
static lwjson_t lwjson;

//const char * test_json = "{\"mykey\":\"myvalue\"}";
const char * test_json_str = "{\"mykey\":\"myvalue\", \"mykey2\":\"myvalue2\"}";
// ----------------------------------------------------------------------------
static bool TokenNameEqual(const lwjson_token_t* tkn, const char * name)
{
  const char * p;

  if( (tkn == NULL) || (name == NULL) )
    return false;

  p = strstr(tkn->token_name, name);

  if(p == NULL)
    return false;

  if( (p - tkn->token_name) > tkn->token_name_len )
    return false;

  return true;
}
// ----------------------------------------------------------------------------
int GetIntArray(int_array_t *array, const lwjson_token_t* sub_tkn)
{
  const lwjson_token_t * next_item = lwjson_get_first_child(sub_tkn);

  array->data = NULL;
  array->size = 0;

  if(sub_tkn->type != LWJSON_TYPE_ARRAY)
    return -1;

  // подсчёт ол-ва элементов массива
  while(next_item != NULL)
  {
    next_item = next_item->next;
    array->size++;
  }

  // выделение памяти под массив
  array->data = malloc(sizeof(int) * array->size);
  if(array->data == NULL)
    return -1;

  // заполнение массива
  next_item = lwjson_get_first_child(sub_tkn);
  for(int i = 0; i < array->size; ++i)
  {
    array->data[i] = lwjson_get_val_int(next_item);
    next_item = next_item->next;
    printf("array[%u] = %u\n", i, array->data[i]);
  }

  return 0;
}
// ----------------------------------------------------------------------------
int GetSizeXY(x_y_pair_t *size, const lwjson_token_t* sub_tkn)
{
  int_array_t array;

  if(GetIntArray(&array, sub_tkn) == -1)
    return -1;

  if(array.size != 2)
    return -1;

  size->x = array.data[0];
  size->y = array.data[1];

  free(array.data); // освобождение памяти массива, выделенной в GetIntArray(...)

  return 0;
}
// ----------------------------------------------------------------------------
int GetString(primitive_t *new_element, const lwjson_token_t* tkn)
{
  size_t file_name_size;
  const char * file_name = lwjson_get_val_string(tkn, &file_name_size);

  if(new_element == NULL)
    return -1;

  file_name_size = sizeof(char) * file_name_size;
  new_element->ImageFileName = malloc(file_name_size);
  memcpy(new_element->ImageFileName, file_name, file_name_size);
  new_element->ImageFileName[file_name_size] = '\0';

  return 0;
}
// ----------------------------------------------------------------------------
static primitive_t * CreateNewBlankPrimitive(void)
{
  primitive_t *new_blank_primitive = NULL;
  // создание нового окна
  new_blank_primitive = malloc(sizeof(primitive_t));
  if(new_blank_primitive == NULL)
    return NULL;
  // инициализация нового окна
  memset(new_blank_primitive, 0, sizeof(primitive_t));
  new_blank_primitive->NextPrimitive      = NULL;
  new_blank_primitive->OwnPrimitivesList  = NULL;
  new_blank_primitive->ImageFileName      = NULL;

  return new_blank_primitive;
}
// ----------------------------------------------------------------------------
void PrimitiveToListAppend(primitive_t ** primitive_list, primitive_t * primitive_child)
{

  if(*primitive_list == NULL)
  {
    *primitive_list = primitive_child;
  }
  else
  {
    // добавляем остальные окна
    primitive_t * next_primitive = *primitive_list;
    while(next_primitive->NextPrimitive != NULL)
    {
      next_primitive = next_primitive->NextPrimitive;
    }
    next_primitive->NextPrimitive = primitive_child;
  }
}
// ----------------------------------------------------------------------------
static int FindHmiSettings(hmi_settings_t *hmi_settings, const lwjson_token_t* tkn)
{
  if(!TokenNameEqual(tkn, HMI_SETTINGS))
//  if(strstr(tkn->token_name, HMI_SETTINGS) == NULL) // ищем заголовок настроек HMI панели
    return -1;

  printf("\nFind HMI_SETTINGS:\n");

  hmi_settings->FrameList = NULL;

  for(const lwjson_token_t* sub_tkn = lwjson_get_first_child(tkn); sub_tkn != NULL; sub_tkn = sub_tkn->next)
  {
    if(strstr(sub_tkn->token_name, MODBUD_ADDR) != NULL)
    {
      hmi_settings->ModbudAddr = lwjson_get_val_int(sub_tkn);
      printf("MODBUD_ADDR: %u\n", hmi_settings->ModbudAddr);
    }
    else if(strstr(sub_tkn->token_name, START_FRAME_ID) != NULL)
    {
      hmi_settings->StartWindId = lwjson_get_val_int(sub_tkn);
      printf("START_WIND_ID: %u\n", hmi_settings->StartWindId);
    }
    else if(strstr(sub_tkn->token_name, CURR_FRAME_ID) != NULL)
    {
      hmi_settings->CurrWindId = lwjson_get_val_int(sub_tkn);
      printf("CURR_FRAME_ID: %u\n", hmi_settings->CurrWindId);
    }
    else if(strstr(sub_tkn->token_name, FRAME_SEQUENCE) != NULL)
    {
      GetIntArray(&hmi_settings->FrameSequence, sub_tkn);
      printf("FRAME_SEQUENCE size: %d\n", hmi_settings->FrameSequence.size);
    }
    else if(strstr(sub_tkn->token_name, SEQUENCE_PERIOD) != NULL)
    {
      hmi_settings->SequencePeriod = lwjson_get_val_int(sub_tkn);
      printf("SEQUENCE_PERIOD: %u\n", hmi_settings->SequencePeriod);
    }
  }

  return 0;
}
// ----------------------------------------------------------------------------
static primitive_t * CreateNewPrimitive(const lwjson_token_t* prim_tkn)
{
  primitive_t *new_primitive = NULL;

  if(prim_tkn == NULL)
    return NULL;

  // создание нового примитива
  new_primitive = CreateNewBlankPrimitive();
  if(new_primitive == NULL)
    return NULL;

  printf("CreateNewPrimitive: %.*s\n", prim_tkn->token_name_len, prim_tkn->token_name);
  for(const lwjson_token_t* prim_stngs_tkn = lwjson_get_first_child(prim_tkn); prim_stngs_tkn != NULL; prim_stngs_tkn = prim_stngs_tkn->next)
  {
    if(TokenNameEqual(prim_stngs_tkn, MB_REG_NUM))
    {
      new_primitive->MbRegNum = lwjson_get_val_int(prim_stngs_tkn);
      printf("MB_REG_NUM: %u\n", new_primitive->MbRegNum);
    }
    else if(TokenNameEqual(prim_stngs_tkn, MB_REG_TYPE))
    {
      new_primitive->MbRegType = lwjson_get_val_int(prim_stngs_tkn);
      printf("MB_REG_TYPE: %u\n", new_primitive->MbRegType);
    }
    else if(TokenNameEqual(prim_stngs_tkn, PRIMITIVE_TYPE))
    {

      new_primitive->PrimitiveType = lwjson_get_val_int(prim_stngs_tkn);
      printf("PRIMITIVE_TYPE: %s\n", primitiveType[new_primitive->PrimitiveType]);
    }
    else if(TokenNameEqual(prim_stngs_tkn, BACK_GROUND))
    {
      new_primitive->BackGround= lwjson_get_val_int(prim_stngs_tkn);
      printf("BACK_GROUND: %u\n", new_primitive->BackGround);
    }
    else if(TokenNameEqual(prim_stngs_tkn, SIZE))
    {
      GetSizeXY(&new_primitive->Size, prim_stngs_tkn);
      printf("SIZE x: %u, y: %u\n", new_primitive->Size.x, new_primitive->Size.y);
    }
    else if(TokenNameEqual(prim_stngs_tkn, LAYER_NUM))
    {
      new_primitive->LayerNum = lwjson_get_val_int(prim_stngs_tkn);
      printf("LAYER_NUM: %u\n", new_primitive->LayerNum);
    }
    else if(TokenNameEqual(prim_stngs_tkn, COORDINATES_XY))
    {
      GetSizeXY(&new_primitive->CoordinatesXY, prim_stngs_tkn);
      printf("CoordinatesXY x: %u, y: %u\n", new_primitive->CoordinatesXY.x,
          new_primitive->CoordinatesXY.y);
    }
    else if(TokenNameEqual(prim_stngs_tkn, IMAGE_FILE))
    {
      size_t file_name_size;
      const char * file_name = lwjson_get_val_string(prim_stngs_tkn, &file_name_size);

      file_name_size = sizeof(char) * file_name_size;
      new_primitive->ImageFileName = malloc(file_name_size);
      memcpy(new_primitive->ImageFileName, file_name, file_name_size);
      new_primitive->ImageFileName[file_name_size] = '\0';
      printf("IMAGE_FILE: %s\n", new_primitive->ImageFileName);
    }
//    else if(TokenNameEqual(prim_stngs_tkn, FRAME_PRIMITIVES))
//    {
//      for(const lwjson_token_t* prim_tkn = lwjson_get_first_child(prim_stngs_tkn); prim_tkn != NULL; prim_tkn = prim_tkn->next)
//        CreateNewPrimitive(prim_tkn);
//    }
  }

  return new_primitive;
}
// ----------------------------------------------------------------------------
static primitive_t * CreateNewFrame(const lwjson_token_t* frame_tkn)
{
  primitive_t *new_frame = NULL;

  if(frame_tkn == NULL)
    return NULL;

  printf("\nFrame: %.*s\n", frame_tkn->token_name_len, frame_tkn->token_name);

  // создание примитива нового окна
  new_frame = CreateNewBlankPrimitive();
  if(new_frame == NULL)
    return NULL;

  // заполнение параметров окна
  for(const lwjson_token_t* win_stngs_tkn = lwjson_get_first_child(frame_tkn); win_stngs_tkn != NULL; win_stngs_tkn = win_stngs_tkn->next)
  {
    if(TokenNameEqual(win_stngs_tkn, MB_REG_NUM))
    {
      new_frame->MbRegNum = lwjson_get_val_int(win_stngs_tkn);
      printf("MB_REG_NUM: %u\n", new_frame->MbRegNum);
    }
    else if(TokenNameEqual(win_stngs_tkn, MB_REG_TYPE))
    {
      new_frame->MbRegType = lwjson_get_val_int(win_stngs_tkn);
      printf("MB_REG_TYPE: %u\n", new_frame->MbRegType);
    }
    else if(TokenNameEqual(win_stngs_tkn, PRIMITIVE_TYPE))
    {
      const char * elementType[] = { "window", "lamp", "button" };
      new_frame->PrimitiveType = lwjson_get_val_int(win_stngs_tkn);
      printf("PRIMITIVE_TYPE: %s\n", elementType[new_frame->PrimitiveType]);
    }
    else if(TokenNameEqual(win_stngs_tkn, BACK_GROUND))
    {
      new_frame->BackGround= lwjson_get_val_int(win_stngs_tkn);
      printf("BACK_GROUND: %u\n", new_frame->BackGround);
    }
    else if(TokenNameEqual(win_stngs_tkn, SIZE))
    {
      GetSizeXY(&new_frame->Size, win_stngs_tkn);
      printf("SIZE x: %u, y: %u\n", new_frame->Size.x, new_frame->Size.y);
    }
    else if(TokenNameEqual(win_stngs_tkn, LAYER_NUM))
    {
      new_frame->LayerNum = lwjson_get_val_int(win_stngs_tkn);
      printf("LAYER_NUM: %u\n", new_frame->LayerNum);
    }
    else if(TokenNameEqual(win_stngs_tkn, COORDINATES_XY))
    {
      GetSizeXY(&new_frame->CoordinatesXY, win_stngs_tkn);
      printf("CoordinatesXY x: %u, y: %u\n", new_frame->CoordinatesXY.x,
                                             new_frame->CoordinatesXY.y);
    }
    else if(TokenNameEqual(win_stngs_tkn, IMAGE_FILE))
    {
      size_t file_name_size;
      const char * file_name = lwjson_get_val_string(win_stngs_tkn, &file_name_size);

      file_name_size = sizeof(char) * file_name_size;
      new_frame->ImageFileName = malloc(file_name_size);
      memcpy(new_frame->ImageFileName, file_name, file_name_size);
      new_frame->ImageFileName[file_name_size] = '\0';
      printf("IMAGE_FILE: %s\n", new_frame->ImageFileName);
    }
    else if(TokenNameEqual(win_stngs_tkn, FRAME_PRIMITIVES))
    {
      for(const lwjson_token_t* prim_tkn = lwjson_get_first_child(win_stngs_tkn); prim_tkn != NULL; prim_tkn = prim_tkn->next)
      {
        primitive_t * primitive = CreateNewPrimitive(prim_tkn);
        if(primitive == NULL)
          continue;

        PrimitiveToListAppend(&new_frame->OwnPrimitivesList, primitive);
      }
    }
  }

  return new_frame;
}
// ----------------------------------------------------------------------------
static int FindFrames(hmi_settings_t *hmi_settings, const lwjson_token_t* _tkn)
{
//  const lwjson_token_t* frame_tkn = NULL;
  primitive_t *new_frame = NULL;

  if( (hmi_settings == NULL) || (_tkn == NULL) )
    return -1;

  // ищем секцию окон
  if(!TokenNameEqual(_tkn, HMI_FRAMES))
    return -1;

  // цикл по токенам окон
  for(const lwjson_token_t* frame_tkn = lwjson_get_first_child(_tkn); frame_tkn != NULL; frame_tkn = frame_tkn->next)
  {
    new_frame = CreateNewFrame(frame_tkn);
    if(new_frame == NULL)
      continue;
    PrimitiveToListAppend(&hmi_settings->FrameList, new_frame);
  }

  return 0;
}
// ----------------------------------------------------------------------------
void LwjsonParse(hmi_settings_t *hmi_settings, const char * json)
{
  lwjson_token_t* first_tkn;


  lwjson_init(&lwjson, tokens, LWJSON_ARRAYSIZE(tokens));

  if(lwjson_parse(&lwjson, json) != lwjsonOK)
  {
    printf("JSON parse error.\r\n");
    return;
  }

  printf("Number of tokens used to parse JSON: %u\r\n", lwjson_get_tokens_used(&lwjson));
  /* Get very first token as top object */
  first_tkn = lwjson_get_first_token(&lwjson);

  if(first_tkn->type != LWJSON_TYPE_OBJECT)
  {
    printf("JSON first token error.\r\n");
    return;
  }

  for(const lwjson_token_t* tkn = lwjson_get_first_child(first_tkn); tkn != NULL; tkn = tkn->next)
  {
    printf("Token: %.*s, Type: %s\n", (int)tkn->token_name_len, tkn->token_name, lwjson_type_s[tkn->type]);

    FindHmiSettings(hmi_settings, tkn);
    FindFrames(hmi_settings, tkn);

  }

  printf("\n\nhmi_settings:\n  ModbudAddr: %u\n  StartWindId: %u\n  CurrWindId: %u\n  SequencePeriod: %u\n",
          hmi_settings->ModbudAddr,
          hmi_settings->StartWindId,
          hmi_settings->CurrWindId,
          hmi_settings->SequencePeriod);
//          hmi_settings->FrameSequence,
//          hmi_settings->FrameList);
  printf("  FrameSequence: ");
  for(int i = 0; i < hmi_settings->FrameSequence.size; ++i)
    printf("%u,", hmi_settings->FrameSequence.data[i]);

  printf("\n  Frames:");
  primitive_t *frame = hmi_settings->FrameList;
  while(frame != NULL)
  {
    printf("\n    %s, %s\n\
      MbRegNum:%u\n\
      MbRegType:%u\n\
      PrimitiveType:%u\n\
      BackGround: %u\n\
      Size:%u, %u\n\
      LayerNum:%u\n\
      CoordinatesXY: %u, %u",

        primitiveType[frame->PrimitiveType], frame->ImageFileName,
        frame->MbRegNum,
        frame->MbRegType,
        frame->PrimitiveType,
        frame->BackGround,
        frame->Size.x, frame->Size.y,
        frame->LayerNum,
        frame->CoordinatesXY.x, frame->CoordinatesXY.y);

        printf("\n      Primitives:");
        primitive_t *primitive = frame->OwnPrimitivesList;
        while(primitive != NULL)
        {
          printf("\n\
          %s, %s\n\
          MbRegNum:%u\n\
          MbRegType:%u\n\
          PrimitiveType:%u\n\
          BackGround: %u\n\
          Size:%u, %u\n\
          LayerNum:%u\n\
          CoordinatesXY: %u, %u",

              primitiveType[primitive->PrimitiveType], primitive->ImageFileName,
              primitive->MbRegNum,
              primitive->MbRegType,
              primitive->PrimitiveType,
              primitive->BackGround,
              primitive->Size.x, primitive->Size.y,
              primitive->LayerNum,
              primitive->CoordinatesXY.x, primitive->CoordinatesXY.y);
          primitive = primitive->NextPrimitive;
        }

    frame = frame->NextPrimitive;
  }

}
// ----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif
