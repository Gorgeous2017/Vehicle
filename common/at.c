#include "at.h"
#include "consol.h"

//extern int flag;

void call_demo(char* phonestr)
{
	//char receive;
	char phone[100];
	int  i=3;
	
	phone[0] ='a';
	phone[1] ='t';
	phone[2] ='d';
	
	while(*phonestr!='\0')
	{
		phone[i++] = *phonestr ++;
	}

	phone[i++] = ';';
	phone[i++] = '\xd';
	phone[i++] = '\xa';
	phone[i] = '\0';
	CONSOL_Select(0);
        CONSOL_Init(115200);
	CONSOL_Printf(phone);
        CONSOL_Select(1);
        CONSOL_Init(115200);

	
	
}

void disconnect_phone(void)
{	
    CONSOL_Select(0);
    CONSOL_Init(115200);		
    CONSOL_Printf("ATH\xd");
    CONSOL_Select(1);
    CONSOL_Init(115200);   
}

void sendmessage_demo(char* phonestr)
{
	//char receive;
	char phone[100];
	//char message[100];
	int  i=8;
	phone[0] ='a';
	phone[1] ='t';
	phone[2] ='+';
	phone[3] ='c';
	phone[4] ='m';
	phone[5] ='g';
	phone[6] ='s';
	phone[7] ='=';

	while(*phonestr!='\0')
	{
		phone[i++] = *phonestr ++;
	}

	phone[i++] = '\xd';
	phone[i++] = '\xa';	
	phone[i] = '\0';
	//����AT����
	CONSOL_Select(0);
        CONSOL_Init(115200);	
	CONSOL_Printf(phone);
	CONSOL_Printf("hello\x1a");
	CONSOL_Select(1);
        CONSOL_Init(115200);   
	
}	



void call(void)
{
	char receive;
	char phone[100];
	int  i=3;

	CONSOL_Printf("\n\r\n\r������绰���루���з���������");
	phone[0] ='a';
	phone[1] ='t';
	phone[2] ='d';
	
	while(!(receive=CONSOL_GetCh()));
	while(receive!='\xd')
	{
		CONSOL_SendCh(receive);
		if((receive<='9')&&(receive>='0'))
		{
			phone[i++] =receive;
			while(!(receive=CONSOL_GetCh()));

		}
		else 
		{
			CONSOL_Printf("\n\r\n\r����绰�����д�");
			return;
		}
	}
	phone[i++] = ';';
	phone[i++] = '\xd';
	phone[i] = '\0';
	CONSOL_Printf("\n\r");
	
	CONSOL_Select(0);
        CONSOL_Init(115200);
	CONSOL_Printf(phone);
        CONSOL_Select(1);
        CONSOL_Init(115200);



	CONSOL_Printf("\n\rcalling...,press x to ask for disconnection,press any key to continue.");	
	while(!(receive=CONSOL_GetCh()));
	if( receive =='x'){    /*ask for disconnection*/
	  CONSOL_Select(0);
    CONSOL_Init(115200);		
		CONSOL_Printf("ATH\xd");
		CONSOL_Select(1);
    CONSOL_Init(115200);   
	}
}
void sendmessage(void)
{
	char receive;
	char phone[100];
	//char message[100];
	int  i=8;

	CONSOL_Printf("\n\r\n\r��Ϣ����Ϊhello");
	CONSOL_Printf("\n\r������绰���루���з���������");
	phone[0] ='a';
	phone[1] ='t';
	phone[2] ='+';
	phone[3] ='c';
	phone[4] ='m';
	phone[5] ='g';
	phone[6] ='s';
	phone[7] ='=';
	while(!(receive=CONSOL_GetCh()));
	while(receive!='\xd')
	{
		CONSOL_SendCh(receive);
		if((receive<='9')&&(receive>='0'))
		{
			phone[i++] =receive;
			while(!(receive=CONSOL_GetCh()));

		}
		else 
		{
			CONSOL_Printf("\n\r\n\r����绰�����д�");
			return;
		}
	}

	phone[i++] = '\xd';
	phone[i++] = '\xa';	
	phone[i] = '\0';
	//����AT����
	
	CONSOL_Select(0);
        CONSOL_Init(115200);	
	CONSOL_Printf(phone);
	CONSOL_Printf("hello\x1a");
	CONSOL_Select(1);
        CONSOL_Init(115200);   


	
}	

void lift_telephone(void)
{
    CONSOL_Select(0);
    CONSOL_Init(115200);		
    CONSOL_Printf("ATH\xd");
    CONSOL_Select(1);
    CONSOL_Init(115200);   

	
}

void practice_AT(void)
{

}

