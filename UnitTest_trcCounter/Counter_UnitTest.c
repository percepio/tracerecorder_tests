#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <Counter_UnitTest.h>
#include <trcRecorder.h>

int group_setup_Counter(void** ppState);
int group_teardown_Counter(void** ppState);
int setup_Counter(void** ppState);
int teardown_Counter(void** ppState);

void test_xTraceCounterSetCallback(void** ppState);
void test_xTraceCounterCreate(void** ppState);
void test_xTraceCounterAdd(void** ppState);
void test_xTraceCounterSet(void** ppState);
void test_xTraceCounterSetBeyondLimit(void** ppState);
void test_xTraceCounterGet(void** ppState);
void test_xTraceCounterIncrease(void** ppState);
void test_xTraceCounterDecrease(void** ppState);
void test_xTraceCounterGetUpperLimit(void** ppState);
void test_xTraceCounterGetLowerLimit(void** ppState);

void counter_callback(TraceCounterHandle_t xCounterHandle);

/******************************************************************************************
*                                        TEST STATE                                       *
******************************************************************************************/

typedef struct CounterTestState
{
	/* Pointer to Buffer */
	TraceCounterData_t xCounterBuffer;
	TraceEntryTable_t xEntryTableBuffer;
	TraceEntryIndexTable_t xEntryIndexTableBuffer;
	TraceDiagnosticsData_t xDiagnosticsBuffer;

	/* Test Objects */
	TraceCounterHandle_t xCounterHandle;

	/* Test Parameters */
	const char *szCounterName;
} CounterTestState_t;

uint32_t MOCK_uiTestTimestampCounter = 0;

volatile uint32_t MOCK_RecorderEnabled = 1;
volatile uint32_t MOCK_RecorderInitialized = 1;
uint8_t MOCK_tmpBuffer[TRC_MAX_BLOB_SIZE];
uint8_t MOCK_streamPortBuffer[TRC_MAX_BLOB_SIZE];
uint32_t MOCK_streamPortMaxTransferSize = 0xFFFFFFFF;
uint32_t MOCK_uiCurrentCore = 0;
int32_t MOCK_iISRNesting = -1;
uint32_t MOCK_uiCriticalSection = 0;
uint32_t MOCK_uiCriticalSectionExpectedValue = 0;
TraceEventHandle_t MOCK_EVENT_BEGIN_xEventHandle = 0;
TraceEventHandle_t MOCK_EVENT_END_xEventHandle = 0;
TraceUnsignedBaseType_t MOCK_EVENT_CREATE0_uiEventCode = 0;
TraceUnsignedBaseType_t MOCK_EVENT_CREATE1_uiEventCode = 0;
TraceUnsignedBaseType_t MOCK_EVENT_CREATE2_uiEventCode = 0;
TraceUnsignedBaseType_t MOCK_EVENT_CREATE3_uiEventCode = 0;
TraceUnsignedBaseType_t MOCK_EVENT_CREATE4_uiEventCode = 0;
TraceUnsignedBaseType_t MOCK_EVENT_CREATE5_uiEventCode = 0;
TraceUnsignedBaseType_t MOCK_EVENT_CREATE6_uiEventCode = 0;

CounterTestState_t xCounterTestState;
TraceCounterHandle_t xCallbackCounterHandle = 0;

void counter_callback(TraceCounterHandle_t xCounterHandle)
{
	xCallbackCounterHandle = xCounterHandle;
}

/******************************************************************************************
*                                        SETUP / TEARDOWN                                 *
******************************************************************************************/

int group_setup_Counter(void **ppState)
{
	MOCK_RecorderInitialized = 1;
	xCounterTestState.xCounterHandle = 0;
	xCounterTestState.szCounterName = "TEST_COUNTER";
	*ppState = &xCounterTestState;

	MOCK_uiCriticalSection = 0;
	MOCK_uiCriticalSectionExpectedValue = 0;

	assert_int_equal(sizeof(TraceUnsignedBaseType_t), sizeof(void*));
	assert_int_equal(sizeof(TraceUnsignedBaseType_t), sizeof(TraceBaseType_t));

	return 0;
}

int group_teardown_Counter(void **ppState)
{
	CounterTestState_t *pxState = (CounterTestState_t*)*ppState;

	MOCK_RecorderInitialized = 1;

	pxState->xCounterHandle = 0;
	pxState->szCounterName = "";

	return 0;
}

