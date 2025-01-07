#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <Dependency_UnitTest.h>
#include <trcRecorder.h>

typedef struct DependencyTestState
{
	/* Pointer to Buffer */
	TraceEntryTable_t xEntryTableBuffer;
	TraceEntryIndexTable_t xEntryIndexTableBuffer;
	TraceDiagnosticsData_t xDiagnosticsBuffer;
} DependencyTestState_t;

DependencyTestState_t xDependencyTestState;
volatile uint32_t MOCK_RecorderInitialized = 1;
volatile uint32_t MOCK_RecorderEnabled = 1;
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

int group_setup_dependency(void **ppState)
{
	*ppState = &xDependencyTestState;

	return 0;
}

int group_teardown_dependency(void **ppState)
{
	DependencyTestState_t* pxState = (DependencyTestState_t*)*ppState;

	return 0;
}

int setup_dependency(void** ppState)
{
	DependencyTestState_t* pxState = (DependencyTestState_t*)*ppState;
	
	assert_int_equal_cs(xTraceEntryTableInitialize(&pxState->xEntryTableBuffer), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEntryIndexTableInitialize(&pxState->xEntryIndexTableBuffer), TRC_SUCCESS);
	assert_int_equal_cs(xTraceDiagnosticsInitialize(&pxState->xDiagnosticsBuffer), TRC_SUCCESS);

	return 0;
}

int teardown_dependency(void **ppState)
{
	DependencyTestState_t* pxState = (DependencyTestState_t*)*ppState;

	return 0;
}

/******************************************************************************************
*                                        TESTS                                            *
******************************************************************************************/

void test_xTraceDependencyRegister(void **ppState)
{
	DependencyTestState_t* pxState = (DependencyTestState_t*)*ppState;
	TraceEntryHandle_t xEntryHandle;
	uint32_t uiOptions;
	TraceUnsignedBaseType_t uxState;
	const char* szName = (char*)0;

	assert_int_equal_cs(xTraceDependencyRegister(0, TRC_DEPENDENCY_TYPE_ELF), TRC_FAIL); /* null test */
	assert_int_equal_cs(xTraceDependencyRegister("", TRC_DEPENDENCY_TYPE_ELF), TRC_FAIL); /* empty string test */
	assert_int_equal_cs(xTraceDependencyRegister("build.elf", 0), TRC_FAIL); /* invalid type test */

	assert_int_equal_cs(xTraceDependencyRegister("build.elf", TRC_DEPENDENCY_TYPE_ELF), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetAtIndex(0, &xEntryHandle), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetSymbol(xEntryHandle, &szName), TRC_SUCCESS);
	assert_string_equal(szName, "build.elf");

	assert_int_equal_cs(xTraceEntryGetOptions(xEntryHandle, &uiOptions), TRC_SUCCESS);
	assert_int_equal_cs(uiOptions, TRC_ENTRY_OPTION_DEPENDENCY);

	assert_int_equal_cs(xTraceEntryGetState(xEntryHandle, 0, &uxState), TRC_SUCCESS);
	assert_int_equal_cs(uxState, TRC_DEPENDENCY_TYPE_ELF);

	assert_int_equal_cs(xTraceEntryGetState(xEntryHandle, 1, &uxState), TRC_SUCCESS);
	assert_int_equal_cs(uxState, 0);

	assert_int_equal_cs(xTraceEntryGetState(xEntryHandle, 2, &uxState), TRC_SUCCESS);
	assert_int_equal_cs(uxState, 0);
}

uint32_t runDependencyTests(void)
{
	uint32_t uiCountFailedTests = 0;

	const struct CMUnitTest aTests[] =
	{
		cmocka_unit_test_setup_teardown(test_xTraceDependencyRegister, setup_dependency, teardown_dependency),
	};

	uiCountFailedTests += cmocka_run_group_tests(aTests, group_setup_dependency, group_teardown_dependency);

	return uiCountFailedTests;
}
