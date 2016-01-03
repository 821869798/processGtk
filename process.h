#ifndef PROCESS_H
#define PROCESS_H
#include <stdio.h>

typedef struct ptime
{
    int hour;
    int min;
}ptime;

typedef struct process
{
    char name[30]; //进程名字
    ptime arriveTime; //到达时间
    ptime startTime; //开始时间
    ptime endTime; //结束时间
    int priority; //优先级
    int workTime; //运行时间
    int turnaroundTime; //周转时间
    double turnaroundRatio; //带权周转时间
    int completeTime; //已经完成的时间
    int isNotComplete; //是否完成工作
}process;

void ptimeToStr(ptime pt,char *timestr) //时间转字符串
{
    sprintf(timestr,"%d:%d",pt.hour,pt.min);
}

int compareTime(ptime t1,ptime t2)  //比较时间
{
    if(t1.hour>t2.hour)
        return 1;
    else if(t1.hour==t2.hour&&t1.min>=t2.min)
        return 1;
    else
        return 0;
}

ptime pTimeAddMinute(ptime pt,int min)
{
    pt.hour += (pt.min+min)/60;
    pt.min = (pt.min+min)%60;
    return pt;
}

int ptimeDifTime(ptime p1,ptime p2)
{
    if(compareTime(p1,p2))
    {
        ptime tmp = p1;
        p1 = p2;
        p2 = tmp;
    }
    if(p1.hour==p2.hour)
        return p2.min-p1.min;
    else
        return p2.min-p1.min+(p2.hour-p1.hour)*60;
}


void pSort(process **datas,int low ,int high)  //根据进程到达时间对进程数据进行排序
{
    if(low>=high)
        return;
    int first = low;
    int last = high;
    process *key = datas[first];
    while(first < last)
    {
        while(first < last && compareTime(datas[last]->arriveTime,key->arriveTime))
        {
            --last;
        }
        datas[first] = datas[last];
        while(first < last && compareTime(key->arriveTime,datas[first]->arriveTime))
        {
            ++first;
        }
        datas[last] = datas[first];
    }
    datas[first] = key;
    pSort(datas, low, first-1);
    pSort(datas, first+1, high);
}

void getTurnaroundTime(process *data) //得到周转时间和带权周转时间
{
    data->turnaroundTime = ptimeDifTime(data->arriveTime,data->endTime);
    data->turnaroundRatio = (double)data->turnaroundTime/data->workTime;
    data->isNotComplete = 0;
}

/*
int getAlreadyEnterProcess(process **datas,int count,ptime beginTime)
{
    int i;
    for(i=0;i<count;i++)
    {
        if(!compareTime(beginTime,datas[i]->arriveTime))
        {
            return i;
        }
    }
    return count;
}
*/

void FCFS(process **datas,int c) //先来先服务
{
    pSort(datas,0,c-1);
    ptime beginTime = datas[0]->arriveTime;
    int i;
    for(i=0;i<c;i++)
    {
        if(compareTime(datas[i]->arriveTime,beginTime))
        {
            beginTime = datas[i]->arriveTime;
        }
        datas[i]->startTime = beginTime;
        beginTime = pTimeAddMinute(beginTime,datas[i]->workTime);
        datas[i]->endTime = beginTime;
        getTurnaroundTime(datas[i]);
    }
}

void SJF(process **datas,int c)  // 最短作业优先
{
    pSort(datas,0,c-1);
    ptime beginTime = datas[0]->arriveTime;
    int i,j;
    for(i=0;i<c;i++)
        datas[i]->isNotComplete = 1;
    int minWorkTime;
    int valueIndex;
    for(i=0;i<c;i++)
    {
        minWorkTime=-1;
        valueIndex=-1;
        for(j=0;j<c;j++)
        {
            if(datas[j]->isNotComplete&&compareTime(beginTime,datas[j]->arriveTime)&&(minWorkTime==-1||datas[j]->workTime<minWorkTime))
            {
                minWorkTime = datas[j]->workTime;
                valueIndex = j;
            }
        }
        if(valueIndex==-1)
        {
            for(j=0;j<c;j++)
                if(datas[j]->isNotComplete)
                {
                    beginTime = datas[j]->arriveTime;
                    break;
                }
            i--;
        }
        else
        {
            datas[valueIndex]->startTime = beginTime;
            beginTime = pTimeAddMinute(beginTime,datas[valueIndex]->workTime);
            datas[valueIndex]->endTime = beginTime;
            getTurnaroundTime(datas[valueIndex]);
        }
    }
}

