#include <stdarg.h>
#include <stdio.h>
#include "def.h"
#include "touch.h"
#include "UartLib.h"
#include "2410lib.h"
#include "2410addr.h"
#include "Int.h"

UART_PORT UartPort0;		// ����0��Ӧ�Ľṹ��
int UartRxEndFlag;

/*
|	����:	Receive_Uart0,	����0�����жϳ���
|	����:	��
|	����:	��
|
|	����:	06/12/18
|	������:	ZengJC
*/
void __irq Receive_Uart0(void)
{
//	int temp;
	U8 receive;
	INT_ADC_Enable(FALSE);
	
	if (UartPort0.RxBufferStatus != BUFFER_FULL)
		do
		{
			receive = RxUart(UART0);
			UartPort0.RxBuffer[UartPort0.RxBufferWrite++] = receive;
		
			// ��黺����
			if (UartPort0.RxBufferWrite == UartPort0.bufferSize)
				UartPort0.RxBufferWrite = 0;
			if (UartPort0.RxBufferWrite == UartPort0.RxBufferRead)
				UartPort0.RxBufferStatus = BUFFER_FULL;
			else
				UartPort0.RxBufferStatus = BUFFER_DATA;
			
		} while (rUTRSTAT0 & 0x1);
		
	
	ClearInt();			// ���ж�
	INT_ADC_Enable(TRUE);
}

/*
|	����:	UartGetChar,		�Ӵ��ڻ�������ȡһ���ַ�
|	����:	UART_PORT *port,	���ڶ�Ӧ�ṹ��
|	����:	U8,					����ȡ�õ��ַ�,����ΪU8(unsigned char)
|
|	����:	06/12/18
|	������:	ZengJC
*/
U8 UartGetChar(UART_PORT *port)
{
	U8 ch = 0;
	
	if (port->RxBufferStatus != BUFFER_EMPTY) 
	{
		ch = port->RxBuffer[port->RxBufferRead++];
		
		if (port->RxBufferRead == port->bufferSize)
			port->RxBufferRead = 0;
		
		if (port->RxBufferRead == port->RxBufferWrite)
			port->RxBufferStatus = BUFFER_EMPTY;
		else
			port->RxBufferStatus = BUFFER_DATA;
	}
	
	Delay(2);
	return ch;
}

/*
|	����:	SetDefaultToUart,	�Ѵ��ڽṹ����ΪĬ��ֵ
|	����1:	UART_PORT *port,	���ڶ�Ӧ�ṹ��
|	����2:	int com,			Ŀ�괮��
|	����:	��
|
|	����:	06/12/18
|	������:	ZengJC
*/
void SetDefaultToUart(UART_PORT *port, int com)
{
	port->comPort	= com;		// Ŀ�괮��
	port->baudRate	= 112500;	// ������
	port->dataBits	= 3;		// 8 bits per frame
	port->stopBits	= 0;		// One stop bit per frame
	port->parity	= 0;		// No parity
	port->dataMode	= 0;		// UART runs in normal operation
	port->communicationMode = 0;// ???
	port->bufferSize = 1024;	// ���ջ����С
}

