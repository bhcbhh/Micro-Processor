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
	
	SIU.PCR[52].R = 0x0200; //0x�� 16������ ǥ���ߴٴ� ���̰� 0200 --> 
	SIU.PCR[53].R = 0x0200; //R�� �������� ������ ��°�� ǥ���ߴٴ� �ǹ�.
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
	
		SIU.GPDO[52].B.PDO = LED1; //port�� ����� 52,53,54,55 ���� ���� LED�� ���� ����
		SIU.GPDO[53].B.PDO = LED2;  // B�� �� bit ������ ���ڴٴ� �ǹ�
		SIU.GPDO[54].B.PDO = LED3;   //���� ���� 0�� �ָ� ������ 1�� ������ ������.
		SIU.GPDO[55].B.PDO = LED4;
		
	}
	
	void Switch(void)
	{
		SWold[0]=SW[0];  //�ʱ⿡�� sw�� �ִ� ����
		SWold[1]=SW[1];   //swold�� �־��ش�.
		SWold[2]=SW[2];
		SWold[3]=SW[3];
		
		SW[0]=SIU.GPDI[48].B.PDI; //�츮�� ����ġ�� ������ ��ȭ�� 
		SW[1]=SIU.GPDI[49].B.PDI;  //���� SW��� ���� ���� ����
		SW[2]=SIU.GPDI[50].B.PDI;
		SW[3]=SIU.GPDI[51].B.PDI;
		
		if(!SWold[0] && SW[0]) cnt += 1; //���� swold�� sw�� �ٸ��� cnt����
		if(!SWold[1] && SW[1]) cnt += 1; //������Ű�ڴ�.
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
         
        


	

