/******************************/
/*          os.c              */
/******************************/

#include <stdio.h>
#include "sys.h"
#include "rtos_api.h"

//Инициализация стеков
void InitializeStacks(int numStack)
{
    char cushionSpace[100000];
    cushionSpace[99999] = 1; // отключаем оптимизацию массивов
    if(!setjmp(InitStacks[numStack]))
    {//инициализация
        if(numStack <= MAX_TASK)
            InitializeStacks(numStack+1);
    }
    else
    { //выполнение
        TaskQueue[RunningTask].entry();
    }
}
//Программа стартует с этой функции

int StartOS(TTaskCall entry,int priority,char* name)
{
    int i;
    RunningTask = TaskHead = -1; //текущая выполняющаяся задача нет списка
    TaskCount = 0; //подсчет задач
    FreeTask=0; //ожидающие задачи
    printf("StartOS!\n");
    InitializeStacks(0); //инициализация стека
    for(i=0;i<MAX_TASK;i++)
    {
        TaskQueue[i].next=i+1; //номер массива на след элемент
        TaskQueue[i].prev = i - 1; //пред элем
        TaskQueue[i].task_state = TASK_SUSPENDED; //пометка ожид(неактив задач)
        TaskQueue[i].switch_count = 0; // ключ
        TaskQueue[i].waiting_events = 0; //ожидание задачи
        TaskQueue[i].working_events = 0; //сработавшие задачи
    }
    //создание массива задач
    TaskQueue[MAX_TASK-1].next=0; //присваиваем последнему элементу 0
    TaskQueue[0].prev=MAX_TASK-1; //присваиваем послед  предпоследний
    if(!setjmp(MainContext)) //если не 0
        ActivateTask(entry,priority,name); //запускаем функцию активации задачи
    return 0;
} //Завершает работу системы(задача завершена)

void ShutdownOS()
{
    printf("ShutdownOS!\n");
}

