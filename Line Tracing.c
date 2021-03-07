// version 별 작업일지 읽을
#include "MPC5604P_M26V.h"
#include "freemaster.h"
#include "init_base.h"


/****************** Variable ******************/
volatile int i = 0, test=0;
int duty_traction=750, duty_servo=0, R_adc=0;
unsigned int echo=0 ,Pit0cnt=0;
int testduty=0;
int traction;
int y = 707;										// 
int x = 707;										// basic Velocity
int d1 = 58;
int d2 = 58;										// distance
int disturb=0;
int t1 = 2;										//Left tiem delay
int t2 = 2;										//Right time delay
int tenmillisec = 0;								// timer 0.01sec


char SW[4] = {1, 1, 1, 1};
char SWold[4] = {1, 1, 1, 1};
int cnt = 0;


unsigned int distance=0;
float old_distance = 0;
float pre_distance = 0;

float now_sensor1;									//sensor Average Filter
float now_sensor2;								
float now_sensor3;
float now_sensor4;
float old_sensor1 = 0;
float old_sensor2 = 0;
float old_sensor3 = 0;
float old_sensor4 = 0;						

float now_survo = 750;
float old_survo = 750;

/*평균 필터 GAIN*/
float k = 0.8;
float k_survo = 0.8;
float k_echo = 0.8;




/* 적외선 센서 */
struct Red_set
{
	union
	{
 		vuint8_t R;
 		struct 
 		{ 
 	 		vuint8_t :4;
  			vuint8_t sensor1:1;
  			vuint8_t sensor2:1;
  			vuint8_t sensor3:1;
  			vuint8_t sensor4:1;
  
 		}s;
	}sen;

};

struct Red_set RED;

/****************** Function ******************/
void init_PIN(void);
void init_FlexPWM0(void);
void ISR_FlexPWM0(void);
void init_eTimer(void);
void ISR_ETC0(void);
void init_PIT(void);
void ISR_PIT0(void);
void infrared(void);
void drive(void);
void Switch(void);

int main(void)
{
	initModesAndClock();
	disableWatchdog();
	enableIrq();
	FMSTR_Init();
	init_PIN();
	init_INTC();
	init_Linflex0();
	init_eTimer();
	init_FlexPWM0();
	init_PIT();
;
 //ccw  : 11.3s
 // cw  : 11.5s
 

 

	INTC_InstallINTCInterruptHandler(ISR_FlexPWM0,183,6);
	INTC_InstallINTCInterruptHandler(ISR_ETC0,157,7);
	INTC_InstallINTCInterruptHandler(ISR_PIT0,59,7);
 
	for (;;) 
 	{	/*	FreeMaster 통신		*/
  		FMSTR_Recorder();
  		FMSTR_Poll();
  		
  		/*	구동 함수 				*/
  		Switch();
  		infrared();
  		drive();
  		i++;
 	}





}


void drive(void)
{
	y = x+cnt;
	old_survo = now_survo;	
							//duty_traction 715

		switch(RED.sen.R)		
		{
			
			case 0b1110  : 
				  now_survo    = 500 ;     // right 
				  old_survo    = 500;
				  duty_traction = y + 12;
				  break;
				  
			case 0b1000  :
				 now_survo = 600;
				 old_survo = 600; 
				 duty_traction = y + 10;				  
				  break;

			
		
			case 0b1100   : 
				  now_survo    = 650;
				  duty_traction = y +5;
				  break;

			case 0b1101   : 
				  now_survo    = 700;
				  duty_traction = y +2;
				  break;

			case 0b1001  : 
				  now_survo    = 750 ;
				  old_survo = 750  ;    // mid 
				  duty_traction = y ;
				  break;

			case 0b1011  : 
				  now_survo    = 800;
				  duty_traction = y +2; //1;
				  break;

			case 0b0011 : 
				  now_survo    = 850;
				  duty_traction = y + 5;//3;
				  break;

			case 0b0001  :
				 now_survo = 900;
				 old_survo = 900; 
		         duty_traction = y + 10; //6				  
				  break;
				  
				
			case 0b0111  : 
				  now_survo    = 1000;
				  old_survo = 1000; 
				  duty_traction = y + 12 ;//9 // left
				  break;
	
			//          duty_traction back foward 790~ 850++                               
			case 0b1111 :
					duty_traction = y+20; //15
		 			break;
		
			
			/*default : duty_servo    = 750;
				  duty_traction = 760 ;
				  break;		  */
		}
	
			duty_servo = (k_survo*old_survo + (1-k_survo)*now_survo )/1;
		
	/*
	if(distance < d2)
	{
		duty_traction = 760  ;
	};

	/* duty_servo calculator (mean)


		duty_servo = (k_survo*old_survo + (1-k_survo)*now_survo )/1;	
 	*/
 
}
//715 speedd set





