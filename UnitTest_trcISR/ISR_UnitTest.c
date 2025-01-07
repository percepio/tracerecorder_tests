#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <ISR_UnitTest.h>
#include <trcRecorder.h>

int group_teardown_ISR(void** ppState);
int group_setup_ISR(void** ppState);
int setup_ISR(void** ppState);
int teardown_ISR(void** ppState);

void test_xTraceISRInitialize(void** ppState);
void test_xTraceISRRegister(void** ppState);
void test_xTraceISRGetCurrentAndCurrentNesting(void** ppState);
void test_xTraceISRBegin(void** ppState);
void test_xTraceISREnd(void** ppState);

/******************************************************************************************
*                                        TEST STATE                                       *
******************************************************************************************/

typedef struct EventBufferTestState
{
	TraceISRData_t xISRInfoBuffer;
	TraceEntryTable_t xEntryTableBuffer;
	TraceEntryIndexTable_t xEntryIndexTableBuffer;
	TraceDiagnosticsData_t xDiagnosticsBuffer;
	uint32_t _dummy;
} ISRTestState_t;

ISRTestState_t xISRTestState;
volatile uint32_t MOCK_RecorderEnabled = 1;
volatile uint32_t MOCK_RecorderInitialized = 1;
uint32_t MOCK_uiCurrentCore = 0;
uint32_t MOCK_uiCriticalSection = 0;
uint32_t MOCK_uiCriticalSectionExpectedValue = 0;
uint8_t MOCK_tmpBuffer[TRC_MAX_BLOB_SIZE];
uint8_t MOCK_streamPortBuffer[TRC_MAX_BLOB_SIZE];
uint32_t MOCK_streamPortMaxTransferSize = 0xFFFFFFFF;
void* MOCK_TASK_pvCurrentTask = (void*)0;

/******************************************************************************************
*                                        SETUP / TEARDOWN                                 *
******************************************************************************************/

int group_setup_ISR(void **ppState)
{
	*ppState = &xISRTestState;

	assert_int_equal(sizeof(TraceUnsignedBaseType_t), sizeof(void*));
	assert_int_equal(sizeof(TraceUnsignedBaseType_t), sizeof(TraceBaseType_t));

	return 0;
}

int group_teardown_ISR(void **ppState)
{
	ISRTestState_t* pxState = (ISRTestState_t*)*ppState;

	return 0;
}

int setup_ISR(void **ppState)
{
	ISRTestState_t* pxState = (ISRTestState_t*)*ppState;

	assert_int_equal_cs(xTraceEntryTableInitialize(&xISRTestState.xEntryTableBuffer), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEntryIndexTableInitialize(&pxState->xEntryIndexTableBuffer), TRC_SUCCESS);
	assert_int_equal_cs(xTraceISRInitialize(&xISRTestState.xISRInfoBuffer), TRC_SUCCESS);
	assert_int_equal_cs(xTraceDiagnosticsInitialize(&pxState->xDiagnosticsBuffer), TRC_SUCCESS);

	return 0;
}

int teardown_ISR(void **ppState)
{
	return 0;
}

/******************************************************************************************
*                                        TESTS                                            *
******************************************************************************************/

void test_xTraceISRInitialize(void** ppState)
{
	ISRTestState_t* pxState = (ISRTestState_t*)*ppState;

	assert_int_equal_asserts_enabled(xTraceISRInitialize(0), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceISRInitialize(&xISRTestState.xISRInfoBuffer), TRC_SUCCESS);
}

