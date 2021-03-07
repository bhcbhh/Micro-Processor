#include "MPC5604P_M26V.h"
#include "freemaster.h"
#include "init_base.h"


/********************* Variables **********************/
volatile int i = 0;
int R_adc=0;
int duty_servo=0;
/********************* Funtions **********************/
void init_ADC1(void);
void ADC_read(void);
void init_FlexPWM0(void);
void PWM_OUT(void);



int main(void)
{
	initModesAndClock();
	disableWatchdog();
	enableIrq();
	FMSTR_Init();
	init_INTC();
	init_Linflex0();
	init_ADC1();		// ADC 초기화 
	init_FlexPWM0();	// PWM 초기화 
	
	
	/* Loop forever */
	for (;;) 
	{
		FMSTR_Recorder();
		FMSTR_Poll();
		ADC_read();
		PWM_OUT();
		i++;
	}
}




/**************** PWM ****************/

 void PWM_OUT(void)
{
		duty_servo = R_adc*2500/1023;
		
		
	    		
		if(duty_servo > 1140 )
		{
			duty_servo = 1140 ;
		}
		
		if(duty_servo < 790 )
		{
			
			duty_servo = 790;
		}
		
		

		FLEXPWM_0.SUB[3].VAL[4].R = (unsigned short)-duty_servo; // FlexPWM0_B[3] PCR[52]->D[4]
		FLEXPWM_0.SUB[3].VAL[5].R = (unsigned short) duty_servo;

		FLEXPWM_0.MCTRL.B.LDOK |= 0xF;
		FLEXPWM_0.MCTRL.B.RUN |=0xF;
}


#define	PWM_period	2500
void init_FlexPWM0(void)
{
	// ============== General PWM initialization =======
	FLEXPWM_0.OUTEN.R  = 0x0000;
	FLEXPWM_0.MASK.R = 0x0000;
	FLEXPWM_0.SWCOUT.R = 0x00F3;
	FLEXPWM_0.DTSRCSEL.R = 0x0000;
	FLEXPWM_0.FCTRL.R = 0x0000;
	FLEXPWM_0.FFILT.R = 0x0000;
	// =============== PWM Sub[3] initialization =======
	// =========================== Set PWM Frequency ===
	FLEXPWM_0.SUB[3].INIT.R = 0xffff - PWM_period +1;
	FLEXPWM_0.SUB[3].VAL[0].R = 0;
	FLEXPWM_0.SUB[3].VAL[1].R = PWM_period;
	FLEXPWM_0.SUB[3].VAL[2].R = 0;
	FLEXPWM_0.SUB[3].VAL[3].R = 0;
	FLEXPWM_0.SUB[3].VAL[4].R = 0;
	FLEXPWM_0.SUB[3].VAL[5].R = 0;
	// ============================== Set PWM Option ===
	FLEXPWM_0.SUB[3].CTRL2.R = 0x0000;
	FLEXPWM_0.SUB[3].CTRL2.B.INDEP = 1;
	FLEXPWM_0.SUB[3].CTRL.R = 0x0000;
	FLEXPWM_0.SUB[3].CTRL.B.HALF = 1;
	FLEXPWM_0.SUB[3].CTRL.B.FULL = 1;
	FLEXPWM_0.SUB[3].CTRL.B.PRSC = 0b110;   // CLK/64
	FLEXPWM_0.SUB[3].OCTRL.R = 0x0000;
	FLEXPWM_0.SUB[3].OCTRL.B.POLA = 0;		// PWM A Polarity : not Inverted
	FLEXPWM_0.SUB[3].OCTRL.B.POLB = 0;		// PWM B Polarity : not Inverted
	// =========================== Set PWM Interrupt ===
//	FLEXPWM_0.SUB[3].TCTRL.R = 0x0004;
	FLEXPWM_0.SUB[3].TCTRL.R = 0x0000;
	// ======================= Set PWM Fault Disable ===
	FLEXPWM_0.SUB[3].DISMAP.R = 0xFF00;
	// ============================ Set PWM Deadtime ===
	FLEXPWM_0.SUB[3].DTCNT0.R = 0x0000;
	FLEXPWM_0.SUB[3].DTCNT1.R = 0x0000;
	// ============== General PWM initialization re =======
	FLEXPWM_0.OUTEN.B.PWMA_EN = 0b1000; 
	FLEXPWM_0.OUTEN.B.PWMB_EN = 0b1000;
	FLEXPWM_0.OUTEN.B.PWMX_EN = 0b0000;	
	FLEXPWM_0.MCTRL.B.LDOK |= 0xF;	     	// Load config values into buffers 
	FLEXPWM_0.MCTRL.B.RUN |=0xF;
	// ================== PWM Pin initialization =======
	SIU.PCR[51].R = 0x0C00;		// FlexPWM0_A[3]
	SIU.PCR[52].R = 0x0C00;		// FlexPWM0_B[3] (On-Board LED) PCR[52]->D[4]

}



/**************** ADC ****************/
void init_ADC1(void)    //ADC 초기화
{
	// ============== ADC initialization =======
	ADC_1.MCR.B.ABORT=1;
	
	ADC_1.MCR.B.OWREN=0;
	ADC_1.MCR.B.WLSIDE=0;
	ADC_1.MCR.B.MODE=0;
	ADC_1.MCR.B.CTUEN=0;
	ADC_1.MCR.B.ADCLKSEL=0;
	ADC_1.MCR.B.ACK0=0;
	ADC_1.MCR.B.PWDN=0;
	ADC_1.CTR[0].R=0x00008208;
	ADC_1.NCMR[0].R=0x00000020;
	ADC_1.CDR[5].R=0x00000000;
	
	ADC_1.MCR.B.ABORT=0;
	
	// ================== ADC Pin initialization =======
	SIU.PCR[64].R = 0x2C00;   // E[0] potentiometer ADC_1 AN[5]
	
}

void ADC_read(void)   //ADC 읽기
{
	ADC_1.MCR.B.NSTART=1;
	while(ADC_1.MCR.B.NSTART)
	{
		asm("nop");
	}
	
	R_adc=ADC_1.CDR[5].B.CDATA;
}
