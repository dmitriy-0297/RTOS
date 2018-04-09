#ifndef RTOS_API_H
#define RTOS_API_H

/****************************************/
/*           rtos_api.h                 */
/****************************************/

#define DeclareTask(TaskID,priority)\
    TASK(TaskID); \
enum {TaskID##prior=priority}

#define TASK(TaskID) void TaskID(void)

typedef void TTaskCall(void);

int ActivateTask(TTaskCall entry,int priority,char* name);
void TerminateTask(void);

int StartOS(TTaskCall entry,int priority,char* name);
void ShutdownOS();

void InitRes(int ResNum, char* name);
void PIP_GetRes(int ResNum);
void PIP_ReleaseRes(int ResNum);

#define DeclareEvent(ID) \
    const int Event_##ID = (ID)*(ID);
void SetEvent(int task, TEventMask mask);
void GetEvent(int task, TEventMask* mask);
void WaitEvent(TEventMask mask);
void ClearEvent (TEventMask mask);

#endif // RTOS_API_H
