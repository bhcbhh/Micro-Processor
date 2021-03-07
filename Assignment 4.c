#include "MPC5604P_M26V.h"
#include "freemaster.h"
#include "init_sys.h"


vint32_t i = 0;

vuint32_t TIMER_cnt = 0;

unsigned short PWM_X = 10;


void init_TIMER(void);
void TIMER(void);
void init_PWM(void);
void PWM_OUT(void);




int main(void) 
{
	initModesAndClock();
	disableWatchdog();
	enableIrq();
	FMSTR_Init();
	init_INTC();
	init_Linflex0();
	
	init_PWM();
	init_TIMER();
	
		
	
	INTC_InstallINTCInterruptHandler(TIMER,60,6);


   
	SIU.PCR[52].R = 0x0C00;

	



   
   
   
   while(1)
   {
   	FMSTR_Poll();
    FMSTR_Recorder();
   	
	PWM_OUT();
	
	i++;
	
   }
   

  
   
}
   
   	
   	

   	
   	
void init_TIMER(void)
{
	PIT.PITMCR.R = 0x00000001;
	PIT.CH[1].LDVAL.R = 1279999;//20ms 마다 interrupt;
	PIT.CH[1].TCTRL.R = 0x00000003;
}








void TIMER(void)
{

   PWM_X += 64;
   if(PWM_X > 3200)
   {
     PWM_X = 10;
   }
    
    PIT.CH[1].TFLG.B.TIF =1;  
                      
                        

                       



                      //여기 코드 
}


 
 
void init_PWM(void)
{
 FLEXPWM_0.OUTEN.B.PWMA_EN = 0b1000; // 3번 모듈 사용
 FLEXPWM_0.OUTEN.B.PWMB_EN = 0b1000;
 FLEXPWM_0.OUTEN.B.PWMX_EN = 0b0000;
 
 FLEXPWM_0.MASK.R = 0x0000;
 
 FLEXPWM_0.SWCOUT.R = 0x00ff;
 
 FLEXPWM_0.SUB[3].INIT.R = -3200;//
 FLEXPWM_0.SUB[3].VAL[0].R = 0;//mid_value
 FLEXPWM_0.SUB[3].VAL[1].R = 3200;//
 
 FLEXPWM_0.SUB[3].CTRL2.B.DBGEN = 0;
 FLEXPWM_0.SUB[3].CTRL2.B.WAITEN = 0;
 FLEXPWM_0.SUB[3].CTRL2.B.INDEP = 0;  // 이부분을 1로 셋팅 하시면 VAL[4], VAL[5] 에도 듀티가 변하도록 입력하셔야 합니다.
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
 
 **************이 주석 처리 된 부분은 위에 초기화 세팅과 같은 역할을 합니다.********/

 
}
void PWM_OUT(void)
{
	FLEXPWM_0.SUB[3].VAL[2].R = -PWM_X;
    FLEXPWM_0.SUB[3].VAL[3].R =  PWM_X;
  
   
    /**********************************
    FLEXPWM_0.SUB[3].VAL[4].R = -PWM_X;
    FLEXPWM_0.SUB[3].VAL[5].R =  PWM_X;
    **** 3번 모듈 B채널을 사용하는데 이 부분이 주석 처리 된 이유는 대칭출력이기 때문에 따로 출력 설정을 하지 않아도 되서 입니다.
    주석 지우고 CTRL2에 INDEP 1로 셋팅 해보시면 확인 가능 합니다. */
   
   
    FLEXPWM_0.MCTRL.B.LDOK = 0xF;
    FLEXPWM_0.MCTRL.B.RUN  = 0xF;
}



