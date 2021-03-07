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
char D[4] = {0,0,0,0};

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
	
	
	SIU.PCR[56].R = 0x0100;
	SIU.PCR[57].R = 0x0100;
	SIU.PCR[58].R = 0x0100;
	SIU.PCR[59].R = 0x0100;
	
	SIU.PCR[52].R = 0x0200;
	SIU.PCR[53].R = 0x0200;
	SIU.PCR[54].R = 0x0200;
	SIU.PCR[55].R = 0x0200;
	
	
	LED1 = 0, LED2 = 0, LED3 = 0, LED4 = 0;
	

	/* Loop forever */
	while(1) 
	{
    	FMSTR_Poll();
    	
    	Switch();
    
    /*-----------------------------------*/
		i++;
	}
}



	
	void Switch(void)
	{
	
		
		D[0]=SIU.GPDI[56].B.PDI;
		D[1]=SIU.GPDI[57].B.PDI;
		D[2]=SIU.GPDI[58].B.PDI;
		D[3]=SIU.GPDI[59].B.PDI;
		
			
	
	}
	



