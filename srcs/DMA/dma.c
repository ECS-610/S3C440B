/****************************************************************************/
/*                                                                          */
/* FILE NAME                                      VERSION                   */
/*                                                                          */
/* DMA.C                                            1.0                     */
/*                                                                          */
/* DESCRIPTION                                                              */
/*                                                                          */
/*     JX44B0(S3C44B0X)DMAʵ��                                              */
/*                                                                          */
/*                                                                          */
/* DATA STRUCTURES                                                          */
/*                                                                          */
/* FUNCTIONS :                                                              */
/*     ��JX44B0��ѧʵ�������DMA��ʽ�ڴ濽����ʵ��                          */
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
/* ѧϰARM��������DMA��ʽ�Ĵ�������                                       */
/* DMA��ʽ:�ڴ�->�ڴ� DMA��ʽ��������                                       */
/* ע��: ѧϰ��ʵ��֮ǰ����ѧϰinterrupt�жϴ���ʵ��                        */
/****************************************************************************/
#include "configure.h"

#ifdef __DMA_EN
/* �����ļ� */
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
// Description	    : ��ʸ����ʽ���жϵĲ����
//					  �жϵ�ַ��λ��0x0c7fff00��ʼ��256�ֽ�
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
// Description	    : ��ʸ����ʽ���ж��������ʼ������
// Return type		: void
// Argument         : irq_handler
//					  �жϴ��������
*****************************************************************************/
void init_interrupt_handler(unsigned int irq_handler) 
{
	int i;
	
	rINTPND = 0x00000000;							/* �������δ�����ж�*/
	rI_ISPC = 0x03FFFFFF; 
	
	for( i = 0; i < 256; i+=4)						/* ����жϱ�*/
	{
		*(unsigned int*)(_ISR_STARTADDRESS+i) = 0;
	}
	
	*(unsigned int*)(HandleIRQ) = irq_handler;		/* ����IRQģʽ������	*/
}

/*****************************************************************************
// Function name	: install_isr_handler
// Description	    : ��ʸ����ʽ���ж������İ�װ
// Return type		: void
// Argument         : irq_no, �жϺ�
//					  irq_routine, �жϴ�������ַ
*****************************************************************************/
void install_isr_handler(int irq_no, void* irq_routine)
{
	*(unsigned int*)(irq_no) = (unsigned int)irq_routine;
}

/*****************************************************************************
// Function name	: Main
// Description	    : DMA���Գ���������
//                    �ڴ�->�ڴ� DMA��ʽ��������
//                    �����������DMA�ж�
//                    ע���ж�Դ:BIT_ZDMA0
// Return type		: int
// Argument         : void
*****************************************************************************/
int main(void)
{
   asm("SWI 0");   //CPSR Iλ����
   
   Uart_Select(0);
   Uart_Init(MCLK, 115200);
	
    Uart_Printf("DMA TEST PROGRAM\n");

    rINTCON=0x7;						/* Non-vect,IRQ disable,FIQ disable	*/
	
	init_interrupt_handler((unsigned int)IsrIRQ);
	install_isr_handler(HandleZDMA0, (void*)isr_dma0);

    rINTMOD=0x0;									/*���������ж�ΪIRQģʽ	*/
	
    rINTMSK=(0x07ffffff&~(BIT_GLOBAL|BIT_ZDMA0));	/* ʹ��TICK�ж�			*/

    rINTCON=0x5;									/* ��IRQģʽ���ж�	*/
    
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
	 
	/* ��dst��������Ϊ��Cacheable���򣬹ر�Cache */ 
    rNCACHBE1=(((((unsigned)dst+0x100000)>>12) +1 )<<16 )|((unsigned)dst>>12);

    Uart_Printf("dst=%x,src=%x\n",(int)dst,(int)src);

    Zdma0((int)src,(int)dst,0x80000); 

    free(src);
    free(dst);
}

/*****************************************************************************
// Function name	: Zdma0
// Description	    : �����ֲ�ͬ�ķ�ʽ�������ݿ����������ʱ������
// Return type		: void
// Argument         : srcAddr���������ݵ���ʼ��ַ
//					  dstAddr���������ݵ�Ŀ�ĵ�ַ
//                    length�� �������ݵĳ���
*****************************************************************************/
void Zdma0(int srcAddr,int dstAddr,int length)
{
    int time;
    
    LED_ADDR |= 0xf<<4;
    /* ��Ŀ���ַ����Ϊ��Cache�� */
    rNCACHBE1 = (((((unsigned)dstAddr+0x100000)>>12) +1 )<<16 ) |
				((unsigned)dstAddr>>12);
    
    /* ���Ե��ֽڷ�ʽ�Ŀ���ʱ�� */
    /* ������ʱ��������128us�����ڼ�ʱ */
    Timer_Start(3);
    for( time = 0; time < length; time++)
    {
    	*(unsigned char*)(dstAddr+time) = *(unsigned char*)(srcAddr+time);
    }
    
    /* ���ص�ǰ��ʱ�������е�ֵ */
    time=Timer_Stop();
    LED_ADDR = 0xe<<4;
   
    /* ���������ʱ�� */
    Uart_Printf("Copy Bytes :time=%f\n",time*128E-6);
    display_ledNum(time,0);
    
    /* �����ַ�ʽ�Ŀ���ʱ�� */
    Timer_Start(3);
    for( time = 0; time < length; time+=2)
    {
    	*(unsigned short*)(dstAddr+time) = *(unsigned short*)(srcAddr+time);
    }
    time=Timer_Stop();

    Uart_Printf("Copy short words :time=%f\n",time*128E-6);
    LED_ADDR = 0xd<<4;
    display_ledNum(time,0);
    
    /* ����˫�ֽڷ�ʽ�Ŀ���ʱ�� */
    Timer_Start(3);
    for( time = 0; time < length; time+=4)
    {
    	*(unsigned long*)(dstAddr+time) = *(unsigned long*)(srcAddr+time);
    }
    time=Timer_Stop();
    LED_ADDR = 0xb<<4;
    Uart_Printf("Copy words :time=%f\n",time*128E-6);
    display_ledNum(time,0);
    
    
    /* ���������ϱ�־ */
    zdma0Done=0;
    
    /* ����DMA�������ֹ��ַ�ͳ��� */
    rZDISRC0 = srcAddr | ( 2<<30) | (1<<28);
    rZDIDES0 = dstAddr | ( 2<<30) | (1<<28);
    rZDICNT0 = length  | ( 2<<28) | (1<<26) | (3<<22) | (1<<20);
    
    /* ����DMA���� */
    rZDCON0=0x1; 

    Timer_Start(3);
    
    /* �ȴ�������� */
    while(zdma0Done==0);                                                                                                                                                      
    
    /* ���ص�ǰ��ʱ�������е�ֵ */
    time=Timer_Stop();
    
    /* ���һ�δ��͵�ʱ�� */
    Uart_Printf("ZDMA0 :time=%f\n",time*128E-6);
    LED_ADDR = 0x7<<4;
    display_ledNum(time,0);
    
    
//    rINTMSK=BIT_GLOBAL;
    rINTMSK=(0x07ffffff&~(BIT_GLOBAL|BIT_ZDMA0));	/* ʹ��TICK�ж�			*/
}

/*****************************************************************************
// Function name	: isr_dma0
// Description	    : DMA0 �жϴ�������
// Return type		: void
// Argument         : void
*****************************************************************************/
void isr_dma0(void)
{
    rI_ISPC=BIT_ZDMA0;					/* ���DMA�жϱ�־ */
    zdma0Done = 1;						/* ���ô�����ϱ�־ */
}
#endif //__DMA_EN