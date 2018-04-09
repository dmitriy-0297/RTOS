/*************************************/
/*             resource.c            */
/*************************************/

#include "sys.h"
#include "rtos_api.h"
#include <stdio.h>

void InitRes(int ResNum, char* name) //инициализ ресурса
{
    printf("InitRes %s\n",name);
    ResourceQueue[ResNum].name = name;
    ResourceQueue[ResNum].task = -1;
}

void PIP_GetRes(int ResNum) //ресурс запрашиваемый задачей
{
    printf("PIP_GetRes %s\n",ResourceQueue[ResNum].name);
    while(ResourceQueue[ResNum].task != -1)
    {// ресурс заблокирован
        int rtask = ResourceQueue[ResNum].task;
        printf("Resource is blocked by %s\n", TaskQueue[rtask].name);
        if(TaskQueue[rtask].ceiling_priority <= TaskQueue[RunningTask].ceiling_priority)
        { //увеличив времен приоритет на 1
            printf("Inverse prioritet from %i to %i\n",
                   TaskQueue[rtask].ceiling_priority,
                   TaskQueue[RunningTask].ceiling_priority+1);
            TaskQueue[rtask].ceiling_priority =
                    TaskQueue[RunningTask].ceiling_priority+1;
            Schedule(rtask); //добавляем в очередь
        }
        Dispatch(); //переключение
    }
    ResourceQueue[ResNum].task = RunningTask; //ресурс занят
    printf("End of PIP_GetRes %s\n",ResourceQueue[ResNum].name);
}

void PIP_ReleaseRes(int ResNum) //ресурс подлежащий освобождению
{
    printf("PIP_ReleaseRes %s\n",ResourceQueue[ResNum].name);
    int rtask = ResourceQueue[ResNum].task;
    ResourceQueue[ResNum].task = -1; //сбрасываем временный приоритет до базового
    if(TaskQueue[rtask].ceiling_priority != TaskQueue[rtask].priority)
    {
        printf("Reset prioritet for %s from %i to %i\n",
               TaskQueue[rtask].name,
               TaskQueue[rtask].ceiling_priority,
               TaskQueue[rtask].priority);
        TaskQueue[rtask].ceiling_priority = TaskQueue[rtask].priority;
        Schedule(rtask);
    }
    Dispatch();
    //переход к след задаче
    printf("End of PIP_ReleaseRes %s\n",ResourceQueue[ResNum].name);
}