/**************************** PIN initiation ****************************/
void init_PIN(void)
{
// ================== 적외선센서 ==============
	SIU.PCR[44].R=0x0100;   //sensor1 C[12]
	SIU.PCR[45].R=0x0100;   //sensor2 C[13]
	SIU.PCR[46].R=0x0100;   //sensor3 C[14]
	SIU.PCR[47].R=0x0100;   //sensor4 C[15]
 
// ================== 초음파센서 ============
	SIU.PCR[59].R = 0x0200;  // Trig GPIO[59]
 	SIU.PCR[0].R = 0x0500;  // Echo eTimer_0


// ================== LED ============
    
    SIU.PCR[52].R = 0x0200;
	SIU.PCR[53].R = 0x0200;
	SIU.PCR[54].R = 0x0200;
	SIU.PCR[55].R = 0x0200;			// output
	
	SIU.PCR[48].R = 0x0100;			// input
	SIU.PCR[49].R = 0x0100;
	SIU.PCR[50].R = 0x0100;
	SIU.PCR[51].R = 0x0100;

    SIU.GPDO[52].B.PDO = 1;
    SIU.GPDO[53].B.PDO = 1;
    SIU.GPDO[54].B.PDO = 1;
    SIU.GPDO[55].B.PDO = 1;
    
// ================== Alarm ============ 
	SIU.PCR[16].R = 0x0200;
	SIU.GPDO[16].B.PDO = 0;   
// ================== PWM Pin ======================
 	SIU.PCR[58].R = 0x0600;  // FlexPWM0_A[0]				// PD [10]
	SIU.PCR[61].R = 0x0600;  // FlexPWM0_A[1]				// PD [13]


}

/*************************** 적외선센서 ******************************/
void infrared(void)
{

	old_sensor1 = now_sensor1;
	old_sensor2 = now_sensor2;
	old_sensor3 = now_sensor3;
	old_sensor4 = now_sensor4;


	now_sensor1 = SIU.GPDI[44].B.PDI;
	now_sensor2 = SIU.GPDI[45].B.PDI;
	now_sensor3 = SIU.GPDI[46].B.PDI;
	now_sensor4 = SIU.GPDI[47].B.PDI;

	now_sensor1 = k*old_sensor1 + (1-k)*now_sensor1;
	now_sensor2 = k*old_sensor2 + (1-k)*now_sensor2;
	now_sensor3 = k*old_sensor3 + (1-k)*now_sensor3;
	now_sensor4 = k*old_sensor4 + (1-k)*now_sensor4;

	if(now_sensor1>0.2)
   		RED.sen.s.sensor1 = 1; 
	else
   		RED.sen.s.sensor1 = 0; 

	if(now_sensor2>0.2)
   		RED.sen.s.sensor2 = 1; 
	else
   		RED.sen.s.sensor2 = 0; 

	if(now_sensor3>0.2)
   		RED.sen.s.sensor3 = 1; 
	else
   		RED.sen.s.sensor3 = 0; 

	if(now_sensor4>0.2)
   		RED.sen.s.sensor4 = 1; 
	else
   		RED.sen.s.sensor4 = 0; 



}



/***************************** Flex_PWM ***********************************/
#define PWM_period_traction 2500     // freq: 200Hz
#define PWM_period_servo 2500        // freq: 200Hz

