/****************************************************************************/
/*                                                                          */
/* FILE NAME                                      VERSION                   */
/*                                                                          */
/* WATCHDOG.C                                            1.0                */
/*                                                                          */
/* DESCRIPTION                                                              */
/*                                                                          */
/*     JX44B0(S3C44B0X)���Ź�ʵ��                                           */
/*                                                                          */
/*                                                                          */
/* DATA STRUCTURES                                                          */
/*                                                                          */
/* FUNCTIONS :                                                              */
/*     ��JX44B0��ѧʵ������п��Ź���ʱ����ʵ��                             */
/*     ��������ʱ�����������ʾ��0��1��2......F                             */
/*     �����쳣ʱ��û������ι��ʱ����������������ʾ                         */
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
/* ѧϰS3C44B0X�������еĿ��Ź���������                                           */
/****************************************************************************/
#include "configure.h"

#ifdef __WATCHDOG_EN
/* �����ļ� */
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

/* �������ʾ�����ο�ledʵ�� */
extern unsigned char seg7table[];



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
// Description	    : ��ʸ����ʽ���ж��������ʼ������
// Return type		: void
// Argument         : irq_handler
//					  �жϴ��������
*****************************************************************************/
void init_interrupt_handler(unsigned int irq_handler) 
{
	int i;
	
	rINTPND = 0x00000000;							/* �������δ�����ж�	*/
	rI_ISPC = 0x03FFFFFF; 
	
	for( i = 0; i < 256; i+=4)						/* ����жϱ�			*/
		*(unsigned int*)(_ISR_STARTADDRESS+i) = 0;

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

/****************************************************************************/
/* ����˵��: JX44B0���Ź�����                                               */
/* ��������: ������7�����������ʾ0123456789ABCDEF                          */                                            
/*     ��������ʱ�����������ʾ��0��1��2......F                             */
/*     �����쳣ʱ��û������ι��ʱ����������������ʾ                         */
/* ���ش���: ��                                                             */
/* ����˵��: ��                                                             */
/****************************************************************************/
void main(void)
{    
   asm("SWI 0");   //CPSR Iλ����
   
   rWATCON = 0x0000;					/* �رտ��Ź� */
	
   rINTCON=0x7;						/* Non-vect,IRQ disable,FIQ disable */
    
   rtc_tick_init(127);
      
	init_interrupt_handler((unsigned int)IsrIRQ);
	install_isr_handler(HandleTICK, (void*)rtc_tick_isr);

    rINTMOD=0x0;									/*���������ж�ΪIRQģʽ*/
	
    rINTMSK=(0x07ffffff&~(BIT_GLOBAL|BIT_TICK));	/*ʹ��TICK�ж�*/

    rINTCON=0x5;						/*Non-vect,IRQ enable,FIQ disable   */
    
	rWATCNT = 0X1000;					/* ���ÿ��Ź���ʼֵ */

	/* �򿪿��Ź� */	
	rWATCON = WDT_ENABLE | WDT_RST_ENABLE | WDT_CLK_SEL | WDT_PRE_SCALER;
	
	/* ����ܴ�0��F���ν��ַ���ʾ���� */
	while(1);
}

/*****************************************************************************
// Function name	: rtc_tick_isr
// Description	    : TICK�жϴ������
// Return type		: int
// Argument         : void
*****************************************************************************/
void rtc_tick_isr(void)
{
    rI_ISPC=BIT_TICK;					/*���ʱ�ӵδ��жϱ�־				*/
    
	/* ��������ܶ�ѡ��ĳλΪ0ʱѡ������ͬʱѡ����������� */
	*((char*) 0x02000006) = 0x00;		
   	*((char*) 0x02000004) = seg7table[led_index%10];  
   	
   	if((led_index%1)==0)	 /* �޸ĸ�����鸴λ���,if((led_index%3)==0) */
   	{
		rWATCNT = 0X1000;				/* �������ÿ��Ź�ʱ��ֵ */
   	}
   	
   	led_index++;
}

/*****************************************************************************
// Function name	: rtc_tick_init
// Description	    : ��ʼ��S3C44B0��TICK��ʱ��
// Return type		: void
// Argument         : tick, ���õ�TICKƵ��(ʱ�ӵδ������Ϊ (1+tick)/128��)
*****************************************************************************/
void rtc_tick_init( char tick )
{
    rRTCCON  = 0x01;	
    
    rTICINT  = (tick&0x7f)|0x80;		/*TICK �ж�ʹ��,����Ϊ(1+tick)/128��*/
    
    rRTCCON  = 0x00;
}
#endif //__WATCHDOG_EN