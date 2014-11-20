#ifndef __LCDLIB_H__
#define __LCDLIB_H__

#include "option.h"

extern void lcd_init(void);
extern void lcd_put_pixel(int, int, unsigned char);
extern void lcd_clear_screen(unsigned char );

#define SCR_XSIZE 	(320)  
#define SCR_YSIZE 	(240)

#define LCD_XSIZE 	(320)
#define LCD_YSIZE 	(240)

#define BLACK 0x00
#define WHITE 0xff
#define RED   0xe0
#define GREEN 0x1c
#define BLUE  0x03

extern unsigned char *frameBuffer256;

#endif /*__LCDLIB_H__*/
