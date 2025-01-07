#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <Interval_UnitTest.h>
#include <trcRecorder.h>


/******************************************************************************************
*                                        TEST STATE                                       *
******************************************************************************************/

typedef struct CounterTestState
{
	/* Pointer to Buffer */
	TraceEntryTable_t xEntryTableBuffer;
	TraceEntryIndexTable_t xEntryIndexTableBuffer;
	TraceDiagnosticsData_t xDiagnosticsBuffer;

	/* Test Objects */
	TraceIntervalChannelHandle_t xIntervalHandle;
	TraceIntervalChannelSetHandle_t xIntervalSetHandle;

	/* Test Parameters */
	const char *szIntervalName;
} IntervalTestState_t;

IntervalTestState_t xIntervalTestState;
volatile uint32_t MOCK_RecorderInitialized = 1;
volatile uint32_t MOCK_RecorderEnabled = 1;
uint32_t MOCK_uiCriticalSection = 0;
uint32_t MOCK_uiCriticalSectionExpectedValue = 0;
uint32_t MOCK_uiTestTimestampCounter = 0;
uint8_t MOCK_tmpBuffer[TRC_MAX_BLOB_SIZE];
uint8_t MOCK_streamPortBuffer[TRC_MAX_BLOB_SIZE];
uint32_t MOCK_streamPortMaxTransferSize = 0xFFFFFFFF;
TraceEventHandle_t MOCK_EVENT_BEGIN_xEventHandle = 0;
TraceEventHandle_t MOCK_EVENT_END_xEventHandle = 0;
TraceUnsignedBaseType_t MOCK_EVENT_CREATE0_uiEventCode = 0;
TraceUnsignedBaseType_t MOCK_EVENT_CREATE1_uiEventCode = 0;
TraceUnsignedBaseType_t MOCK_EVENT_CREATE2_uiEventCode = 0;
TraceUnsignedBaseType_t MOCK_EVENT_CREATE3_uiEventCode = 0;
TraceUnsignedBaseType_t MOCK_EVENT_CREATE4_uiEventCode = 0;
TraceUnsignedBaseType_t MOCK_EVENT_CREATE5_uiEventCode = 0;
TraceUnsignedBaseType_t MOCK_EVENT_CREATE6_uiEventCode = 0;

/******************************************************************************************
*                                        SETUP / TEARDOWN                                 *
******************************************************************************************/

int group_setup_Interval(void **ppState)
{
	xIntervalTestState.xIntervalHandle = 0;
	xIntervalTestState.szIntervalName = "TEST_INTERVAL";
	*ppState = &xIntervalTestState;

	assert_int_equal(sizeof(TraceUnsignedBaseType_t), sizeof(void*));
	assert_int_equal(sizeof(TraceUnsignedBaseType_t), sizeof(TraceBaseType_t));

	return 0;
}

int group_teardown_Interval(void **ppState)
{
	IntervalTestState_t*pxState = (IntervalTestState_t*)*ppState;

	pxState->xIntervalHandle = 0;
	pxState->szIntervalName = "";

	return 0;
}

int setup_emptyInterval(void **ppState)
{
	IntervalTestState_t *pxState = (IntervalTestState_t*)*ppState;


	assert_int_equal_cs(xTraceEntryTableInitialize(&pxState->xEntryTableBuffer), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEntryIndexTableInitialize(&pxState->xEntryIndexTableBuffer), TRC_SUCCESS);
	assert_int_equal_cs(xTraceDiagnosticsInitialize(&pxState->xDiagnosticsBuffer), TRC_SUCCESS);

	assert_int_equal_cs(xTraceIntervalChannelSetCreate("TEST_INTERVAL_SET", &pxState->xIntervalSetHandle), TRC_SUCCESS);

	assert_int_equal_cs(xTraceIntervalChannelCreate(pxState->szIntervalName, pxState->xIntervalSetHandle , &pxState->xIntervalHandle), TRC_SUCCESS);

	return 0;
}

int teardown_Interval(void **ppState)
{
	IntervalTestState_t *pxState = (IntervalTestState_t*)*ppState;

	pxState->xIntervalHandle = 0;
	pxState->szIntervalName = "";

	return 0;
}

/******************************************************************************************
*                                        TESTS                                            *
******************************************************************************************/

void test_xTraceIntervalSetCreate(void** ppState)
{
	IntervalTestState_t* pxState = (IntervalTestState_t*)*ppState;

	assert_int_equal_cs(xTraceIntervalChannelSetCreate(0, &pxState->xIntervalSetHandle), TRC_SUCCESS); /* null test */
	assert_int_equal_cs(xTraceIntervalChannelSetCreate(pxState->szIntervalName, 0), TRC_FAIL); /* null test */

	pxState->xIntervalSetHandle = 0;
	assert_int_equal_cs(xTraceIntervalChannelSetCreate("TEST_INTERVAL_SET", &pxState->xIntervalSetHandle), TRC_SUCCESS);

	assert_int_not_equal((TraceUnsignedBaseType_t)pxState->xIntervalSetHandle, (TraceUnsignedBaseType_t)0);
}

