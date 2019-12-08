#include <string.h>
#include <stdarg.h>


#include "key.h"
#include "2410addr.h"
#include "GUI.h"

#define TRUE 	1   
#define FALSE 	0

extern int keyvalue;
extern char phone_num[30];
extern int  phone_index;
extern int a[4];
static volatile   int flag=0;
extern int gTimeIndex;
extern char gTimestr[20];


void __irq KB_LISR(void);
void keyinit(void);
int KeyScan(void);
void LedShow( int key,int i);
void KB_Enable(int flag);



void KBD_Entry()
{
    int i;
   
    keyinit();     
    KB_Enable(TRUE);                      //�ж�ʹ�ܺ���
}
   

void swap(int b[])
{
    int temp,temp1,temp2;
    temp=b[0];
    temp1=b[1];
    temp2=b[2];
    
    b[1]=temp;
    b[2]=temp1;
    b[3]=temp2;
         
}
void drawtimestr(char* ptimestr)
{
	char tempTimeStr[6];
	char timeStr[20];
	char weekStr[7];
	int i;
	strcpy(tempTimeStr,ptimestr);
	for(i=gTimeIndex;i<5;i++)
	 tempTimeStr[i] = '0';
	sprintf(timeStr,"%c%c:%c%c",tempTimeStr[0],tempTimeStr[1],tempTimeStr[2],tempTimeStr[3]);
	fillrectangle(118,120,160,138,rgb(0,0,255));
	drawascii168(120,138,timeStr,rgb(255,255,0));
	
	if(gTimeIndex>=2) i = gTimeIndex+1;
		else i = gTimeIndex ;
	if(i > 4) i=4;
	if(gTimeIndex <4 ) line(120+i*8,122,120+(i+1)*8,122,rgb(255,0,0));
	
	if(tempTimeStr[4] == '1')  strcpy(weekStr,"����һ");
	else if(tempTimeStr[4] == '2')  strcpy(weekStr,"���ڶ�");
	else if(tempTimeStr[4] == '3')  strcpy(weekStr,"������");
	else if(tempTimeStr[4] == '4')   strcpy(weekStr,"������");
	else if(tempTimeStr[4] == '5')   strcpy(weekStr,"������");
	else if(tempTimeStr[4] == '6')   strcpy(weekStr,"������");
	else    strcpy(weekStr,"������");
	
	fillrectangle(178,120,230,140,rgb(0,0,255));	
	drawtext(180,138,weekStr,1,0,rgb(255,255,0));			
	if(gTimeIndex == 4) line(180,120,230,120,rgb(255,0,0));
}