void init_FlexPWM0(void)
{
 // ============== General PWM initialization =======
	FLEXPWM_0.OUTEN.R  = 0x0000;
 	FLEXPWM_0.MASK.R = 0x0000;
 	FLEXPWM_0.SWCOUT.R = 0x00F3;
 	FLEXPWM_0.DTSRCSEL.R = 0x0000;
 	FLEXPWM_0.FCTRL.R = 0x0000;
 	FLEXPWM_0.FFILT.R = 0x0000;
 
 
 // =============== PWM Sub[0] initialization ======= ***** servo ***** 
 // =========================== Set PWM Frequency ===
	FLEXPWM_0.SUB[0].INIT.R = 0xffff - PWM_period_servo +1;
	FLEXPWM_0.SUB[0].VAL[0].R = 0;
 	FLEXPWM_0.SUB[0].VAL[1].R = PWM_period_servo;
 	FLEXPWM_0.SUB[0].VAL[2].R = 0;
 	FLEXPWM_0.SUB[0].VAL[3].R = 0;
 	FLEXPWM_0.SUB[0].VAL[4].R = 0;
 	FLEXPWM_0.SUB[0].VAL[5].R = 0;
 // ============================== Set PWM Option ===
 	FLEXPWM_0.SUB[0].CTRL2.R = 0x0000;
	FLEXPWM_0.SUB[0].CTRL2.B.INDEP = 1;  // Pair Operation : 1-Independent, 0-Pair
 	FLEXPWM_0.SUB[0].CTRL.R = 0x0000;
 	FLEXPWM_0.SUB[0].CTRL.B.HALF = 1;  // Half Cycle Reload : Enable
 	FLEXPWM_0.SUB[0].CTRL.B.FULL = 1;  // Full Cycle Reload : Enable
 	FLEXPWM_0.SUB[0].CTRL.B.PRSC = 0b110;   // CLK/64
 	FLEXPWM_0.SUB[0].OCTRL.R = 0x0000;
 	FLEXPWM_0.SUB[0].OCTRL.B.POLA = 0;  // PWM A Polarity : not Inverted
 	FLEXPWM_0.SUB[0].OCTRL.B.POLB = 0;  // PWM B Polarity : not Inverted
 // =========================== Set PWM Interrupt ===
//  FLEXPWM_0.SUB[0].TCTRL.R = 0x0004;
 	FLEXPWM_0.SUB[0].TCTRL.R = 0x0000;
 // ======================= Set PWM Fault Disable ===
 	FLEXPWM_0.SUB[0].DISMAP.R = 0xFF00;
 // ============================ Set PWM Deadtime ===
 	FLEXPWM_0.SUB[0].DTCNT0.R = 0x0000;
 	FLEXPWM_0.SUB[0].DTCNT1.R = 0x0000;
 
  
 // =============== PWM Sub[1] initialization ======= ***** traction *****
 // =========================== Set PWM Frequency ===
 	FLEXPWM_0.SUB[1].INIT.R = 0xffff - PWM_period_traction +1;
 	FLEXPWM_0.SUB[1].VAL[0].R = 0;
 	FLEXPWM_0.SUB[1].VAL[1].R = PWM_period_traction;
 	FLEXPWM_0.SUB[1].VAL[2].R = 0;
 	FLEXPWM_0.SUB[1].VAL[3].R = 0;
 	FLEXPWM_0.SUB[1].VAL[4].R = 0;
 	FLEXPWM_0.SUB[1].VAL[5].R = 0;
 // ============================== Set PWM Option ===
 	FLEXPWM_0.SUB[1].CTRL2.R = 0x0000;
 	FLEXPWM_0.SUB[1].CTRL2.B.INDEP = 0;  // Pair Operation : 1-Independent, 0-Pair
 	FLEXPWM_0.SUB[1].CTRL.R = 0x0000;
 	FLEXPWM_0.SUB[1].CTRL.B.HALF = 1;  // Half Cycle Reload : Enable
 	FLEXPWM_0.SUB[1].CTRL.B.FULL = 1;  // Full Cycle Reload : Enable
 	FLEXPWM_0.SUB[1].CTRL.B.PRSC = 0b110;   // CLK/64
 	FLEXPWM_0.SUB[1].OCTRL.R = 0x0000;
 	FLEXPWM_0.SUB[1].OCTRL.B.POLA = 0;  // PWM A Polarity : not Inverted
 	FLEXPWM_0.SUB[1].OCTRL.B.POLB = 0;  // PWM B Polarity : not Inverted
 // =========================== Set PWM Interrupt ===
// 	FLEXPWM_0.SUB[1].TCTRL.R = 0x0004;
 	FLEXPWM_0.SUB[1].TCTRL.R = 0x0000;
 	FLEXPWM_0.SUB[1].INTEN.R = 0x0001;
 // ======================= Set PWM Fault Disable ===
 	FLEXPWM_0.SUB[1].DISMAP.R = 0xFF00;
 // ============================ Set PWM Deadtime ===
 	FLEXPWM_0.SUB[1].DTCNT0.R = 0x0000;
 	FLEXPWM_0.SUB[1].DTCNT1.R = 0x0000;
 
 // ============== General PWM initialization re =======
 	FLEXPWM_0.OUTEN.B.PWMA_EN = 0b0011; 
 	FLEXPWM_0.OUTEN.B.PWMB_EN = 0b0000;
 	FLEXPWM_0.OUTEN.B.PWMX_EN = 0b0000; 
	FLEXPWM_0.MCTRL.B.LDOK |= 0xF;       // Load config values into buffers 
 	FLEXPWM_0.MCTRL.B.RUN |=0xF;

}

void ISR_FlexPWM0(void)
{

  	FLEXPWM_0.SUB[1].STS.B.CMPF = 0x0001;
  	
  	FLEXPWM_0.SUB[0].VAL[2].R = (unsigned short)-duty_servo;
  	FLEXPWM_0.SUB[0].VAL[3].R = (unsigned short) duty_servo;
  	FLEXPWM_0.SUB[1].VAL[2].R = (unsigned short)-duty_traction;
  	FLEXPWM_0.SUB[1].VAL[3].R = (unsigned short) duty_traction;

  	FLEXPWM_0.MCTRL.B.LDOK |= 0xF; // Load config values into buffers 
  	FLEXPWM_0.MCTRL.B.RUN |=0xF;

  	test++;
}