int setup_Counter(void **ppState)
{
	CounterTestState_t *pxState = (CounterTestState_t*)*ppState;

	MOCK_RecorderInitialized = 1;

	MOCK_uiCriticalSection = 0;
	MOCK_uiCriticalSectionExpectedValue = 0;

	assert_int_equal_cs(xTraceEntryTableInitialize(&pxState->xEntryTableBuffer), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEntryIndexTableInitialize(&pxState->xEntryIndexTableBuffer), TRC_SUCCESS);
	assert_int_equal_cs(xTraceDiagnosticsInitialize(&pxState->xDiagnosticsBuffer), TRC_SUCCESS);
	assert_int_equal_cs(xTraceCounterInitialize(&pxState->xCounterBuffer), TRC_SUCCESS);

	return 0;
}

int teardown_Counter(void **ppState)
{
	CounterTestState_t *pxState = (CounterTestState_t*)*ppState;

	MOCK_RecorderInitialized = 1;

	return 0;
}

/******************************************************************************************
*                                        TESTS                                            *
******************************************************************************************/

void test_xTraceCounterSetCallback(void** ppState)
{
	CounterTestState_t* pxState = (CounterTestState_t*)*ppState;
	TraceBaseType_t xValue = 0;
	TraceBaseType_t xLimit = 0;

	assert_int_equal_cs(xTraceEntryTableInitialize(&pxState->xEntryTableBuffer), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEntryIndexTableInitialize(&pxState->xEntryIndexTableBuffer), TRC_SUCCESS);
	assert_int_equal_cs(xTraceDiagnosticsInitialize(&pxState->xDiagnosticsBuffer), TRC_SUCCESS);

	assert_int_equal_cs(xTraceCounterSetCallback(counter_callback), TRC_FAIL); /* before initialize */

	assert_int_equal_cs(xTraceCounterInitialize(&pxState->xCounterBuffer), TRC_SUCCESS);

	assert_int_equal_cs(xTraceCounterSetCallback(0), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceCounterSetCallback(counter_callback), TRC_SUCCESS);
}

void test_xTraceCounterCreate(void **ppState)
{
	CounterTestState_t *pxState = (CounterTestState_t*)*ppState;

	assert_int_equal_cs(xTraceCounterCreate(0, 0, INT32_MIN, INT32_MAX, &pxState->xCounterHandle), TRC_SUCCESS); /* null test */
	assert_int_equal_cs(xTraceCounterCreate(pxState->szCounterName, 0, INT32_MIN, INT32_MAX, 0), TRC_FAIL); /* null test */
	assert_int_equal_cs(xTraceCounterCreate(pxState->szCounterName, -1, 0, 5, 0), TRC_FAIL); /* initial value below lower limit */
	assert_int_equal_cs(xTraceCounterCreate(pxState->szCounterName, 6, 0, 5, 0), TRC_FAIL); /* initial value above upper limit */

	assert_int_equal_cs(xTraceCounterCreate(pxState->szCounterName, 0, INT32_MIN, INT32_MAX, &pxState->xCounterHandle), TRC_SUCCESS);
}

void test_xTraceCounterIncrease(void** ppState)
{
	CounterTestState_t* pxState = (CounterTestState_t*)*ppState;
	TraceBaseType_t xValue = 0;

	assert_int_equal_cs(xTraceCounterCreate(pxState->szCounterName, 10, INT32_MIN, INT32_MAX, &pxState->xCounterHandle), TRC_SUCCESS);

	assert_int_equal_cs(xTraceCounterGet(pxState->xCounterHandle, &xValue), TRC_SUCCESS);
	assert_int_equal_cs(xValue, (TraceBaseType_t)10);

	assert_int_equal_cs(xTraceCounterIncrease(pxState->xCounterHandle), TRC_SUCCESS);

	assert_int_equal_cs(xTraceCounterGet(pxState->xCounterHandle, &xValue), TRC_SUCCESS);
	assert_int_equal_cs(xValue, (TraceBaseType_t)11);

	assert_int_equal_cs(xTraceCounterIncrease(pxState->xCounterHandle), TRC_SUCCESS);

	assert_int_equal_cs(xTraceCounterGet(pxState->xCounterHandle, &xValue), TRC_SUCCESS);
	assert_int_equal_cs(xValue, (TraceBaseType_t)12);
}

