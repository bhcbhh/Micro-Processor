#include "MPC5604P_M26V.h"
#include "freemaster.h"
#include "init_sys.h"

/********************* Variable **********************/
int i=0, R_adc=0, V_adc=0;
vuint32_t TIMER_cnt = 0;
unsigned short PWM_X = 10;

/********************* Funtion **********************/
void init_PIN(void);
void init_ADC0(void);
void init_ADC1(void);
void ADCRead_0(void);
void ADCRead_1(void);
void init_PWM(void);
void PWM_OUT(void);






int main(void) 
{
	initModesAndClock();
	disableWatchdog();
	enableIrq();
	init_INTC();
	init_Linflex0();
	
	
	FMSTR_Init();
	
	init_PIN();
//	init_ADC0();
	init_ADC1();
    init_PWM();

   	SIU.PCR[52].R = 0x0C00;


   
	while(1)
	{
		FMSTR_Poll();
	
		ADCRead_1();
		// ADCRead_0();	
	    PWM_OUT();
		i++;
	}
	
}







void init_PIN(void)
{
	SIU.PCR[64].R = 0x2C00;    // E[0] (Potentiometer)
   
}


void init_ADC0(void)
{
	ADC_0.MCR.B.ABORT = 1;         // Hold starting ADC_1 during 'init'
	
	ADC_0.MCR.B.OWREN = 0;        // disable overwritting, AD �������- ����� ����� �����;� ���� ����� �����ϴµ�, enable�̸� ������ �ְ� diable�̸� �����;� ��������
	ADC_0.MCR.B.WLSIDE = 0;        // conversion data is written right_aligned, 
	ADC_0.MCR.B.MODE = 0;         // One shot mode, �ѹ��� ���������ϰ� ������,,, �� ���ͷ��̼Ǹ��� �������϶�� Ʈ���Ÿ� ����������
	ADC_0.MCR.B.CTUEN = 0;         // disable CTU triggered conversion, PWM�̳� etimer ��� �ֺ���ɿ��� �������� �϶�� ��ȣ�� �ִ°�쿡 ���, �̶� CTU �̿�
	ADC_0.MCR.B.ADCLKSEL = 0;      // The ADC clock is half of the MC_PLL_CLK frequency ( Set ADClock 32MHz  )
	//ADC_0.MCR.B.ADCLKSEL = 1;	    // Set ADClock 64MHz when this bit is set                                                      	
	ADC_0.MCR.B.ACK0 = 0 ;          // disable auto clock off, ADC�� ������� �������� ������ �Ƴ��� ���� Ŭ���� ���� �ʴ´�
	ADC_0.MCR.B.PWDN = 0;          // disable power down mode, CPU�� �����ϴ� ��尡 ���������� �ִµ� �� �߿� �����¸�尡 ����, �� �����¸�忡���� ADC����� ������� �ʰڴ�
	
	ADC_0.CTR[0].R = 0x00008208;     // Conversion Timing Register
	ADC_0.NCMR[0].R = 0x00000002;  // Sampling is enabled for CH1
	ADC_0.CDR[1].R = 0x00000000;    // ADC conversion results are stored in data registers Channel 1
	
	ADC_0.MCR.B.ABORT = 0;         // now it starts
}

//


void init_ADC1(void)
{
	ADC_1.MCR.B.ABORT = 1;                   
	
	ADC_1.MCR.B.OWREN = 0;         
	ADC_1.MCR.B.WLSIDE = 0;        
	ADC_1.MCR.B.MODE = 0;          
	ADC_1.MCR.B.CTUEN = 0;         
	ADC_1.MCR.B.ADCLKSEL = 0;      
	                               
	ADC_1.MCR.B.ACK0 = 0 ;         
	ADC_1.MCR.B.PWDN = 0;          
	
	ADC_1.CTR[0].R = 0x00008208;   
	ADC_1.NCMR[0].R = 0x00000020;  // Sampling is enabled for CH5
	ADC_1.CDR[5].R = 0x00000000;    // ADC conversion results are stored in data registers CH 5
	
	ADC_1.MCR.B.ABORT = 0;
}

void ADCRead_1(void)
{
	ADC_1.MCR.B.NSTART = 1; // Module 1 Conversion Start, ������忡���� Ʈ���Ž�ȣ�� ������� �������� ��, �������̳����� 0���� �ڵ����� �ٲ� �� ��Ʈ��


	while(ADC_1.MCR.B.NSTART) asm("nop");  // �������� ������ CDATA���� Ư�������� ������
	R_adc = ADC_1.CDR[5].B.CDATA;
	
	
	
	
    PWM_X  = R_adc*3;
	
	
	
	
	
	/*
		
	if(R_adc >= 0 && R_adc < 200)
	{
       PWM_X = 10;
	}
	if(R_adc >= 200 && R_adc < 600)
	{
	   PWM_X = 1000;  
	
	}
	if(R_adc >= 600 && R_adc < 900)
	{
	  PWM_X = 2000;
	}
    else
	{
      PWM_X = 3000;
	}

	*/
	
	
	
	
	
	
	
	
	
	
}

 void ADCRead_0(void)
{
	ADC_0.MCR.B.NSTART = 1; // Module 0 Conversion Start
	asm("nop");
	
	while(ADC_0.MCR.B.NSTART) asm("nop");
	V_adc = ADC_0.CDR[1].B.CDATA;
	V_adc = V_adc*5/1023;	
}


 
 
