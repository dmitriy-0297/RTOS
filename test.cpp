/*******************************/
/*           test.c            */
/*******************************/

#include <stdio.h>
#include <stdlib.h>
#include "rtos_api.h"
DeclareTask(Task1,1); //декларация, объявление задач
DeclareTask(Task2,2);
DeclareTask(Task3,3);
DeclareResource(Res1,5); //объявление рисурсов
DeclareResource(Res2,5);
DeclareResource(Res3,5);
DeclareEvent(1);
DeclareEvent (2);
DeclareEvent (3);

int main(void) //активкация первой задачи
{
    printf("Hello!\n");
    // StartOS(Task1,Task1prior,"Task1");
    StartOS(Task6,Task6prior,"Task6");
    ShutdownOS();
    return 0;
}

TASK(Task1)
{
    printf("Start Task1\n");
    ActivateTask(Task2,Task2prior,"Task2");
    printf("Task1\n");
    TerminateTask();
}

//Работа с событиями
int Task7_ID;
TASK(Task7)
{
    printf("Start Task7\n");
    Task7_ID = RunningTask;
    ActivateTask(Task8,Task8prior,"Task8");
    ClearEvent(Event_1);
    WaitEvent(Event_1);
    TEventMask evnt;
    GetEvent(Task7_ID, &evnt);
    printf("Task7 GetEvent=%i\n", evnt);
    printf("Task7\n"); TerminateTask();
}


