#ifndef SYS_H
#define SYS_H

/****************************************/
/*               sys.h                   /
/****************************************/

#include "defs.h"
#include <csetjmp>

enum TTaskState { TASK_RUNNING = 0, TASK_READY = 1, TASK_SUSPENDED = 2,
                  TASK_WAITING = 3 };
typedef unsigned int TEventMask;

typedef struct Type_Task
{
    int next, prev; //след и пред элем в стеке
    int priority; //чем меньше число, тем меньше приоритет
    int ceiling_priority; //переменная для сравнив с больш приорит
    void (*entry)(void);
    char* name;
    TTaskState task_state;
    int switch_count;
    jmp_buf context;
    TEventMask waiting_events;
    TEventMask working_events; //сработавшие события
} TTask;

typedef struct Type_resource
{
    int task;
    char* name;
} TResource;

extern jmp_buf InitStacks[MAX_TASK];
extern TTask TaskQueue[MAX_TASK];

extern TResource ResourceQueue[MAX_RES];
extern int RunningTask; //текущая выполняющаяся задача, изменять только в функции TaskSwitch
extern int TaskHead; //первая задача в списке, голова
extern int TaskCount;
extern int FreeTask;
extern jmp_buf MainContext; //главный контекст

void Schedule(int task, int dont_show = 0);
void Dispatch();

#endif // SYS_H
