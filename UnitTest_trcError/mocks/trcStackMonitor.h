/*
* trcStackMonitor.h mock
*/

#ifndef TRC_STACK_MONITOR_H
#define TRC_STACK_MONITOR_H

#include <stdint.h>
#include <trcRecorder.h>

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

#ifdef __cplusplus
extern "C" {
#endif

typedef enum TraceStackMonitorDiagnosticsType
{
	TRC_STACK_MONITOR_DIAGNOSTICS_TASKS_NO_ROOM,
} TraceStackMonitorDiagnosticsType_t;

typedef struct TraceStackMonitorBuffer
{
	uint32_t buffer[1];
} TraceStackMonitorBuffer_t;

#define xTraceStackMonitorInitialize(pxBuffer) ((void)pxBuffer, TRC_SUCCESS)

#define xTraceStackMonitorDiagnosticsGet(xType, puiValue) ((void)xType, puiValue != 0 ? *puiValue = 0 : 0, puiValue != 0 ? TRC_SUCCESS : TRC_FAIL)

#define xTraceStackMonitorDiagnosticsSet(xType, uiValue) ((void)xType, (void)uiValue, TRC_SUCCESS)

#define xTraceStackMonitorAdd(pvTask) ((void)pvTask, TRC_SUCCESS)

#define xTraceStackMonitorRemove(pvTask) ((void)pvTask, TRC_SUCCESS)

extern void *MOCK_pvTask;
extern TraceUnsignedBaseType_t MOCK_uxLowWaterMark;
#define xTraceStackMonitorGetAtIndex(uiIndex, ppvTask, puxLowWaterMark) ((void)uiIndex, ppvTask != 0 ? *ppvTask = MOCK_pvTask : 0, puxLowWaterMark != 0 ? *puxLowWaterMark = MOCK_uxLowWaterMark : 0, (ppvTask != 0 && puxLowWaterMark != 0) ? TRC_SUCCESS : TRC_FAIL)

#define xTraceStackMonitorReport(void) (TRC_SUCCESS)

#ifdef __cplusplus
}
#endif

#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

#endif /* (TRC_USE_TRACEALYZER_RECORDER == 1) */

#endif /* TRC_STACK_MONITOR_H */
