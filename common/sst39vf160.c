
#include "typDef.h"
#include "sst39vf160.h"
#include "2410lib.h"

#define SST_START_ADDR          0x08000000
#define SST_CHIP_HWORD_SIZE     0x100000                 /*1M Hwords*/
#define SST_SECTOR_HWORD_SIZE   0x800                 /*2k HWords*/

#define SST_ADDR_UNLOCK1        0x5555
#define SST_ADDR_UNLOCK2        0x2aaa
#define SST_DATA_UNLOCK1        0xaaaa
#define SST_DATA_UNLOCK2        0x5555
#define SST_SETUP_WRITE         0xa0a0
#define SST_SETUP_ERASE         0x8080
#define SST_CHIP_ERASE          0x1010
#define SST_SECTOR_ERASE		0x3030


/******************************************************
��������:       sstOpOverDetect()
��������:       ����poll��ʽ���flash��д�Ƿ����.
��ڲ���:       ptr         ����д���ַ/����������ַ.
				trueData    Ҫд���ֵ.
				timeCounter ��ʱ����.
�� �� ֵ:       OK		�����ɹ�.
				ERROR	����ʧ��.
��    ע:       ��Ԥ��ʱ�������d7,d6�Բ���truedata,�򷵻�
				ERROR.
*******************************************************/    
STATUS  sstOpOverDetect(UINT16 *ptr,  UINT16 trueData, ULONG timeCounter)
{
	ULONG timeTmp = timeCounter;
	volatile UINT16 *pFlash = ptr;
	UINT16 buf1, buf2,curTrueData;
	
	curTrueData = trueData & 0x8080;                  //�ȼ��d7λ.
	while((*pFlash & 0x8080) != curTrueData)
	{
		if(timeTmp-- == 0) break;
	}
	
	timeTmp = timeCounter;
	buf1 = *pFlash & 0x4040;						  //(Ϊ����)�ټ��d6λ.
	while(1)
	{
		    buf2  = *pFlash & 0x4040;
		    if(buf1 == buf2) 
				break;
		    else
			    buf1 = buf2;
		    if(timeTmp-- == 0) 
		    {
				return ERROR;
		    }
	}

	return OK;
}

/********************************************************
��������:       sstWrite()
��������:       ��ȡ���������ݸ��ݸ����ĳ���д��ָ����ַ.
��ڲ���:       flashAddr    ����Ŀ���ַ(flash).
				buffer       ����Դ��ַ.
				length 		 Ҫд����ֽ���.
				
�� �� ֵ:      	NULL		 дʧ��.
				flashPtr     flash����һ����ַ.
��    ע:       ����sst39vf160ֻ�ܰ�����(16bit)����,����
                ���Ҫ��ε������������д��һ���ļ�,��Ӧ
                ÿ�ζ�ȡż�����ֽ�,�Ա�֤������.
*********************************************************/    

UINT16 *sstWrite(UINT16 *flashAddr, UINT8 *buffer, ULONG length)
{
	ULONG i, cLength;
	volatile UINT16 *flashPtr;
	volatile UINT16 *gBuffer;
	
    flashPtr = flashAddr;
	cLength = (length + 1)/2;				//������ֳ���.
	gBuffer = (UINT16 *)buffer;      
	
	while (cLength > 0) 
	{
		*((volatile UINT16 *)SST_START_ADDR + SST_ADDR_UNLOCK1) = SST_DATA_UNLOCK1;          //����.
		*((volatile UINT16 *)SST_START_ADDR + SST_ADDR_UNLOCK2) = SST_DATA_UNLOCK2;
    	*((volatile UINT16 *)SST_START_ADDR + SST_ADDR_UNLOCK1) = SST_SETUP_WRITE;
		*flashPtr = *gBuffer;                                                    //д������.
		
		if(sstOpOverDetect((UINT16 *)flashPtr, *gBuffer, 0x1000))             //���д���Ƿ�ɹ�.
		{
			//printf("warning: write flash may failed at:0x%x.\n", (int)flashPtr);
		}
		cLength--;
		flashPtr++;
		gBuffer++;
	}	
	
    flashPtr = flashAddr;
	gBuffer = (UINT16 *)buffer; 
	cLength = length/2;
	for(i=0; i<cLength; i++)			//д�������ȫ��У��һ��.
	{
	    if(*flashPtr++ != *gBuffer++)
	    {    
	        //printf("Error: write failed in SST39vf160 at 0x%x on verification.\n", (int)flashPtr);
	        return NULL;
	    }
	}
	if(length%2)
	{
	    if((*flashPtr++ & 0x00ff) != (*gBuffer++ & 0x00ff))             /*�������ȵ����һ���ֽ�.*/
	    {
	        //printf("Error: write failed in SST39vf160 at 0x%x on verification.\n", (int)flashPtr);
	        return NULL;	    
	    }
	}
	return (UINT16 *)flashPtr;
}