void __irq KB_LISR(void)
{
    int value,i,j;

    value=KeyScan();                      //ɨ�����
    
    
    for(i=0;i<3000;i++);
    if(value>9&&value <15) 		//��������
    {
   	
      rGPGCON = rGPGCON&~(0x300000)|(0x200000);  //����GPG10=EINT18
      rGPECON=rGPECON&~(0X3FC00)|(0x15400);                //GPE[5:8]=OUTPUT
      rGPEUP=0x3fe0;                      //�ر���������
      rGPEDAT=rGPEDAT&0xfe1f;     //GPE[5:8]��ʼΪ�͵�ƽ 
   
      rEINTPEND	|=0x40000;                      //�ж�Դ����ر�
      rSRCPND=0x20;                        //�ж�Դ����ر�
      rINTPND=0X20;
	return;
	}
    swap(a);
    a[0]=value;						      //led��ʾ��ֵ    // 
    
    keyvalue = value;
    
    if(!(win_ID == CALL_WIN || win_ID == MESSAGE_WIN ))	/*LedShow(keyvalue,i)*/; 

    //���Ŀǰ���洦��GPRS����Ӧִ��if������
   if(win_ID == CALL_WIN || win_ID == MESSAGE_WIN )
   	{
 
 		for(i=0;i<3000;i++);	  	
 	  	//�ر���ʾ
 	  	for(i=0;i<4;i++)  	  	LedShow(15,i);
 	  	//��ӵ绰����
	   	if(keyvalue>=0&& keyvalue<=9)
	   	  {
  	   		 if(phone_index<15)
  	   		   	   		 phone_num [phone_index++] = '0'+ keyvalue;
	   		  
 	  		 phone_num [phone_index] ='\0';			//����ַ�����β��
 			 drawascii168(200,148,phone_num,rgb(255,255,0));

   		  }

		if(keyvalue == 15) //ɾ���绰����
		{
 	   		 if(phone_index>0)
 	   		 {
 		   		 drawascii168(200,148,phone_num,rgb(0,0,255)); 
	      		  	phone_num [--phone_index] ='\0';
	 	   		 drawascii168(200,148,phone_num,rgb(255,255,0)); 
 	   		}
 		}		  	
  
   	}
    //���Ŀǰ���洦������ʱ�䴰�ڣ���Ӧִ��if������
   if(win_ID == TIME_CONFIG_WIN )
   	{
 
 		for(i=0;i<3000;i++);	  	
 	  	//�ر���ʾ
 	  	for(i=0;i<4;i++)  	  	LedShow(15,i);
 	  	//��ӵ绰����
	   	if(keyvalue>=0&& keyvalue<=9)
	   	  {
  	   		 if(gTimeIndex<5)
 	   	   		 gTimestr [gTimeIndex++] = '0'+ keyvalue;
	   		  
 	  		 gTimestr [gTimeIndex] ='\0';			//����ַ�����β��
 //			 drawascii168(200,148,gTimestr,rgb(255,255,0));

   		  }

		if(keyvalue == 15) //ɾ���绰����
		{
 	   		 if(gTimeIndex>0)
 	   		 {
//	 	   		 drawascii168(200,148,gTimestr,rgb(0,0,255)); 
	      		  	gTimestr [--gTimeIndex] ='\0';
//	 	   		 drawascii168(200,148,gTimestr,rgb(255,255,0)); 
	 	   	}
 		}
 		drawtimestr(gTimestr);		  	
  
   	}
  	
      rGPGCON = rGPGCON&~(0x300000)|(0x200000);  //����GPG10=EINT18
      rGPECON=rGPECON&~(0X3FC00)|(0x15400);                //GPE[5:8]=OUTPUT
      rGPEUP=0x3fe0;                      //�ر���������
      rGPEDAT=rGPEDAT&0xfe1f;     //GPE[5:8]��ʼΪ�͵�ƽ 
   
      rEINTPEND	|=0x40000;                      //�ж�Դ����ر�
      rSRCPND=0x20;                        //�ж�Դ����ر�
      rINTPND=0X20;

     

}


void keyinit(void)
{
    rGPECON=0xfffd57ff;                  //GPE[5:8]=OUTPUT
    rGPEUP=0x1e0;                        //�ر���������
    rGPEDAT=rGPEDAT&0xfe1f;              //GPE[5:8]��ʼΪ�͵�ƽ
    
    rGPGCON=rGPGCON&~(0x300000)|(0x200000);      //����GPG10=EINT18
    rGPGUP=rGPGUP|0x400;                   //�ر�GPG10�������� 
}

