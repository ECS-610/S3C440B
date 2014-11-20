/****************************************************************************/
/*                                                                          */
/* FILE NAME                                      VERSION                   */
/*                                                                          */
/* 44BLIB.C                                        1.0                      */
/*                                                                          */
/* DESCRIPTION                                                              */
/*                                                                          */
/*     LIBRARY CODE for EB44B0(S3C44B0X)                                    */
/*                                                                          */
/*                                                                          */
/* DATA STRUCTURES                                                          */
/*                                                                          */
/* FUNCTIONS :                                                              */
/*                                                                          */
/* DEPENDENCIES                                                             */
/*                                                                          */
/*                                                                          */
/* NAME:                           					                        */
/* REMARKS:                                                                 */
/*                                                                          */
/*								Copyright (C) 2003 Wuhan CVTECH CO.,LTD     */
/****************************************************************************/

/* includes */
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "44b.h"
#include "44blib.h"
#include "def.h"
#include "option.h"

/* defines */
#define STACKSIZE    0xa00
#define HEAPEND     (_ISR_STARTADDRESS-STACKSIZE-0x500)

/*globals*/
static int delayLoopCount=401;
static int whichUart=0;


/*****************************************************************************
// Function name	: Delay
// Description	    : delay 'time' ms 
// Return type		: void
// Argument         : int time : time by ms to be delayed
*****************************************************************************/
void Delay(int time)
{
    int i;

    for(;time>0;time--)
		for(i=0;i<delayLoopCount;i++);
}
/*****************************************************************************
// Function name	: Uart_Init
// Description	    : uart initiazation
// Return type		: void
// Argument         : int mclk : mclk, such as 60000000 -> 60MHZ
// Argument         : int baud : baudrate, such as 115200
*****************************************************************************/
void Uart_Init(int mclk,int baud)
{
    int i;
    if(mclk==0)
		mclk=MCLK;
    rUFCON0=0xf7;       
    rUFCON1=0x0;
    rUMCON0=0x0;
    rUMCON1=0x0;

    rULCON0=0x3;						/*Normal,No parity,1 stop,8 bit		*/
    rUCON0=0x345;						/*rx=edge,tx=level,disable timeout	*/
										/*enable rx error int.,				*/
										/*normal,interrupt or polling		*/

    rUBRDIV0=( (int)(mclk/16./baud + 0.5) -1 );

    rULCON1=0x3;
    rUCON1=0x245;
    rUBRDIV1=( (int)(mclk/16./baud + 0.5) -1 );

    for(i=0;i<100;i++);
}
/*****************************************************************************
// Function name	: Uart_Select
// Description	    : choose uart 0 or uart1
// Return type		: void
// Argument         : int ch : 0 - uart0, 1 - uart1
*****************************************************************************/
void Uart_Select(int ch)
{
    whichUart=ch;
}
/*****************************************************************************
// Function name	: Uart_TxEmpty
// Description	    : wait for tx shifter empty
// Return type		: void
// Argument         : int ch : 0 - uart0, 1 - uart1
*****************************************************************************/
void Uart_TxEmpty(int ch)
{
    if(ch==0)
		while(!(rUTRSTAT0 & 0x4));		/*wait until tx shifter is empty.*/
    else
    	while(!(rUTRSTAT1 & 0x4));		/*wait until tx shifter is empty.*/
}
/*****************************************************************************
// Function name	: Uart_Getch
// Description	    : receive char from current uart
// Return type		: char : received char
// Argument         : void
*****************************************************************************/
char Uart_Getch(void)
{
    if(whichUart==0)
    {	
		while(!(rUTRSTAT0 & 0x1));		/*Receive data ready*/
			return	rURXH0;
    }
    else
    {
		while(!(rUTRSTAT1 & 0x1));		/*Receive data ready*/
			return	rURXH1;
    }
}
/*****************************************************************************
// Function name	: Uart_GetKey
// Description	    : receive char from current uart, if not return 0
// Return type		: char
// Argument         : void
*****************************************************************************/
char Uart_GetKey(void)
{
    if(whichUart==0)
    {	    
		if(rUTRSTAT0 & 0x1)				/*Receive data ready*/
    	    return rURXH0;
		else
			return 0;
    }
    else
    {
		if(rUTRSTAT1 & 0x1)				/*Receive data ready*/
			return rURXH1;
		else
			return 0;
    }
}
/*****************************************************************************
// Function name	: Uart_GetString
// Description	    : get a string from current uart
// Return type		: void
// Argument         : char *string : received string buffer point
*****************************************************************************/
void Uart_GetString(char *string)
{
    char *string2=string;
    char c;
    while((c=Uart_Getch())!='\r')
    {
		if(c=='\b')
		{
		    if(	(int)string2 < (int)string )
		    {
			Uart_Printf("\b \b");
			string--;
		    }
		}
		else 
		{
		    *string++=c;
		    Uart_SendByte(c);
		}
    }
    *string='\0';
    Uart_SendByte('\n');
}
/*****************************************************************************
// Function name	: Uart_GetIntNum
// Description	    : get a int type var from current uart
// Return type		: int : received int value
// Argument         : void
*****************************************************************************/
int Uart_GetIntNum(void)
{
    char str[30];
    char *string=str;
    int base=10;
    int minus=0;
    int lastIndex;
    int result=0;
    int i;
    
    Uart_GetString(string);
    
    if(string[0]=='-')
    {
        minus=1;
        string++;
    }
    
    if(string[0]=='0' && (string[1]=='x' || string[1]=='X'))
    {
		base=16;
		string+=2;
    }
    
    lastIndex=strlen(string)-1;
    if( string[lastIndex]=='h' || string[lastIndex]=='H' )
    {
		base=16;
		string[lastIndex]=0;
		lastIndex--;
    }

    if(base==10)
    {
		result=atoi(string);
		result=minus ? (-1*result):result;
    }
    else
    {
		for(i=0;i<=lastIndex;i++)
		{
	    	if(isalpha(string[i]))
		    {
				if(isupper(string[i]))
				    result=(result<<4)+string[i]-'A'+10;
				else
				    result=(result<<4)+string[i]-'a'+10;
		    }
		    else
		    {
				result=(result<<4)+string[i]-'0';
		    }
		}
		result=minus ? (-1*result):result;
    }
    return result;
}
/*****************************************************************************
// Function name	: Uart_SendByte
// Description	    : send a int type var to current uart
// Return type		: void
// Argument         : int data
*****************************************************************************/
void Uart_SendByte(int data)
{
    if(whichUart==0)						/*通过串口0发送*/
    {
		if(data=='\n')
		{
		    while(!(rUTRSTAT0 & 0x2));		/*等待发送缓存区空*/
			WrUTXH0('\r');					/*发送回车*/
		}
		while(!(rUTRSTAT0 & 0x2));			/*Wait until THR is empty.*/
		WrUTXH0(data);						/*发送数据*/
    }
    else									/*通过串口1发送*/
    {
		if(data=='\n')
		{
  	      	while(!(rUTRSTAT1 & 0x2));
			rUTXH1='\r';
		}
		while(!(rUTRSTAT1 & 0x2));			/*Wait until THR is empty.*/
		rUTXH1=data;
    }	
}		
/*****************************************************************************
// Function name	: Uart_SendString                                
// Description	    : send a string to current uart
// Return type		: void
// Argument         : char *pt : string buffer point to be sended
*****************************************************************************/
void Uart_SendString(char *pt)
{
    while(*pt)
	Uart_SendByte(*pt++);
}
/*****************************************************************************
// Function name	: Uart_Get_Data
// Description	    : receive data from com( file )
// Return type		: void
// Argument         : unsigned int addr : ram address
*****************************************************************************/
void Uart_Get_Data(unsigned int addr)
{
	unsigned char *   pData;
	int 			  size;
	
	pData = (unsigned char *)addr;

	Uart_Printf("Please enter the size of file to be downloaded:");
	size = Uart_GetIntNum();
	Uart_Printf("\r\nDownloading, size = %d(0x%x)... \r\n", size, size);

	while((int)(pData - (unsigned char *)addr) < size)
	{
		*pData = Uart_Getch();
		pData ++;
	}

	Uart_Printf("Download complete\r\n");
}
/*****************************************************************************
// Function name	: Uart_Printf
// Description	    : output debug information to uart
// Return type		: void
// Argument         : char *fmt
// Argument         : ...
*****************************************************************************/
char debug_string[1024];
void Uart_Printf(char *fmt,...)
{
    va_list ap;

    va_start(ap,fmt);
    vsprintf(debug_string,fmt,ap);
    Uart_SendString(debug_string);
    va_end(ap);
}
/*****************************************************************************
// Function name	: Led_Display
// Description	    : LED display
// Return type		: void
// Argument         : int data
*****************************************************************************/
void Led_Display(int data)
{
    rPDATB=(rPDATB & 0x1ff) | ((data & 0x3)<<9);
}
/*****************************************************************************
// Function name	: buzzer_on_off
// Description	    : buzzer on or off control
// Return type		: void
// Argument         : int count
// Argument         : int delay
*****************************************************************************/
void buzzer_on_off(int count,int delay)
{
	int i, rTEMP;

	rTEMP=rPCONE;						/*保存E口控制寄存器的值*/
	rPCONE=(rPCONE & 0xfff3f) | 0x40;	/*porte.3 out mode*/

	for(i=0;i<count;i++)
	{
		rPDATE=rPDATE & 0x1f7;			/*porte.3 clear (buzzer on)*/
		Delay(delay);
		rPDATE=rPDATE | 0x8;			/*porte.3.set (buzzer off)*/
		Delay(delay);
   	}
   	rPCONE=rTEMP;						/*恢复E口控制寄存器的值*/
}	
/*****************************************************************************
// Function name	: Timer_Start
// Description	    : start the watchdog timer
//                    t_watchdog=1/(MCLK/(Prescaler value+1)/Division_factor)
// Return type		: void
// Argument         : int divider : the clock division factor
//                        0 : 1/16  - 16 us
//                        1 : 1/32  - 32 us
//                        2 : 1/64  - 64 us
//                        3 : 1/128 - 128 us
*****************************************************************************/
void Timer_Start(int divider)  
{
    /* 1/16/(65+1),nRESET & interrupt disable */
    rWTCON=((MCLK/1000000-1)<<8)|(divider<<3);
    rWTDAT=0xffff;
    rWTCNT=0xffff;   

    /* 1/16/(65+1),nRESET & interrupt enable  */
    rWTCON=((MCLK/1000000-1)<<8)|(divider<<3)|(1<<5);	
}
/*****************************************************************************
// Function name	: Timer_Stop
// Description	    : stop the watchdog timer
// Return type		: int
// Argument         : void
*****************************************************************************/
int Timer_Stop(void)
{
    rWTCON=((MCLK/1000000-1)<<8);
    return (0xffff-rWTCNT);
}
/*****************************************************************************
// Function name	: ChangePllValue
// Description	    : change the PLL value
// Return type		: void
// Argument         : int mdiv
// Argument         : int pdiv
// Argument         : int sdiv
*****************************************************************************/
void ChangePllValue(int mdiv,int pdiv,int sdiv)
{
    rPLLCON=(mdiv<<12)|(pdiv<<4)|sdiv;
}
/*****************************************************************************
// Function name	: Cache_Flush
// Description	    : cache flush
// Return type		: void
// Argument         : void
*****************************************************************************/
void Cache_Flush(void)
{
    int i,saveSyscfg;
    
    saveSyscfg=rSYSCFG;

    rSYSCFG=SYSCFG_0KB;						/*Disable cache*/
    for(i=0x10004000;i<0x10004800;i+=16)	/*128字(512字节)的LRU全清0*/
    {					   
		*((int *)i)=0x0;		   
    }
    rSYSCFG=saveSyscfg; 			    
}
unsigned short ntohs(unsigned short s)
{
	return (s >> 8) | (s << 8);
}

unsigned long ntohl(unsigned long l)
{
	return	((l >> 24) & 0x000000ff) |
		((l >>	8) & 0x0000ff00) |
		((l <<	8) & 0x00ff0000) |
		((l << 24) & 0xff000000);
}

unsigned short htons(unsigned short s)
{
	return (s >> 8) | (s << 8);
}

unsigned long htonl(unsigned long l)
{
	return	((l >> 24) & 0x000000ff) |
		((l >>	8) & 0x0000ff00) |
		((l <<	8) & 0x00ff0000) |
		((l << 24) & 0xff000000);
}



