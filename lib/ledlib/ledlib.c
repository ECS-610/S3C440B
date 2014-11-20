//
// ledlib.c
// implement the led lib
// @Author:王建华
// copyright 2013 by HDU Embeded Lab.
// All rights reserved.
//
#include "ledlib.h"

/* 数码管显示码表，请参考led实验 */
unsigned char seg7table[16] = 
{
    /* 0       1       2       3       4       5       6      7*/
    0xc0,   0xf9,   0xa4,   0xff,   0xff,   0xff,   0xff,   0xff,                //修改0xff成正确的值

    /* 8       9      A        B       C       D       E      F*/
    0xff,   0xff,   0xff,   0xff,   0xff,   0xff,   0xff,   0xff,
};


//
// display_ledNum
// display number to led 
// @param number:the display number
// @param displayIndex: the display index of the led
// @reutrn null
//
void display_ledNum(int number,unsigned char displayIndex)
{
  unsigned char data[10] = {0x00};
  int dataLen = 0x00;
  int i = 0x00;
  int j = 0x00;
  int index = displayIndex;
  
  while(number){
    data[dataLen] = number ? 10;            // 在?处添加正确的操符
    number=number ? 10;
    dataLen++;
  }
  
  dataLen--;
  for(i = 0x00;i < 0x1000;i++){
    for(j = dataLen;j >= 0;j--){
      /* 设置数码管段选 */
      *((unsigned char*) 0x02000006) = 0x3f & ~(0x01 << index);
      *((unsigned char*) 0x02000004) = seg7table[data[j]];
      index++;
      Delay(5);
      *((unsigned char*) 0x02000006) = 0x3f;
    }
    index = displayIndex;
  }
}