int KeyScan(void)
{
      short a,b,c,d,e,i,j;
      int temp=0;
      rGPEUP=0x1e0;
  

       rGPGCON=rGPGCON&~(0x300000)|(0x0);                    //����GPG10Ϊ����
       for(j=0;j<10;j++);									 //ȥ����
      while((rGPGDAT&(0x400))==0x400)
      {
         ;;
      }
      for(j=0;j<5000;j++);

      
    //  rGPECON=0xfffc03ff;
      
      //��һ�м��
      rGPECON=0xfffc07ff;                   //����GPE5=OUTPUT��GPE[6��8]=INPUT
      //rGPEDAT=rGPEDAT&~(0x1e0)|0x20;                          
      rGPEDAT=rGPEDAT&0xffdf;                        //����GPE5�͵�ƽ�����GPE[6��8]����
      for(i=0;i<=50;i++);                    //��ʱȥ����                     
      a=rGPEDAT&(0x100);                    //GPEDAT��GPE8λ
      b=rGPEDAT&(0x80);                     //GPEDAT��GPE7λ  
      c=rGPEDAT&(0x40);                     //GPEDAT��GPE6λ
      d=rGPEDAT&(0x20);                     //GPEDAT��GPE5λ
      e=rGPGDAT&(0x400);                      //GPGDAT��GPG10λ
     
      if(e==0x0)
      {
          if(((a>>6)+(b>>6)+(c>>6))==0x7)
              temp=1;
          else if(((a>>6)+(b>>6)+(c>>6))==0x6)
              temp=2;
          else if(((a>>6)+(b>>6)+(c>>6))==0x5)
              temp=3;
          else if(((a>>6)+(b>>6)+(c>>6))==0x3)
              temp=15;
      }
      else
      {
          //�ڶ��м��
          rGPECON=0xfffc13ff;                   //����GPE6=OUTPUT��GPE[5\7\8]=INPUT
          rGPEDAT=rGPEDAT&0xffbf;               //ʹGPE6����͵�ƽ
          for(i=0;i<=50;i++);                  //��ʱȥ����
          a=rGPEDAT&(0x100);                    //GPEDAT��GPE8λ
          b=rGPEDAT&(0x80);                     //GPEDAT��GPE7λ  
          c=rGPEDAT&(0x40);                     //GPEDAT��GPE6λ
          d=rGPEDAT&(0x20);                     //GPEDAT��GPE5λ
          e=rGPGDAT&(0x400);                       //GPGDAT��GPG10λ
          
          if(e==0x0) 
          {
              if(((a>>6)+(b>>6)+(d>>5))==0x6)    
                 temp=4;
              else if(((a>>6)+(b>>6)+(d>>5))==0x7)
                 temp=5;
              else if(((a>>6)+(b>>6)+(d>>5))==0x5) 
                 temp=6;
              else if(((a>>6)+(b>>6)+(d>>5))==0x3)
                 temp=14;
          }
          else
          {
              //�����м��
              rGPECON=0xfffc43ff;                   //����GPE7=OUTPUT��GPE[5\6\8]=INPUT
              rGPEDAT=rGPEDAT&0xff7f;                        //ʹGPE7����͵�ƽ
              for(i=0;i<=50;i++);                  //��ʱȥ����

              a=rGPEDAT&(0x100);                    //GPEDAT��GPE8λ
              b=rGPEDAT&(0x80);                     //GPEDAT��GPE7λ  
              c=rGPEDAT&(0x40);                     //GPEDAT��GPE6λ
              d=rGPEDAT&(0x20);                     //GPEDAT��GPE5λ
              e=rGPGDAT&(0x400);
              
              if(e==0x0) 
              {
                   if(((a>>6)+(c>>5)+(d>>5))==0x6)     
                      temp=7;
                   else if(((a>>6)+(c>>5)+(d>>5))==0x5)
                      temp=8;
                   else if(((a>>6)+(c>>5)+(d>>5))==0x7) 
                      temp=9;
                   else if(((a>>6)+(c>>5)+(d>>5))==0x3) 
                      temp=13;
              }
              else
              {
                   //�����м��
                   rGPECON=0xfffd03ff;            //����GPE8=OUTPUT��GPE[5:7]=INPUT
                   rGPEDAT=rGPEDAT&0xfeff;                //ʹGPE8����͵�ƽ
   //                for(i=0;i<=50;i++);                  //��ʱȥ����

                   a=rGPEDAT&(0x100);                    //GPEDAT��GPE8λ
                   b=rGPEDAT&(0x80);                     //GPEDAT��GPE7λ  
                   c=rGPEDAT&(0x40);                     //GPEDAT��GPE6λ
                   d=rGPEDAT&(0x20);                     //GPEDAT��GPE5λ
                   e=rGPGDAT&(0x400);
                  
                   if(((b>>5)+(c>>5)+(d>>5))==0x6)    
                        temp=10;
                   else if(((b>>5)+(c>>5)+(d>>5))==0x5)
                        temp=0;
                   else if(((b>>5)+(c>>5)+(d>>5))==0x3) 
                        temp=11;
                   else if(((b>>5)+(c>>5)+(d>>5))==0x7) 
                        temp=12;
                    
              }
          }        
      }
      
    /*  rEINTPEND=0x400;                      //�ж�Դ����ر�
      rSRCPND=0x20;                        //�ж�Դ����ر�
      rINTPND=0X20;
      rGPGCON = rGPGCON&~(0x30)|(0x20);  //����GPG2=EINT10
      rGPECON=0xfffd57ff;                //GPE[5:8]=OUTPUT
      rGPEUP=0x1e0;                      //�ر���������
      rGPEDAT=0x1e0;                     //GPE[5:8]��ʼΪ�ߵ�ƽ
     */
      for(j=0;j<10;j++);
      while((rGPGDAT&(0x400))!=0x400)
      {
         ;;
      }
      for(j=0;j<1000;j++);
      while((rGPGDAT&(0x400))!=0x400)
      {
         ;;
      }
      return temp;
}


