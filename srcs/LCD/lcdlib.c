/****************************************************************************/
/*                                                                          */
/* FILE NAME                                      VERSION                   */
/*                                                                          */
/* LCDLIB.C                                        1.0                      */
/*                                                                          */
/* DESCRIPTION                                                              */
/*                                                                          */
/*     LCD LIBRARY CODE for EB44B0(S3C44B0X)                                */
/*                                                                          */
/*                                                                          */
/* DATA STRUCTURES                                                          */
/*                                                                          */
/* FUNCTIONS : 320-240 DOT 256 COLOR LCD ROUTINE                            */
/*                                                                          */
/* DEPENDENCIES                                                             */
/*                                                                          */
/*                                                                          */
/* NAME:                       					                            */
/* REMARKS:                                                                 */
/*                                                                          */
/*								Copyright (C) 2003 Wuhan CVTECH CO.,LTD     */
/****************************************************************************/

#include "def.h"
#include "44b.h"
#include "44blib.h"
#include "lcdlib.h"

#define M5D(n)				((n) & 0x1fffff)

#define ARRAY_SIZE_COLOR 	(SCR_XSIZE/1*SCR_YSIZE)

#define HOZVAL			    (LCD_XSIZE/4-1) 
#define HOZVAL_COLOR	    (LCD_XSIZE*3/8-1) 
#define LINEVAL			    (LCD_YSIZE-1)  
#define MVAL			    (13) 
#define CLKVAL_COLOR 	    (8) 

#define MVAL_USED			0

unsigned char *frameBuffer256 = 0;

/*****************************************************************************
// Function name	: lcd_init
// Description	    : LCD显示驱动初始化函数
// Return type		: void
// Argument         : 无
*****************************************************************************/
void lcd_init()
{
	/*初始化LCD端口*/
    rPDATC=rPDATC&~(1<<8)|(1<<8);					/*GPE5=H	 lcd enable	*/
    rPCONC=rPCONC&~(0xff<<8)|(0xff<<8);				/*GPC[4:7] => VD[7:4]	*/

	/*申请显示缓冲区，大小与屏幕一样,320*240*/
 	if((U32)frameBuffer256==0)
	{
	    frameBuffer256=(unsigned char *)0x0c400000; 
		rNCACHBE1 = (((unsigned int)frameBuffer256)>>12) +  
			    ((((unsigned int)(frameBuffer256+ARRAY_SIZE_COLOR))>>12)<<16);
	}

	/*初始化LCD控制寄存器*/
	/*disable,8B_SNGL_SCAN,WDLY=8clk,WLH=8clk,*/
	rLCDCON1=(0)|(2<<5)|(MVAL_USED<<7)|(0x3<<8)|(0x3<<10)|(CLKVAL_COLOR<<12);
	
	/*LINEBLANK=10 (without any calculation) */
	rLCDCON2=(LINEVAL)|(HOZVAL_COLOR<<10)|(10<<21);  

	/*256-color, LCDBANK, LCDBASEU*/
	rLCDSADDR1= (0x3 << 27) | (((U32)frameBuffer256 >> 22) << 21) | 
				M5D((U32)frameBuffer256 >> 1); 

	rLCDSADDR2= M5D((((U32)frameBuffer256+(SCR_XSIZE*LCD_YSIZE))>>1)) | 
		        (MVAL<<21) | (1 << 29); 
	
	rLCDSADDR3= (LCD_XSIZE/2) | ( ((SCR_XSIZE-LCD_XSIZE)/2)<<9 );

	/* The following value has to be changed for better display.*/
	rREDLUT  =0xfdb96420;
	rGREENLUT=0xfdb96420;
	rBLUELUT =0xfb40;

	rDITHMODE=0x0;
	rDP1_2 =0xa5a5;      
	rDP4_7 =0xba5da65;
	rDP3_5 =0xa5a5f;
	rDP2_3 =0xd6b;
	rDP5_7 =0xeb7b5ed;
	rDP3_4 =0x7dbe;
	rDP4_5 =0x7ebdf;
	rDP6_7 =0x7fdfbfe;

	/* enable,8B_SNGL_SCAN,WDLY=8clk,WLH=8clk*/
	rLCDCON1=(1)|(2<<5)|(MVAL_USED<<7)|(0x3<<8)|(0x3<<10)|(CLKVAL_COLOR<<12); 
}

/*****************************************************************************
// Function name	: lcd_put_pixel
// Description	    : LCD打点函数
// Return type		: void
// Argument         : int x : x坐标
// Argument         : int y : y坐标
// Argument         : unsigned char c : 颜色
*****************************************************************************/
void lcd_put_pixel(int x,int y,unsigned char c)
{
    if(x<SCR_XSIZE && y<SCR_YSIZE)
  		*(((unsigned char *)(frameBuffer256)) + y * SCR_XSIZE + x) = c;
}

/*****************************************************************************
// Function name	: lcd_clear_screen
// Description	    : 清除LCD屏幕
// Return type		: void
// Argument         : unsigned char c : 屏幕颜色
*****************************************************************************/
void lcd_clear_screen(unsigned char c)
{	
    int i,j;
	
    for(j=0;j<SCR_YSIZE;j++)
    {
       	for(i=0;i<SCR_XSIZE;i++)
    	{
    		lcd_put_pixel(i,j,c);
	    }
	}
}