void test_xTraceCounterDecrease(void** ppState)
{
	CounterTestState_t* pxState = (CounterTestState_t*)*ppState;
	TraceBaseType_t xValue = 0;

	assert_int_equal_cs(xTraceCounterCreate(pxState->szCounterName, 10, INT32_MIN, INT32_MAX, &pxState->xCounterHandle), TRC_SUCCESS);

	assert_int_equal_cs(xTraceCounterGet(pxState->xCounterHandle, &xValue), TRC_SUCCESS);
	assert_int_equal_cs(xValue, (TraceBaseType_t)10);

	assert_int_equal_cs(xTraceCounterDecrease(pxState->xCounterHandle), TRC_SUCCESS);

	assert_int_equal_cs(xTraceCounterGet(pxState->xCounterHandle, &xValue), TRC_SUCCESS);
	assert_int_equal_cs(xValue, (TraceBaseType_t)9);

	assert_int_equal_cs(xTraceCounterDecrease(pxState->xCounterHandle), TRC_SUCCESS);

	assert_int_equal_cs(xTraceCounterGet(pxState->xCounterHandle, &xValue), TRC_SUCCESS);
	assert_int_equal_cs(xValue, (TraceBaseType_t)8);
}


void test_xTraceCounterAdd(void **ppState)
{
	CounterTestState_t *pxState = (CounterTestState_t*)*ppState;
	TraceBaseType_t xValue = 0;

	assert_int_equal_cs(xTraceCounterCreate(pxState->szCounterName, 0, INT32_MIN, INT32_MAX, &pxState->xCounterHandle), TRC_SUCCESS);

	assert_int_equal_cs(xTraceCounterGet(pxState->xCounterHandle, &xValue), TRC_SUCCESS);
	assert_int_equal_cs(xValue, (TraceBaseType_t)0);

	assert_int_equal_cs(xTraceCounterAdd(pxState->xCounterHandle, 0xDEADBEEF), TRC_SUCCESS);

	assert_int_equal_cs(xTraceCounterGet(pxState->xCounterHandle, &xValue), TRC_SUCCESS);
	assert_int_equal_cs(xValue, (TraceBaseType_t)0xDEADBEEF);

	assert_int_equal_cs(xTraceCounterAdd(pxState->xCounterHandle, -(TraceBaseType_t)(0xD00DB00F)), TRC_SUCCESS);

	assert_int_equal_cs(xTraceCounterGet(pxState->xCounterHandle, &xValue), TRC_SUCCESS);
	assert_int_equal_cs(xValue, (TraceBaseType_t)0x0EA00EE0);
}

void test_xTraceCounterSet(void **ppState)
{
	CounterTestState_t *pxState = (CounterTestState_t*)*ppState;
	TraceBaseType_t xValue = 0;

	assert_int_equal_cs(xTraceCounterCreate(pxState->szCounterName, 0, INT32_MIN, INT32_MAX, &pxState->xCounterHandle), TRC_SUCCESS);

	assert_int_equal_cs(xTraceCounterGet(pxState->xCounterHandle, &xValue), TRC_SUCCESS);
	assert_int_equal_cs(xValue, (TraceBaseType_t)0);

	assert_int_equal_cs(xTraceCounterSet(pxState->xCounterHandle, 0xDEADBEEF), TRC_SUCCESS);

	assert_int_equal_cs(xTraceCounterGet(pxState->xCounterHandle, &xValue), TRC_SUCCESS);
	assert_int_equal_cs(xValue, (TraceBaseType_t)0xDEADBEEF);
}

void test_xTraceCounterSetBeyondLimit(void** ppState)
{
	CounterTestState_t* pxState = (CounterTestState_t*)*ppState;
	TraceBaseType_t xValue = 0;

	assert_int_equal_cs(xTraceCounterCreate(pxState->szCounterName, 1, 0, 2, &pxState->xCounterHandle), TRC_SUCCESS);

	xCallbackCounterHandle = (TraceCounterHandle_t)0x1234;
	assert_int_equal_cs(xTraceCounterAdd(pxState->xCounterHandle, -2), TRC_SUCCESS);
	assert_int_equal_cs((TraceUnsignedBaseType_t)xCallbackCounterHandle, (TraceUnsignedBaseType_t)0x1234); /* No callback is set */
	
	assert_int_equal_cs(xTraceCounterAdd(pxState->xCounterHandle, 2), TRC_SUCCESS);
	assert_int_equal_cs((TraceUnsignedBaseType_t)xCallbackCounterHandle, (TraceUnsignedBaseType_t)0x1234); /* No callback is set */

	xTraceCounterSetCallback(counter_callback);

	xCallbackCounterHandle = (TraceCounterHandle_t)0x1234;
	assert_int_equal_cs(xTraceCounterAdd(pxState->xCounterHandle, -2), TRC_SUCCESS);
	assert_int_equal_cs((TraceUnsignedBaseType_t)xCallbackCounterHandle, (TraceUnsignedBaseType_t)pxState->xCounterHandle);

	xCallbackCounterHandle = (TraceCounterHandle_t)0x1234;
	assert_int_equal_cs(xTraceCounterAdd(pxState->xCounterHandle, 2), TRC_SUCCESS);
	assert_int_equal_cs((TraceUnsignedBaseType_t)xCallbackCounterHandle, (TraceUnsignedBaseType_t)0x1234);

	xCallbackCounterHandle = (TraceCounterHandle_t)0x1234;
	assert_int_equal_cs(xTraceCounterAdd(pxState->xCounterHandle, 2), TRC_SUCCESS);
	assert_int_equal_cs((TraceUnsignedBaseType_t)xCallbackCounterHandle, (TraceUnsignedBaseType_t)pxState->xCounterHandle);

	xCallbackCounterHandle = (TraceCounterHandle_t)0x1234;
	assert_int_equal_cs(xTraceCounterAdd(pxState->xCounterHandle, -2), TRC_SUCCESS);
	assert_int_equal_cs((TraceUnsignedBaseType_t)xCallbackCounterHandle, (TraceUnsignedBaseType_t)0x1234);
}

