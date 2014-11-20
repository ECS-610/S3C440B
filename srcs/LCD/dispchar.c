/****************************************************************************/
/*                                                                          */
/* FILE NAME                                      VERSION                   */
/*                                                                          */
/* DISPCHAR.C                                            1.0                */
/*                                                                          */
/* DESCRIPTION                                                              */
/*                                                                          */
/*     JX44B0(S3C44B0X)�ַ���ʾʵ��                                         */
/*                                                                          */
/*                                                                          */
/* DATA STRUCTURES                                                          */
/*                                                                          */
/* FUNCTIONS :                                                              */
/*     ��JX44B0��ѧʵ�������ASCII�ַ��ͺ�����ʾ��ʵ��                      */
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
/* ѧϰJX44B0���ַ���ASCII�ַ��ͺ��֣�����ʾ������                          */
/* ��ʾ��ɫ˵����                                                           */
/*     ��JX44B0ʵ��ϵͳ�У�1���ֽڴ�����Ļ��һ����                          */
/*     ÿ���ֽڸ�λ�����壺                                                 */
/*     bit0 - bit1 : ��ɫ                                                   */
/*     bit2 - bit4 : ��ɫ                                                   */
/*     bit5 - bit7 : ��ɫ                                                   */
/****************************************************************************/
#include "configure.h"

#ifdef __DIAPLYCHAR_EN
/* �����ļ� */
#include "lcdlib.h"
#include "hzk16.h"
void showHZ(int x,int y,int colour);
/*****************************************************************************
// Function name	: lcd_disp_hzk16
// Description	    : ��LCD��(x,y)���괦��colour��ɫ��ʾs�еĺ���
// Return type		: void
// Argument         : int x : x����
// Argument         : int y : y����
// Argument         : char *s : ����ʾ�ַ���
// Argument         : int colour : ��ʾ��ɫ
*****************************************************************************/
void lcd_disp_hzk16(int x,int y,char *s,int colour)
{
	char buffer[32];							/* 32�ֽڵ���ģ������		*/
 	int i,j,k;
 	unsigned char qh,wh;
 	unsigned long location;

 	while(*s)
  	{
   		qh=*s-0xa0;					                 /* ��������					*/
   		wh=*(s+1)-0xa0;							/* ����λ��					*/
   		location=(94*(qh-1)+(wh-1))*32L;		/* ������ģ���ļ��е�λ��	*/
   		memcpy(buffer, &hzk16[location], 32);	/* ��ȡ������ģ				*/
                
		for(i=0;i<16;i++)						/* ÿһ��					*/
		{
    		for(j=0;j<2;j++)					/* һ�������ֽ�				*/
			{
     			for(k=0;k<8;k++)				/* ÿ���ֽڰ�λ��ʾ			*/
				{
      				if(((buffer[i*2+j]>>(7-k)) & 0x1) != 0)
       					lcd_put_pixel(x+8*(j)+k,y+i,colour); /* ��ʾһλ	*/
				}
			}
		}
   		s+=2;												/* ��һ������	*/
   		x+=16;												/* ���ּ��		*/
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
	/* ��ʾ��ʼ�� */
    lcd_init();
    lcd_clear_screen(WHITE);			/* �������LCD��Ļ					*/

    /* ��ʾ�ַ��� */
    lcd_disp_hzk16(100,100,"���磬�Ұ��㣡", BLUE);
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
   location=(94*(qh-1)+(wh-1))*32L;		/* ������ģ���ļ��е�λ��	*/
   memcpy(buffer, &hzk16[location], 32);	/* ��ȡ������ģ				*/
                
   for(i=0;i<16;i++)						/* ÿһ��					*/
   {
     for(j=0;j<2;j++)					/* һ�������ֽ�				*/
     {
     	for(k=0;k<8;k++)				/* ÿ���ֽڰ�λ��ʾ			*/
	{
      	   if(((buffer[i*2+j]>>(7-k)) & 0x1) != 0)
       		lcd_put_pixel(x+8*(j)+k,y+i,colour); /* ��ʾһλ	*/
	}
     }
   }  


}

#endif //__DIAPLYCHAR_EN

