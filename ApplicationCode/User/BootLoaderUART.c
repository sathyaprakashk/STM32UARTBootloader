#include "BootLoaderUART.h"
extern uint8_t uartDataBytes[320];
extern __IO unsigned int uiCountNumBytesRecv;
extern __IO uint8_t ucReceiveStatus;
uint8_t ListofCMD[11] = {GETCMD,GETVER,GETID,READMEMORY,GOCMD,WRITEMEM,ERASECMD,WRITEPRO,WRITEUNPRO,READOUTPRO,READOUTUNPRO};
uint16_t rgusiReceivedAddress[5];
__IO unsigned int uiProcessedReceivedNumOfBytes=0;
uint8_t GetCommandMessage[20] = {BOOTLOADERACK,0x0B,BOOTLOADERVER,GETCMD,GETVER,GETID,READMEMORY,GOCMD,WRITEMEM,ERASECMD,WRITEPRO,WRITEUNPRO,READOUTPRO,READOUTUNPRO,BOOTLOADERACK};
uint8_t sendPIDMessage[5] = {BOOTLOADERACK,0x01,0x04,0x13,BOOTLOADERACK};
typedef void (*pFunction)(void);
uint16_t EraseSectorNum[16];

void interpretBootLoaderCmds()
{

	
	uint32_t uiDiff;
	//initial sequence
	while(!ucReceiveStatus);
	ucReceiveStatus=0;
	if(uartDataBytes[uiCountNumBytesRecv-1]==0x7f)
		USART_SendData(Port_USART,BOOTLOADERACK);
	uiProcessedReceivedNumOfBytes++;
	while(1)
	{
		uiDiff = uiCountNumBytesRecv - uiProcessedReceivedNumOfBytes;
		//printf("Num received %d Last received = %02x\r\n",uiDiff,uartDataBytes[uiCountNumBytesRecv-1]);
		if(uiDiff>=2)
		{
		
		switch(receiveCMD(2))
		{
			case GETCMD:
				printf("GETCMD \r\n");
				sendCMDS();
				break;
			case GETID:
				printf("GETID\r\n");
				sendID();
				break;
			case READMEMORY:
				printf("READMEMORY\r\n");
				handleReadMemoryCMD();
				break;
			case WRITEPRO:
				printf("Write Protected\r\n");
				break;
			case GETVER:
				printf("GETVERSION\r\n");
				break;
			case GOCMD:
				printf("GOCOMMAND\r\n");
				handleGOCMD();
				break;
			case WRITEMEM:
				printf("WRITEMEMORY\r\n");
				handleWriteMemoryCMD();
				break;
			case ERASECMD:
				printf("ERASE Command\r\n");
				handleEraseCMD();
				break;
			case WRITEUNPRO:
				printf("Write Unprotected\r\n");
				break;
			case READOUTPRO:
				printf("Read out protected\r\n");
				break;
			case READOUTUNPRO:
				printf("Read out unprotected\r\n");
				USART_SendData(Port_USART,BOOTLOADERACK);
				USART_SendData(Port_USART,BOOTLOADERACK);
				Delay_ms(50);
				interpretBootLoaderCmds();
				break;
			case ERRORBL:
				printf("Unknown Sequence \r\n");
				break;
				
		}
				
		}
	}
}

FlagStatus receiveAddress(uint16_t *receiveAddress)
{
	uint8_t checksum=0;
	for(int i=0;i<4;i++)
	{
		while(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)==RESET);
		receiveAddress[i] = USART_ReceiveData(USART1);
		checksum ^= receiveAddress[i];
		uartDataBytes[uiCountNumBytesRecv++] = (uint8_t)(0xFF&receiveAddress[i]);
		printf("%02x \r\n",receiveAddress[i]);
	}
	uartDataBytes[0] = uiCountNumBytesRecv;
	if(checksum == 0x00)
		return SET;
	return RESET;
}

void sendCMDS(void)
{
			for(int i=0;i<15;i++)
			{
				//Delay_ms(1);
				while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
				{}
				USART_SendData(Port_USART,GetCommandMessage[i]);
			}
}

void sendID(void)
{
			for(int i=0;i<5;i++)
			{
				//Delay_ms(1);
				while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
				{}
				USART_SendData(Port_USART,sendPIDMessage[i]);
			}
}

