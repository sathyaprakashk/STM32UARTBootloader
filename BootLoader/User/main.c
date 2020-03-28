/**
  ******************************************************************************
  * @file    main.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-April-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************  
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "usart.h"
#include "timer.h"
#include "BootLoaderUART.h"
#include<string.h>
#include<stdlib.h>

#define JUMPDEFAULTADDRESS (uint32_t)0x0800C000
extern __IO unsigned int uiCountNumBytesRecv;
extern __IO uint8_t ucExternalTrigger;
uint8_t uartDataBytes[320];
__IO uint16_t uart4Received=0;
extern __IO uint8_t 	ucReceiveStatus;
/** @addtogroup STM32F2xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup IOToggle
  * @{
  */ 


/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



/*
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
	char ch=0;
	int i=0;
	SystemCoreClockUpdate();
				/*__disable_irq() instruction raises the execution priority to 0. This prevents all 
       exceptions with configurable priority from activating, other than through 
       the HardFault fault escalation mechanism. This is implemented using PRIMASK 
			 register in System Control Block.
   		 Setting PRIMASK to 1 raises the current execution priortity to 0. Execptions 
		   other than RESET, NMI, Hardfault cannot preempt the current execution */
	__disable_irq();
	
	USART_Configuration();		//USART1 configuration PA10-Rx PA9-Tx pin used for BootLoader Commands and data
	UART_Configuration();			//Debug port for printf messages. PC10-Tx pin , PC11-Rx pin.
	USART_NVIC_Config();		
	UART4_NVIC_Config();
	
	TimerConfig();		//Timer1 is configured this timer is used in 1ms delay generation function(Delay_ms())
	FLASH_Unlock();
	
	__enable_irq();
	printf("Welcome to STM32F407 Board\r\n");
	
	printf("Press Y/y to Enter Boot Mode in 10 seconds\r\n");
	i=0;
	while(i<10)
	{

		Delay_ms(1000);
		printf("%d Second Remaining\r\n",9-i);
		if(uart4Received==1)
		{
			ch=USART_ReceiveData(UART4);
			break;
		}
		i++;
	}
	if(ch=='y' || ch == 'Y')
	{
		printf("Entering Bootloader Use Flash Demo Software with \r\n\tBaud Rate - 115200\r\n\tParity- Even\r\n\tDatabits - 8\r\n\tEcho - Disabled\r\n\tTimout(s) - 10\r\n");
		interpretBootLoaderCmds();
	}
	else
	{
	 printf("Entering Application at location %08x \r\n",JUMPDEFAULTADDRESS);
	 deInitAndJumpApplicationCode(JUMPDEFAULTADDRESS);
	}

	

	while(1);

}
	


/**
  * @brief  Delay Function.
  * @param  nCount:specifies the Delay time length.
  * @retval None
  */



#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {

  }
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