void test_xTraceISRRegister(void** ppState)
{
	ISRTestState_t* pxState = (ISRTestState_t*)*ppState;
	TraceISRHandle_t xISRHandle = 0;
	const char* szName;
	TraceUnsignedBaseType_t uxState;

	assert_int_equal_asserts_enabled(xTraceISRRegister("TEST_ISR", 0xFF, 0), TRC_FAIL); /* null test */
	
	xISRHandle = 0;
	assert_int_equal_cs_create_data2(xTraceISRRegister(0, 1, &xISRHandle), TRC_SUCCESS, PSF_EVENT_DEFINE_ISR, (TraceUnsignedBaseType_t)xISRHandle, 1, "", 1);
	
	xISRHandle = 0;
	assert_int_equal_cs_create_data2(xTraceISRRegister("TEST_ISR1", 1, &xISRHandle), TRC_SUCCESS, PSF_EVENT_DEFINE_ISR, xISRHandle, 1, "TEST_ISR1", strlen("TEST_ISR1") + 1);
	assert_int_not_equal((TraceUnsignedBaseType_t)xISRHandle, (TraceUnsignedBaseType_t)0);
	assert_int_equal_cs(xTraceEntryGetSymbol((TraceEntryHandle_t)xISRHandle, &szName), TRC_SUCCESS);
	assert_string_equal(szName, "TEST_ISR1");
	assert_int_equal_cs(xTraceEntryGetState((TraceEntryHandle_t)xISRHandle, 0, &uxState), TRC_SUCCESS);
	assert_int_equal_cs(uxState, 1);
	
	xISRHandle = 0;
	assert_int_equal_cs_create_data2(xTraceISRRegister("TEST_ISR2", 5, &xISRHandle), TRC_SUCCESS, PSF_EVENT_DEFINE_ISR, xISRHandle, 5, "TEST_ISR2", strlen("TEST_ISR2") + 1);
	assert_int_not_equal((TraceUnsignedBaseType_t)xISRHandle, (TraceUnsignedBaseType_t)0);
	assert_int_equal_cs(xTraceEntryGetSymbol((TraceEntryHandle_t)xISRHandle, &szName), TRC_SUCCESS);
	assert_string_equal(szName, "TEST_ISR2");
	assert_int_equal_cs(xTraceEntryGetState((TraceEntryHandle_t)xISRHandle, 0, &uxState), TRC_SUCCESS);
	assert_int_equal_cs(uxState, 5);
}

