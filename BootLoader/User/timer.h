#ifndef _TIMER_H
#define _TIMER_H

#include "stm32f4xx.h"
#include "stm32f4xx_tim.h"


#define TIM1_UPDATE_GENERATION 		 ((uint16_t)0x0001)
#define TIM1_UPDATE_INTERRUPT_FLAG ((uint16_t)0x0001)
#define TIM1_COUNTER_Enable 			 ((uint16_t)0x0001)

#define TIM1_PRESCALAR	((uint16_t)0x0014)
#define TIM1_ARR				((uint16_t)0x0100)
#define TIM1_REPETITION	((uint16_t)0x000A)

#define TIM2_PRESCALAR	((uint16_t)0x0014)
#define TIM2_ARR				((uint32_t)0x00003200)
#define TIM3_ARR				((uint32_t)0x00003200)
void TimerConfig(void);
void TIM_Config(void);
uint16_t Delay_ms(uint16_t);

#endif
