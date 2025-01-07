#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <Timestamp_UnitTest.h>
#include <trcRecorder.h>

int group_teardown_Timestamp(void** ppState);
int group_setup_Timestamp(void** ppState);
int setup_Timestamp(void** ppState);
int teardown_Timestamp(void** ppState);

void test_xTraceTimestampInitialize(void** ppState);
void test_xTraceTimestampSetGetFrequency(void** ppState);
void test_xTraceTimestampSetGetPeriod(void** ppState);
void test_xTraceTimestampSetGetOsTickCount(void** ppState);
void test_xTraceTimestampGet(void** ppState);

/******************************************************************************************
*                                        TEST STATE                                       *
******************************************************************************************/

typedef struct TimestampRecordTestState
{
	/* Pointer to Buffer */
	TraceTimestampData_t *pvTimestampBuffer;

} TimestampRecordTestState_t;

TimestampRecordTestState_t xTimestampRecordTestState;

TraceTimestampData_t xTimestampRecordBuffer;

uint32_t MOCK_uiCycleCounterTest;
uint32_t MOCK_uiCycleCounterIncrementTest;
uint32_t MOCK_uiOsTickCounterTest;
uint32_t MOCK_uiPeriod = 0;
uint32_t MOCK_trc_hwtc_type = TRC_FREE_RUNNING_32BIT_INCR;
uint32_t MOCK_uiCriticalSection = 0;
uint32_t MOCK_uiCriticalSectionExpectedValue = 0;
uint8_t MOCK_tmpBuffer[TRC_MAX_BLOB_SIZE];
uint8_t MOCK_streamPortBuffer[TRC_MAX_BLOB_SIZE];
uint32_t MOCK_streamPortMaxTransferSize = 0xFFFFFFFF;

volatile uint32_t MOCK_RecorderInitialized = 1;

/******************************************************************************************
*                                        SETUP / TEARDOWN                                 *
******************************************************************************************/

int group_setup_Timestamp(void **ppState)
{
	xTimestampRecordTestState.pvTimestampBuffer = &xTimestampRecordBuffer;
	*ppState = &xTimestampRecordTestState;

	assert_int_equal(sizeof(TraceUnsignedBaseType_t), sizeof(void*));
	assert_int_equal(sizeof(TraceUnsignedBaseType_t), sizeof(TraceBaseType_t));

	return 0;
}

int group_teardown_Timestamp(void **ppState)
{
	TimestampRecordTestState_t* pxState = (TimestampRecordTestState_t*)*ppState;

	pxState->pvTimestampBuffer = 0;

	return 0;
}

int setup_Timestamp(void **ppState)
{
	TimestampRecordTestState_t* pxState = (TimestampRecordTestState_t*)*ppState;
	
	assert_int_equal_cs(xTraceTimestampInitialize(pxState->pvTimestampBuffer), TRC_SUCCESS);

	return 0;
}

int teardown_Timestamp(void **ppState)
{
	return 0;
}

/******************************************************************************************
*                                        TESTS                                            *
******************************************************************************************/

void test_xTraceTimestampInitialize(void **ppState)
{
	TimestampRecordTestState_t* pxState = (TimestampRecordTestState_t*)*ppState;

	assert_int_equal_asserts_enabled(xTraceTimestampInitialize(0), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceTimestampInitialize(pxState->pvTimestampBuffer), TRC_SUCCESS);
}

void test_xTraceTimestampSetGetPeriod(void** ppState)
{
	uint32_t uiPeriod = 0;

	assert_int_equal_asserts_enabled(xTraceTimestampGetPeriod(0), TRC_FAIL); /* null test */
	
	assert_int_equal_cs(xTraceTimestampSetPeriod(0), TRC_SUCCESS);
	assert_int_equal_cs(xTraceTimestampGetPeriod(&uiPeriod), TRC_SUCCESS);
	assert_int_equal_cs(uiPeriod, 0);
	assert_int_equal_cs(xTraceTimestampSetPeriod(10), TRC_SUCCESS);
	assert_int_equal_cs(xTraceTimestampGetPeriod(&uiPeriod), TRC_SUCCESS);
	assert_int_equal_cs(uiPeriod, 10);
	assert_int_equal_cs(xTraceTimestampSetPeriod(20), TRC_SUCCESS);
	assert_int_equal_cs(xTraceTimestampGetPeriod(&uiPeriod), TRC_SUCCESS);
	assert_int_equal_cs(uiPeriod, 20);
	assert_int_equal_cs(xTraceTimestampSetPeriod(123456), TRC_SUCCESS);
	assert_int_equal_cs(xTraceTimestampGetPeriod(&uiPeriod), TRC_SUCCESS);
	assert_int_equal_cs(uiPeriod, 123456);
}

