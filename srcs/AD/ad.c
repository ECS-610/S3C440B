/****************************************************************************/
/*                                                                          */
/* FILE NAME                                      VERSION                   */
/*                                                                          */
/* AD.C                                            1.0                      */
/*                                                                          */
/* DESCRIPTION                                                              */
/*                                                                          */
/*     JX44B0(S3C44B0X)AD采样实验                                           */
/*                                                                          */
/*                                                                          */
/* DATA STRUCTURES                                                          */
/*                                                                          */
/* FUNCTIONS :                                                              */
/*     在JX44B0教学实验箱进行AD采样实验                                     */
/*     主程序循环采集AD值保存到display_buffer中                             */
/*     开启一个定时器定时显示display_buffer中的值到数码管中                 */
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
/* 学习S3C44B0X处理器中的AD采样方法：                                       */
/****************************************************************************/
#include "configure.h"

#ifdef __AD_EN
/* 包含文件 */
#include "44b.h"

/* defines */
#define MAX_VALUE			0x3ff
#define MIN_VALUE			0x0

typedef void (*ISR_ROUTINE_ENTRY)(void);

void Test_Adc(void);
char trans_input(int value);
void init_interrupt_handler(unsigned int irq_handler);
void install_isr_handler(int irq_no, void* irq_routine);

void timer0_isr(void);
__irq void  IsrIRQ(void);

char display_buffer[6];
int  refresh_index = 0;

extern unsigned char seg7table[];


/********************************************************************
// Function name	: Main
// Description	    : AD采样主函数
// Return type		: void
// Argument         : void
*********************************************************************/
void main(void)
{
      asm("SWI 0");   //CPSR I位清零
      
      rTCFG0=0x0;  		// prescaler=0,means "1" devider the same as timer0
      rTCNTB0=0x2000;  	//10ms

      rTCON= 0x00000202;
	
      rTCON= 0x00000009;	//T0,auto reload and start
	
    rINTCON=0x7;		/* Non-vect,IRQ disable,FIQ disable	*/
	
    init_interrupt_handler((unsigned int)IsrIRQ);
    install_isr_handler(HandleTIMER0, 	(void*)timer0_isr);

    rINTMOD=0x0;
    rINTMSK=0x07ffffff &~(BIT_GLOBAL|BIT_TIMER0);
    rINTCON=0x5;

	Test_Adc();
}

/********************************************************************
// Function name	: Test_Adc
// Description	    : test adc
// Return type		: void
// Argument         : void
*********************************************************************/
void Test_Adc(void)
{
	int loop_delay;
	
	int data = 0;
	
	int get_index;
	
    rCLKCON=0x7ff8;	
    
    while(1)
    {
    	char value;

		rADCPSR=0x0;
		
		data = 0;	
		
		for(get_index = 0; get_index < 16; get_index++)
		{
			rADCCON=0x1|(0x2<<2);	// AIN2
			while(!(rADCCON & 0x40));
			data+=rADCDAT;
			
			for(loop_delay=1000; loop_delay; loop_delay--);
		}
		
		data /= 16;
		
		value = trans_input(data);		
		display_buffer[0] = value / 10;
		display_buffer[1] = value % 10;

		data = 0;	
		
		for(get_index = 0; get_index < 16; get_index++)
		{
			rADCCON=0x1|(0x3<<2);	// AIN3
			while(!(rADCCON & 0x40));
			data+=rADCDAT;
			
			for(loop_delay=1000; loop_delay; loop_delay--);
		}
		
		data /= 16;
		
		value = trans_input(data);	
		
		display_buffer[3] = value / 10;
		display_buffer[4] = value % 10;
		
	    for(loop_delay=100000; loop_delay; loop_delay--);
    }
}

char trans_input(int value)
{
	if(value == MAX_VALUE) return 50;
	
	return (char)(value * 50 / (MAX_VALUE-MIN_VALUE));
}

/*****************************************************************************
// Function name	: rtc_tick_isr
// Description	    : TICK中断处理程序
// Return type		: int
// Argument         : void
*****************************************************************************/
void timer0_isr(void)
{
    rI_ISPC=BIT_TIMER0;

	if(refresh_index==2) refresh_index++;
	if(refresh_index==5) refresh_index = 0;

    /* 设置数码管段选 */
    *((unsigned char*) 0x02000006) = 0x3f & ~(0x01 << refresh_index);
    *((unsigned char*) 0x02000004) = seg7table[display_buffer[refresh_index]];
    
    refresh_index++;
}

/*****************************************************************************
// Function name	: IsrIRQ
// Description	    : 非矢量方式下中断的查表处理
//					  中断地址表位于0x0c7fff00开始的256字节
// Return type		: void
// Argument         : void
*****************************************************************************/
__irq void IsrIRQ( void )
{
	int count = 0;
	unsigned int isr_pending;
	unsigned int isr_mask = 0x00000001;
	
	unsigned int isr_mask_set = rINTMSK;
	
	ISR_ROUTINE_ENTRY isr_routine_entry = (ISR_ROUTINE_ENTRY)0x0;
	
	isr_pending = (rINTPND & ~isr_mask_set);
	
	while(isr_mask)
	{
		if(isr_pending&isr_mask)
		{
			isr_routine_entry = (ISR_ROUTINE_ENTRY)(*(int*)(HandleADC+count));
			break;
		}
		
		count+=4;
		isr_mask <<= 1;
	}
	
	if(isr_routine_entry) (*isr_routine_entry)();
}

/*****************************************************************************
// Function name	: init_interrupt_handler
// Description	    : 非矢量方式下中断向量表初始化处理
// Return type		: void
// Argument         : irq_handler
//					  中断处理函数入口
*****************************************************************************/
void init_interrupt_handler(unsigned int irq_handler) 
{
	int i;
	
	rINTPND = 0x00000000;							/* 清除所有未决的中断	*/
	rI_ISPC = 0x03FFFFFF; 
	
	for( i = 0; i < 256; i+=4)						/* 清除中断表			*/
		*(unsigned int*)(_ISR_STARTADDRESS+i) = 0;

	*(unsigned int*)(HandleIRQ) = irq_handler;		/* 设置IRQ模式处理函数	*/
}

/*****************************************************************************
// Function name	: install_isr_handler
// Description	    : 非矢量方式下中断向量的安装
// Return type		: void
// Argument         : irq_no, 中断号
//					  irq_routine, 中断处理函数地址
*****************************************************************************/
void install_isr_handler(int irq_no, void* irq_routine)
{
	*(unsigned int*)(irq_no) = (unsigned int)irq_routine;
}
#endif //__AD_EN