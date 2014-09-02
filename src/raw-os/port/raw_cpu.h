#ifndef RAW_CPU_H
#define RAW_CPU_H
                 
unsigned int OS_CPU_SR_Save(void);
void OS_CPU_SR_Restore(unsigned int sr);

                                                              
#define  RAW_SR_ALLOC()                 unsigned int  cpu_sr = 0

#define	 USER_CPU_INT_DISABLE()			{cpu_sr = OS_CPU_SR_Save();}

#define  USER_CPU_INT_ENABLE()			{OS_CPU_SR_Restore(cpu_sr);}

#endif