void test_xTraceTimestampSetGetFrequency(void **ppState)
{
	TraceUnsignedBaseType_t uxFrequency = 0;
	
	assert_int_equal_asserts_enabled(xTraceTimestampGetFrequency(0), TRC_FAIL); /* null test */
	
	assert_int_equal_cs(xTraceTimestampSetFrequency(0), TRC_SUCCESS);
	assert_int_equal_cs(xTraceTimestampGetFrequency(&uxFrequency), TRC_SUCCESS);
	assert_int_equal_cs(uxFrequency, 0);
	assert_int_equal_cs(xTraceTimestampSetFrequency(10), TRC_SUCCESS);
	assert_int_equal_cs(xTraceTimestampGetFrequency(&uxFrequency), TRC_SUCCESS);
	assert_int_equal_cs(uxFrequency, 10);
	assert_int_equal_cs(xTraceTimestampSetFrequency(20), TRC_SUCCESS);
	assert_int_equal_cs(xTraceTimestampGetFrequency(&uxFrequency), TRC_SUCCESS);
	assert_int_equal_cs(uxFrequency, 20);
	assert_int_equal_cs(xTraceTimestampSetFrequency(123456), TRC_SUCCESS);
	assert_int_equal_cs(xTraceTimestampGetFrequency(&uxFrequency), TRC_SUCCESS);
	assert_int_equal_cs(uxFrequency, 123456);
}

void test_xTraceTimestampSetGetOsTickCount(void** ppState)
{
	uint32_t uiOsTickCount = 0;

	assert_int_equal_asserts_enabled(xTraceTimestampGetOsTickCount(0), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceTimestampSetOsTickCount(0), TRC_SUCCESS);
	assert_int_equal_cs(xTraceTimestampGetOsTickCount(&uiOsTickCount), TRC_SUCCESS);
	assert_int_equal_cs(uiOsTickCount, 0);
	assert_int_equal_cs(xTraceTimestampSetOsTickCount(10), TRC_SUCCESS);
	assert_int_equal_cs(xTraceTimestampGetOsTickCount(&uiOsTickCount), TRC_SUCCESS);
	assert_int_equal_cs(uiOsTickCount, 10);
	assert_int_equal_cs(xTraceTimestampSetOsTickCount(20), TRC_SUCCESS);
	assert_int_equal_cs(xTraceTimestampGetOsTickCount(&uiOsTickCount), TRC_SUCCESS);
	assert_int_equal_cs(uiOsTickCount, 20);
	assert_int_equal_cs(xTraceTimestampSetOsTickCount(123456), TRC_SUCCESS);
	assert_int_equal_cs(xTraceTimestampGetOsTickCount(&uiOsTickCount), TRC_SUCCESS);
	assert_int_equal_cs(uiOsTickCount, 123456);
}

