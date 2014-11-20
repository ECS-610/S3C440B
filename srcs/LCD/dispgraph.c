/****************************************************************************/
/*                                                                          */
/* FILE NAME                                      VERSION                   */
/*                                                                          */
/* DISPGRAPH.C                                            1.0               */
/*                                                                          */
/* DESCRIPTION                                                              */
/*                                                                          */
/*     JX44B0(S3C44B0X)ͼ����ʾʵ��                                         */
/*                                                                          */
/*                                                                          */
/* DATA STRUCTURES                                                          */
/*                                                                          */
/* FUNCTIONS :                                                              */
/*     ��JX44B0��ѧʵ�������ͼ����ʾ��ʵ��                                 */
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
/* ѧϰJX44B0��ͼ�ε���ʾ������                                             */
/* ��ʾ��ɫ˵����                                                           */
/*     ��JX44B0ʵ��ϵͳ�У�1���ֽڴ�����Ļ��һ����                          */
/*     ÿ���ֽڸ�λ�����壺                                                 */
/*     bit0 - bit1 : ��ɫ                                                   */
/*     bit2 - bit4 : ��ɫ                                                   */
/*     bit5 - bit7 : ��ɫ                                                   */
/****************************************************************************/
#include "configure.h"

#ifdef __DISPLAYGRAPH_EN
/* �����ļ� */
#include "lcdlib.h"
#include "picture/02.h"

/*****************************************************************************
// Function name	: lcd_disp_graph
// Description	    : 
// Return type		: void
// Argument         : unsigned char * image : ����ʾͼ������ָ��
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

