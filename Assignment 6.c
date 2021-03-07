#include "MPC5604P_M26V.h"
#include "freemaster.h"
#include "init_base.h"


/********************* Variable **********************/
volatile int i = 0;
int R_adc=0;
int pwm=0;

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
	init_ADC1();
	init_FlexPWM0();
	
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

void init_ADC1(void)    //ADC_1 초기화 -
{
	// ============== ADC initialization =======
	ADC_1.MCR.B.ABORT = 1;         // Hold starting ADC_1 during 'init'
	
	ADC_1.MCR.B.OWREN = 0;        // disable overwritting
	ADC_1.MCR.B.WLSIDE = 0;        // conversion data is written right_aligned, 
	ADC_1.MCR.B.MODE = 0;         // One shot mode
	ADC_1.MCR.B.CTUEN = 0;         // disable CTU triggered conversion
	ADC_1.MCR.B.ADCLKSEL = 0;      // The ADC clock is half of the MC_PLL_CLK frequency
	                                         // Set ADClock 32MHz
                                             // Set ADClock 64MHz when this bit is set	
	ADC_1.MCR.B.ACK0 = 0 ;         // disable auto clock off
	ADC_1.MCR.B.PWDN = 0;          // disable power down mode	
	ADC_1.CTR[0].R = 0x00008208;     // Conversion Timing Register
	ADC_1.NCMR[0].R = 0x00000020;  // Sampling is enabled for CH5
	ADC_1.CDR[5].R = 0x00000000;    // ADC conversion results are stored in data registers Channel 5
	
	ADC_1.MCR.B.ABORT = 0;         // now it starts
	ADC_1.MCR.B.NSTART=1;
	// ================== PWM Pin initialization =======
	SIU.PCR[64].R = 0x2C00;   // E[0] potentiometer ADC_1 AN[5] --> 가변저항을 사용하겠다. 
	
}

void ADC_read(void)   //ADC 읽기
{
	ADC_1.MCR.B.NSTART = 1;  // 변환 시작 ADC라는 Unit에서 MCR레지스터에 NSTART라는 bit가 1이면 변환 시작.


	while(ADC_1.MCR.B.NSTART) asm("nop"); //NSTART가 변환이 끝나면 0으로 바뀐다. assembly어 nop --> no operation. (변환이 될 때 까지 기다려라)
	R_adc = ADC_1.CDR[5].B.CDATA; // CDR이 16bit 인데 CDATA 가 그중에 10bit 로 되어있다. 5번째  CDR을 사용하겠다는 뜻.
}



void init_FlexPWM0(void)	//PWM 초기화
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
	FLEXPWM_0.SUB[3].INIT.R = (unsigned short) -3200;
	FLEXPWM_0.SUB[3].VAL[0].R = 0;
	FLEXPWM_0.SUB[3].VAL[1].R = 3200;
	FLEXPWM_0.SUB[3].VAL[2].R = 0;
	FLEXPWM_0.SUB[3].VAL[3].R = 0;
	FLEXPWM_0.SUB[3].VAL[4].R = 0;
	FLEXPWM_0.SUB[3].VAL[5].R = 0;
	// ============================== Set PWM Option ===
	FLEXPWM_0.SUB[3].CTRL2.R = 0x0000;
	FLEXPWM_0.SUB[3].CTRL2.B.INDEP = 1;		// Pair Operation : 1-Independent, 0-Pair
	FLEXPWM_0.SUB[3].CTRL.R = 0x0000;
	FLEXPWM_0.SUB[3].CTRL.B.HALF = 1;		// Half Cycle Reload : Enable
	FLEXPWM_0.SUB[3].CTRL.B.FULL = 1;		// Full Cycle Reload : Enable
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
	FLEXPWM_0.MCTRL.B.LDOK |= 0xFF;
	FLEXPWM_0.MCTRL.B.RUN |=0xFF;
	// ================== PWM Pin initialization =======
	
	SIU.PCR[51].R = 0x0C00;		// D[3] FlexPWM0_A3 
	SIU.PCR[52].R = 0x0C00;		// D[4] FlexPWM0_B3
	SIU.PCR[53].R = 0x0200;     // D[5] PWM Enable  
	SIU.GPDO[53].B.PDO = 1;
}



void PWM_OUT(void)	// PWM 출력
{
	

		FLEXPWM_0.MCTRL.B.LDOK |= 0xFF;
		FLEXPWM_0.MCTRL.B.RUN |=0xFF;
		
		pwm = ((1600/512) * (R_adc-512)) + 1600;
         

           	FLEXPWM_0.SUB[3].VAL[2].R = -pwm;
            FLEXPWM_0.SUB[3].VAL[3].R = pwm;
    	    FLEXPWM_0.SUB[3].VAL[4].R = pwm;
            FLEXPWM_0.SUB[3].VAL[5].R = -pwm;










		
}


