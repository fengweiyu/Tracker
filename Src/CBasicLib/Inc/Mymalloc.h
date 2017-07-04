/*
************************************************************************************************************************
*
************************************************************************************************************************
*/

#ifndef _MY_MALLOC_
#define _MY_MALLOC_

#define MAX_MEMERY_SIZE   	(1080)

#define MEM_COMMON       		(0)
#define MEM_SPCIAL         		(1)

void MAB_MallocInit(void);
unsigned char *mymalloc(unsigned int Size);
void myfree(unsigned char *buff, unsigned int Size);

u8 MAB_ram12used(void);
u8 MAB_ram128used(void);
u8 MAB_ram256used(void);
u8 MAB_ram512used(void);
u8 MAB_ram1024used(void);
u8 MAB_ram2048used(void);

#endif