void init_PWM(void)
{
 FLEXPWM_0.OUTEN.B.PWMA_EN = 0b1000; // 3�� ��� ���
 FLEXPWM_0.OUTEN.B.PWMB_EN = 0b1000;
 FLEXPWM_0.OUTEN.B.PWMX_EN = 0b0000;
 
 FLEXPWM_0.MASK.R = 0x0000;
 
 FLEXPWM_0.SWCOUT.R = 0x00ff;
 
 FLEXPWM_0.SUB[3].INIT.R = -3200;//
 FLEXPWM_0.SUB[3].VAL[0].R = 0;//mid_value
 FLEXPWM_0.SUB[3].VAL[1].R = 3200;//
 
 FLEXPWM_0.SUB[3].CTRL2.B.DBGEN = 0;
 FLEXPWM_0.SUB[3].CTRL2.B.WAITEN = 0;
 FLEXPWM_0.SUB[3].CTRL2.B.INDEP = 0;  // �̺κ��� 1�� ���� �Ͻø� VAL[4], VAL[5] ���� ��Ƽ�� ���ϵ��� �Է��ϼž� �մϴ�.
 FLEXPWM_0.SUB[3].CTRL2.B.PWMA_INIT = 0;
 FLEXPWM_0.SUB[3].CTRL2.B.PWMB_INIT = 0;
 FLEXPWM_0.SUB[3].CTRL2.B.PWMX_INIT = 0;
 FLEXPWM_0.SUB[3].CTRL2.B.INIT_SEL = 0b00;
 FLEXPWM_0.SUB[3].CTRL2.B.FRCEN = 0;
 FLEXPWM_0.SUB[3].CTRL2.B.FORCE = 0;
 FLEXPWM_0.SUB[3].CTRL2.B.FORCE_SEL = 0b000;
 FLEXPWM_0.SUB[3].CTRL2.B.RELOAD_SEL = 0;
 FLEXPWM_0.SUB[3].CTRL2.B.CLK_SEL = 0b00;
 
 FLEXPWM_0.SUB[3].CTRL.B.LDFQ = 0b0000;
 FLEXPWM_0.SUB[3].CTRL.B.HALF = 1;
 FLEXPWM_0.SUB[3].CTRL.B.FULL = 1;
 FLEXPWM_0.SUB[3].CTRL.B.DT = 0b00;
 FLEXPWM_0.SUB[3].CTRL.B.PRSC = 0b000;//disable
 FLEXPWM_0.SUB[3].CTRL.B.DBLEN = 0;
 
 FLEXPWM_0.SUB[3].OCTRL.B.PWMA_IN = 0;
 FLEXPWM_0.SUB[3].OCTRL.B.PWMB_IN = 0;
 FLEXPWM_0.SUB[3].OCTRL.B.PWMX_IN = 0;
 FLEXPWM_0.SUB[3].OCTRL.B.POLA = 0;
 FLEXPWM_0.SUB[3].OCTRL.B.POLB = 0;
 FLEXPWM_0.SUB[3].OCTRL.B.POLX = 0;
 FLEXPWM_0.SUB[3].OCTRL.B.PWMAFS = 0b00;
 FLEXPWM_0.SUB[3].OCTRL.B.PWMBFS = 0b00;
 FLEXPWM_0.SUB[3].OCTRL.B.PWMXFS = 0b00;
 
 FLEXPWM_0.SUB[3].DISMAP.B.DISX = 0b1111;//disable
 FLEXPWM_0.SUB[3].DISMAP.B.DISB = 0b0000;//enable
 FLEXPWM_0.SUB[3].DISMAP.B.DISA = 0b0000;
 
 FLEXPWM_0.SUB[3].DTCNT0.R = 0;
 FLEXPWM_0.SUB[3].DTCNT1.R = 0;
 
 
 /*********SUB[3]*******************************************************************
 FLEXPWM_0.SUB[3].INIT.R = 0xFFFF - 0x0C80 + 1;
 FLEXPWM_0.SUB[3].VAL[0].R = 0;
 FLEXPWM_0.SUB[3].VAL[1].R = 0x0C80;
 
 FLEXPWM_0.SUB[3].CTRL2.R = 0x0000;
 FLEXPWM_0.SUB[3].CTRL.R = 0x0C00;
 FLEXPWM_0.SUB[3].OCTRL.R = 0x0000;
 FLEXPWM_0.SUB[3].DISMAP.R = 0xFF00;
 
 FLEXPWM_0.SUB[3].DTCNT0.R = 0;
 FLEXPWM_0.SUB[3].DTCNT1.R = 0;
 
 **************�� �ּ� ó�� �� �κ��� ���� �ʱ�ȭ ���ð� ���� ������ �մϴ�.********/

 
}

void PWM_OUT(void)
{
	FLEXPWM_0.SUB[3].VAL[2].R = -PWM_X;
    FLEXPWM_0.SUB[3].VAL[3].R =  PWM_X;
  
   
    /**********************************
    FLEXPWM_0.SUB[3].VAL[4].R = -PWM_X;
    FLEXPWM_0.SUB[3].VAL[5].R =  PWM_X;
    **** 3�� ��� Bä���� ����ϴµ� �� �κ��� �ּ� ó�� �� ������ ��Ī����̱� ������ ���� ��� ������ ���� �ʾƵ� �Ǽ� �Դϴ�.
    �ּ� ����� CTRL2�� INDEP 1�� ���� �غ��ø� Ȯ�� ���� �մϴ�. */
   
   
    FLEXPWM_0.MCTRL.B.LDOK = 0xF;
    FLEXPWM_0.MCTRL.B.RUN  = 0xF;
}



