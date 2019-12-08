/*!	\file		CAN.c
	\brief		CAN�շ�����ʵ���ļ�
	\author		����
	\version	0.1

	�û�ͨ���ļ�ʵ��CAN���ߵĳ�ʼ�������ݵķ��ͣ����ݵ��жϽ��ܡ�
*/
#include<stdlib.h>
#include<string.h>

#include "CAN.h"
#include "def.h"
#include "2410addr.h"
#include "option.h"
//#include "GUI.h"
#include "2410lib.h"
//#include "lcd.h"
#include "spi.h"
#include "mcp2515.h"
//#include "consol.h"
#include "spi.h"
#include "lcdlib.h"

/*!	\defgroup	CANDAT	CAN������Ϣ���ܴ�ŵ�Ԫ
*/
//@{
char messagebuf[100];	//!<�ַ������ݻ�����
char RxMsgBuf[256];
int  RxMsgBufW;
int  RxMsgBufR;
//@}

/** CAN������Ϣ�ṹ���ʼ������ռ亯�� 
*  @param[in] msg CAN������Ϣ�ṹ��
*/
void can_initMessageStruct(CanMessage* msg)
{
	memset(msg,0,sizeof(CanMessage));
}

void can_initBuf(void)
{
	memset(RxMsgBuf,0,sizeof(RxMsgBuf));
	RxMsgBufW = 0;
	RxMsgBufR = 0;
}

/** оƬMCP2515������з��ͻ��������� 
*  @param[in] txbuf_n ���ͻ��������
*/
unsigned char mcp2515_getNextFreeTXBuf(unsigned char *txbuf_n)
{
	unsigned char res, i, ctrlval;
	unsigned char ctrlregs[MCP_N_TXBUFFERS] = { MCP_TXB0CTRL, MCP_TXB1CTRL, MCP_TXB2CTRL };
	
	res = MCP_ALLTXBUSY;
	*txbuf_n = 0x00;
	
	// check all 3 TX-Buffers
	for (i=0; i<MCP_N_TXBUFFERS; i++) {
		ctrlval = mcp2515_readRegister( ctrlregs[i] );
		if ( (ctrlval & MCP_TXB_TXREQ_M) == 0 ) {
			*txbuf_n = ctrlregs[i]+1; // return SIDH-address of Buffer
			res = MCP2515_OK;
			return res; /* ! function exit */
		}
	}
	
	return res;
}


/** CAN���߷������ݺ���
*  @param[in] msg CAN������Ϣ�ṹ��
*/
unsigned char can_sendMessage(CanMessage* msg)
{
	unsigned char res, txbuf_n;
	int i=0;
	//unsigned char timeout = 0;

	do {
		res = mcp2515_getNextFreeTXBuf(&txbuf_n); // info = addr.
		i++;
	} while (res == MCP_ALLTXBUSY && i<1000);

	
	if (res!=MCP_ALLTXBUSY) {
		mcp2515_write_canMsg( txbuf_n, msg);
		mcp2515_start_transmit( txbuf_n );
		return CAN_OK;
	}
	else {
		return CAN_FAILTX;
	}
}

/** ���ݷ��ͺ��� 
*  @param[in] value Ҫ���͵�����
*  @param[in] n		Ҫ���͵����ݵĳ���
*/
void CAN_Write(char * value,int n)
{
   CanMessage msg;
   can_initMessageStruct(&msg);
   msg.identifier = CANDEFAULTIDENT;
   msg.extended_identifier = CANDEFAULTIDENTEXT;
   msg.dlc = n;
   msg.rtr = 0;
   strncpy(msg.dta,value,n);
   	
   can_sendMessage(&msg);

}