void test_xTraceIntervalCreate(void **ppState)
{
	IntervalTestState_t *pxState = (IntervalTestState_t*)*ppState;
	TraceUnsignedBaseType_t uxState;

	assert_int_equal_cs(xTraceIntervalChannelSetCreate("TEST_INTERVAL_SET", &pxState->xIntervalSetHandle), TRC_SUCCESS);

	assert_int_equal_cs(xTraceIntervalChannelCreate(0, pxState->xIntervalSetHandle , &pxState->xIntervalHandle), TRC_SUCCESS); /* null test */
	assert_int_equal_cs(xTraceIntervalChannelCreate(pxState->szIntervalName, 0, &pxState->xIntervalHandle), TRC_FAIL); /* null test */
	assert_int_equal_cs(xTraceIntervalChannelCreate(pxState->szIntervalName, pxState->xIntervalSetHandle, 0), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceIntervalChannelCreate(pxState->szIntervalName, pxState->xIntervalSetHandle, &pxState->xIntervalHandle), TRC_SUCCESS);

	assert_int_not_equal((TraceUnsignedBaseType_t)pxState->xIntervalHandle, (TraceUnsignedBaseType_t)0);

	assert_int_equal_cs(xTraceIntervalGetState(pxState->xIntervalHandle, &uxState), TRC_SUCCESS);

	assert_int_equal_cs((TraceUnsignedBaseType_t)uxState, (TraceUnsignedBaseType_t)pxState->xIntervalSetHandle);
}

void test_xTraceIntervalGetState(void **ppState)
{
	TraceUnsignedBaseType_t uxState;
	IntervalTestState_t *pxState = (IntervalTestState_t*)*ppState;

	assert_int_equal_cs(xTraceIntervalGetState(0, &uxState), TRC_FAIL);
	assert_int_equal_cs(xTraceIntervalGetState(pxState->xIntervalHandle, 0), TRC_FAIL);

	assert_int_equal_cs(xTraceIntervalGetState(pxState->xIntervalHandle, &uxState), TRC_SUCCESS);
}

void test_xTraceIntervalStart(void **ppState)
{
	IntervalTestState_t *pxState = (IntervalTestState_t*)*ppState;
	TraceIntervalInstanceHandle_t xIntervalInstanceHandle1;
	TraceIntervalInstanceHandle_t xIntervalInstanceHandle2;

	assert_int_equal_cs(xTraceIntervalStart(0, 0, &xIntervalInstanceHandle1), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceIntervalStart(pxState->xIntervalHandle, 0, 0), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceIntervalStart(pxState->xIntervalHandle, 0, &xIntervalInstanceHandle1), TRC_SUCCESS);

	assert_int_equal_cs(xTraceIntervalStart(pxState->xIntervalHandle, 0, &xIntervalInstanceHandle2), TRC_SUCCESS);

	assert_int_not_equal((TraceUnsignedBaseType_t)xIntervalInstanceHandle1, (TraceUnsignedBaseType_t)xIntervalInstanceHandle2);
}

void test_xTraceIntervalStop(void **ppState)
{
	IntervalTestState_t *pxState = (IntervalTestState_t*)*ppState;
	TraceIntervalInstanceHandle_t xIntervalInstanceHandle1;
	TraceIntervalInstanceHandle_t xIntervalInstanceHandle2;

	assert_int_equal_cs(xTraceIntervalStop(0, 0), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceIntervalStart(pxState->xIntervalHandle, 0, &xIntervalInstanceHandle1), TRC_SUCCESS);

	assert_int_equal_cs(xTraceIntervalStart(pxState->xIntervalHandle, 0, &xIntervalInstanceHandle2), TRC_SUCCESS);

	assert_int_not_equal((TraceUnsignedBaseType_t)xIntervalInstanceHandle1, (TraceUnsignedBaseType_t)xIntervalInstanceHandle2);

	assert_int_equal_cs(xTraceIntervalStop(pxState->xIntervalHandle, xIntervalInstanceHandle1), TRC_SUCCESS);

	assert_int_equal_cs(xTraceIntervalStop(pxState->xIntervalHandle, xIntervalInstanceHandle2), TRC_SUCCESS);
}

uint32_t runIntervalTests(void)
{
	uint32_t uiCountFailedTests = 0;

	const struct CMUnitTest aTests_trcInterval[] =
	{
		cmocka_unit_test_setup_teardown(test_xTraceIntervalSetCreate, setup_emptyInterval, teardown_Interval),
		cmocka_unit_test_setup_teardown(test_xTraceIntervalCreate, setup_emptyInterval, teardown_Interval),
		cmocka_unit_test_setup_teardown(test_xTraceIntervalGetState, setup_emptyInterval, teardown_Interval),
		cmocka_unit_test_setup_teardown(test_xTraceIntervalStart, setup_emptyInterval, teardown_Interval),
		cmocka_unit_test_setup_teardown(test_xTraceIntervalStop, setup_emptyInterval, teardown_Interval),
	};

	uiCountFailedTests += cmocka_run_group_tests(aTests_trcInterval, group_setup_Interval, group_teardown_Interval);

	return uiCountFailedTests;
}
