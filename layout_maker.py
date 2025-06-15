import json
from enum import Enum
from builtins import input

JSON_INTENT = 2

HMI_SETTINGS    = 'HmiSettings'
# Ключи параметров HMI панели
MODBUD_ADDR     = 'ModbudAddr'
START_FRAME_ID  = 'StartFrameId'
CURR_FRAME_ID   = 'CurrFrameId'
FRAME_SEQUENCE  = 'FrameSequence'
SEQUENCE_PERIOD = 'SequencePeriod'

HMI_FRAMES  = 'HmiFrames'
# Ключи параметров окна
MB_REG_NUM      = 'MbRegNum'
MB_REG_TYPE     = 'MbRegType'
PRIMITIVE_TYPE  = 'PrimitiveType'
BACK_GROUND     = 'BackGround'
SIZE            = 'Size'
LAYER_NUM       = 'LayerNum'
COORDINATES_XY  = 'CoordinatesXY'
IMAGE_FILE      = 'ImageFile'

FRAME_PRIMITIVES = 'FramePrimitives'

SETTINGS_FILE = 'settings.json'

class MbRegType(Enum):
    DI      = 0
    DO      = 1
    INPUT   = 2
    HOLDING = 3

class BackGround(Enum):
    IMAGE   = 0
    COLOR   = 1

class PrimitiveType(Enum):
    FRAME   = 0
    LAMP    = 1
    BUTTON  = 3


HMI_RAW = \
{\
  HMI_SETTINGS :\
  {\
    MODBUD_ADDR        : 1,\
    START_FRAME_ID     : 2,\
    CURR_FRAME_ID      : 3,\
    FRAME_SEQUENCE     : [1,2,3],\
    SEQUENCE_PERIOD    : 4\
  },\
  HMI_FRAMES :\
  {\
    'Frame_0' :\
    {\
      PRIMITIVE_TYPE : PrimitiveType.FRAME.value,\
      MB_REG_NUM     : 5,\
      MB_REG_TYPE    : MbRegType.HOLDING.value,\
      BACK_GROUND    : BackGround.IMAGE.value,\
      SIZE           : [240,320],\
      LAYER_NUM      : 6,\
      COORDINATES_XY : [0,0],\
      IMAGE_FILE     : "win.bmp",\
      FRAME_PRIMITIVES :\
      {\
        'Button_0' :\
        {\
          PRIMITIVE_TYPE : PrimitiveType.BUTTON.value,\
          MB_REG_NUM     : 7,\
          MB_REG_TYPE    : MbRegType.HOLDING.value,\
          BACK_GROUND    : BackGround.IMAGE.value,\
          SIZE           : [10,10],\
          LAYER_NUM      : 8,\
          COORDINATES_XY : [5,5],\
          IMAGE_FILE     : "button.bmp"\
        },\
        'Button_1' :\
        {\
          PRIMITIVE_TYPE : PrimitiveType.BUTTON.value,\
          MB_REG_NUM     : 9,\
          MB_REG_TYPE    : MbRegType.HOLDING.value,\
          BACK_GROUND    : BackGround.IMAGE.value,\
          SIZE           : [10,10],\
          LAYER_NUM      : 10,\
          COORDINATES_XY : [5,5],\
          IMAGE_FILE     : "button.bmp"\
        },\
      }\
    },\
    'Frame_1' :\
    {\
      PRIMITIVE_TYPE : PrimitiveType.FRAME.value,\
      MB_REG_NUM     : 11,\
      MB_REG_TYPE    : MbRegType.HOLDING.value,\
      BACK_GROUND    : BackGround.IMAGE.value,\
      SIZE           : [240,320],\
      LAYER_NUM      : 12,\
      COORDINATES_XY : [0,0],\
      IMAGE_FILE     : "win.bmp",\
      FRAME_PRIMITIVES :\
      {\
        'Image_0' :\
        {\
          PRIMITIVE_TYPE : PrimitiveType.LAMP.value,\
          MB_REG_NUM     : 13,\
          MB_REG_TYPE    : MbRegType.HOLDING.value,\
          BACK_GROUND    : BackGround.IMAGE.value,\
          SIZE           : [10,10],\
          LAYER_NUM      : 14,\
          COORDINATES_XY : [5,5],\
          IMAGE_FILE     : "image.bmp"\
        },\
      }\
    }\
  }\
}


def main():
    print(f'RAW HMI structure:\n{HMI_RAW}')
    HMI_JSON = json.dumps(HMI_RAW, indent=JSON_INTENT)
    print(f'\nSerialized HMI structure:\n{HMI_JSON}')
    f = open(SETTINGS_FILE, mode='wt')
    f.write(HMI_JSON)
    f.close()
    input()
    pass

if __name__ == '__main__':
    main()