void test_xTraceCounterGet(void **ppState)
{
	CounterTestState_t *pxState = (CounterTestState_t*)*ppState;
	TraceBaseType_t xValue = 0;
	
	assert_int_equal_cs(xTraceCounterCreate(pxState->szCounterName, 12345678, INT32_MIN, INT32_MAX, &pxState->xCounterHandle), TRC_SUCCESS);

	assert_int_equal_cs(xTraceCounterGet(pxState->xCounterHandle, &xValue), TRC_SUCCESS);
	assert_int_equal_cs(xValue, (TraceBaseType_t)12345678);
}

void test_xTraceCounterGetUpperLimit(void** ppState)
{
	CounterTestState_t* pxState = (CounterTestState_t*)*ppState;
	TraceBaseType_t xLimit = 0;

	assert_int_equal_cs(xTraceCounterCreate(pxState->szCounterName, 0, INT32_MIN, INT32_MAX, &pxState->xCounterHandle), TRC_SUCCESS);

	assert_int_equal_cs(xTraceCounterGetUpperLimit(pxState->xCounterHandle, &xLimit), TRC_SUCCESS);
	assert_int_equal_cs(xLimit, (TraceBaseType_t)INT32_MAX);
}

void test_xTraceCounterGetLowerLimit(void** ppState)
{
	CounterTestState_t* pxState = (CounterTestState_t*)*ppState;
	TraceBaseType_t xLimit = 0;

	assert_int_equal_cs(xTraceCounterCreate(pxState->szCounterName, 0, INT32_MIN, INT32_MAX, &pxState->xCounterHandle), TRC_SUCCESS);

	assert_int_equal_cs(xTraceCounterGetLowerLimit(pxState->xCounterHandle, &xLimit), TRC_SUCCESS);
	assert_int_equal_cs(xLimit, (TraceBaseType_t)INT32_MIN);
}

uint32_t runCounterTests(void)
{
	uint32_t uiCountFailedTests = 0;

	const struct CMUnitTest aTests_trcCounter[] =
	{
		cmocka_unit_test_teardown(test_xTraceCounterSetCallback, teardown_Counter),
		cmocka_unit_test_setup_teardown(test_xTraceCounterCreate, setup_Counter, teardown_Counter),
		cmocka_unit_test_setup_teardown(test_xTraceCounterSet, setup_Counter, teardown_Counter),
		cmocka_unit_test_setup_teardown(test_xTraceCounterSetBeyondLimit, setup_Counter, teardown_Counter),
		cmocka_unit_test_setup_teardown(test_xTraceCounterIncrease, setup_Counter, teardown_Counter),
		cmocka_unit_test_setup_teardown(test_xTraceCounterDecrease, setup_Counter, teardown_Counter),
		cmocka_unit_test_setup_teardown(test_xTraceCounterAdd, setup_Counter, teardown_Counter),
		cmocka_unit_test_setup_teardown(test_xTraceCounterGet, setup_Counter, teardown_Counter),
		cmocka_unit_test_setup_teardown(test_xTraceCounterGetLowerLimit, setup_Counter, teardown_Counter),
		cmocka_unit_test_setup_teardown(test_xTraceCounterGetUpperLimit, setup_Counter, teardown_Counter),
	};

	uiCountFailedTests += cmocka_run_group_tests(aTests_trcCounter, group_setup_Counter, group_teardown_Counter);

	return uiCountFailedTests;
}
