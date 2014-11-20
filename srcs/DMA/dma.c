/****************************************************************************/
/*                                                                          */
/* FILE NAME                                      VERSION                   */
/*                                                                          */
/* DMA.C                                            1.0                     */
/*                                                                          */
/* DESCRIPTION                                                              */
/*                                                                          */
/*     JX44B0(S3C44B0X)DMA实验                                              */
/*                                                                          */
/*                                                                          */
/* DATA STRUCTURES                                                          */
/*                                                                          */
/* FUNCTIONS :                                                              */
/*     在JX44B0教学实验箱进行DMA方式内存拷贝的实验                          */
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
/* 学习ARM处理器中DMA方式的处理方法：                                       */
/* DMA方式:内存->内存 DMA方式传输数据                                       */
/* 注意: 学习该实验之前请先学习interrupt中断处理实验                        */
/****************************************************************************/
#include "configure.h"

#ifdef __DMA_EN
/* 包含文件 */
#include "44b.h"
#include "44blib.h"
#include "Rtc.h"

typedef void (*ISR_ROUTINE_ENTRY)(void);

#define Printf Uart_Printf
#define LED_ADDR *(unsigned char*)0x02000000

/* functions */
void Zdma0(int srcAddr,int dstAddr,int length);
void Zdma0Done(void);
void Test_Zdma0(void);
void isr_dma0(void);

/* globals */
volatile int zdma0Done;

__irq  void IsrIRQ(void);

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
	
	rINTPND = 0x00000000;							/* 清除所有未决的中断*/
	rI_ISPC = 0x03FFFFFF; 
	
	for( i = 0; i < 256; i+=4)						/* 清除中断表*/
	{
		*(unsigned int*)(_ISR_STARTADDRESS+i) = 0;
	}
	
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

/*****************************************************************************
// Function name	: Main
// Description	    : DMA测试程序主函数
//                    内存->内存 DMA方式传输数据
//                    传输完毕引发DMA中断
//                    注册中断源:BIT_ZDMA0
// Return type		: int
// Argument         : void
*****************************************************************************/
int main(void)
{
   asm("SWI 0");   //CPSR I位清零
   
   Uart_Select(0);
   Uart_Init(MCLK, 115200);
	
    Uart_Printf("DMA TEST PROGRAM\n");

    rINTCON=0x7;						/* Non-vect,IRQ disable,FIQ disable	*/
	
	init_interrupt_handler((unsigned int)IsrIRQ);
	install_isr_handler(HandleZDMA0, (void*)isr_dma0);

    rINTMOD=0x0;									/*设置所有中断为IRQ模式	*/
	
    rINTMSK=(0x07ffffff&~(BIT_GLOBAL|BIT_ZDMA0));	/* 使能TICK中断			*/

    rINTCON=0x5;									/* 打开IRQ模式的中断	*/
    
    while(1)
      Test_Zdma0();
}

/*****************************************************************************
// Function name	: Test_Zdma0
// Description	    : test zdma 0
// Return type		: void
// Argument         : void
*****************************************************************************/
void Test_Zdma0(void)
{
    unsigned char *src, *dst;

    Uart_Printf("[ZDMA0 MEM2MEM Test]\n");

    dst=(unsigned char *)malloc(0x80000);
    src=(unsigned char *)malloc(0x80000);
	 
	/* 将dst区域设置为非Cacheable区域，关闭Cache */ 
    rNCACHBE1=(((((unsigned)dst+0x100000)>>12) +1 )<<16 )|((unsigned)dst>>12);

    Uart_Printf("dst=%x,src=%x\n",(int)dst,(int)src);

    Zdma0((int)src,(int)dst,0x80000); 

    free(src);
    free(dst);
}

/*****************************************************************************
// Function name	: Zdma0
// Description	    : 以三种不同的方式进行数据拷贝，检查其时间消耗
// Return type		: void
// Argument         : srcAddr，拷贝数据的起始地址
//					  dstAddr，拷贝数据的目的地址
//                    length， 拷贝数据的长度
*****************************************************************************/
void Zdma0(int srcAddr,int dstAddr,int length)
{
    int time;
    
    LED_ADDR |= 0xf<<4;
    /* 将目标地址设置为非Cache区 */
    rNCACHBE1 = (((((unsigned)dstAddr+0x100000)>>12) +1 )<<16 ) |
				((unsigned)dstAddr>>12);
    
    /* 测试单字节方式的拷贝时间 */
    /* 启动定时器，精度128us，用于计时 */
    Timer_Start(3);
    for( time = 0; time < length; time++)
    {
    	*(unsigned char*)(dstAddr+time) = *(unsigned char*)(srcAddr+time);
    }
    
    /* 返回当前定时计数器中的值 */
    time=Timer_Stop();
    LED_ADDR = 0xe<<4;
   
    /* 输出拷贝的时间 */
    Uart_Printf("Copy Bytes :time=%f\n",time*128E-6);
    display_ledNum(time,0);
    
    /* 测试字方式的拷贝时间 */
    Timer_Start(3);
    for( time = 0; time < length; time+=2)
    {
    	*(unsigned short*)(dstAddr+time) = *(unsigned short*)(srcAddr+time);
    }
    time=Timer_Stop();

    Uart_Printf("Copy short words :time=%f\n",time*128E-6);
    LED_ADDR = 0xd<<4;
    display_ledNum(time,0);
    
    /* 测试双字节方式的拷贝时间 */
    Timer_Start(3);
    for( time = 0; time < length; time+=4)
    {
    	*(unsigned long*)(dstAddr+time) = *(unsigned long*)(srcAddr+time);
    }
    time=Timer_Stop();
    LED_ADDR = 0xb<<4;
    Uart_Printf("Copy words :time=%f\n",time*128E-6);
    display_ledNum(time,0);
    
    
    /* 清除传输完毕标志 */
    zdma0Done=0;
    
    /* 设置DMA传输的起止地址和长度 */
    rZDISRC0 = srcAddr | ( 2<<30) | (1<<28);
    rZDIDES0 = dstAddr | ( 2<<30) | (1<<28);
    rZDICNT0 = length  | ( 2<<28) | (1<<26) | (3<<22) | (1<<20);
    
    /* 启动DMA传输 */
    rZDCON0=0x1; 

    Timer_Start(3);
    
    /* 等待传输完成 */
    while(zdma0Done==0);                                                                                                                                                      
    
    /* 返回当前定时计数器中的值 */
    time=Timer_Stop();
    
    /* 输出一次传送的时间 */
    Uart_Printf("ZDMA0 :time=%f\n",time*128E-6);
    LED_ADDR = 0x7<<4;
    display_ledNum(time,0);
    
    
//    rINTMSK=BIT_GLOBAL;
    rINTMSK=(0x07ffffff&~(BIT_GLOBAL|BIT_ZDMA0));	/* 使能TICK中断			*/
}

/*****************************************************************************
// Function name	: isr_dma0
// Description	    : DMA0 中断处理例程
// Return type		: void
// Argument         : void
*****************************************************************************/
void isr_dma0(void)
{
    rI_ISPC=BIT_ZDMA0;					/* 清除DMA中断标志 */
    zdma0Done = 1;						/* 设置传输完毕标志 */
}
#endif //__DMA_EN