void test_xTraceISRGetCurrentAndCurrentNesting(void** ppState)
{
	ISRTestState_t* pxState = (ISRTestState_t*)*ppState;

	int32_t iISRNesting;
	TraceISRHandle_t xISRHandle = 0;

	assert_int_equal_asserts_enabled(xTraceISRGetCurrentNesting(0), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceISRGetCurrentNesting(&iISRNesting), TRC_SUCCESS);
	assert_int_equal_cs(iISRNesting, -1);
	assert_int_equal_cs(iISRNesting, xTraceISRGetCurrentNestingReturned());
	assert_int_equal_cs(xTraceISRGetCurrent(&xISRHandle), TRC_FAIL); /* stack index is -1 */
	
	assert_int_equal_cs_create1(xTraceISRBegin((TraceISRHandle_t)1), TRC_SUCCESS, PSF_EVENT_ISR_BEGIN, 1);

	assert_int_equal_cs(xTraceISRGetCurrentNesting(&iISRNesting), TRC_SUCCESS);
	assert_int_equal_cs(iISRNesting, 0);
	assert_int_equal_cs(iISRNesting, xTraceISRGetCurrentNestingReturned());
	assert_int_equal_cs(xTraceISRGetCurrent(&xISRHandle), TRC_SUCCESS);
	assert_int_equal_cs((TraceUnsignedBaseType_t)xISRHandle, (TraceUnsignedBaseType_t)1);

	assert_int_equal_cs_create1(xTraceISRBegin((TraceISRHandle_t)2), TRC_SUCCESS, PSF_EVENT_ISR_BEGIN, 2);

	assert_int_equal_cs(xTraceISRGetCurrentNesting(&iISRNesting), TRC_SUCCESS);
	assert_int_equal_cs(iISRNesting, 1);
	assert_int_equal_cs(iISRNesting, xTraceISRGetCurrentNestingReturned());
	assert_int_equal_cs(xTraceISRGetCurrent(&xISRHandle), TRC_SUCCESS);
	assert_int_equal_cs((TraceUnsignedBaseType_t)xISRHandle, (TraceUnsignedBaseType_t)2);

	assert_int_equal_cs_create1(xTraceISRBegin((TraceISRHandle_t)3), TRC_SUCCESS, PSF_EVENT_ISR_BEGIN, 3);

	assert_int_equal_cs(xTraceISRGetCurrentNesting(&iISRNesting), TRC_SUCCESS);
	assert_int_equal_cs(iISRNesting, 2);
	assert_int_equal_cs(iISRNesting, xTraceISRGetCurrentNestingReturned());
	assert_int_equal_cs(xTraceISRGetCurrent(&xISRHandle), TRC_SUCCESS);
	assert_int_equal_cs((TraceUnsignedBaseType_t)xISRHandle, (TraceUnsignedBaseType_t)3);

	assert_int_equal_cs_create1(xTraceISREnd(0), TRC_SUCCESS, PSF_EVENT_ISR_RESUME, 2);

	assert_int_equal_cs(xTraceISRGetCurrentNesting(&iISRNesting), TRC_SUCCESS);
	assert_int_equal_cs(iISRNesting, 1);
	assert_int_equal_cs(iISRNesting, xTraceISRGetCurrentNestingReturned());
	assert_int_equal_cs(xTraceISRGetCurrent(&xISRHandle), TRC_SUCCESS);
	assert_int_equal_cs((TraceUnsignedBaseType_t)xISRHandle, (TraceUnsignedBaseType_t)2);

	assert_int_equal_cs_create1(xTraceISRBegin((TraceISRHandle_t)3), TRC_SUCCESS, PSF_EVENT_ISR_BEGIN, 3);

	assert_int_equal_cs(xTraceISRGetCurrentNesting(&iISRNesting), TRC_SUCCESS);
	assert_int_equal_cs(iISRNesting, 2);
	assert_int_equal_cs(iISRNesting, xTraceISRGetCurrentNestingReturned());
	assert_int_equal_cs(xTraceISRGetCurrent(&xISRHandle), TRC_SUCCESS);
	assert_int_equal_cs((TraceUnsignedBaseType_t)xISRHandle, (TraceUnsignedBaseType_t)3);

	assert_int_equal_cs_create1(xTraceISREnd(0), TRC_SUCCESS, PSF_EVENT_ISR_RESUME, 2);

	assert_int_equal_cs(xTraceISRGetCurrentNesting(&iISRNesting), TRC_SUCCESS);
	assert_int_equal_cs(iISRNesting, 1);
	assert_int_equal_cs(iISRNesting, xTraceISRGetCurrentNestingReturned());
	assert_int_equal_cs(xTraceISRGetCurrent(&xISRHandle), TRC_SUCCESS);
	assert_int_equal_cs((TraceUnsignedBaseType_t)xISRHandle, (TraceUnsignedBaseType_t)2);

	assert_int_equal_cs_create1(xTraceISREnd(0), TRC_SUCCESS, PSF_EVENT_ISR_RESUME, 1);

	assert_int_equal_cs(xTraceISRGetCurrentNesting(&iISRNesting), TRC_SUCCESS);
	assert_int_equal_cs(iISRNesting, 0);
	assert_int_equal_cs(iISRNesting, xTraceISRGetCurrentNestingReturned());
	assert_int_equal_cs(xTraceISRGetCurrent(&xISRHandle), TRC_SUCCESS);
	assert_int_equal_cs((TraceUnsignedBaseType_t)xISRHandle, (TraceUnsignedBaseType_t)1);

	MOCK_TASK_pvCurrentTask = (void*)12343214;
	assert_int_equal_cs_create1(xTraceISREnd(0), TRC_SUCCESS, PSF_EVENT_TASK_ACTIVATE, 12343214);

	assert_int_equal_cs(xTraceISRGetCurrentNesting(&iISRNesting), TRC_SUCCESS);
	assert_int_equal_cs(iISRNesting, -1);
	assert_int_equal_cs(iISRNesting, xTraceISRGetCurrentNestingReturned());
	assert_int_equal_cs(xTraceISRGetCurrent(&xISRHandle), TRC_FAIL);
}