void handleReadMemoryCMD(void)
{
	volatile int iDiff=0;
	uint8_t ucCheckSum;
	uint8_t ucValue;
	uint32_t uiReceivedAddress=0;
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	USART_SendData(Port_USART,BOOTLOADERACK);
	while(DIFF(uiCountNumBytesRecv,uiProcessedReceivedNumOfBytes)<5);
	uiReceivedAddress=0;
	ucCheckSum=0;
	for(int i=0;i<5;i++)
	{
		ucValue = uartDataBytes[uiProcessedReceivedNumOfBytes+i];
		if(i<4)
		{
			uiReceivedAddress |= ucValue<<(3-i)*8;
		}
		ucCheckSum ^= ucValue;	
	}
	
	if(!ucCheckSum)
	{
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
		USART_SendData(Port_USART,BOOTLOADERACK);
		uiProcessedReceivedNumOfBytes += 5;
		while(DIFF(uiCountNumBytesRecv,uiProcessedReceivedNumOfBytes)<2);
		ucValue = uartDataBytes[uiProcessedReceivedNumOfBytes];
		ucCheckSum = ucValue ^ uartDataBytes[uiProcessedReceivedNumOfBytes+1];
		if(ucCheckSum==0xFF)
		{
			while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
			USART_SendData(Port_USART,BOOTLOADERACK);
			printf("Read %02x byte Starting from Address 0x%08x\r\n",ucValue,uiReceivedAddress);
			for(int i=0;i<ucValue+1;i++)
			{
				while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
				USART_SendData(USART1,*(__IO uint8_t *)(uiReceivedAddress+i));
			}
			uiProcessedReceivedNumOfBytes += 2;
		}
		else
		{
			while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
			USART_SendData(Port_USART,BOOTLOADERNACK);
		}
	}
	else
	{
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
		USART_SendData(Port_USART,BOOTLOADERNACK);
	}
	return;
}
BLCMD receiveCMD(uint8_t numOfReceiveCmd)
{
	uint8_t CMD;
	uint16_t usiChecksum=0x00;
	if(numOfReceiveCmd==2)
	{
				ucReceiveStatus=0;
				usiChecksum = uartDataBytes[uiProcessedReceivedNumOfBytes+1];
				CMD= uartDataBytes[uiProcessedReceivedNumOfBytes];
				usiChecksum ^= CMD;
				uiProcessedReceivedNumOfBytes += 2;
				int i=0;
				if(usiChecksum==0xFF)
				{
					while(i<11)
					{
						if(CMD==ListofCMD[i++])
						{
	
							return (BLCMD)CMD;
						}
					}
					return ERRORBL;
				}
				else
					return ERRORBL;
	}
	return ERRORBL;
}

int16_t receiveLength(void)
{
	uint16_t usiUSARTReceivedData;
	uint16_t usiChecksum;
		while(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)==RESET);
	//Get Command
	usiUSARTReceivedData =USART_ReceiveData(USART1);
	while(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)==RESET);
	usiChecksum = USART_ReceiveData(USART1);
	uartDataBytes[uiCountNumBytesRecv++] = (uint8_t)(0xFF&usiUSARTReceivedData);
	uartDataBytes[uiCountNumBytesRecv++] = (uint8_t)(0xFF&usiChecksum);
	uartDataBytes[0] = uiCountNumBytesRecv;
	if((usiChecksum^usiUSARTReceivedData) == 0xFF)
		return(0xFF & usiUSARTReceivedData);
	return -1;
}