/*
|	����:	InitUartPort,		��ʼ������
|	����:	UART_PORT *port,	���ڶ�Ӧ�ṹ��
|	����:	int,				����(δʹ��)
|
|	����:	06/12/18
|	������:	ZengJC
*/
int InitUartPort(UART_PORT *port)
{
//	int temp;
	
	if (port->comPort == UART0)
	{
		port->vector = 28;
		port->baseAddr = UART0_BASE;
	}
	else
		if (port->comPort == UART1)
		{
			port->vector = 23;
			port->baseAddr = UART1_BASE;
		}
		else
			if (port->comPort == UART2)
			{
				port->vector = 15;
				port->baseAddr = UART2_BASE;
			}
			else
				return 0;
	
	// Setup the RX SD buffer
	port->RxBufferRead = port->RxBufferWrite = 0;
	port->RxBufferStatus = BUFFER_EMPTY;
	
	// Clear Control registers
	SET_REGISTER(port->baseAddr + rULCON_OFFSET, 0);
	SET_REGISTER(port->baseAddr + rUCON_OFFSET, 0);
	SET_REGISTER(port->baseAddr + rUFCON_OFFSET, 0);
	SET_REGISTER(port->baseAddr + rUMCON_OFFSET, 0);
	
	// Setup baud rate
	SetBaudrate(port->baudRate, port->comPort);
	
	// Setup Mode, Parity, Stop Bits and Data Size in ULCON Reg.
	SET_REGISTER(port->baseAddr + rULCON_OFFSET
				,port->parity | port->dataBits | port->stopBits);
	rUCON0  = 0x245;
	
	//do{
	//	temp = GET_REGISTER(port->baseAddr + rURXH_OFFSET);
	//} while (GET_REGISTER(port->baseAddr + rUTRSTAT_OFFSET) & 1);
	
	UartInterrupt(TRUE, port->comPort);
	
	return 0;
}

/*
|	����:	SetBaudrate,	���ô��ڲ�����
|	����1:	U32 baudRate,	������
|	����2:	int com,		Ŀ�괮��
|	����:	��
|
|	����:	06/12/18
|	������:	ZengJC
*/
void SetBaudrate(U32 baudRate, int com)
{
	int pclk = 202800000/4;
	switch(com)
	{
	case UART0:
		{
			rUBRDIV0 = (int)(pclk/16./baudRate + 0.5) - 1;
		}break;
	case UART1:
		{
			rUBRDIV1 = (int)(pclk/16./baudRate + 0.5) - 1;
		}break;
	case UART2:
		{
			rUBRDIV2 = (int)(pclk/16./baudRate + 0.5) - 1;
		}break;
	default:
		{
		}break;
	}
}

/*
|	����:	UartPutChar,		�򴮷���һ���ַ�
|	����1:	U8 ch,				��Ҫ���͵��ַ�
|	����2:	UART_PORT *port,	Ŀ�괮��
|	����:	��
|
|	����:	06/12/18
|	������:	ZengJC
*/
void UartPutChar(U8 ch, UART_PORT *port)
{
	int status;
	status = GET_REGISTER(port->baseAddr + rUTRSTAT_OFFSET);

	while(!(status & 0x2));
	Delay(2);
	
	switch(port->comPort)
	{
	case UART0:
		{
			WrUTXH0(ch);
		}break;
	case UART1:
		{
			WrUTXH1(ch);
		}break;
	case UART2:
		{
			WrUTXH2(ch);
		}break;
	}
}

void UartSimplePutChar(U8 ch, int com)
{
	switch(com)
	{
	case UART0:
		{
			while(rUFSTAT0 & 0x2);	//Wait until THR is empty.
			Delay(2);
			WrUTXH0(ch);
		}break;

	case UART1:
		{
			while(rUFSTAT1 & 0x2);	//Wait until THR is empty.
			Delay(2);
			WrUTXH1(ch);
		}break;

	case UART2:
		{
			while(rUFSTAT2 & 0x2);	//Wait until THR is empty.
			Delay(2);
			WrUTXH2(ch);
		}break;
	}
}

/*
|	����:	UartPutStr,			�򴮷���һ���ַ���
|	����1:	char *str,			��Ҫ���͵��ַ�
|	����2:	UART_PORT *port,	Ŀ�괮��
|	����:	��
|
|	����:	06/12/18
|	������:	ZengJC
*/
void UartPutStr(char *str, UART_PORT *port)
{
	while(*str)
		UartPutChar(*str++, port);
}

void UartSimplePutStr(char *str, int com)
{
	while(*str)
		UartSimplePutChar(*str++, com);
}

void UartPrintf(int com, char *strFmt,...)
{
	va_list ap;
	char String[256];
	
	va_start(ap, strFmt);
	vsprintf(String, strFmt, ap);
	UartSimplePutStr(String,com);
	va_end(ap);
}

