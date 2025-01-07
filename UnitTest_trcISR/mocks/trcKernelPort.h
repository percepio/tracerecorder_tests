/*
 * trcKernelPort.h mock
 */

#ifndef TRC_KERNEL_PORT_H
#define TRC_KERNEL_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

#define TRC_USE_TRACEALYZER_RECORDER 1

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

#define TRC_PLATFORM_CFG ""
#define TRC_PLATFORM_CFG_MAJOR 1
#define TRC_PLATFORM_CFG_MINOR 0
#define TRC_PLATFORM_CFG_PATCH 0

extern uint32_t MOCK_uiUnusedStack;
#define xTraceKernelPortGetUnusedStack(pvTask, puiValue) (puiValue != 0 ? *puiValue = MOCK_uiUnusedStack : 0, (pvTask != 0 && puiValue != 0) ? TRC_SUCCESS : TRC_FAIL)

#define xTraceKernelPortIsSchedulerSuspended() (0)

#define TRACE_KERNEL_VERSION 0xABCD
#define TRC_TICK_RATE_HZ 1000
#define TRACE_CPU_CLOCK_HZ 100000000

#ifndef TRACE_ENTER_CRITICAL_SECTION
	#error "This hardware port has no definition for critical sections! See http://percepio.com/2014/10/27/how-to-define-critical-sections-for-the-recorder/"
#endif

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

#define TRC_KERNEL_PORT_BUFFER (sizeof(uint32_t))

typedef struct TraceKernelPortBuffer
{
	uint32_t buffer[1];
} TraceKernelPortDataBuffer_t;

#define xTraceKernelPortInitialize(pxBuffer) (TRC_SUCCESS)

#define xTraceKernelPortEnable() (TRC_SUCCESS)

/*** Event codes for streaming - should match the Tracealyzer config file ***/
#define PSF_EVENT_NULL_EVENT								0x00

#define PSF_EVENT_TRACE_START								0x01
#define PSF_EVENT_TS_CONFIG									0x02
#define PSF_EVENT_OBJ_NAME									0x03
#define PSF_EVENT_TASK_PRIORITY								0x04
#define PSF_EVENT_TASK_PRIO_INHERIT							0x05
#define PSF_EVENT_TASK_PRIO_DISINHERIT						0x06
#define PSF_EVENT_DEFINE_ISR								0x07

#define PSF_EVENT_TASK_CREATE								0x10

#define PSF_EVENT_TASK_DELETE								0x20

#define PSF_EVENT_TASK_READY								0x30
#define PSF_EVENT_ISR_BEGIN									0x33
#define PSF_EVENT_ISR_RESUME								0x34
#define PSF_EVENT_TS_BEGIN									0x35
#define PSF_EVENT_TS_RESUME									0x36
#define PSF_EVENT_TASK_ACTIVATE								0x37

#define PSF_EVENT_MALLOC									0x38
#define PSF_EVENT_FREE										0x39

#define PSF_EVENT_LOWPOWER_BEGIN							0x3A
#define PSF_EVENT_LOWPOWER_END								0x3B

#define PSF_EVENT_IFE_NEXT									0x3C
#define PSF_EVENT_IFE_DIRECT								0x3D

#define PSF_EVENT_USER_EVENT								0x90

#define PSF_EVENT_USER_EVENT_FIXED							0x98

#define PSF_EVENT_MALLOC_FAILED								0xE9

#define PSF_EVENT_UNUSED_STACK								0xEA
	
#define PSF_EVENT_FREE_FAILED								0xEB

#define PSF_EVENT_STATEMACHINE_STATE_CREATE					0xEC
#define PSF_EVENT_STATEMACHINE_CREATE						0xED
#define PSF_EVENT_STATEMACHINE_STATECHANGE					0xEE

#define PSF_EVENT_INTERVAL_CHANNEL_CREATE						0xEF
#define PSF_EVENT_INTERVAL_START						0xF0

#define PSF_EVENT_EXTENSION_CREATE						0xF1
#define PSF_EVENT_HEAP_CREATE							0xF2

#define PSF_EVENT_COUNTER_CREATE						0xF3
#define PSF_EVENT_COUNTER_CHANGE						0xF4
#define PSF_EVENT_COUNTER_LIMIT_EXCEEDED					0xF5

#define PSF_EVENT_MUTEX_TAKE_RECURSIVE_BLOCK				0xF6

#define PSF_EVENT_INTERVAL_STOP								0xF7
#define PSF_EVENT_INTERVAL_CHANNEL_SET_CREATE							0xF8

#define PSF_EVENT_RUNNABLE_REGISTER							0xF9
#define PSF_EVENT_RUNNABLE_START							0xFA
#define PSF_EVENT_RUNNABLE_STOP								0xFB

#define PSF_EVENT_DEPENDENCY_REGISTER						0xFC

#define TRC_EVENT_LAST_ID									PSF_EVENT_DEPENDENCY_REGISTER

#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

#else /* (TRC_USE_TRACEALYZER_RECORDER == 1) */
	
#endif /* (TRC_USE_TRACEALYZER_RECORDER == 1) */

#ifdef __cplusplus
}
#endif

#endif /* TRC_KERNEL_PORT_H */