void test_xTraceTimestampGet(void **ppState)
{
	uint32_t uiTimestamp = 0, uiWraparounds = 0;

	assert_int_equal_asserts_enabled(xTraceTimestampGet(0), TRC_FAIL); /* null test */

	/* Prime the mocked counter */
	MOCK_uiCycleCounterTest = pxTraceTimestamp->latestTimestamp;

#if (TRC_HWTC_TYPE == TRC_FREE_RUNNING_32BIT_INCR || TRC_HWTC_TYPE == TRC_CUSTOM_TIMER_INCR)
	MOCK_uiCycleCounterIncrementTest = 1;

	assert_int_equal_cs(xTraceTimestampGet(&uiTimestamp), TRC_SUCCESS);
	assert_int_equal_cs(uiTimestamp, 1);
	assert_int_equal_cs(uiTimestamp, MOCK_uiCycleCounterTest);
	assert_int_equal_cs(xTraceTimestampGet(&uiTimestamp), TRC_SUCCESS);
	assert_int_equal_cs(uiTimestamp, 2);
	assert_int_equal_cs(uiTimestamp, MOCK_uiCycleCounterTest);
	assert_int_equal_cs(xTraceTimestampGet(&uiTimestamp), TRC_SUCCESS);
	assert_int_equal_cs(uiTimestamp, 3);
	assert_int_equal_cs(uiTimestamp, MOCK_uiCycleCounterTest);
	assert_int_equal_cs(xTraceTimestampGet(&uiTimestamp), TRC_SUCCESS);
	assert_int_equal_cs(uiTimestamp, 4);
	assert_int_equal_cs(uiTimestamp, MOCK_uiCycleCounterTest);

	assert_int_equal_cs(xTraceTimestampGetWraparounds(&uiWraparounds), TRC_SUCCESS);
	assert_int_equal_cs(uiWraparounds, 0);

	MOCK_uiCycleCounterIncrementTest = (int32_t)(MOCK_uiPeriod - 1);

	assert_int_equal_cs(xTraceTimestampGet(&uiTimestamp), TRC_SUCCESS);
	assert_int_equal_cs(uiTimestamp, 3);
	assert_int_equal_cs(uiTimestamp, MOCK_uiCycleCounterTest);

	assert_int_equal_cs(xTraceTimestampGetWraparounds(&uiWraparounds), TRC_SUCCESS);
	assert_int_equal_cs(uiWraparounds, 1);

	assert_int_equal_cs(xTraceTimestampGet(&uiTimestamp), TRC_SUCCESS);
	assert_int_equal_cs(uiTimestamp, 2);
	assert_int_equal_cs(uiTimestamp, MOCK_uiCycleCounterTest);

	assert_int_equal_cs(xTraceTimestampGetWraparounds(&uiWraparounds), TRC_SUCCESS);
	assert_int_equal_cs(uiWraparounds, 2);

#elif (TRC_HWTC_TYPE == TRC_FREE_RUNNING_32BIT_DECR || TRC_HWTC_TYPE == TRC_CUSTOM_TIMER_DECR)
	MOCK_uiCycleCounterIncrementTest = 1;

	assert_int_equal_cs(xTraceTimestampGet(&uiTimestamp), TRC_SUCCESS);
	assert_int_equal_cs(uiTimestamp, ((uint32_t)-2));
	assert_int_equal_cs(uiTimestamp, MOCK_uiCycleCounterTest);
	assert_int_equal_cs(xTraceTimestampGet(&uiTimestamp), TRC_SUCCESS);
	assert_int_equal_cs(uiTimestamp, ((uint32_t)-3));
	assert_int_equal_cs(uiTimestamp, MOCK_uiCycleCounterTest);
	assert_int_equal_cs(xTraceTimestampGet(&uiTimestamp), TRC_SUCCESS);
	assert_int_equal_cs(uiTimestamp, ((uint32_t)-4));
	assert_int_equal_cs(uiTimestamp, MOCK_uiCycleCounterTest);
	assert_int_equal_cs(xTraceTimestampGet(&uiTimestamp), TRC_SUCCESS);
	assert_int_equal_cs(uiTimestamp, ((uint32_t)-5));
	assert_int_equal_cs(uiTimestamp, MOCK_uiCycleCounterTest);

	assert_int_equal_cs(xTraceTimestampGetWraparounds(&uiWraparounds), TRC_SUCCESS);
	assert_int_equal_cs(uiWraparounds, 0);

	MOCK_uiCycleCounterIncrementTest = (int32_t)(MOCK_uiPeriod - 1);

	assert_int_equal_cs(xTraceTimestampGet(&uiTimestamp), TRC_SUCCESS);
	assert_int_equal_cs(uiTimestamp, ((uint32_t)-4));
	assert_int_equal_cs(uiTimestamp, MOCK_uiCycleCounterTest);

	assert_int_equal_cs(xTraceTimestampGetWraparounds(&uiWraparounds), TRC_SUCCESS);
	assert_int_equal_cs(uiWraparounds, 1);

	assert_int_equal_cs(xTraceTimestampGet(&uiTimestamp), TRC_SUCCESS);
	assert_int_equal_cs(uiTimestamp, ((uint32_t)-3));
	assert_int_equal_cs(uiTimestamp, MOCK_uiCycleCounterTest);

	assert_int_equal_cs(xTraceTimestampGetWraparounds(&uiWraparounds), TRC_SUCCESS);
	assert_int_equal_cs(uiWraparounds, 2);
#elif (TRC_HWTC_TYPE == TRC_OS_TIMER_INCR)
	MOCK_uiCycleCounterIncrementTest = 1;

	assert_int_equal_cs(xTraceTimestampGet(&uiTimestamp), TRC_SUCCESS);
	assert_int_equal_cs(uiTimestamp, 0xFFFFFF & 1);
	assert_int_equal_cs(xTraceTimestampGet(&uiTimestamp), TRC_SUCCESS);
	assert_int_equal_cs(uiTimestamp, 0xFFFFFF & 2);
	assert_int_equal_cs(xTraceTimestampGet(&uiTimestamp), TRC_SUCCESS);
	assert_int_equal_cs(uiTimestamp, 0xFFFFFF & 3);
	assert_int_equal_cs(xTraceTimestampGet(&uiTimestamp), TRC_SUCCESS);
	assert_int_equal_cs(uiTimestamp, 0xFFFFFF & 4);

	assert_int_equal_cs(xTraceTimestampGetWraparounds(&uiWraparounds), TRC_SUCCESS);
	assert_int_equal_cs(uiWraparounds, 0);

	assert_int_equal_cs(xTraceTimestampSetOsTickCount(1), TRC_SUCCESS);

	assert_int_equal_cs(xTraceTimestampGet(&uiTimestamp), TRC_SUCCESS);
	assert_int_equal_cs(uiTimestamp, (1 << 24) | 5);

	assert_int_equal_cs(xTraceTimestampGetWraparounds(&uiWraparounds), TRC_SUCCESS);
	assert_int_equal_cs(uiWraparounds, 1);

	assert_int_equal_cs(xTraceTimestampSetOsTickCount(2), TRC_SUCCESS);

	assert_int_equal_cs(xTraceTimestampGet(&uiTimestamp), TRC_SUCCESS);
	assert_int_equal_cs(uiTimestamp, (2 << 24) | 6);

	assert_int_equal_cs(xTraceTimestampGetWraparounds(&uiWraparounds), TRC_SUCCESS);
	assert_int_equal_cs(uiWraparounds, 2);
#elif (TRC_HWTC_TYPE == TRC_OS_TIMER_DECR)
	MOCK_uiCycleCounterIncrementTest = 1;

	assert_int_equal_cs(xTraceTimestampGet(&uiTimestamp), TRC_SUCCESS);
	assert_int_equal_cs(uiTimestamp, (0xFFFFFF & (uint32_t)-2));
	assert_int_equal_cs(xTraceTimestampGet(&uiTimestamp), TRC_SUCCESS);
	assert_int_equal_cs(uiTimestamp, (0xFFFFFF & (uint32_t)-3));
	assert_int_equal_cs(xTraceTimestampGet(&uiTimestamp), TRC_SUCCESS);
	assert_int_equal_cs(uiTimestamp, (0xFFFFFF & (uint32_t)-4));
	assert_int_equal_cs(xTraceTimestampGet(&uiTimestamp), TRC_SUCCESS);
	assert_int_equal_cs(uiTimestamp, (0xFFFFFF & (uint32_t)-5));

	assert_int_equal_cs(xTraceTimestampGetWraparounds(&uiWraparounds), TRC_SUCCESS);
	assert_int_equal_cs(uiWraparounds, 0);

	assert_int_equal_cs(xTraceTimestampSetOsTickCount(1), TRC_SUCCESS);

	assert_int_equal_cs(xTraceTimestampGet(&uiTimestamp), TRC_SUCCESS);
	assert_int_equal_cs(uiTimestamp, (1 << 24) | (0xFFFFFF & (uint32_t)-6));

	assert_int_equal_cs(xTraceTimestampGetWraparounds(&uiWraparounds), TRC_SUCCESS);
	assert_int_equal_cs(uiWraparounds, 1);

	assert_int_equal_cs(xTraceTimestampSetOsTickCount(2), TRC_SUCCESS);

	assert_int_equal_cs(xTraceTimestampGet(&uiTimestamp), TRC_SUCCESS);
	assert_int_equal_cs(uiTimestamp, (2 << 24) | (0xFFFFFF & (uint32_t)-7));

	assert_int_equal_cs(xTraceTimestampGetWraparounds(&uiWraparounds), TRC_SUCCESS);
	assert_int_equal_cs(uiWraparounds, 2);
#endif
}

