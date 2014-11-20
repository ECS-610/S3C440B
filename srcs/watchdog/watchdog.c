/****************************************************************************/
/*                                                                          */
/* FILE NAME                                      VERSION                   */
/*                                                                          */
/* WATCHDOG.C                                            1.0                */
/*                                                                          */
/* DESCRIPTION                                                              */
/*                                                                          */
/*     JX44B0(S3C44B0X)看门狗实验                                           */
/*                                                                          */
/*                                                                          */
/* DATA STRUCTURES                                                          */
/*                                                                          */
/* FUNCTIONS :                                                              */
/*     在JX44B0教学实验箱进行看门狗定时器的实验                             */
/*     正常运行时数码管依次显示出0、1，2......F                             */
/*     程序异常时（没有正常喂狗时），将不能正常显示                         */
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
/* 学习S3C44B0X处理器中的看门狗处理方法：                                           */
/****************************************************************************/
#include "configure.h"

#ifdef __WATCHDOG_EN
/* 包含文件 */
#include "44b.h"

/* defines */
#define WATCON				0X01D30000
#define WATDAT				0X01D30004
#define WATCNT				0X01D30008

#define WATCH_ENABLE

#define WDT_ENABLE			(0x01<<5)
#define WDT_INT_ENABLE		(0x01<<2)
#define WDT_RST_ENABLE		(0x01<<0)	

#define WDT_CLK_SEL			(0X3    <<3)		/* 1/128 */
#define WDT_PRE_SCALER		(0XFF   <<8)		/* 255    */

typedef void (*ISR_ROUTINE_ENTRY)(void);

/* functions */
void rtc_tick_init( char tick );
void rtc_tick_isr(void);
__irq void  IsrIRQ(void);

/* globals */
int led_index = 0;

/* 数码管显示码表，请参考led实验 */
extern unsigned char seg7table[];



/*****************************************************************************
// Function name	: IsrIRQ
// Description	    : 非矢量方式下中断的查表处理
//					  中断地址表位于0x0c7fff00开始的256字节
// Return type		: void
// Argument         : void
*****************************************************************************/
__irq void IsrIRQ()
{
	int count = 0;
	unsigned int isr_pending;
	unsigned int isr_mask = 0x00000001;
	
	unsigned int isr_mask_set = rINTMSK;
	
	ISR_ROUTINE_ENTRY isr_routine_entry = (ISR_ROUTINE_ENTRY)0x0;
	
	isr_pending = (rINTPND & ~isr_mask_set);
	
	while(isr_mask)
	{
		if((isr_pending&isr_mask) == 0)
		{
			count+=4;
			isr_mask <<= 1;
			continue;
		}

		isr_routine_entry = (ISR_ROUTINE_ENTRY)(*(int*)(HandleADC+count));
		break;
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

/****************************************************************************/
/* 函数说明: JX44B0看门狗测试                                               */
/* 功能描述: 依次在7段数码管上显示0123456789ABCDEF                          */                                            
/*     正常运行时数码管依次显示出0、1，2......F                             */
/*     程序异常时（没有正常喂狗时），将不能正常显示                         */
/* 返回代码: 无                                                             */
/* 参数说明: 无                                                             */
/****************************************************************************/
void main(void)
{    
   asm("SWI 0");   //CPSR I位清零
   
   rWATCON = 0x0000;					/* 关闭看门狗 */
	
   rINTCON=0x7;						/* Non-vect,IRQ disable,FIQ disable */
    
   rtc_tick_init(127);
      
	init_interrupt_handler((unsigned int)IsrIRQ);
	install_isr_handler(HandleTICK, (void*)rtc_tick_isr);

    rINTMOD=0x0;									/*设置所有中断为IRQ模式*/
	
    rINTMSK=(0x07ffffff&~(BIT_GLOBAL|BIT_TICK));	/*使能TICK中断*/

    rINTCON=0x5;						/*Non-vect,IRQ enable,FIQ disable   */
    
	rWATCNT = 0X1000;					/* 设置看门狗初始值 */

	/* 打开看门狗 */	
	rWATCON = WDT_ENABLE | WDT_RST_ENABLE | WDT_CLK_SEL | WDT_PRE_SCALER;
	
	/* 数码管从0到F依次将字符显示出来 */
	while(1);
}

/*****************************************************************************
// Function name	: rtc_tick_isr
// Description	    : TICK中断处理程序
// Return type		: int
// Argument         : void
*****************************************************************************/
void rtc_tick_isr(void)
{
    rI_ISPC=BIT_TICK;					/*清除时钟滴答中断标志				*/
    
	/* 设置数码管段选，某位为0时选择，下面同时选中所有数码管 */
	*((char*) 0x02000006) = 0x00;		
   	*((char*) 0x02000004) = seg7table[led_index%10];  
   	
   	if((led_index%1)==0)	 /* 修改该语句检查复位情况,if((led_index%3)==0) */
   	{
		rWATCNT = 0X1000;				/* 重新设置看门狗时间值 */
   	}
   	
   	led_index++;
}

/*****************************************************************************
// Function name	: rtc_tick_init
// Description	    : 初始化S3C44B0的TICK定时器
// Return type		: void
// Argument         : tick, 设置的TICK频率(时钟滴答的周期为 (1+tick)/128秒)
*****************************************************************************/
void rtc_tick_init( char tick )
{
    rRTCCON  = 0x01;	
    
    rTICINT  = (tick&0x7f)|0x80;		/*TICK 中断使能,周期为(1+tick)/128秒*/
    
    rRTCCON  = 0x00;
}
#endif //__WATCHDOG_EN