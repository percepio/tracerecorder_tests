/*
 * trcRecorder.h mock
 */

#ifndef TRC_RECORDER_H
#define TRC_RECORDER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <trcConfig.h>
#include <trcDefines.h>

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

typedef const void* traceHandle;

/* Unless specified in trcConfig.h we assume this is a single core target */
#ifndef TRC_CFG_CORE_COUNT
#define TRC_CFG_CORE_COUNT 1
#endif

#ifndef TRC_CFG_TEST_MODE
#define TRC_CFG_TEST_MODE 1
#endif

#define TRC_MAX_BLOB_SIZE (16 * sizeof(TraceUnsignedBaseType_t))

#include <trcTypes.h>
#include <trcHardwarePort.h>
#include <trcKernelPort.h>
#include <trcString.h>
#include <trcStaticBuffer.h>
#include <trcError.h>
#include <trcEvent.h>
#include <trcStreamPort.h>
#include <trcEventBuffer.h>
#include <trcMultiCoreEventBuffer.h>
#include <trcInternalEventBuffer.h>
#include <trcTimestamp.h>
#include <trcEntryTable.h>
#include <trcISR.h>
#include <trcTask.h>
#include <trcObject.h>
#include <trcPrint.h>
#include <trcHeap.h>
#include <trcStateMachine.h>
#include <trcExtension.h>
#include <trcInterval.h>
#include <trcUtility.h>
#include <trcStackMonitor.h>
#include <trcDiagnostics.h>
#include <trcAssert.h>
#include <trcCounter.h>
#include <trcRunnable.h>
#include <trcDependency.h>
#include <trcProcess.h>
#include <trcThread.h>

#if (TRC_CFG_USE_TRACE_ASSERT == 1)
#define assert_int_equal_asserts_enabled(_x, _y) assert_int_equal_cs(_x, _y)
#else
#define assert_int_equal_asserts_enabled(_x, _y)
#endif

/* This will perform the intended assert and then also assert that MOCK_uiCriticalSection is equal to MOCK_uiCriticalSectionExpectedValue */
#define assert_int_equal_cs(_x, _y) assert_int_equal(_x, _y); assert_int_equal(MOCK_uiCriticalSection, MOCK_uiCriticalSectionExpectedValue)


#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

extern uint32_t MOCK_uiCurrentCore;

/* Unless specified in trcConfig.h we assume this is a single core target */
#ifndef TRC_CFG_GET_CURRENT_CORE
#define TRC_CFG_GET_CURRENT_CORE() (MOCK_uiCurrentCore)
#endif

#define xTraceEnable(uiStartOption) ((void)uiStartOption, TRC_SUCCESS)

/* Backwards compatibility */
#define vTraceEnable xTraceEnable

#define xTraceDisable() (TRC_SUCCESS)

#define vTraceInstanceFinishedNow() 

#define vTraceInstanceFinishedNext() 

#define vTraceStop() 

#define xTraceSetRecorderDataBuffer(pRecorderData) ((void*)pRecorderData)

#define vTraceSetRecorderDataBuffer xTraceSetRecorderDataBuffer

extern volatile uint32_t MOCK_RecorderEnabled;
extern volatile uint32_t MOCK_RecorderInitialized;

#define xTraceSetComponentInitialized(uiComponentBit) (MOCK_RecorderInitialized |= (uiComponentBit), TRC_SUCCESS)
#define xTraceIsComponentInitialized(uiComponentBit) ((MOCK_RecorderInitialized & (uiComponentBit)) ? 1 : 0)

#define xTraceIsRecorderEnabled() (MOCK_RecorderEnabled)
#define xTraceIsRecorderInitialized() xTraceIsComponentInitialized(TRC_RECORDER_COMPONENT_CORE)

#define vTraceSetFilterGroup(filterGroup) ((void)filterGroup)

#define vTraceSetFilterMask(filterMask) ((void)filterGroup)

#define xTraceInitialize() (TRC_SUCCESS)

#define xTraceStateSet(s) ((void)s)

/* Backwards compatibility */
#define vTraceInitialize xTraceInitialize

#define xTraceTzCtrlPump() (TRC_SUCCESS)

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

#ifndef TRC_STREAM_PORT_USE_INTERNAL_BUFFER
#define TRC_STREAM_PORT_USE_INTERNAL_BUFFER 1
#endif

#define prvTraceStoreEvent0(eventID) ((void)eventID)
#define prvTraceStoreEvent1(eventID, param1) ((void)eventID); ((void)param1)
#define prvTraceStoreEvent2(eventID, param1, param2) ((void)eventID); ((void)param1); ((void)param2)
#define prvTraceStoreEvent3(eventID, param1, param2, param3) ((void)eventID); ((void)param1); ((void)param2); ((void)param3)
#define prvTraceStoreEvent(nParam, EventID, ...) ((void)nParam); ((void)eventID)
#define prvTraceStoreStringEvent(nArgs, eventID, str, ...); ((void)nArgs); ((void)eventID); ((void*)str)

#define vTraceSetStopHook(x) ((void)(x))

#define vTraceInitTimestamps() 

#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

#endif /* (TRC_USE_TRACEALYZER_RECORDER == 1) */

#ifdef __cplusplus
}
#endif

#endif /* TRC_RECORDER_H */