/*
|	����:	RxUart,		�Ӵ��ڽ���һ���ַ�
|	����:	int com,	Ŀ�괮��
|	����:	U8			�յ����ַ�
|
|	����:	06/12/18
|	������:	ZengJC
*/
U8 RxUart(int com)
{
    switch(com)
    {
        case UART0:
                return (char)RdURXH0();
                break;
                
        case UART1:
                return (char)RdURXH1();
                break;
                
        case UART2:
                return (char)RdURXH2();
                break;
    }
    return 0;
}

/*
|	����:	RxUartS,	�Ӵ��ڽ���һ���ַ�(������״̬)
|	����:	char *data,	�յ����ַ�
|	����:	int Uart,	Ŀ�괮��
|	����:	int			����״̬;TRUE,�ɹ�;FALSEʧ��
|
|	����:	06/12/18
|	������:	ZengJC
*/
int RxUartS(char *data,int Uart)
{
	switch(Uart)
	{
		case UART0:
			if(rUFSTAT0 & 0x1)
			{
				*data = rURXH0;
				return TRUE;
			}break;
			
		case UART1:
			if(rUFSTAT1 & 0x1)
			{
				*data = rURXH1;
				return TRUE;
			}break;
			
		case UART2:
			if(rUFSTAT2 & 0x1)
			{
				*data = rURXH2;
				return TRUE;
			}break;
    }
    return FALSE;
}

void UartWaitForStr(char *str, int Uart)
{
	char *tempStr = str;
	char ch = 0;
	int flag = 1;
	
	UartRxEndFlag = FALSE;
	while(ch !='\r' && UartRxEndFlag == FALSE)
	{
		flag = RxUartS(&ch,Uart);
		if (flag == TRUE)
		{
			if (ch == '\b')
			{
				if ((int)tempStr < (int)str)
				{
					UartPrintf(Uart ,"\b \b");
					str--;
				}
			}
			else
			{
				*str = ch;
				str++;
				UartSimplePutChar(ch,Uart);
			}
		}
	}
	
	UartRxEndFlag = TRUE;
	str--;
	*str = '\0';
	UartPrintf(Uart ,"\n\n");
}

/*
|	����:	UartInterrupt,	�����ж�ʹ��
|	����1:	int enable,		ʹ��
|	����2:	int com,		Ŀ�괮��
|	����:	��
|
|	����:	06/12/18
|	������:	ZengJC
*/
void UartInterrupt(int enable, int com)
{
	switch(com)
	{
	case UART0:
		{
			if (enable == TRUE)
			{
				rINTMSK &= ~BIT_UART0;
				//rINTMOD &= ~BIT_UART0;
				rINTSUBMSK &= ~(BIT_SUB_RXD0 | BIT_SUB_RXD0);
			}else{
				rINTMSK |= BIT_UART0;
				rINTSUBMSK |= BIT_SUB_RXD0;
			}
		}break;
	case UART1:
		{
			if (enable == TRUE)
			{
				rINTMSK &= ~BIT_UART1;
				rINTSUBMSK &= ~BIT_SUB_RXD1;
			}else{
				rINTMSK |= BIT_UART1;
				rINTSUBMSK |= BIT_SUB_RXD1;
			}
		}break;
	case UART2:
		{
			if (enable == TRUE)
			{
				rINTMSK &= ~BIT_UART2;
				rINTSUBMSK &= ~BIT_SUB_RXD2;
			}else{
				rINTMSK |= BIT_UART2;
				rINTSUBMSK |= BIT_SUB_RXD2;
			}
		}break;
	default:
		{
		}break;
	}
}

void UartDefaultInit(int Uart)
{
	switch(Uart)
	{
	case UART0:
		{
			rUFCON0	= 0;
			rUMCON0	= 0;
			rULCON0	= 0x23;
			rUCON0	= 0x245;
		}break;
	case UART1:
		{
			rUFCON1	= 0;
			rUMCON1	= 0;
			rULCON1	= 0x23;
			rUCON1	= 0x245;
		}break;
	case UART2:
		{
			rUFCON2	= 0;
			rUMCON2	= 0;
			rULCON2	= 0x23;
			rUCON2	= 0x245;
		}break;
	}
}