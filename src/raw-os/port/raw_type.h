#ifndef RAW_OS_TYPE_H
#define  RAW_OS_TYPE_H

typedef  void       		 RAW_VOID; /*void*/
typedef  signed char         RAW_S8;                        /*  8-bit character                                     */
typedef  unsigned  char      RAW_BOOLEAN;                     /*  8-bit boolean or logical                  */
typedef  unsigned  char      RAW_U8;                      /*  8-bit unsigned integer                      */

typedef  unsigned  short     RAW_U16;                      /* 16-bit unsigned integer                         */
typedef  signed  short       RAW_S16;                      /* 16-bit   signed integer                           */
typedef  unsigned  int       RAW_U32;                      /* 32-bit unsigned integer                    */
typedef  signed int          RAW_S32;                      /* 32-bit   signed integer     */
typedef  unsigned long long  RAW_U64;                      /* 64-bit   signed integer     */

typedef  unsigned int        PORT_STACK;                      /* 32-bit   signed integer     */

typedef  RAW_U32  CPU_DATA;

typedef            void     (*CPU_FNCT_VOID)(void);             /* See Note #2a.                                        */
typedef            void     (*CPU_FNCT_PTR )(void *);           /* See Note #2b.                                        */

void        CPU_IntDis       (void);
void        CPU_IntEn        (void);

void        CPU_IntSrcDis    (RAW_U8  pos);
void        CPU_IntSrcEn     (RAW_U8  pos);

#define RAW_INLINE  		 static __inline

#endif

