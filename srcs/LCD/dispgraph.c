/****************************************************************************/
/*                                                                          */
/* FILE NAME                                      VERSION                   */
/*                                                                          */
/* DISPGRAPH.C                                            1.0               */
/*                                                                          */
/* DESCRIPTION                                                              */
/*                                                                          */
/*     JX44B0(S3C44B0X)图形显示实验                                         */
/*                                                                          */
/*                                                                          */
/* DATA STRUCTURES                                                          */
/*                                                                          */
/* FUNCTIONS :                                                              */
/*     在JX44B0教学实验箱进行图形显示的实验                                 */
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
/* 学习JX44B0中图形的显示方法：                                             */
/* 显示颜色说明：                                                           */
/*     在JX44B0实验系统中，1个字节代表屏幕上一个点                          */
/*     每个字节各位的意义：                                                 */
/*     bit0 - bit1 : 蓝色                                                   */
/*     bit2 - bit4 : 绿色                                                   */
/*     bit5 - bit7 : 红色                                                   */
/****************************************************************************/
#include "configure.h"

#ifdef __DISPLAYGRAPH_EN
/* 包含文件 */
#include "lcdlib.h"
#include "picture/02.h"

/*****************************************************************************
// Function name	: lcd_disp_graph
// Description	    : 
// Return type		: void
// Argument         : unsigned char * image : 待显示图形数据指针
*****************************************************************************/
void lcd_disp_graph(unsigned char * image)
{
	unsigned int x,y;

    for(y=0;y<SCR_YSIZE;y++)
   		for(x=0;x<SCR_XSIZE;x++)
    		lcd_put_pixel(x,y,*(image+y*SCR_XSIZE+x));
}

/*****************************************************************************
// Function name	: Main
// Description	    : C entry function of demo
// Return type		: void
// Argument         : void
*****************************************************************************/
void main(void)
{
    lcd_init();
    lcd_clear_screen(WHITE);

	while(1)
	{		
    	lcd_disp_graph((unsigned char *)IMAGE_02);
	}
}
#endif //__DISPLAYGRAPH_EN

