#ifndef PORT_IDLE_CONFIG_H

#define PORT_IDLE_CONFIG_H


/*if 1 fsm is activated, otherwise hsm is activated*/
#define RAW_FSM_ACTIVE 1


/*Max IDLE_EVENT_TASK is 8*/
#define MAX_IDLE_EVENT_TASK 2




#if (MAX_IDLE_EVENT_TASK >= 9)
#error  "Max IDLE_EVENT_TASK is 8"
#endif

#endif
