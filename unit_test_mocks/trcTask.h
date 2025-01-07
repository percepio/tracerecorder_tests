/*
* trcTask.h mock
*/

#ifndef TRC_TASK_H
#define TRC_TASK_H

extern void* MOCK_TASK_pvCurrentTask;

#define xTraceTaskGetCurrentReturn() (MOCK_TASK_pvCurrentTask)

#endif
