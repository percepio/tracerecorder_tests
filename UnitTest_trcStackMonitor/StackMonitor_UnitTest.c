#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <StackMonitor_UnitTest.h>
#include <trcRecorder.h>

int group_setup_StackMonitor(void** ppState);
int group_teardown_StackMonitor(void** ppState);
int setup_StackMonitor(void** ppState);
int teardown_StackMonitor(void** ppState);

void test_xTraceStackMonitorInitialize(void** ppState);
void test_xTraceStackMonitorDiagnosticsGetSet(void** ppState);
void test_xTraceStackMonitorAdd(void** ppState);
void test_xTraceStackMonitorRemove(void** ppState);
void test_xTraceStackMonitorGetAtIndex(void** ppState);
void test_xTraceStackMonitorReport(void** ppState);

/******************************************************************************************
*                                        TEST STATE                                       *
******************************************************************************************/

typedef struct StackMonitorTestState
{
	/* Pointer to Buffer */
	TraceStackMonitorData_t xStackMonitorBuffer;
	TraceDiagnosticsData_t xDiagnosticsBuffer;
} StackMonitorTestState_t;

StackMonitorTestState_t xStackMonitorTestState;

volatile uint32_t MOCK_RecorderEnabled = 1;
volatile uint32_t MOCK_RecorderInitialized = 1;
uint32_t MOCK_uiUnusedStack = 0xFFFF;
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

int group_setup_StackMonitor(void **ppState)
{
	*ppState = &xStackMonitorTestState;

	assert_int_equal(sizeof(TraceUnsignedBaseType_t), sizeof(void*));
	assert_int_equal(sizeof(TraceUnsignedBaseType_t), sizeof(TraceBaseType_t));

	return 0;
}

int group_teardown_StackMonitor(void **ppState)
{
	return 0;
}

int setup_StackMonitor(void **ppState)
{
	StackMonitorTestState_t* pxState = (StackMonitorTestState_t*)*ppState;
	
	assert_int_equal_cs(xTraceStackMonitorInitialize(&pxState->xStackMonitorBuffer), TRC_SUCCESS);
	assert_int_equal_cs(xTraceDiagnosticsInitialize(&pxState->xDiagnosticsBuffer), TRC_SUCCESS);

	return 0;
}

int teardown_StackMonitor(void **ppState)
{
	StackMonitorTestState_t* pxState = (StackMonitorTestState_t*)*ppState;
	
	return 0;
}

/******************************************************************************************
*                                        TESTS                                            *
******************************************************************************************/

void test_xTraceStackMonitorInitialize(void **ppState)
{
	StackMonitorTestState_t* pxState = (StackMonitorTestState_t*)*ppState;

	assert_int_equal_cs(xTraceStackMonitorInitialize(0), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceStackMonitorInitialize(&pxState->xStackMonitorBuffer), TRC_SUCCESS);
}

void test_xTraceStackMonitorAdd(void **ppState)
{
	StackMonitorTestState_t* pxState = (StackMonitorTestState_t*)*ppState;

	assert_int_equal_cs(xTraceStackMonitorAdd(0), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceStackMonitorAdd(0), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceStackMonitorAdd((void*)0x12345678), TRC_SUCCESS);

	assert_int_equal_cs(xTraceStackMonitorAdd((void*)0x12345678), TRC_SUCCESS);

	assert_int_equal_cs(xTraceStackMonitorAdd((void*)0x12345678), TRC_SUCCESS);

	assert_int_equal_cs(xTraceStackMonitorAdd((void*)0x12345678), TRC_SUCCESS);

	assert_int_equal_cs(xTraceStackMonitorAdd((void*)0x12345678), TRC_FAIL); /* full */
}

void test_xTraceStackMonitorRemove(void **ppState)
{
	StackMonitorTestState_t* pxState = (StackMonitorTestState_t*)*ppState;
	
	assert_int_equal_cs(xTraceStackMonitorRemove(0), TRC_FAIL); /* null test */
	
	assert_int_equal_cs(xTraceStackMonitorRemove((void*)0x12345678), TRC_FAIL); /* not added yet */

	assert_int_equal_cs(xTraceStackMonitorAdd((void*)0x12345678), TRC_SUCCESS);
	assert_int_equal_cs(xTraceStackMonitorRemove((void*)0x12345678), TRC_SUCCESS);

	assert_int_equal_cs(xTraceStackMonitorRemove((void*)0x12345678), TRC_FAIL); /* already gone */


	assert_int_equal_cs(xTraceStackMonitorAdd((void*)0x12345678), TRC_SUCCESS);
	assert_int_equal_cs(xTraceStackMonitorAdd((void*)0x12345679), TRC_SUCCESS);
	assert_int_equal_cs(xTraceStackMonitorAdd((void*)0x1234567A), TRC_SUCCESS);
	assert_int_equal_cs(xTraceStackMonitorAdd((void*)0x1234567B), TRC_SUCCESS);

	assert_int_equal_cs(xTraceStackMonitorRemove((void*)0x1234567A), TRC_SUCCESS);
	assert_int_equal_cs(xTraceStackMonitorRemove((void*)0x1234567B), TRC_SUCCESS);
	assert_int_equal_cs(xTraceStackMonitorRemove((void*)0x12345678), TRC_SUCCESS);
	assert_int_equal_cs(xTraceStackMonitorRemove((void*)0x12345679), TRC_SUCCESS);

	assert_int_equal_cs(xTraceStackMonitorRemove((void*)0x1234567A), TRC_FAIL); /* already gone */
	assert_int_equal_cs(xTraceStackMonitorRemove((void*)0x1234567B), TRC_FAIL); /* already gone */
	assert_int_equal_cs(xTraceStackMonitorRemove((void*)0x12345678), TRC_FAIL); /* already gone */
	assert_int_equal_cs(xTraceStackMonitorRemove((void*)0x12345679), TRC_FAIL); /* already gone */
}

