#ifndef __OPTION_H__
#define __OPTION_H__

// ************* OPTIONS **************

#define MCLK (60000000)
#define BAUD (115200)
#define SDRAM_BASE_ADDRESS 0xc000000
#define DOWNLOAD_ADDRESS SDRAM_BASE_ADDRESS + 0x0200000

#define PLLON 1
#define PLL_M (0x34)
#define PLL_P (0x3)
#define PLL_S (0x1)

#define WRBUFOPT (0x8)   //write_buf_on
#define SYSCFG_0KB (0x0|WRBUFOPT)//Disable cache,ENable write buffer
#define SYSCFG_4KB (0x2|WRBUFOPT)//half cache enable,Enable write buffer
#define SYSCFG_8KB (0x6|WRBUFOPT)//full cache enable,Enable write buffer

#define SDRAM	    2		//In case SDRAM is used
#define BUSWIDTH    (16)
#define CACHECFG    SYSCFG_8KB
#define _RAM_STARTADDRESS 0x0c100000	
#define _ISR_STARTADDRESS 0x0c7fff00     

#endif /*__OPTION_H__*/