void handleWriteMemoryCMD(void)
{
	uint8_t ucChecksum=0,ucValue=0;
	uint32_t uiReceivedAddress=0;
	uint8_t ucNumOfBytesToWrite=0;
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	USART_SendData(Port_USART,BOOTLOADERACK);
	while(DIFF(uiCountNumBytesRecv,uiProcessedReceivedNumOfBytes)<5);
	for(int i=0;i<5;i++)
	{
		ucValue = uartDataBytes[uiProcessedReceivedNumOfBytes+i];
		if(i<4)
		{
			uiReceivedAddress |= ucValue<<(3-i)*8;
		}
		ucChecksum ^= ucValue;	
	}
	uiProcessedReceivedNumOfBytes += 5;
	if(!ucChecksum)
	{
		for(int i=0;i<uiCountNumBytesRecv;i++)
		{
			uartDataBytes[i]=0;
		}
		uiCountNumBytesRecv=0;
		uiProcessedReceivedNumOfBytes=0;
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
		USART_SendData(Port_USART,BOOTLOADERACK);
		while(uiCountNumBytesRecv<1);
		ucNumOfBytesToWrite = uartDataBytes[uiProcessedReceivedNumOfBytes++];
		while(uiCountNumBytesRecv<ucNumOfBytesToWrite+3);
		ucChecksum =0;
		for(int i=0;i<=ucNumOfBytesToWrite+2;i++)
		{
			ucChecksum ^= uartDataBytes[i];
		}
		if(!ucChecksum)
		{
			for(int i=0;i<=ucNumOfBytesToWrite;i++)
			{
				FLASH_ProgramByte(uiReceivedAddress+i,uartDataBytes[uiProcessedReceivedNumOfBytes++]);
			}
			uiProcessedReceivedNumOfBytes++;
			if(uiProcessedReceivedNumOfBytes == uiCountNumBytesRecv)
				printf("Successfully written %d number of Bytes Starting from Address 0x%08x \r\n",ucNumOfBytesToWrite+1,uiReceivedAddress);
			while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
			USART_SendData(Port_USART,BOOTLOADERACK);
		}
		
	}
	
}
void handleEraseCMD(void)
{
	uint16_t usiNumSectorErased=0;
	uint8_t ucCheckSum=0;
	
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	USART_SendData(Port_USART,BOOTLOADERACK);
	
	while(DIFF(uiCountNumBytesRecv,uiProcessedReceivedNumOfBytes)<2);
	usiNumSectorErased = (uint16_t)(uartDataBytes[uiProcessedReceivedNumOfBytes]<<8);
	ucCheckSum ^= uartDataBytes[uiProcessedReceivedNumOfBytes++];
	usiNumSectorErased |= (uint16_t)(0xFF & uartDataBytes[uiProcessedReceivedNumOfBytes]);
	ucCheckSum ^= uartDataBytes[uiProcessedReceivedNumOfBytes++];

	while(DIFF(uiCountNumBytesRecv,uiProcessedReceivedNumOfBytes)<(usiNumSectorErased+1)*2+1);
	for(int i=0,j=0;i<(usiNumSectorErased+1)*2;i++)
	{
		if(i%2)
		{
			EraseSectorNum[j++] |= (uint16_t)(0xFF & uartDataBytes[uiProcessedReceivedNumOfBytes]);
		}
		else
		{
			EraseSectorNum[j] = (uint16_t)(uartDataBytes[uiProcessedReceivedNumOfBytes]<<8);
		}
		ucCheckSum ^= uartDataBytes[uiProcessedReceivedNumOfBytes++]; 
	}

	ucCheckSum ^= uartDataBytes[uiProcessedReceivedNumOfBytes++];

	if(!ucCheckSum)
	{
		for(int i=0;i<(usiNumSectorErased+1);i++)
		{	
			if(FLASH_EraseSector(EraseSectorNum[i]<<3,VoltageRange_3)!=FLASH_COMPLETE)
			{
				while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
				USART_SendData(Port_USART,BOOTLOADERNACK);
				break;
			}
			if(i==usiNumSectorErased)
			{
				while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
				USART_SendData(Port_USART,BOOTLOADERACK);	
			}
		}
	
	}
	else
	{
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
		USART_SendData(Port_USART,BOOTLOADERNACK);
	}
	
}

void handleGOCMD(void)
{

	uint32_t uiJumpAddress=0;
	uint8_t ucValue=0;
	uint8_t ucChecksum=0;
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	USART_SendData(Port_USART,BOOTLOADERACK);
	while(DIFF(uiCountNumBytesRecv,uiProcessedReceivedNumOfBytes)<5);
	for(int i=0;i<5;i++)
	{
		ucValue = uartDataBytes[uiProcessedReceivedNumOfBytes+i];
		if(i<4)
		{
			uiJumpAddress |= ucValue<<(3-i)*8;
		}
		ucChecksum ^= ucValue;	
	}
	uiProcessedReceivedNumOfBytes += 5;
	if(!ucChecksum)
	{
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
		USART_SendData(Port_USART,BOOTLOADERACK);
	}
	else
	{
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
		USART_SendData(Port_USART,BOOTLOADERNACK);	
	}

	deInitAndJumpApplicationCode(uiJumpAddress);

}

void deInitAndJumpApplicationCode(uint32_t AppAddress)
{
		//before Jumping to Application program DeInitialize all the GPIO's USART Timer 
	pFunction appEntry;
	uint32_t appStack=0;
	appStack = (uint32_t)(*(__IO uint32_t*)AppAddress);
	__set_MSP(appStack);
	SCB->VTOR = AppAddress;
	appEntry = (pFunction)*(__IO uint32_t*)(AppAddress+4);
	printf("Application Program begins....\r\n");
	USART_DeInit(USART1);
	USART_DeInit(UART4);
	GPIO_DeInit(GPIOA);
	GPIO_DeInit(GPIOC);
	TIM_DeInit(TIM1);
	appEntry();
}





