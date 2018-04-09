
#include <stdio.h> #include "sys.h" #include "rtos_api.h"
Устанавливает для задачи события, задаваемые маской void SetEvent(int task, TEventMask mask) { printf("SetEvent %i for task %s\n", mask, TaskQueue[task].name); TaskQueue[task].working_events |= mask; if(TaskQueue[task].task_state == TASK_WAITING && (TaskQueue[task].working_events & TaskQueue[task].waiting_events)) { TaskQueue[task].waiting_events &= !mask; TaskQueue[task].working_events &= !mask; TaskQueue[task].task_state = TASK_READY; Dispatch(); } printf("End of SetEvent %i for task %s\n", mask, TaskQueue[task].name); }
Возвращает текущее состояние маски установленных событий для задачи void GetEvent(int task, TEventMask* mask) { *mask = TaskQueue[task].working_events; }
Задача переводится в состояние ожидания
void WaitEvent(TEventMask mask) { printf("WaitEvent %i to task %s\n", mask, TaskQueue[RunningTask].name); TaskQueue[RunningTask].waiting_events = mask; if((TaskQueue[RunningTask].working_events & mask) == 0) {//ждемс TaskQueue[RunningTask].task_state = TASK_WAITING; Dispatch(); } printf("End of WaitEvent %i to task %s\n", mask, TaskQueue[RunningTask].name); }
Очистка признаков событий (в маске установленных событий задачи) в соответствии с параметром маски void ClearEvent (TEventMask mask) { TaskQueue[RunningTask].working_events &= !mask; }