void test_xTraceStackMonitorGetAtIndex(void **ppState)
{
	StackMonitorTestState_t* pxState = (StackMonitorTestState_t*)*ppState;
	void *pvAddress;
	TraceUnsignedBaseType_t uxLowWaterMark;
	uint32_t i;
	
	assert_int_equal_cs(xTraceStackMonitorGetAtIndex(0, 0, &uxLowWaterMark), TRC_FAIL); /* null test */
	assert_int_equal_cs(xTraceStackMonitorGetAtIndex(0, &pvAddress, 0), TRC_FAIL); /* null test */
	assert_int_equal_cs(xTraceStackMonitorGetAtIndex(9999, &pvAddress, &uxLowWaterMark), TRC_FAIL); /* index test */

	for (i = 0; i < TRC_CFG_STACK_MONITOR_MAX_TASKS; i++)
	{
		assert_int_equal_cs(xTraceStackMonitorAdd((void*)(TraceUnsignedBaseType_t)(0x12345678 + i)), TRC_SUCCESS);
	}

	for (i = 0; i < TRC_CFG_STACK_MONITOR_MAX_TASKS; i++)
	{
		assert_int_equal_cs(xTraceStackMonitorGetAtIndex(i, &pvAddress, &uxLowWaterMark), TRC_SUCCESS);
		assert_int_equal_cs((TraceUnsignedBaseType_t)pvAddress, (TraceUnsignedBaseType_t)(0x12345678 + i));
		assert_int_equal_cs(uxLowWaterMark, 0xFFFF);
	}
}

void test_xTraceStackMonitorReport(void **ppState)
{
	StackMonitorTestState_t* pxState = (StackMonitorTestState_t*)*ppState;

	assert_int_equal_cs(xTraceStackMonitorReport(), TRC_SUCCESS); /* not a real test */
}

void test_xTraceStackMonitorDiagnosticsGetSet(void** ppState)
{
	StackMonitorTestState_t* pxState = (StackMonitorTestState_t*)*ppState;
	TraceBaseType_t uxValue = 0;

	assert_int_equal_cs(xTraceDiagnosticsSet(TRC_DIAGNOSTICS_STACK_MONITOR_NO_SLOTS, 0x12345678), TRC_SUCCESS);
	assert_int_equal_cs(xTraceDiagnosticsGet(TRC_DIAGNOSTICS_STACK_MONITOR_NO_SLOTS, &uxValue), TRC_SUCCESS);
	assert_int_equal_cs(uxValue, 0x12345678);

	assert_int_equal_cs(xTraceDiagnosticsSet(TRC_DIAGNOSTICS_STACK_MONITOR_NO_SLOTS, (TraceBaseType_t)0x77654321), TRC_SUCCESS);
	assert_int_equal_cs(xTraceDiagnosticsGet(TRC_DIAGNOSTICS_STACK_MONITOR_NO_SLOTS, &uxValue), TRC_SUCCESS);
	assert_int_equal_cs(uxValue, (TraceBaseType_t)0x77654321);

	assert_int_equal_cs(xTraceDiagnosticsSet(TRC_DIAGNOSTICS_STACK_MONITOR_NO_SLOTS, 0x12344321), TRC_SUCCESS);
	assert_int_equal_cs(xTraceDiagnosticsGet(TRC_DIAGNOSTICS_STACK_MONITOR_NO_SLOTS, &uxValue), TRC_SUCCESS);
	assert_int_equal_cs(uxValue, 0x12344321);
}

uint32_t runStackMonitorTests(void)
{
	uint32_t uiCountFailedTests = 0;

	const struct CMUnitTest aTests_trcStackMonitor[] =
	{
		cmocka_unit_test_setup_teardown(test_xTraceStackMonitorInitialize, setup_StackMonitor, teardown_StackMonitor),
		cmocka_unit_test_setup_teardown(test_xTraceStackMonitorAdd, setup_StackMonitor, teardown_StackMonitor),
		cmocka_unit_test_setup_teardown(test_xTraceStackMonitorRemove, setup_StackMonitor, teardown_StackMonitor),
		cmocka_unit_test_setup_teardown(test_xTraceStackMonitorGetAtIndex, setup_StackMonitor, teardown_StackMonitor),
		cmocka_unit_test_setup_teardown(test_xTraceStackMonitorReport, setup_StackMonitor, teardown_StackMonitor),
		cmocka_unit_test_setup_teardown(test_xTraceStackMonitorDiagnosticsGetSet, setup_StackMonitor, teardown_StackMonitor),
	};

	uiCountFailedTests += cmocka_run_group_tests(aTests_trcStackMonitor, group_setup_StackMonitor, group_teardown_StackMonitor);
	
	return uiCountFailedTests;
}
