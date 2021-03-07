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


vuint32_t i = 0;			    /* Dummy idle counter */
vuint32_t Pit0cnt =0;

char LED1 = 1;
char LED2 = 1;
char LED3 = 1;
char LED4 = 1;


void init_PIT(void);
void PIT0ISR(void);
void display_Pit(void);




void main(void) 
{
    initModesAndClock(); 		
	disableWatchdog();    		
	enableIrq();		   		
	
	init_INTC();				
	init_Linflex0();	
	init_PIT();	
	FMSTR_Init();
	
	INTC.CPR.B.PRI = 0;


	INTC_InstallINTCInterruptHandler(PIT0ISR,59,6);

	

	
	SIU.PCR[52].R = 0x0200;
	SIU.PCR[53].R = 0x0200;
	SIU.PCR[54].R = 0x0200;
	SIU.PCR[55].R = 0x0200;
	
	



	

	/* Loop forever */
	while(1) 
	{   FMSTR_Recorder(); 
    	FMSTR_Poll();
        
        display_Pit();
    
   
		i++;
	}
}



	void init_PIT(void)
	{
	
      PIT.PITMCR.R = 0x00000001;// Timer are stopped in debug mode
      PIT.CH[0].LDVAL.R = 6400000;
      PIT.CH[0].TCTRL.R= 0x00000003;// Timer Interrupt Enable and Start
		
	}
	
	void PIT0ISR(void)
	{
	    Pit0cnt ++;
	    
	    PIT.CH[0].TFLG.B.TIF = 1; // Clear PIT0 flag
	
		
	
	}
	void display_Pit(void)
	{
        
    
    
       	
       	SIU.GPDO[52].B.PDO = LED1;
		SIU.GPDO[53].B.PDO = LED2;
		SIU.GPDO[54].B.PDO = LED3;
		SIU.GPDO[55].B.PDO = LED4;
		
		
		
		if(Pit0cnt & 0x01) LED1 = 1;
		else               LED2 = 0;
			
		
	
		                        
		if(Pit0cnt & 0x02 ) LED2 = 1;
		else                LED1 = 0;
		               
		
 
	    if(Pit0cnt & 0x04) LED3 = 1;
	    else               LED4 = 0;
	       
	
	    if(Pit0cnt & 0x08)  LED4 = 1;
	    else                LED3 = 0;
	
	
	
	}
	

	