/** CAN���߶����ݺ��� 
*/
void CAN_Read()
{
    
    unsigned char stat, res;
	CanMessage msg;
	
	stat = mcp2515_readStatus();
	
	
	if ( stat & MCP_STAT_RX0IF ) {
		// Msg in Buffer 0
		mcp2515_read_canMsg( MCP_RXBUF_0,&msg);
		mcp2515_modifyRegister(MCP_CANINTF, MCP_RX0IF, 0);
		res = CAN_OK;
	}
	else if ( stat & MCP_STAT_RX1IF ) {
		// Msg in Buffer 1
		mcp2515_read_canMsg( MCP_RXBUF_1,&msg);
		mcp2515_modifyRegister(MCP_CANINTF, MCP_RX1IF, 0);
		res = CAN_OK;
	}
	else {
		res = CAN_NOMSG;
	}	             
//    CONSOL_Printf(msg.dta);
//    CONSOL_Printf(" ");  
  
}

/** CAN���߳�ʼ������ 
*/
void CAN_Init(void)
{
    spi_init();
    mcp2515_Init(CAN_20KBPS);
    rGPGCON&=0xffffff3f;
    rGPGCON|=(1<<7);
    rGPGUP&=0xfff7;
    rGPGDAT|=(1<<3);
	rINTMSK &= ~(BIT_EINT8_23);
	rEINTMASK&=~(1<<11);
}


/** CAN�����ж���Ӧ���� 
*/
void __irq CAN_LISR(void)
{
    int temp;
    
    unsigned char stat, res;
	CanMessage msg;
	
	stat = mcp2515_readStatus();
	
	
	if ( stat & MCP_STAT_RX0IF ) {
		// Msg in Buffer 0
		mcp2515_read_canMsg( MCP_RXBUF_0,&msg);
		mcp2515_modifyRegister(MCP_CANINTF, MCP_RX0IF, 0);
		res = CAN_OK;
	}
	else if ( stat & MCP_STAT_RX1IF ) {
		// Msg in Buffer 1
		mcp2515_read_canMsg( MCP_RXBUF_1,&msg);
		mcp2515_modifyRegister(MCP_CANINTF, MCP_RX1IF, 0);
		res = CAN_OK;
	}
	else {
		res = CAN_NOMSG;
		mcp2515_modifyRegister(MCP_CANINTF, 0xff, 0x00);
	    temp=rEINTPEND;
	    rEINTPEND=temp;
		temp = rSRCPND;
		rSRCPND = temp;
		temp = rINTPND;
		rINTPND = temp;
		
	    rGPGDAT|=(1<<3);
	    return;
	}	     
    strcpy(messagebuf,msg.dta);
    messagebuf[msg.dlc]='\0';
    
 //   CONSOL_Printf(messagebuf);
 //   PrintTextEdit("�յ���Ϣ->$",RGB(255,255,0),65,150,280,40,1);
	PrintTextEdit(messagebuf,RGB(255,255,0),65,150,280,40,1);
/*	
	for (i=0; i<CAN_MAX_CHAR_IN_MESSAGE; i++)
	{
		if (msg.dta[i] == 0)
		{
			i = CAN_MAX_CHAR_IN_MESSAGE;
		}
		else
		{
			if (RxMsgBufW >= 256)
				RxMsgBufW = 0;
			
			RxMsgBuf[RxMsgBufW] = msg.dta[i];
			RxMsgBufW++;

		}
	}
*/
    
    temp=rEINTPEND;
    rEINTPEND=temp;
	temp = rSRCPND;
	rSRCPND = temp;
	temp = rINTPND;
	rINTPND = temp;
	
    rGPGDAT|=(1<<3);
 
}

int ReadForRxMsgBuf(char *msg)
{
	int i = 0;
	
	for(i=0; ;i++)
	{
	
		if (RxMsgBufR >= 256)
			RxMsgBufR = 0;
		
		if (RxMsgBufR < RxMsgBufW)
		{
			msg[i] = RxMsgBuf[RxMsgBufR];
			RxMsgBufR++;
		}
		else
		{
			if (i > 0)
				return TRUE;
			return FALSE;
		}		
	}
	return FALSE;
}