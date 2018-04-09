/*********************************/
/*          task.c               */
/*********************************/

#include <stdio.h>
#include <string.h>
#include "sys.h"
#include "rtos_api.h"

int ActivateTask(TTaskCall entry,int priority,char* name) //функция активации задачи
{
    int task,occupy;
    printf("ActivateTask %s\n",name);
    task=TaskHead; //указатель на голову
    occupy=FreeTask; //указатель на свободную
    //изменяем список свободных задач
    RemoveTask(occupy, &FreeTask);
    //удаление задачи из списка
    TaskQueue[occupy].priority=priority;
    TaskQueue[occupy].ceiling_priority=priority;
    TaskQueue[occupy].name=name;
    TaskQueue[occupy].entry=entry;
    TaskQueue[occupy].switch_count = 0;
    TaskQueue[occupy].task_state = TASK_READY; //пометка
    TaskCount++;
    printf("End of ActivateTask %s\n",name);
    Schedule(occupy); //добавление задачи в очередь активных задач

    if(task!=TaskHead)
    {
        Dispatch(); //обеспечивает переключение задач
    }
    return occupy;
}

void TerminateTask(void) //завершить задачу
{
    int task;
    TaskCount--; //вычитаем из количества задач
    task=RunningTask; //текущая выполняющаяся задача
    printf("TerminateTask %s\n",TaskQueue[task].name);
    TaskQueue[task].task_state = TASK_SUSPENDED; //вносим в ожидающие
    RemoveTask(task, &TaskHead); //удаляем задачу из списка
    //добавляем задачу в список свободных
    InsertTaskBefore(task, FreeTask, &FreeTask);
    printf("End of TerminateTask %s\n",TaskQueue[task].name);

    if(TaskCount == 0)
        longjmp(MainContext, 1); //то все закончили
    Dispatch(); //перескакиваем на другую
}

void RemoveTask(int task, int *head) //удаление задачи из списка
{
    if(TaskQueue[task].next == -1) //есть ли список
        return;
    if(*head == task)
    {
        if(TaskQueue[task].next == task)
            *head = -1; //если это был послед невыполнен задача то списка больше нет
        else
            *head = TaskQueue[task].next; //тогда перходим к след задачи из списка
    }
    TaskQueue[TaskQueue[task].prev].next = TaskQueue[task].next; //заменяем задачи на оставшиеся
    TaskQueue[TaskQueue[task].next].prev = TaskQueue[task].prev;
    TaskQueue[task].next = -1;
    TaskQueue[task].prev = -1;
}

void Schedule(int task, int dont_show) //добавление в список активных задач
{
    int cur; //времен перемен содерж раб номер головы
    int priority; //времен, запом приоритет для сравнения
    if(TaskQueue[task].task_state == TASK_SUSPENDED) //?если равно ожидает
        return;
    if(!dont_show)
        printf("Schedule %s\n",TaskQueue[task].name);
    if(TaskHead == -1) //если это первая задача
    {
        TaskHead = task; //то это и голова и след и пред
        TaskQueue[task].next = task;
        TaskQueue[task].prev = task;
    }
    else if(TaskCount > 1) //если список уже составляется
    {
        cur = TaskHead; //указатель на голову
        priority=TaskQueue[task].ceiling_priority; //запоминаем времен приоритет
        RemoveTask(task, &TaskHead); //удаление задачи из списка
        while (TaskQueue[cur].ceiling_priority >= priority)
        {
            cur=TaskQueue[cur].next;
            if(cur == TaskHead) //нет задачи с таким низким приоритетом - поставим ее в конец списка
                break;
        }
        if(priority <= TaskQueue[TaskHead].ceiling_priority && cur == TaskHead) //вставляем в конец списка
            InsertTaskAfter(task, TaskQueue[TaskHead].prev);
        else
            InsertTaskBefore(task, cur, &TaskHead); //вставляем перед элементом
    }
    if(!dont_show)
        printf("End of Schedule %s\n",TaskQueue[task].name);
}

// добавить задачу перед item, head указывает на голову списка
void InsertTaskBefore(int task, int item, int *head) //вставить элемент перед текущ, head, указат на голову
{
    if(TaskQueue[task].next != -1) //если списка нет
        return;
    if(item == *head) //если указатель на перв элемент(0)
        *head = task; //то меняем голову
    TaskQueue[task].next = item;
    TaskQueue[task].prev = TaskQueue[item].prev;
    TaskQueue[TaskQueue[item].prev].next = task;
    TaskQueue[item].prev = task;
}

void InsertTaskAfter(int task, int item) //вставить элемент после тек элем
{
    if(TaskQueue[task].next != -1) //если списка нет
        return;
    TaskQueue[task].next = TaskQueue[item].next;
    TaskQueue[task].prev = item;
    TaskQueue[TaskQueue[item].next].prev = task;
    TaskQueue[item].next = task;
}

void Dispatch() //переключение задач
{
    if(RunningTask != -1) //если есть список, изменяется только в функ TaskSwitch
        printf("Dispatch - %s\n", TaskQueue[RunningTask].name);
    else printf("Dispatch\n");
    if(RunningTask != -1 && TaskQueue[RunningTask].task_state == TASK_RUNNING)
        TaskQueue[RunningTask].task_state = TASK_READY; //меняем пометку
    int cur = TaskHead; //указатель на голову
    while(TaskCount)
    {
        if(TaskQueue[cur].task_state == TASK_READY)
        { //переключаемся на следующую задачу
            if(RunningTask == -1 || TaskQueue[RunningTask].task_state == TASK_SUSPENDED)
            {//запоминать контекст не надо
                TaskSwitch(cur);
            }
            else if(!setjmp(TaskQueue[RunningTask].context))
                TaskSwitch(cur);
            break;
        }
        cur = TaskQueue[cur].next;
        if(cur == TaskHead)
        {
            printf("Dont have ready tasks\n");
            longjmp(MainContext,1);
        }
    }
    printf("End of Dispatch - %s\n", TaskQueue[RunningTask].name);
    Schedule(cur, 1); //добавляем в список активных задач
}

void TaskSwitch(int nextTask) //востоновление задачи nexttext переключатель задач
{
    if(nextTask == -1) //если нет массива возврат nextTask –элем с котом мы сейчас работ cur
        return;
    TaskQueue[nextTask].task_state = TASK_RUNNING;
    RunningTask = nextTask; //текущая выполняющаяся задача
    TaskQueue[nextTask].switch_count++; //ставим номер счетчика задачи
    if(TaskQueue[nextTask].switch_count == 1)
    {
        printf("Dispatch - run task %s\n", TaskQueue[nextTask].name);
        longjmp(InitStacks[nextTask], 1); //востоновление предыдущей задачи
    }
    else
        longjmp(TaskQueue[nextTask].context, 1);
}

