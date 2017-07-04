/*
************************************************************************************************************************
*
************************************************************************************************************************
*/
#include "CBasicTools.h"
#include "Config.h"
#include "Myqueue.h"
#include "Mymalloc.h"
#include "Ucos_ii.h"
unsigned char myQueueCreate(myqueue *queue)
{
	queue->head = queue->tail = (struct queueitem *)mymalloc(sizeof(struct queueitem));
	if(NULL == queue->head)
	{
		DebugPrintf(ERR"queue create can't malloc!\n");
		return(0);
	}

	queue->head->next = queue->tail->next = NULL;
	queue->item_num = 0;
	return(1);
	
	/*
	queue->head = queue->tail = NULL;
	queue->item_num = 0;
	return(1);
	*/
}

unsigned char myQueueWrite(myqueue *queue, myevent *src)
{
	struct queueitem *tmp_ptr;
	OS_CPU_SR  cpu_sr = 0;
	
	tmp_ptr = (struct queueitem *)mymalloc(sizeof(struct queueitem));
	if(NULL == tmp_ptr)
	{
		DebugPrintf(ERR"queue write can't malloc!\n");
		return(0);
	}	

	OS_ENTER_CRITICAL();
	//OSSchedLock();
	tmp_ptr->event.buff = src->buff;
	tmp_ptr->event.size = src->size;
	tmp_ptr->next = NULL;

	queue->head->next = tmp_ptr;
	queue->head = tmp_ptr;
	queue->item_num++;
	OS_EXIT_CRITICAL();
	//OSSchedUnlock();
	
	return(1);

	/*
	struct queueitem *tmp_ptr;

	OSSchedLock();
	tmp_ptr = (struct queueitem *)mymalloc(sizeof(struct queueitem));
	if(NULL == tmp_ptr)
	{
		OSSchedUnlock();
		debugString("queue write can't malloc!\n");
		return(0);
	}
	tmp_ptr->event.buff = src->buff;
	tmp_ptr->event.size = src->size;
	
	if(queue->item_num == 0)
	{
		queue->tail = tmp_ptr;
	}
	else
	{
		queue->head->next = tmp_ptr;
	}
	queue->head = tmp_ptr;
	tmp_ptr->next = NULL;
	
	queue->item_num ++;
	OSSchedUnlock();
	
	return(1);
	*/
}

unsigned char myQueueRead(myevent *dest, myqueue *queue)
{
	struct queueitem *tmp_ptr;
	OS_CPU_SR  cpu_sr = 0;

	OS_ENTER_CRITICAL();
	//OSSchedLock();
	if((NULL == queue->tail->next) ||(queue->head == queue ->tail))
	{
		OS_EXIT_CRITICAL();
		//OSSchedUnlock();
		return(0);
	}

	tmp_ptr = queue->tail->next;
	myfree((unsigned char *)queue->tail, sizeof(struct queueitem));

	dest->size = tmp_ptr->event.size;
	dest->buff = tmp_ptr->event.buff;
	queue->tail = tmp_ptr;
	OS_EXIT_CRITICAL();
	//OSSchedUnlock();
	
	queue->item_num --;
	return(1);

	/*
	struct queueitem *tmp_ptr;

	if(queue->item_num == 0)
	{
		return(0);
	}
	
	OSSchedLock();
	tmp_ptr = queue->tail;
	
	dest->buff = tmp_ptr->event.buff;
	dest->size = tmp_ptr->event.size;
	queue->tail = tmp_ptr->next;
	myfree((unsigned char *)tmp_ptr, sizeof(struct queueitem));

	queue->item_num --;
	OSSchedUnlock();
	return(1);
	*/
}

