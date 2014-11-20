#ifndef __RTC_H__
#define __RTC_H__

extern int Test_Rtc_Alarm(void);
extern void Test_Rtc_Tick(void);
extern void Display_Rtc(void);

void Rtc_Init(void);

#define TESTYEAR 	(0x99)
#define TESTMONTH 	(0x12)
#define TESTDAY		(0x31)
#define TESTDATE    (0x06)  // SUN:1 MON:2 TUE:3 WED:4 THU:5 FRI:6 SAT:7   
#define TESTHOUR	(0x23)
#define TESTMIN		(0x59)
#define TESTSEC		(0x58)

#define TESTYEAR2	(0x00)
#define TESTMONTH2 	(0x01)
#define TESTDAY2	(0x01)
#define TESTDATE2	(0x07)  // SUN:1 MON:2 TUE:3 WED:4 THU:5 FRI:6 SAT:7   
#define TESTHOUR2	(0x00)
#define TESTMIN2	(0x00)
#define TESTSEC2	(0x00)

#endif //__RTC_H__


