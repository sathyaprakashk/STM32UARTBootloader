#ifndef _BOOTLOADERUART_H

#define _BOOTLOADERUART_H
#include "usart.h"
#include "timer.h"
#include "stm32f4xx_flash.h"
#define BOOTLOADERACK (uint8_t)0x79
#define BOOTLOADERNACK (uint8_t)0x1F
#define DIFF(x,y) (int)(x-y)
#define BOOTLOADERVER (uint8_t)0x31
void deInitAndJumpApplicationCode(uint32_t);
FlagStatus receiveAddress(uint16_t *receiveAddress);
typedef enum {ERRORBL=-1,GETCMD=0x00,GETVER=0x01,GETID=0x02,READMEMORY=0x11,GOCMD=0x21,WRITEMEM=0x31,ERASECMD=0x44,WRITEPRO=0x63,WRITEUNPRO=0x73,READOUTPRO=0x82,READOUTUNPRO=0x92}BLCMD;
BLCMD receiveCMD(uint8_t );
int16_t receiveLength(void);
void interpretBootLoaderCmds(void);
void sendCMDS(void);
void sendID(void);
void handleReadMemoryCMD(void);
void handleWriteMemoryCMD(void);
void handleEraseCMD(void);
void handleGOCMD(void);

#endif
	
	
	
	
	
	