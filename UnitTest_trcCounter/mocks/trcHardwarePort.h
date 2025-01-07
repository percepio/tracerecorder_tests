#ifndef TRC_HARDWARE_PORT_H
#define TRC_HARDWARE_PORT_H

#include <stdint.h>

extern uint32_t MOCK_uiCycleCounterTest;
extern uint32_t MOCK_uiCycleCounterIncrementTest;
extern uint32_t MOCK_uiOsTickCounterTest;
extern uint32_t MOCK_uiPeriod;
extern uint32_t MOCK_uiCriticalSection;
extern uint32_t MOCK_uiCriticalSectionExpectedValue;

#define TRC_HWTC_TYPE TRC_FREE_RUNNING_32BIT_DECR
#if (TRC_HWTC_TYPE == TRC_FREE_RUNNING_32BIT_INCR || TRC_HWTC_TYPE == TRC_CUSTOM_TIMER_INCR || TRC_HWTC_TYPE == TRC_OS_TIMER_INCR)
#define TRC_HWTC_COUNT (MOCK_uiCycleCounterTest += MOCK_uiCycleCounterIncrementTest)
#elif (TRC_HWTC_TYPE == TRC_FREE_RUNNING_32BIT_DECR || TRC_HWTC_TYPE == TRC_CUSTOM_TIMER_DECR || TRC_HWTC_TYPE == TRC_OS_TIMER_DECR)
#define TRC_HWTC_COUNT (MOCK_uiCycleCounterTest -= MOCK_uiCycleCounterIncrementTest)
#endif
#define TRC_HWTC_PERIOD (MOCK_uiPeriod)
#define TRACE_GET_OS_TICKS() (MOCK_uiOsTickCounterTest++)

#define TRACE_ALLOC_CRITICAL_SECTION_NAME xTraceCriticalSectionStatus

#define TRACE_ALLOC_CRITICAL_SECTION() TraceUnsignedBaseType_t TRACE_ALLOC_CRITICAL_SECTION_NAME;
#define TRACE_ENTER_CRITICAL_SECTION() TRACE_ALLOC_CRITICAL_SECTION_NAME = 0; (++MOCK_uiCriticalSection)
#define TRACE_EXIT_CRITICAL_SECTION() (void)TRACE_ALLOC_CRITICAL_SECTION_NAME; (--MOCK_uiCriticalSection)

#ifdef _WIN64
#define TRC_UNSIGNED_BASE_TYPE uint64_t
#define TRC_BASE_TYPE int64_t
#endif

#if __x86_64__
#define TRC_UNSIGNED_BASE_TYPE uint64_t
#define TRC_BASE_TYPE int64_t
#endif

#endif /* TRC_HARDWARE_PORT_H */