void TRACE_ASSERTS_DISABLED(void **ppState)
{
}

uint32_t runTimestampTests(void)
{
	uint32_t uiCountFailedTests = 0;

	const struct CMUnitTest aTests_trcTimestamp[] =
	{
#if (TRC_CFG_USE_TRACE_ASSERT == 0)
		cmocka_unit_test(TRACE_ASSERTS_DISABLED),
#endif
		cmocka_unit_test(test_xTraceTimestampInitialize),
		cmocka_unit_test_setup_teardown(test_xTraceTimestampSetGetFrequency, setup_Timestamp, teardown_Timestamp),
		cmocka_unit_test_setup_teardown(test_xTraceTimestampSetGetPeriod, setup_Timestamp, teardown_Timestamp),
		cmocka_unit_test_setup_teardown(test_xTraceTimestampSetGetOsTickCount, setup_Timestamp, teardown_Timestamp),
		cmocka_unit_test_setup_teardown(test_xTraceTimestampGet, setup_Timestamp, teardown_Timestamp),
#if (TRC_CFG_USE_TRACE_ASSERT == 0)
		cmocka_unit_test(TRACE_ASSERTS_DISABLED),
#endif
	};

	uiCountFailedTests += cmocka_run_group_tests(aTests_trcTimestamp, group_setup_Timestamp, group_teardown_Timestamp);
	
	return uiCountFailedTests;
}
