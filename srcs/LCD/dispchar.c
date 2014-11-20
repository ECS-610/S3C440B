/****************************************************************************/
/*                                                                          */
/* FILE NAME                                      VERSION                   */
/*                                                                          */
/* DISPCHAR.C                                            1.0                */
/*                                                                          */
/* DESCRIPTION                                                              */
/*                                                                          */
/*     JX44B0(S3C44B0X)字符显示实验                                         */
/*                                                                          */
/*                                                                          */
/* DATA STRUCTURES                                                          */
/*                                                                          */
/* FUNCTIONS :                                                              */
/*     在JX44B0教学实验箱进行ASCII字符和汉字显示的实验                      */
/*                                                                          */
/* DEPENDENCIES                                                             */
/*     JX44B0-1                                                             */
/*     JX44B0-2                                                             */
/*     JX44B0-3                                                             */
/*                                                                          */
/*                                                                          */
/* NAME:                                                                    */
/* REMARKS:                                                                 */
/*                                                                          */
/*								Copyright (C) 2003 Wuhan CVTECH CO.,LTD     */
/****************************************************************************/

/****************************************************************************/
/* 学习JX44B0中字符（ASCII字符和汉字）的显示方法：                          */
/* 显示颜色说明：                                                           */
/*     在JX44B0实验系统中，1个字节代表屏幕上一个点                          */
/*     每个字节各位的意义：                                                 */
/*     bit0 - bit1 : 蓝色                                                   */
/*     bit2 - bit4 : 绿色                                                   */
/*     bit5 - bit7 : 红色                                                   */
/****************************************************************************/
#include "configure.h"

#ifdef __DIAPLYCHAR_EN
/* 包含文件 */
#include "lcdlib.h"
#include "hzk16.h"
void showHZ(int x,int y,int colour);
/*****************************************************************************
// Function name	: lcd_disp_hzk16
// Description	    : 在LCD的(x,y)坐标处以colour颜色显示s中的汉字
// Return type		: void
// Argument         : int x : x坐标
// Argument         : int y : y坐标
// Argument         : char *s : 待显示字符串
// Argument         : int colour : 显示颜色
*****************************************************************************/
void lcd_disp_hzk16(int x,int y,char *s,int colour)
{
	char buffer[32];							/* 32字节的字模缓冲区		*/
 	int i,j,k;
 	unsigned char qh,wh;
 	unsigned long location;

 	while(*s)
  	{
   		qh=*s-0xa0;					                 /* 计算区码					*/
   		wh=*(s+1)-0xa0;							/* 计算位码					*/
   		location=(94*(qh-1)+(wh-1))*32L;		/* 计算字模在文件中的位置	*/
   		memcpy(buffer, &hzk16[location], 32);	/* 获取汉字字模				*/
                
		for(i=0;i<16;i++)						/* 每一行					*/
		{
    		for(j=0;j<2;j++)					/* 一行两个字节				*/
			{
     			for(k=0;k<8;k++)				/* 每个字节按位显示			*/
				{
      				if(((buffer[i*2+j]>>(7-k)) & 0x1) != 0)
       					lcd_put_pixel(x+8*(j)+k,y+i,colour); /* 显示一位	*/
				}
			}
		}
   		s+=2;												/* 下一个汉字	*/
   		x+=16;												/* 汉字间距		*/
  	}
}
/*****************************************************************************
// Function name	: Main
// Description	    : C entry function of demo
// Return type		: void
// Argument         : void
*****************************************************************************/
void main(void)
{
	/* 显示初始化 */
    lcd_init();
    lcd_clear_screen(WHITE);			/* 清除整个LCD屏幕					*/

    /* 显示字符串 */
    lcd_disp_hzk16(100,100,"杭电，我爱你！", BLUE);
//    showHZ(10,10,BLUE);
    
    while(1);
}
void showHZ(int x,int y,int colour)
{
   char buffer[32];
   unsigned char qh,wh;
   unsigned long location;  
  int i,j,k;  
  
  qh = 1;
  wh = 1;
   location=(94*(qh-1)+(wh-1))*32L;		/* 计算字模在文件中的位置	*/
   memcpy(buffer, &hzk16[location], 32);	/* 获取汉字字模				*/
                
   for(i=0;i<16;i++)						/* 每一行					*/
   {
     for(j=0;j<2;j++)					/* 一行两个字节				*/
     {
     	for(k=0;k<8;k++)				/* 每个字节按位显示			*/
	{
      	   if(((buffer[i*2+j]>>(7-k)) & 0x1) != 0)
       		lcd_put_pixel(x+8*(j)+k,y+i,colour); /* 显示一位	*/
	}
     }
   }  


}

#endif //__DIAPLYCHAR_EN