void test_xTraceISRBegin(void** ppState)
{
	ISRTestState_t* pxState = (ISRTestState_t*)*ppState;
	TraceISRHandle_t xISRHandle = 0;

	assert_int_equal_cs_create1(xTraceISRBegin((TraceISRHandle_t)5), TRC_SUCCESS, PSF_EVENT_ISR_BEGIN, 5);
	assert_int_equal_cs(xTraceISRGetCurrent(&xISRHandle), TRC_SUCCESS);
	assert_int_equal_cs((TraceUnsignedBaseType_t)xISRHandle, (TraceUnsignedBaseType_t)5);

	assert_int_equal_cs_create1(xTraceISRBegin((TraceISRHandle_t)4), TRC_SUCCESS, PSF_EVENT_ISR_BEGIN, 4);
	assert_int_equal_cs(xTraceISRGetCurrent(&xISRHandle), TRC_SUCCESS);
	assert_int_equal_cs((TraceUnsignedBaseType_t)xISRHandle, (TraceUnsignedBaseType_t)4);

	assert_int_equal_cs_create1(xTraceISRBegin((TraceISRHandle_t)2), TRC_SUCCESS, PSF_EVENT_ISR_BEGIN, 2);
	assert_int_equal_cs(xTraceISRGetCurrent(&xISRHandle), TRC_SUCCESS);
	assert_int_equal_cs((TraceUnsignedBaseType_t)xISRHandle, (TraceUnsignedBaseType_t)2);

	assert_int_equal_cs_create1(xTraceISRBegin((TraceISRHandle_t)3), TRC_SUCCESS, PSF_EVENT_ISR_BEGIN, 3);
	assert_int_equal_cs(xTraceISRGetCurrent(&xISRHandle), TRC_SUCCESS);
	assert_int_equal_cs((TraceUnsignedBaseType_t)xISRHandle, (TraceUnsignedBaseType_t)3);

	assert_int_equal_cs_create1(xTraceISRBegin((TraceISRHandle_t)1), TRC_SUCCESS, PSF_EVENT_ISR_BEGIN, 1);
	assert_int_equal_cs(xTraceISRGetCurrent(&xISRHandle), TRC_SUCCESS);
	assert_int_equal_cs((TraceUnsignedBaseType_t)xISRHandle, (TraceUnsignedBaseType_t)1);

	assert_int_equal_cs_create1(xTraceISRBegin((TraceISRHandle_t)9), TRC_SUCCESS, PSF_EVENT_ISR_BEGIN, 9);
	assert_int_equal_cs(xTraceISRGetCurrent(&xISRHandle), TRC_SUCCESS);
	assert_int_equal_cs((TraceUnsignedBaseType_t)xISRHandle, (TraceUnsignedBaseType_t)9);
}

