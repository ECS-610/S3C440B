/****************************************************************************/
/*                                                                          */
/* FILE NAME                                      VERSION                   */
/*                                                                          */
/* KEY.C                                            1.0                     */
/*                                                                          */
/* DESCRIPTION                                                              */
/*                                                                          */
/*     JX44B0(S3C44B0X)键盘实验                                             */
/*                                                                          */
/*                                                                          */
/* DATA STRUCTURES                                                          */
/*                                                                          */
/* FUNCTIONS :                                                              */
/*     在JX44B0教学实验箱进行键盘扫描的实验                                 */
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
/* 学习JX44B0中键盘的处理方法：                                             */
/****************************************************************************/


#include "configure.h"

#ifdef __KEY_EN
/* functions */
void delay(int count);
void display_num(unsigned char number);

extern unsigned char seg7table[];


/*****************************************************************************
// Function name	: key_get_char
// Description	    : 根据键盘行列键值查询键
// Return type		: char
//     键盘定义
//     1         2         3        UP(C)
//     4         5         6        DOWN(D)
//     7         8         9        Cancel(E)
//     0         .(A)      Del(B)   Enter(F)
// Argument         : int row：行号
// Argument         : int col：列号
*****************************************************************************/
char key_get_char(int row, int col)
{
	char key;
	
	switch( row )
	{
	case 0:
		if((col & 0x01) == 0) key = '9';              //<-----修改key的赋值使按下键盘显示对应的正确数值
		else if((col & 0x02) == 0) key = '1'; 
		else if((col & 0x04) == 0) key = '0'; 
		else if((col & 0x08) == 0) key = '5'; 
		break;
	case 1:
		if((col & 0x01) == 0) key = '7';                //<-----修改key的赋值使按下键盘显示对应的正确数值 
		else if((col & 0x02) == 0) key = '2'; 
		else if((col & 0x04) == 0) key = 'A';
		else if((col & 0x08) == 0) key = '6';
		break;
	case 2:
		if((col & 0x01) == 0) key = '8';                //<-----修改key的赋值使按下键盘显示对应的正确数值
		else if((col & 0x02) == 0) key = '3'; 
		else if((col & 0x04) == 0) key = 'B'; 
		else if((col & 0x08) == 0) key = '4'; 
		break;
	case 3:
		if((col & 0x01) == 0) key = 'E'; 
		else if((col & 0x02) == 0) key = 'C'; 
		else if((col & 0x04) == 0) key = 'F'; 
		else if((col & 0x08) == 0) key = 'D'; 
		break;
	default:
		break;
	}
	return key;
}

/*****************************************************************************
// Function name	: display_num
// Description	    : 在LED上显示按键
// Return type		: void
// Argument         : unsigned char number
*****************************************************************************/
void display_num(unsigned char number)
{
	*((unsigned char*) 0x02000006) = 0x00;
	switch( number )
	{
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
			number = (number - 'A') + 0X0A;	
			break;
		default:
			number = number - '0';
			break;
	}

	*((unsigned char*)0x02000004) = seg7table[number];      
}

/*****************************************************************************
// Function name	: delay
// Description	    : 延时子程序
// Return type		: void
// Argument         : count,延时的数值
*****************************************************************************/
void delay( int count )
{
	for( ; count>0; count--);
}

/*****************************************************************************
// Function name	: delay
// Description	    : 键盘测试程序
//                    循环扫描键盘，将扫描到的键盘键值显示在数码管上
// Return type		: void
// Argument         : void
*****************************************************************************/
void main(void)
{
	int row;
	unsigned char 	ascii_key, input_key, key_mask = 0x0F;
	unsigned char*	keyboard_port_scan = (unsigned char*)0x02000000;
	unsigned char*	keyboard_port_value = (unsigned char*)0x02000002;
	
	while(1)
	{
		for( row = 0; row < 4; row++)
		{
			*keyboard_port_scan = ~(0x00000001<<row);        /*将row列置低电平	*/

			delay(10000);								/*延时				*/
			input_key = (*keyboard_port_value) & key_mask;	/*并获取第一次扫描值*/
			
			if(input_key == key_mask)	continue;		/* 没有按键			*/
					
			/* 延时，再次获取扫描值，如果两次的值不等，则认为是一个干扰		*/
			delay(10000);
			if (((*keyboard_port_value) & key_mask) != input_key) continue;
			
			ascii_key = key_get_char(row, input_key);	/* 查表				*/
			
			display_num( ascii_key);					/* 显示结果			*/
		}
	}
}
#endif //__KEY_EN
