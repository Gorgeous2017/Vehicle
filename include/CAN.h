/*!	\file		CAN.h
	\brief		CAN�����շ�����ͷ�ļ�
	\author		����
	\version	0.1
*/
#ifndef	CAN_H
#define CAN_H


#define CAN_MAX_CHAR_IN_MESSAGE (8)	//!<MAX_CHAR_IN_MESSAGE

#define CAN_OK         (0)	//!<OK
#define CAN_FAILINIT   (1)	//!<FAILTX
#define CAN_FAILTX     (2)	//!<FAILTX
#define CAN_MSGAVAIL   (3)	//!<MSGAVAIL
#define CAN_NOMSG      (4)	//!<NOMSG
#define CAN_CTRLERROR  (5)	//!<CTRLERROR
#define CAN_FAIL       (0xff)	//!<FAIL

#define CAN_STDID (0)	//!<STDID
#define CAN_EXTID (1)	//!<EXTID

#define CANDEFAULTIDENT    (0x1111)	//!<DEFAUTIDENT
#define CANDEFAULTIDENTEXT (0x0)	//!<DEFAULTIDENTEXT

// �Զ�������
#define RED_LIGHT_OPEN			101
#define RED_LIGHT_CLOSE			102
#define GREEN_LIGHT_OPEN		103
#define GREEN_LIGHT_CLOSE		104
#define BLUE_LIGHT_OPEN			105
#define BLUE_LIGHT_CLOSE		106

extern char CAN_Message[8];



typedef struct {
	
	unsigned char  extended_identifier;  /*!< identifier CAN_xxxID */ 
	
	unsigned int identifier; /*!< either extended (the 29 LSB) or standard (the 11 LSB) */ 
	
	unsigned int  dlc;	//!< data length:
	char  dta[CAN_MAX_CHAR_IN_MESSAGE];
	
	//!<used for receive only:
	//!< Received Remote Transfer Bit 
	//!< (0=no... 1=remote transfer request received)
	unsigned char  rtr;  
	
	unsigned char  filhit;	//!<Acceptence Filter that enabled the reception
} CanMessage;



#define CAN_20KBPS   1				//!<CAN���߱�����20k
#define CAN_125KBPS  CAN_20KBPS+1	//!<CAN���߱�����125k

/** ���ݷ��ͺ��� 
*  @param[in] value Ҫ���͵�����
*  @param[in] n		Ҫ���͵����ݵĳ���
*/
void CAN_Write(char * value,int n);
/** CAN���߳�ʼ������ 
*/
void CAN_Init(void); //CAN���߳�ʼ��
/** CAN�����ж���Ӧ���� 
*/
void __irq CAN_LISR(void); 

/** CAN���߷������ݺ��� 
*  @param[in] msg �����͵�CanMessage�ṹ��
*  @param[out] ���������Ƿ�ɹ�״̬
*/
unsigned char can_sendMessage(CanMessage* msg);

int ReadForRxMsgBuf(char *msg);
void can_initBuf(void);

#endif