void test_xTraceISREnd(void** ppState)
{
	ISRTestState_t* pxState = (ISRTestState_t*)*ppState;
	TraceISRHandle_t xISRHandle = 0;

	assert_int_equal_cs(xTraceISRBegin((TraceISRHandle_t)5), TRC_SUCCESS);
	assert_int_equal_cs(xTraceISRBegin((TraceISRHandle_t)4), TRC_SUCCESS);
	assert_int_equal_cs(xTraceISRBegin((TraceISRHandle_t)2), TRC_SUCCESS);
	assert_int_equal_cs(xTraceISRBegin((TraceISRHandle_t)3), TRC_SUCCESS);
	assert_int_equal_cs(xTraceISRBegin((TraceISRHandle_t)1), TRC_SUCCESS);
	assert_int_equal_cs(xTraceISRBegin((TraceISRHandle_t)9), TRC_SUCCESS);
	
	assert_int_equal_cs(xTraceISRGetCurrent(&xISRHandle), TRC_SUCCESS);
	assert_int_equal_cs((TraceUnsignedBaseType_t)xISRHandle, (TraceUnsignedBaseType_t)9);

	assert_int_equal_cs_create1(xTraceISREnd(0), TRC_SUCCESS, PSF_EVENT_ISR_RESUME, 1);
	assert_int_equal_cs(xTraceISRGetCurrent(&xISRHandle), TRC_SUCCESS);
	assert_int_equal_cs((TraceUnsignedBaseType_t)xISRHandle, (TraceUnsignedBaseType_t)1);

	assert_int_equal_cs_create1(xTraceISREnd(0), TRC_SUCCESS, PSF_EVENT_ISR_RESUME, 3);
	assert_int_equal_cs(xTraceISRGetCurrent(&xISRHandle), TRC_SUCCESS);
	assert_int_equal_cs((TraceUnsignedBaseType_t)xISRHandle, (TraceUnsignedBaseType_t)3);

	assert_int_equal_cs_create1(xTraceISREnd(0), TRC_SUCCESS, PSF_EVENT_ISR_RESUME, 2);
	assert_int_equal_cs(xTraceISRGetCurrent(&xISRHandle), TRC_SUCCESS);
	assert_int_equal_cs((TraceUnsignedBaseType_t)xISRHandle, (TraceUnsignedBaseType_t)2);

	assert_int_equal_cs_create1(xTraceISREnd(0), TRC_SUCCESS, PSF_EVENT_ISR_RESUME, 4);
	assert_int_equal_cs(xTraceISRGetCurrent(&xISRHandle), TRC_SUCCESS);
	assert_int_equal_cs((TraceUnsignedBaseType_t)xISRHandle, (TraceUnsignedBaseType_t)4);

	assert_int_equal_cs_create1(xTraceISREnd(0), TRC_SUCCESS, PSF_EVENT_ISR_RESUME, 5);
	assert_int_equal_cs(xTraceISRGetCurrent(&xISRHandle), TRC_SUCCESS);
	assert_int_equal_cs((TraceUnsignedBaseType_t)xISRHandle, (TraceUnsignedBaseType_t)5);

	MOCK_TASK_pvCurrentTask = (void*)12343214;
	assert_int_equal_cs_create1(xTraceISREnd(0), TRC_SUCCESS, PSF_EVENT_TASK_ACTIVATE, 12343214);
	assert_int_equal_cs(xTraceISRGetCurrent(&xISRHandle), TRC_FAIL);
}

void TRACE_ASSERTS_DISABLED(void** ppState)
{
}

uint32_t runISRTests(void)
{
	uint32_t uiCountFailedTests = 0;

	const struct CMUnitTest aTests_trcISR[] =
	{
#if (TRC_CFG_USE_TRACE_ASSERT == 0)
		cmocka_unit_test(TRACE_ASSERTS_DISABLED),
#endif
		cmocka_unit_test(test_xTraceISRInitialize),
		cmocka_unit_test_setup_teardown(test_xTraceISRRegister, setup_ISR, teardown_ISR),
		cmocka_unit_test_setup_teardown(test_xTraceISRGetCurrentAndCurrentNesting, setup_ISR, teardown_ISR),
		cmocka_unit_test_setup_teardown(test_xTraceISRBegin, setup_ISR, teardown_ISR),
		cmocka_unit_test_setup_teardown(test_xTraceISREnd, setup_ISR, teardown_ISR),
#if (TRC_CFG_USE_TRACE_ASSERT == 0)
		cmocka_unit_test(TRACE_ASSERTS_DISABLED),
#endif
	};

	uiCountFailedTests += cmocka_run_group_tests(aTests_trcISR, group_setup_ISR, group_teardown_ISR);
	
	return uiCountFailedTests;
}
