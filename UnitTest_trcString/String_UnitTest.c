#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <String_UnitTest.h>
#include <trcRecorder.h>

static TraceUnsignedBaseType_t prvIs64bit(void);

static TraceUnsignedBaseType_t prvIs64bit(void)
{
	return sizeof(TraceUnsignedBaseType_t) == 8;
}

int group_setup_Runnable(void** ppState);
int group_teardown_Runnable(void** ppState);
int setup_Runnable(void** ppState);
int teardown_Runnable(void** ppState);

void test_xTraceStringRegister(void** ppState);

/******************************************************************************************
*                                        TEST STATE                                       *
******************************************************************************************/

typedef struct StringTestState
{
	/* Test Objects */
	TraceEntryTable_t xEntryTableBuffer;
	TraceEntryIndexTable_t xEntryIndexTableBuffer;
	TraceDiagnosticsData_t xDiagnosticsBuffer;
} StringTestState_t;

StringTestState_t xStringTestState;
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

int group_setup_Runnable(void **ppState)
{
	*ppState = &xStringTestState;

	assert_int_equal(sizeof(TraceUnsignedBaseType_t), sizeof(void*));
	assert_int_equal(sizeof(TraceUnsignedBaseType_t), sizeof(TraceBaseType_t));

	return 0;
}

int group_teardown_Runnable(void **ppState)
{
	*ppState = 0;
	
	return 0;
}

int setup_Runnable(void **ppState)
{
	StringTestState_t* pxState = (StringTestState_t*)*ppState;

	assert_int_equal_cs(xTraceEntryTableInitialize(&pxState->xEntryTableBuffer), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEntryIndexTableInitialize(&pxState->xEntryIndexTableBuffer), TRC_SUCCESS);
	assert_int_equal_cs(xTraceDiagnosticsInitialize(&pxState->xDiagnosticsBuffer), TRC_SUCCESS);

	return 0;
}

int teardown_Runnable(void **ppState)
{
	StringTestState_t* pxState = (StringTestState_t*)*ppState;

	return 0;
}

/******************************************************************************************
*                                        TESTS                                            *
******************************************************************************************/

void test_xTraceStringRegister(void** ppState)
{
	traceString str;
	TraceEntryHandle_t xEntryHandle;
	const char* szStr;
	
	assert_int_equal_cs(xTraceStringRegister(0, &str), TRC_FAIL); /* null test */
	assert_int_equal_cs(xTraceStringRegister("test", 0), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceStringRegister("test", &str), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryFind((void*)str, &xEntryHandle), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetSymbol(xEntryHandle, &szStr), TRC_SUCCESS);

	assert_int_equal_cs(TRC_CFG_ENTRY_SLOTS, 1);
	assert_int_equal_cs(xTraceStringRegister("test", &str), TRC_SUCCESS);	/* Even though we only want 1 slot, we have to align the slot count due to entry index table */
	assert_int_equal_cs(xTraceStringRegister("test", &str), TRC_SUCCESS);
	assert_int_equal_cs(xTraceStringRegister("test", &str), TRC_SUCCESS);
	assert_int_equal_cs(xTraceStringRegister("test", &str), TRC_SUCCESS);
	assert_int_equal_cs(xTraceStringRegister("test", &str), TRC_SUCCESS);
	assert_int_equal_cs(xTraceStringRegister("test", &str), TRC_SUCCESS);
	assert_int_equal_cs(xTraceStringRegister("test", &str), TRC_SUCCESS);
	
	assert_int_equal_cs(xTraceStringRegister("will_not_fit", &str), TRC_FAIL); /* Entry Table full */

	assert_string_equal(szStr, "test");
}

uint32_t runStringTests(void)
{
	uint32_t uiCountFailedTests = 0;

	const struct CMUnitTest aTests_trcString[] =
	{
		cmocka_unit_test_setup_teardown(test_xTraceStringRegister, setup_Runnable, teardown_Runnable),
	};

	uiCountFailedTests += cmocka_run_group_tests(aTests_trcString, group_setup_Runnable, group_teardown_Runnable);

	return uiCountFailedTests;
}
