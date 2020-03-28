#include "timer.h"
#include<stdio.h>

TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
void TimerConfig(void){
	//RCC_GetClocksFreq(RCC_Clocks);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, DISABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	  /* Set the default configuration 
			 APB2 timer Clock is  to be 53,760,000 based on that 				
			 Register values are set to generate one millisecond Delay
			 (1/(53,760,000HZ))*256*21*10 = 1ms;
				AutoReload Register(TIM1_ARR) = 255(0x00ff) 255+1 =256
				Prescaler (TIM1_PSC)					=  20(0x14)	 	 20+1 =21
				Repetation Count(TIM1_RCR)    =   9(0x0009)		9+1 =10*/
	if(HSE_VALUE == ((uint32_t)8000000))														
		TIM_TimeBaseInitStruct.TIM_Period = 0x000000ff;      					//AutoReload Register Value 255 Counts
	else 																														//change HSE_VALUE in stm32f4xx.h if the crystal is 25MHZ 
		TIM_TimeBaseInitStruct.TIM_Period = 0x000002ff;      					//AutoReload Register Value 3X256-1 Counts it is assumed 25MHZ HSE VALUE
  TIM_TimeBaseInitStruct.TIM_Prescaler = 0x0014;			 					//0x0014= 20 , After 20+1 CLK_IN one (upcount or Downcount) count 
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;  
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;   //Upcounter
  TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0x09; 					 //After 10 times counter reset Update Event occurs and Update Interrupt Flag is set
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStruct);
	TIM_UpdateRequestConfig(TIM1,TIM_UpdateSource_Regular);
	TIM_SelectOnePulseMode(TIM1,((uint16_t)0));										 //Timer not used in one pulse mode

}


uint16_t Delay_ms(uint16_t uiDelay)
{
	int i;
  TIM1->EGR |= TIM1_UPDATE_GENERATION ;          							//Update Generation
	TIM1->SR &= ~TIM1_UPDATE_INTERRUPT_FLAG; 										//Clear Update Interrupt Flag
	TIM1->CR1 |= TIM1_COUNTER_Enable;            								//Counter Enable / Start Counter	
	for(i=0;i<uiDelay;i++)
	{
		while(!(TIM1->SR & TIM1_UPDATE_INTERRUPT_FLAG));     			// Check Update Interrupt Flag;
		TIM1->SR &= ~TIM1_UPDATE_INTERRUPT_FLAG;									// Clear Update Interrupt Flag;// One millisecond is completed;
	}
	TIM1->CR1 &= ~TIM1_COUNTER_Enable;					    					 //Stop the counter	
	return uiDelay;
}