/************************ 초음파센서 Trig ****************************/
void init_PIT(void)
{
	PIT.PITMCR.R = 0x00000001;             // PIT Enabled and Config stop in debug mode
	PIT.CH[0].LDVAL.R = 0x00000280;     // 10 us
	PIT.CH[0].TCTRL.R = 0x3;               // Timer interrupt enabled & start
	
 
}



void ISR_PIT0(void)
{
 	PIT.CH[0].TFLG.B.TIF = 1;              // Clear PIT0 Flag
 	Pit0cnt++;
 
 	if(Pit0cnt!=10000)
 	{
  		SIU.GPDO[59].B.PDO = 0;
 	}
 	else
 	{
 		 SIU.GPDO[59].B.PDO = 1;
 		 Pit0cnt=0;
 		 tenmillisec ++;
 		 if(tenmillisec ==100)
		{
			tenmillisec = 0;
		}
	
 	}
}


/************************* 초음파센서 Echo ********************************/
void init_eTimer(void)
{

// ============== eTimer_0 initialization =======
    ETIMER_0.ENBL.R = 0b000000;
    
    ETIMER_0.CHANNEL[0].CTRL.B.CNTMODE = 0b001;    // Count rising edges of primary source
    ETIMER_0.CHANNEL[0].CTRL.B.PRISRC  = 0b11111;  // From Table 'Count source values'
    ETIMER_0.CHANNEL[0].CTRL.B.ONCE    = 0;        // Count repeatedly
    ETIMER_0.CHANNEL[0].CTRL.B.LENGTH  = 0;        // Continue counting to roll over
    ETIMER_0.CHANNEL[0].CTRL.B.DIR     = 0;        // Count up
    ETIMER_0.CHANNEL[0].CTRL.B.SECSRC  = 0b00000;  // From Table 'Count source values'
    
    ETIMER_0.CHANNEL[0].CCCTRL.B.CPT2MODE = 0b01;  // Capture falling edges
    ETIMER_0.CHANNEL[0].CCCTRL.B.CPT1MODE = 0b10;  // Capture rising edges
    ETIMER_0.CHANNEL[0].CCCTRL.B.ONESHOT = 0;      // Free-running mode is selected
    ETIMER_0.CHANNEL[0].CCCTRL.B.ARM = 1;          // Input capture operation as specified by the CPT1MODE and CPT2MODE bits is enabled
    
    ETIMER_0.CHANNEL[0].CTRL3.B.ROC   = 0b01;      // Reload the counter on a capture 1 event
     
	ETIMER_0.CHANNEL[0].INTDMA.B.ICF2IE = 1;       // Input Capture 2 flag interrupt Enable
 
    ETIMER_0.ENBL.R = 0b000001;                    // ETC[0] Enabled
    
}

void ISR_ETC0(void)
{

 	ETIMER_0.CHANNEL[0].STS.B.ICF2 = 1;
 
 	echo = ETIMER_0.CHANNEL[0].CAPT2.R;
 /*
 	time_high  = 128/64M sec x echo (2 micro sec per 1 count)
	distance   = 340 x time_high / 2
            = 340 x echo
 */
 	pre_distance = 340 * echo / 10000; // centimeter
 
 	if(pre_distance > 90)
 	{
 		pre_distance = 90;
 	}
 	
 	distance = (k_echo*pre_distance + (1-k_echo)*pre_distance)/1;

}

/************************* LED SWITCH ********************************/
void Switch(void)
{

	SWold[0] = SW[0];
	SWold[1] = SW[1];
   	SWold[2] = SW[2];
   	SWold[3] = SW[3];
   
   	SW[0] = SIU.GPDI[48].B.PDI;
   	SW[1] = SIU.GPDI[49].B.PDI;
   	SW[2] = SIU.GPDI[50].B.PDI;
   	SW[3] = SIU.GPDI[51].B.PDI; 
   
   	if(!SWold[0] && SW[0]) cnt += 1;
   	if(!SWold[1] && SW[1]) cnt += 2;
   	if(!SWold[2] && SW[2]) cnt += 4;
   	if(!SWold[3] && SW[3]) cnt += 8;

   	cnt = cnt%16;
	
	SIU.GPDO[52].B.PDO = !(cnt & 0b0001);
	SIU.GPDO[53].B.PDO = !(cnt & 0b0010);
	SIU.GPDO[54].B.PDO = !(cnt & 0b0100);
	SIU.GPDO[55].B.PDO = !(cnt & 0b1000);
}