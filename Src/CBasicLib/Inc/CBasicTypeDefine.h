
#ifndef C_BASIC_TYPE_DEFINE_
#define C_BASIC_TYPE_DEFINE_
#include"stdint.h"
#include"stddef.h"

typedef int32_t  s32;
typedef int16_t s16;
typedef char s8;

typedef const int32_t sc32;  /*!< Read Only */
typedef const int16_t sc16;  /*!< Read Only */
typedef const int8_t sc8;   /*!< Read Only */

typedef volatile int32_t  vs32;
typedef volatile int16_t  vs16;
typedef volatile int8_t   vs8;

typedef volatile const int32_t vsc32;  /*!< Read Only */
typedef volatile const int16_t vsc16;  /*!< Read Only */
typedef volatile const int8_t vsc8;   /*!< Read Only */

typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef const uint32_t uc32;  /*!< Read Only */
typedef const uint16_t uc16;  /*!< Read Only */
typedef const uint8_t uc8;   /*!< Read Only */

typedef volatile uint32_t  vu32;
typedef volatile uint16_t vu16;
typedef volatile uint8_t  vu8;

typedef volatile const uint32_t vuc32;  /*!< Read Only */
typedef volatile const uint16_t vuc16;  /*!< Read Only */
typedef volatile const uint8_t vuc8;   /*!< Read Only */

#ifndef NULL
#define NULL   0
#endif


#ifndef FALSE
#define FALSE   0
#endif

#ifndef TRUE
#define TRUE   1
#endif


typedef struct
{
	u8 year;
	u8 month;
	u8 day;
}date_t;
typedef struct
{
	u8 hour;
	u8 minute;
	u8 second;
}time_t;
typedef struct
{
	date_t date;
	time_t time;
}datetime_t;

typedef struct
{
	datetime_t b_time;
	datetime_t e_time;
}BETime_t;


#endif