void SRTF(process **datas,int c) //最短剩余时间优先
{
    pSort(datas,0,c-1);
    ptime beginTime = datas[0]->arriveTime;
    int i;
    int completeCount=0;
    for(i=0;i<c;i++)
    {
        datas[i]->completeTime = 0;
        datas[i]->isNotComplete = 1;
    }
    int minNotCompleteTime;
    int valueIndex;
    int nextValueOffset=-1;
    while(completeCount<c)
    {
        minNotCompleteTime = -1;
        valueIndex = -1;
        for(i=0;i<c;i++)
        {
            if(datas[i]->isNotComplete&&compareTime(beginTime,datas[i]->arriveTime)&&(minNotCompleteTime==-1||datas[i]->workTime-datas[i]->completeTime<minNotCompleteTime))
            {
                minNotCompleteTime = datas[i]->workTime-datas[i]->completeTime;
                valueIndex = i;
            }
        }
        if(valueIndex==-1)
        {
            for(i=0;i<c;i++)
                if(datas[i]->isNotComplete)
                {
                    beginTime = datas[i]->arriveTime;
                    break;
                }
        }
        else
        {
           if(datas[valueIndex]->completeTime==0)
           {
                datas[valueIndex]->startTime = beginTime;
                if(nextValueOffset==-1)
                    nextValueOffset = valueIndex+1;
           }
           //int enterProcessCount = getAlreadyEnterProcess(datas,c,beginTime);
           if(nextValueOffset==c)
           {
                beginTime = pTimeAddMinute(beginTime,datas[valueIndex]->workTime-datas[valueIndex]->completeTime);
                datas[valueIndex]->endTime = beginTime;
                getTurnaroundTime(datas[valueIndex]);
                completeCount++;
           }
           else
           {
               if(compareTime(datas[nextValueOffset]->arriveTime,pTimeAddMinute(beginTime,datas[valueIndex]->workTime-datas[valueIndex]->completeTime)))
               {
                    beginTime = pTimeAddMinute(beginTime,datas[valueIndex]->workTime-datas[valueIndex]->completeTime);
                    datas[valueIndex]->endTime = beginTime;
                    getTurnaroundTime(datas[valueIndex]);
                    completeCount++;
               }
               else
               {
                   datas[valueIndex]->completeTime += ptimeDifTime(beginTime,datas[nextValueOffset]->arriveTime);
                   beginTime = datas[nextValueOffset]->arriveTime;
                   nextValueOffset++;
               }
           }
        }
    }
}

void NPP(process **datas,int c) //非抢占式优先级
{
    pSort(datas,0,c-1);
    ptime beginTime = datas[0]->arriveTime;
    int i,j;
    for(i=0;i<c;i++)
        datas[i]->isNotComplete = 1;
    int maxPriority;
    int valueIndex;
    for(i=0;i<c;i++)
    {
        maxPriority=-1;
        valueIndex=-1;
        for(j=0;j<c;j++)
        {
            if(datas[j]->isNotComplete&&compareTime(beginTime,datas[j]->arriveTime)&&(maxPriority==-1||datas[j]->priority>maxPriority))
            {
                maxPriority = datas[j]->priority;
                valueIndex = j;
            }
        }
        if(valueIndex==-1)
        {
            for(j=0;j<c;j++)
                if(datas[j]->isNotComplete)
                {
                    beginTime = datas[j]->arriveTime;
                    break;
                }
            i--;
        }
        else
        {
            datas[valueIndex]->startTime = beginTime;
            beginTime = pTimeAddMinute(beginTime,datas[valueIndex]->workTime);
            datas[valueIndex]->endTime = beginTime;
            getTurnaroundTime(datas[valueIndex]);
        }
    }
}

void PP(process **datas,int c) //抢占式优先级
{
    pSort(datas,0,c-1);
    ptime beginTime = datas[0]->arriveTime;
    int i;
    int completeCount=0;
    for(i=0;i<c;i++)
    {
        datas[i]->completeTime = 0;
        datas[i]->isNotComplete = 1;
    }
    int maxPriority;
    int valueIndex;
    int nextValueOffset=-1;
    while(completeCount<c)
    {
        maxPriority = -1;
        valueIndex = -1;
        for(i=0;i<c;i++)
        {
            if(datas[i]->isNotComplete&&compareTime(beginTime,datas[i]->arriveTime)&&(maxPriority==-1||datas[i]->priority>maxPriority))
            {
                maxPriority = datas[i]->priority;
                valueIndex = i;
            }
        }
        if(valueIndex==-1)
        {
            for(i=0;i<c;i++)
                if(datas[i]->isNotComplete)
                {
                    beginTime = datas[i]->arriveTime;
                    break;
                }
        }
        else
        {
           if(datas[valueIndex]->completeTime==0)
           {
                datas[valueIndex]->startTime = beginTime;
                if(nextValueOffset==-1)
                    nextValueOffset = valueIndex+1;
           }
           //int enterProcessCount = getAlreadyEnterProcess(datas,c,beginTime);
           if(nextValueOffset==c)
           {
                beginTime = pTimeAddMinute(beginTime,datas[valueIndex]->workTime-datas[valueIndex]->completeTime);
                datas[valueIndex]->endTime = beginTime;
                getTurnaroundTime(datas[valueIndex]);
                completeCount++;
           }
           else
           {
               if(compareTime(datas[nextValueOffset]->arriveTime,pTimeAddMinute(beginTime,datas[valueIndex]->workTime-datas[valueIndex]->completeTime)))
               {
                    beginTime = pTimeAddMinute(beginTime,datas[valueIndex]->workTime-datas[valueIndex]->completeTime);
                    datas[valueIndex]->endTime = beginTime;
                    getTurnaroundTime(datas[valueIndex]);
                    completeCount++;
               }
               else
               {
                   datas[valueIndex]->completeTime += ptimeDifTime(beginTime,datas[nextValueOffset]->arriveTime);
                   beginTime = datas[nextValueOffset]->arriveTime;
                   nextValueOffset++;
               }
           }
        }
    }
}

#endif // PROCESS_H
