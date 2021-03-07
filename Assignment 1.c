/******************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2004-2011 Freescale Semiconductor
* ALL RIGHTS RESERVED.
*
*******************************************************************************
*
* FILE NAME: main.c   
*
* DESCRIPTION: Advanced sample application demonstrating the functionality of 
*              FreeMASTER Serial Communication Driver. 
*
*******************************************************************************/

#include "MPC5604P_M26V.h"
#include "freemaster.h"
#include "init_sys.h"



char LED1 = 1, LED2 = 1, LED3 = 1, LED4 = 1;
char cnt=0;
char SW[4]={0,0,0,0};
char SWold[4]={0,0,0,0};
int LED[4];
char cnt_last=0;


vuint32_t i = 0;			    /* Dummy idle counter */
void LED_Ctr(void);
void Switch(void);
void main(void) 
{
    initModesAndClock(); 		
	disableWatchdog();    		
	enableIrq();		   		
	
	init_INTC();				
	init_Linflex0();

	INTC.CPR.B.PRI = 0;
	
	FMSTR_Init();
	
	
	SIU.PCR[48].R = 0x0100;   
	SIU.PCR[49].R = 0x0100;
	SIU.PCR[50].R = 0x0100;
	SIU.PCR[51].R = 0x0100;
	
	SIU.PCR[52].R = 0x0200; //0x는 16진수로 표현했다는 뜻이고 0200 --> 
	SIU.PCR[53].R = 0x0200; //R은 레지스터 단위로 통째로 표현했다는 의미.
	SIU.PCR[54].R = 0x0200;
	SIU.PCR[55].R = 0x0200;
	
	
	LED1 = 1, LED2 = 1, LED3 = 1, LED4 = 1;
	

	/* Loop forever */
	while(1) 
	{
    	FMSTR_Poll();
    	LED_Ctr();
    	Switch();
        
    /*-----------------------------------*/
		i++;
	}
}



	void LED_Ctr(void)
	{
	
		SIU.GPDO[52].B.PDO = LED1; //port가 연결된 52,53,54,55 한테 각각 LED의 값을 대입
		SIU.GPDO[53].B.PDO = LED2;  // B는 한 bit 단위로 쓰겠다는 의미
		SIU.GPDO[54].B.PDO = LED3;   //변수 값에 0을 주면 켜지고 1을 넣으면 꺼진다.
		SIU.GPDO[55].B.PDO = LED4;
		
	}
	
	void Switch(void)
	{
		SWold[0]=SW[0];  //초기에는 sw에 있는 값을
		SWold[1]=SW[1];   //swold에 넣어준다.
		SWold[2]=SW[2];
		SWold[3]=SW[3];
		
		SW[0]=SIU.GPDI[48].B.PDI; //우리가 스위치를 누르면 변화된 
		SW[1]=SIU.GPDI[49].B.PDI;  //값이 SW라는 값에 들어가는 것임
		SW[2]=SIU.GPDI[50].B.PDI;
		SW[3]=SIU.GPDI[51].B.PDI;
		
		if(!SWold[0] && SW[0]) cnt += 1; //만약 swold와 sw가 다르며 cnt값을
		if(!SWold[1] && SW[1]) cnt += 1; //증가시키겠다.
		if(!SWold[2] && SW[2]) cnt += 1;
		if(!SWold[3] && SW[3]) cnt += 1;
     
    
    
    	if(cnt >15)
    	{
    		cnt =0;
    	}
        LED1 = !(cnt & 0x01);
        LED2 = !(cnt & 0x02);
        LED3 = !(cnt & 0x04);
        LED4 = !(cnt & 0x08);
     
     
    
         
	}
         
        


	