/*!	\mainpage	CAN����ʵ��
	\author		 ����
	\version 1.0
	\date    2006-9-1
	\section ABSTRACT ��Ҫ
        \n  ����̨Ƕ��ʽʵ���豸֮��ʵ��������վ����ģʽ����վ�ʹ�վ֮�������Ϣ���ݵĻ��ഫ�У���վ�Դ�վ��Զ�̿��ƣ�
        \n  ��վʵ���豸״̬�ĸı���Լ�ʱ�ķ�ӳ����վ������豸�ϡ�
	\section HARDWARE Ӳ��
	\n   2410ʵ���䡢CAN������չ��(MCP2515,MCP2551)
	\section SOFTERWARE ���
	\n   DEMOʵ��ƽ̨
*/	
/*!	\defgroup	Introduction   ʹ��˵��
        \nÿһ��S3C2410ʵ��嶼����ѡ����վ/��վģʽ��ͨ�����������Ի水ť��ѡ�񣩡�������ʵ������/��վģʽѡ������Ժ�����ʵ�����CPU��CAN���߿�������CAN�����շ������豸�ĳ�ʼ���ͻ������ɡ�����ʵ���ֱ������վ���վ���ڡ�
	\image html images\main.gif
	\n��վ����
	\n"��"�����һ�£���"��ֵ��ʾ��"�е�ԭ����ֵ��1���ڴ�վʵ����"��ֵ��ʾ��"��ʾ��
	\n"��"�����һ�£���"��ֵ��ʾ��"�е�ԭ����ֵ��1���ڴ�վʵ����"��ֵ��ʾ��"��ʾ��
	\n��ƿ���ť��������վʵ����ϵĺ�ƣ������ڴ�վ�����к���Ա���ʾ"��"��
	\n��ƹذ�ť��Ϩ���վʵ����ϵĺ�ƣ������ڴ�վ�����к���Ա���ʾ"��"��
	\n�̵ƿ���ť��������վʵ����ϵ��̵ƣ������ڴ�վ�������̵��Ա���ʾ"��"��
	\n�̵ƹذ�ť��Ϩ���վʵ����ϵ��̵ƣ������ڴ�վ�������̵��Ա���ʾ"��"��
	\n�����ı���������ʾ��վʵ����ϵ�һЩ������¼�����磺�ⲿ�жϼ����µȡ�

	\image html images\master.gif
	\n��վ����
	\n"��"�����һ�£���"��ֵ��ʾ��"�е�ԭ����ֵ��1������վʵ����"��ֵ��ʾ��"��ʾ��
	\n"��"�����һ�£���"��ֵ��ʾ��"�е�ԭ����ֵ��1������վʵ����"��ֵ��ʾ��"��ʾ��
	\n�����ı���������ʾ��վ����ʵ����ϵ�һЩ������¼�����磺�ⲿ�жϰ��µȡ�
	
	\image html images\slave.gif

*/
/*!	\defgroup CONSTRUCTION   ��CAN���ߡ����ģ��ṹͼ
	\n	CAN����ʵ���Ϊ3��ģ�飺Ӧ�ò�ģ�飬SPI��дģ���MCP2515��������������ģ��֮��ĵ��ù�ϵ��ͼ��ʾ��	
	\image html images\construction.gif

*/
/*!	\defgroup SPI   SPI��дģ��
	\n	SPI��������unsigned char ReadSPI( void )
        \n           ���أ���SPI0�ӿڶ�ȡһ���ֽڵ�����
	\n	ReadSPI��������ͼ���£�
	\image html images\spiread.gif
	\n	SPIд������void WriteSPI(unsigned char DataOut)
        \n           ���룺Ҫͨ��SPI0�ӿ������һ�ֽ�����
	\n	WriteSPI��������ͼ���£�
	\image html images\spiwrite.gif
*/
/*!	\defgroup MCP2515   MCP2515��������
	\n	MCP2515оƬ��ʼ��������unsigned char mcp2515_Init(const unsigned char canSpeed)
        \n      ���룺MCP2515��λ����
        \n	�����MCP2515��ʼ���ɹ�����״̬
	\n	mcp2515_Init��������ͼ���£�
	\image html images\mcp2515init.gif
	
	\n	MCP2515���뷢�ͻ�����������unsigned char mcp2515_getNextFreeTXBuf(unsigned char *txbuf_n)
        \n      ���룺һ���ַ����������뵽�ķ��ͻ����������
	\n	�����MCP2515���뷢�ͻ������ɹ�����״̬
	\n	mcp2515_getNextFreeTXBuf��������ͼ���£�
	\image html images\mcp2515_getNextFreeTXBuf.gif
	
	\nMCP2515д���ͻ�����������void mcp2515_write_canMsg(unsigned char buffer_sidh_addr, CanMessage * msg)
	\n���룺buffer_sidh_addr--���ͻ�������ַ    Msg--Ҫ��CAN�����Ϸ��͵���Ϣ�Ľṹ��
	\n���ܣ���Ҫ���͵����ݵ����ݣ����ݵĳ��ȣ�һ֡���ݵı�ʶ���ȷֱ���뷢�ͻ�����n����Ӧ���׼Ĵ����С�
	
	\n
	\n  MCP2515����һ֡���ݷ��ͺ�����void mcp2515_start_transmit(const unsigned char buffer_sidh_addr)
	\n���룺buffer_sidh_addr--���ͻ�������ַ
	\n���ܣ����ݷ��ͻ������ĵ�ַ��������Ӧ��SPI  RTS�����������ǰ���뻺�����е�һ֡���ݵķ��͡�
	\n ������		RTS����
	\n���ͻ�����0		0x81
	\n���ͻ�����1		0x82
	\n���ͻ�����2		0x84
	
	\n	MCP2515��״̬�Ĵ��������� unsigned char mcp2515_readStatus(void)
        \n���أ�״̬�Ĵ����е���ֵ
	\n	mcp2515_readStatus��������ͼ���£�
	\image html images\mcp2515_readStatus.gif
	
	\n	MCP2515��һ֡���ݺ�����void mcp2515_read_canMsg(unsigned char buffer_sidh_addr,CanMessage * msg)
	\n���룺buffer_sidh_addr--���ջ�������ַ    Msg--��Ž��յ���һ֡���ݵ���Ϣ�ṹ��
	\n���ܣ����ݽ��ջ�������ַbuffer_sidh_addr���ӽ��ջ�����RXBn����ؼĴ����ж�ȡ��Ҫ��һ֡�������ݵ���Ϣ�ṹ��msg�С�
	\n
	\n	MCP2515�Ĵ���λ�޸ĺ�����void mcp2515_modifyRegister(const unsigned char address, const unsigned char mask, const unsigned char data)
	\n���룺address--Ҫ�޸ĵļĴ����ĵ�ַ
	\n      Mask--�Ĵ�����Ҫ�޸ĵ���Ӧλ�����롣�����ο�����2��
	\n      Data--��Ҫ�޸ĵ�����
	\image html images\mcp2515_modifyRegister.gif	

*/
/*!	\defgroup APPLICATION   Ӧ�ò�ģ��
	\n	CAN������Ϣ�ṹ�壺
	\n	typedef struct {
	\n		unsigned char  extended_identifier; //��չ��ʶ��
	\n		unsigned int identifier; //��ʶ��
	\n		unsigned int  dlc;	//�������ݳ���
	\n		char  dta[CAN_MAX_CHAR_IN_MESSAGE];//������������
	\n		unsigned char  rtr;  //�Ƿ�ΪԶ�̷�������
	\n		unsigned char  filhit;	//Acceptence Filter that enabled the reception
	\n	} CanMessage;

 	\n	CAN���߷������ݺ�����unsigned char can_sendMessage(CanMessage* msg)
        \n      ���룺msg--�����͵�CanMessage�ṹ��
        \n	��������������Ƿ�ɹ�״̬
	\n	WriteSPI��������ͼ���£�
	\image html images\can_sendMessage.gif
	
	\n	���ݷ��ͺ�����void CAN_Write(char * value, int n)
        \n      ���룺value--���͵�����    n--���͵����ݳ���
        \n	���ܣ������������value��n��ʼ��һ���µ�CanMessage�ṹ�壬Ȼ�����can_sendMessage(CanMessage* msg)��������һ֡���ݡ�

 	\n	CAN�����жϷ������void __irq CAN_LISR(void)
	\n	CAN_LISR��������ͼ���£�
	\image html images\CAN_LISR.gif

	
*/
