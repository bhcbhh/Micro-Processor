#include "MPC5604P_M26V.h"
#include "freemaster.h"
#include "init_sys.h"

int i=0;
//배열 초기화
char hex[16] = {0x03,0x9F,0x25,0x0D,0x99,0x49,0x41,0x1B,0x01,0x19,0x00,0x00,0x00,0x00,0x00,0x00}; 

int input_val;

void out_pin();

void display_seg();

struct LED_tag
{	
	union
	{
		vuint8_t R;
		struct
		{	
			vuint8_t A:1;
			vuint8_t B:1;
			vuint8_t C:1;
			vuint8_t D:1;
			vuint8_t E:1;
			vuint8_t F:1;
			vuint8_t G:1;
			vuint8_t DP:1;	            
		}
		B;
	}
	LEDSET;
};

struct LED_tag LED;


int main(void) 
{
	initModesAndClock();
	disableWatchdog();
	enableIrq();
	FMSTR_Init();
	init_INTC();
	init_Linflex0();   
	INTC.CPR.B.PRI = 0;
	FMSTR_Init();
   
   
   
  	out_pin();
	

	
	input_val=3;
	while(1)
	{
	
		FMSTR_Poll();
		
		i++;
		display_seg();
	}
}
   
   
   
   
   	void out_pin(void)
   	{
   		
   		SIU.PCR[48].R=0x0200;
   		SIU.PCR[49].R=0x0200;
   		SIU.PCR[50].R=0x0200;
   		SIU.PCR[51].R=0x0200;
   		SIU.PCR[52].R=0x0200;
   		SIU.PCR[53].R=0x0200;
   		SIU.PCR[54].R=0x0200;
   		SIU.PCR[55].R=0x0200;
   		
   	}
   	void display_seg(void)
   	{
   		LED.LEDSET.R =hex[(input_val%16)]; //얘는 R이라는 변수에 hex[] 행렬을 사용해서 한것임.
   		
   		//아래 코드 완성
   		SIU.GPDO[48].B.PDO= !(LED.LEDSET.B.A);
   		SIU.GPDO[49].B.PDO= !(LED.LEDSET.B.B);
   		SIU.GPDO[50].B.PDO= !(LED.LEDSET.B.C);
   		SIU.GPDO[51].B.PDO= !(LED.LEDSET.B.D);
   		SIU.GPDO[52].B.PDO= !(LED.LEDSET.B.E);
   		SIU.GPDO[53].B.PDO= !(LED.LEDSET.B.F);
   		SIU.GPDO[54].B.PDO= !(LED.LEDSET.B.G);
   		SIU.GPDO[55].B.PDO= !(LED.LEDSET.B.DP);
   	}

   	