#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <Error_UnitTest.h>
#include <trcRecorder.h>

int group_setup_Error(void** ppState);
int group_teardown_Error(void** ppState);
int setup_Error(void** ppState);
int teardown_Error(void** ppState);

void test_xTraceErrorInitialize(void** ppState);
void test_xTraceWarning(void** ppState);
void test_xTraceError(void** ppState);
void test_xTraceErrorGetLast(void** ppState);
void test_xTraceErrorClear(void** ppState);

/******************************************************************************************
*                                        TEST STATE                                       *
******************************************************************************************/

typedef struct ErrorTestState
{
	/* Buffer */
	TraceEntryTable_t xEntryBuffer;
	TraceEntryIndexTable_t xEntryIndexBuffer;
	TraceDiagnosticsData_t xDiagnosticsBuffer;
	TraceErrorData_t xErrorBuffer;
	

	/* Test Objects */
	TraceEntryHandle_t xEntryHandle;
} ErrorTestState_t;

ErrorTestState_t xErrorTestState;

uint32_t uiTestMaxBytesTruncated = 0;
uint32_t MOCK_uiStackMonitorNoSlots = 0;
volatile uint32_t MOCK_RecorderInitialized = 1;
uint32_t MOCK_uiCriticalSection = 0;
uint32_t MOCK_uiCriticalSectionExpectedValue = 0;
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

int group_setup_Error(void **ppState)
{
	*ppState = &xErrorTestState;

	assert_int_equal(sizeof(TraceUnsignedBaseType_t), sizeof(void*));
	assert_int_equal(sizeof(TraceUnsignedBaseType_t), sizeof(TraceBaseType_t));

	return 0;
}

int group_teardown_Error(void **ppState)
{
	ErrorTestState_t* pxState = (ErrorTestState_t*)*ppState;

	return 0;
}

int setup_Error(void **ppState)
{
	ErrorTestState_t* pxState = (ErrorTestState_t*)*ppState;

	assert_true(TRC_ENTRY_TABLE_SLOTS > 1);

	assert_int_equal_cs(xTraceEntryTableInitialize(&pxState->xEntryBuffer), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEntryIndexTableInitialize(&pxState->xEntryIndexBuffer), TRC_SUCCESS);
	assert_int_equal_cs(xTraceDiagnosticsInitialize(&pxState->xDiagnosticsBuffer), TRC_SUCCESS);
	assert_int_equal_cs(xTraceErrorInitialize(&pxState->xErrorBuffer), TRC_SUCCESS);

	pxState->xEntryHandle = 0;

	return 0;
}

int teardown_Error(void **ppState)
{
	ErrorTestState_t* pxState = (ErrorTestState_t*)*ppState;

	pxState->xEntryHandle = 0;

	return 0;
}

/******************************************************************************************
*                                        TESTS                                            *
******************************************************************************************/

void test_xTraceErrorInitialize(void **ppState)
{
	ErrorTestState_t* pxState = (ErrorTestState_t*)*ppState;

	assert_int_equal_cs(xTraceErrorInitialize(0), TRC_FAIL);
	assert_int_equal_cs(xTraceErrorInitialize(&pxState->xErrorBuffer), TRC_SUCCESS);
}

void test_xTraceWarning(void **ppState)
{
	ErrorTestState_t* pxState = (ErrorTestState_t*)*ppState;

	assert_int_equal_cs(xTraceWarning(0xDEADBEEF), TRC_SUCCESS); /* Unknown errors will be accepted and printed */
	assert_int_equal_cs(xTraceWarning(TRC_WARNING_STREAM_PORT_WRITE), TRC_SUCCESS);
}

void test_xTraceError(void **ppState)
{
	assert_int_equal_cs(xTraceWarning(0xDEADBEEF), TRC_SUCCESS); /* Unknown errors will be accepted and printed */
	assert_int_equal_cs(xTraceWarning(TRC_WARNING_STREAM_PORT_WRITE), TRC_SUCCESS);
}

void test_xTraceErrorGetLast(void **ppState)
{
	const char* szDesc = 0;

	assert_int_equal_cs(xTraceErrorGetLast(0), TRC_FAIL);
	
	assert_int_equal_cs(xTraceErrorClear(), TRC_SUCCESS);

	assert_int_equal_cs(xTraceErrorGetLast(&szDesc), TRC_FAIL); /* no error */

	assert_int_equal_cs(xTraceWarning(TRC_WARNING_STREAM_PORT_WRITE), TRC_SUCCESS);
	assert_int_equal_cs(xTraceErrorGetLast(&szDesc), TRC_FAIL); /* no error has been flagged, only warning */

	assert_int_equal_cs(xTraceError(TRC_ERROR_DWT_NOT_SUPPORTED), TRC_SUCCESS);
	assert_int_equal_cs(xTraceErrorGetLast(&szDesc), TRC_SUCCESS);
	assert_int_not_equal((TraceUnsignedBaseType_t)szDesc, (TraceUnsignedBaseType_t)0);
}

void test_xTraceErrorClear(void** ppState)
{
	const char* szDesc = 0;
	
	assert_int_equal_cs(xTraceError(TRC_ERROR_DWT_NOT_SUPPORTED), TRC_SUCCESS);
	assert_int_equal_cs(xTraceErrorGetLast(&szDesc), TRC_SUCCESS);
	
	assert_int_equal_cs(xTraceErrorClear(), TRC_SUCCESS);
	
	assert_int_equal_cs(xTraceErrorGetLast(&szDesc), TRC_FAIL);
}

uint32_t runErrorTests(void)
{
	uint32_t uiCountFailedTests = 0;

	const struct CMUnitTest aTests_trcError[] =
	{
		cmocka_unit_test_setup_teardown(test_xTraceErrorInitialize, setup_Error, teardown_Error),
		cmocka_unit_test_setup_teardown(test_xTraceWarning, setup_Error, teardown_Error),
		cmocka_unit_test_setup_teardown(test_xTraceError, setup_Error, teardown_Error),
		cmocka_unit_test_setup_teardown(test_xTraceErrorGetLast, setup_Error, teardown_Error),
		cmocka_unit_test_setup_teardown(test_xTraceErrorClear, setup_Error, teardown_Error),
	};

	uiCountFailedTests += cmocka_run_group_tests(aTests_trcError, group_setup_Error, group_teardown_Error);

	return uiCountFailedTests;
}
