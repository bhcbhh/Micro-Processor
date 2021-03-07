#include "MPC5604P_M26V.h"
#include "freemaster.h"
#include "init_base.h"


/****************** Variable ******************/
volatile int i = 0;
unsigned int distance=0,echo=0,eTimerCnt=0,Pit0cnt=0;


/****************** Function ******************/
void init_eTimer(void);
void eTimer_0(void);
void init_PIT(void);
void PIT0ISR(void);

int main(void)
{
	initModesAndClock();
	disableWatchdog();
	enableIrq();
	FMSTR_Init();
	init_INTC();
	init_Linflex0();
	init_PIT();
	init_eTimer();
	INTC_InstallINTCInterruptHandler(eTimer_0,157,14); // CAPT2 이벤트 마다 함수 호출
	INTC_InstallINTCInterruptHandler(PIT0ISR,59,6);      // 10us마다 함수 호출
	
	/* Loop forever */
	for (;;) 
	{
		FMSTR_Recorder();
		FMSTR_Poll();
		i++;
	}
}








void init_PIT(void)
{
	PIT.PITMCR.R = 0x00000001;             // PIT Enabled and Config stop in debug mode
	PIT.CH[0].LDVAL.R = 0x00000280; 	// 10 us
	PIT.CH[0].TCTRL.R = 0x3;               	// Timer interrupt enabled & start
	
	SIU.PCR[55].R = 0x0200; //D7 port as output port 
}



void PIT0ISR(void)
{
	PIT.CH[0].TFLG.B.TIF = 1;              // Clear PIT0 Flag
	Pit0cnt++;
	
	if(Pit0cnt==10000)
	{
		SIU.GPDO[55].B.PDO = 1;
		Pit0cnt=0;
	}
	else
	{
		SIU.GPDO[55].B.PDO = 0;
	}

}








void eTimer_0(void)
{

	ETIMER_0.CHANNEL[0].STS.B.ICF2 = 1;		// Clear ETC0 Flag
	
	echo = ETIMER_0.CHANNEL[0].CAPT2.R;

	distance =   0.034 * echo;	// centimeter
}


void init_eTimer(void)
{
// ============== eTimer initialization =======
    ETIMER_0.ENBL.R = 0b000000;
    
    ETIMER_0.CHANNEL[0].CTRL.B.CNTMODE = 0b001;    // Count rising edges of primary source
    ETIMER_0.CHANNEL[0].CTRL.B.PRISRC  = 0b11111;    // From Table 'eTimer Count source values': 0b11111 -> clk/128
    ETIMER_0.CHANNEL[0].CTRL.B.SECSRC  = 0b00000;   // From Table 'eTimer Count source values': 0b00000 -> ETC[0] channel 
    
    ETIMER_0.CHANNEL[0].CCCTRL.B.CPT2MODE = 0b01;  // Capture falling edges
    ETIMER_0.CHANNEL[0].CCCTRL.B.CPT1MODE = 0b10;  // Capture rising edges
    ETIMER_0.CHANNEL[0].CCCTRL.B.ARM = 1;          // capture operation as specified by the CPT1MODE and CPT2MODE bits is enabled
//  ETIMER_0.CHANNEL[0].CCCTRL.B.ARM = 0;          // capture operation is disabled

    ETIMER_0.CHANNEL[0].CTRL3.B.ROC   = 0b01;      // Reload the counter on a capture 1 event
    ETIMER_0.CHANNEL[0].INTDMA.B.ICF2IE = 1;       // Input Capture 2 flag interrupt Enable
	
    ETIMER_0.ENBL.R = 0b000001;                    // ETC[0] Enabled
    
// ============== eTimer PIN initialization =======    
    SIU.PCR[0].R = 0x0500;			   // eTimer_0 Func_ETC[0] PIN A[0]
}











