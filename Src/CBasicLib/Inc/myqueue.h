/*
************************************************************************************************************************
*
************************************************************************************************************************
*/

#ifndef _MY_QUEUE_
#define _MY_QUEUE_

#include "CBasicTypeDefine.h"

#pragma pack(1)

typedef struct  
{
	unsigned int size;
	unsigned char *buff;
}myevent;

#define ST_MYEVENT_SIZE		sizeof(myevent)

struct queueitem
{
	myevent event;
	struct queueitem *next;
};

typedef struct 
{
	struct queueitem *head;
	struct queueitem *tail;
	u8 item_num;
}myqueue;

#pragma pack()

unsigned char myQueueCreate(myqueue *queue);
unsigned char myQueueWrite(myqueue *queue, myevent *src);
unsigned char myQueueRead(myevent *dest, myqueue *queue);
	
#endif 