/********************************************************
��������:       sstChipErase()
��������:       ��������flashоƬ.
��ڲ���:       ��.
				
�� �� ֵ:      	OK		 ������ȫ��ȷ.
				ERROR    �е�Ԫ������ȷ����.
��    ע:       
*********************************************************/    
STATUS sstChipErase(void)
{
    int i;
    volatile UINT16 *flashPtr = NULL;
   	*((volatile UINT16 *)SST_START_ADDR + SST_ADDR_UNLOCK1) = SST_DATA_UNLOCK1;			//��������.
	*((volatile UINT16 *)SST_START_ADDR + SST_ADDR_UNLOCK2) = SST_DATA_UNLOCK2;
	*((volatile UINT16 *)SST_START_ADDR + SST_ADDR_UNLOCK1) = SST_SETUP_ERASE;
	*((volatile UINT16 *)SST_START_ADDR + SST_ADDR_UNLOCK1) = SST_DATA_UNLOCK1;
	*((volatile UINT16 *)SST_START_ADDR + SST_ADDR_UNLOCK2) = SST_DATA_UNLOCK2;
	*((volatile UINT16 *)SST_START_ADDR + SST_ADDR_UNLOCK1) = SST_CHIP_ERASE;			//д���������.

	flashPtr = (volatile UINT16 *)SST_START_ADDR;			  

	if(sstOpOverDetect((UINT16 *)flashPtr, 0xffff, 0x3000000) != OK)
	{
		//printf("warning: Chip Erase time out!\n");	
	}	
	
	flashPtr = (volatile UINT16 *)SST_START_ADDR;			  
	for(i=0; i<SST_CHIP_HWORD_SIZE; i++,flashPtr++)  					//У���Ƿ�ȫΪ0xffff.
	{
	   if(*flashPtr != 0xffff)
	   {
	    	//printf("Debug: Erase failed at 0x%x in SST39VF160 on verification.\n", (int)flashPtr);
		    return ERROR;
       }
	}
	return OK;
}    

/********************************************************
��������:       sstSectorErase()
��������:       ����ָ����flash����.
��ڲ���:       ������ַ.
				
�� �� ֵ:      	OK		 ������ȫ��ȷ.
				ERROR    �е�Ԫ������ȷ����.
��    ע:       
*********************************************************/    
STATUS sstSectorErase(UINT16 *pSector)
{
    int i;
    volatile UINT16 *flashPtr = pSector;
    
	*((volatile UINT16 *)SST_START_ADDR + SST_ADDR_UNLOCK1) = SST_DATA_UNLOCK1;			//��������.
	*((volatile UINT16 *)SST_START_ADDR + SST_ADDR_UNLOCK2) = SST_DATA_UNLOCK2;
	*((volatile UINT16 *)SST_START_ADDR + SST_ADDR_UNLOCK1) = SST_SETUP_ERASE;
	*((volatile UINT16 *)SST_START_ADDR + SST_ADDR_UNLOCK1) = SST_DATA_UNLOCK1;
	*((volatile UINT16 *)SST_START_ADDR + SST_ADDR_UNLOCK2) = SST_DATA_UNLOCK2;
	*(volatile UINT16 *)flashPtr = SST_SECTOR_ERASE;			//д���������.

	if(sstOpOverDetect((UINT16 *)flashPtr, 0xffff, 0x20000) != OK)
	{
		//printf("warning: Chip Erase time out!\n");	
	}	
	
	for(i=0; i<SST_SECTOR_HWORD_SIZE; i++,flashPtr++)  					//У���Ƿ�ȫΪ0xffff.
	{
	   if(*flashPtr != 0xffff)
	   {
	    	//printf("Debug: Erase failed at 0x%x in SST39VF160 on verification.\n", (int)flashPtr);
		    return ERROR;
       }
	}
	return OK;
} 