void LedShow( int key,int i)
{
    int  j;
    int table[16]={0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb,0xc,0xd,0xe,0xf};
   //unsigned short dis_buf[3]={0x3,0x2,0x1,0x0}
   
   rGPHCON=rGPHCON&~(0x30000)|0x10000;            //���õ㣬����
   rGPHUP=rGPHUP|0x100;
   rGPHDAT=rGPHDAT&~(0x100)|0x0; 
   
   rGPECON=rGPECON&~(0xffc0000)|(0x5540000);  //����GPE[9��13]=OUTPUT
   rGPEUP|=0x3e00;
   rGPGCON=rGPGCON&~(0x30)|(0x10);             //����GPG2ΪOUTPUT
   rGPGUP|=0x4;
   
     rGPEDAT=rGPEDAT&~(0x3C00)|(table[15]<<10);
   
   switch(i)
   {
   case 0:
      rGPGDAT=rGPGDAT|(1<<2);                   //��������ܵ�λѡGPG2=1,A
      rGPEDAT=rGPEDAT|(1<<9);                   //��������ܵ�λѡGPE9=1,B
      break;
   case 1:
      rGPGDAT=rGPGDAT&~(1<<2);                   //��������ܵ�λѡGPG2=0,A
      rGPEDAT=rGPEDAT|(1<<9);                   //��������ܵ�λѡGPE9=1,B
      break;
   case 2:
      rGPGDAT=rGPGDAT|(1<<2);                   //��������ܵ�λѡGPG2=1,A
      rGPEDAT=rGPEDAT&~(1<<9);                   //��������ܵ�λѡGPE9=0,B
      break;
   case 3:
      rGPGDAT=rGPGDAT&~(1<<2);                   //��������ܵ�λѡGPG2=0,A
      rGPEDAT=rGPEDAT&~(1<<9);                   //��������ܵ�λѡGPE9=0,B
      break;
      }
  // if((key>0x0)&&(key<=0xf))

   rGPEDAT=rGPEDAT&~(0x3C00)|(table[15]<<10);
   for(j=0;j<50;j++);
    rGPEDAT=rGPEDAT&~(0x3C00)|(table[key]<<10);
   for(j=0;j<=1000;j++);  
   
  
 //   rGPGCON=rGPGCON&~(0x30);             //����GPG2ΪINPUT
 //   rGPGUP|=0x4;
  //  rGPECON=rGPECON&~(0xffc0000)|(0x5540000);  //����GPE[9]=INPUT
  //  rGPEUP|=0x3e00;
   }


void KB_Enable(int flag)
{
     int temp;
     if(flag == TRUE)
     {
           
            temp = rGPGCON;
 	        rGPGCON = temp&~(0x300000)|(0x200000);        //����GPG10=EINT18
 	        temp = rGPGUP;
 	        rGPGUP = temp|0x400;                    //�ر�GPG10�������� 
 	        
            rSRCPND=0x20;                         //�ж�Դ���� 
	        rINTMSK&= ~(BIT_EINT8_23);	          //�ж�Դʹ��
            temp=rEXTINT2;
            rEXTINT2&=0xfffff8ff;                 //ʹEINT18Ϊ�͵�ƽ����
            rEINTPEND=0X40000;                      //�ⲿ�ж�Դ����
            rEINTMASK=0xfbfff8;                   //ʹ���ⲿ�ж�EINT18
      }
      else
      {
	        rINTMSK&= BIT_EINT8_23;
            
      }		
      	
}


