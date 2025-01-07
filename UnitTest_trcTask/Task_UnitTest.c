#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <Task_UnitTest.h>
#include <trcRecorder.h>

/******************************************************************************************
*                                        SETUP / TEARDOWN                                 *
******************************************************************************************/

int group_teardown_Task(void** ppState);

int group_setup_Task(void** ppState);

int setup_Task(void** ppState);

int teardown_Task(void** ppState);

/******************************************************************************************
*                                        TESTS                                            *
******************************************************************************************/

void test_xTraceTaskInitialize(void** ppState);
void test_xTraceTaskRegister(void** ppState);
void test_xTraceTaskRegisterWithoutHandle(void** ppState);
void test_xTraceTaskUnregister(void** ppState);
void test_xTraceTaskUnregisterWithoutHandle(void** ppState);
void test_xTraceTaskSetName(void** ppState);
void test_xTraceTaskSetNameWithoutHandle(void** ppState);
void test_xTraceTaskSetPriority(void** ppState);
void test_xTraceTaskSetPriorityWithoutHandle(void** ppState);
void test_xTraceTaskSwitch(void** ppState);
void test_xTraceTaskReady(void** ppState);
void test_xTraceTaskSetGetCurrent(void** ppState);

/******************************************************************************************
*                                        TEST STATE                                       *
******************************************************************************************/

typedef struct TaskTestState
{
	/* Pointer to Buffer */
	TraceEntryTable_t xEntryTableBuffer;
	TraceEntryIndexTable_t xEntryIndexTableBuffer;
	TraceTaskData_t xTaskInfoBuffer;
	TraceDiagnosticsData_t xDiagnosticsBuffer;
} TaskTestState_t;

TaskTestState_t xTestState;

uint32_t uiTraceSystemState = TRC_STATE_IN_STARTUP;
uint32_t MOCK_uiUnusedStack = 0x1234;
volatile uint32_t MOCK_RecorderEnabled = 1;
volatile uint32_t MOCK_RecorderInitialized = 1;
uint32_t MOCK_uiCurrentCore = 0;
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

int group_setup_Task(void **ppState)
{
	*ppState = &xTestState;

	assert_int_equal(sizeof(TraceUnsignedBaseType_t), sizeof(void*));
	assert_int_equal(sizeof(TraceUnsignedBaseType_t), sizeof(TraceBaseType_t));

	return 0;
}

int group_teardown_Task(void **ppState)
{
	return 0;
}

int setup_Task(void **ppState)
{
	TaskTestState_t* pxState = (TaskTestState_t*)*ppState;

	assert_int_equal_cs(xTraceEntryTableInitialize(&pxState->xEntryTableBuffer), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEntryIndexTableInitialize(&pxState->xEntryIndexTableBuffer), TRC_SUCCESS);
	assert_int_equal_cs(xTraceTaskInitialize(&pxState->xTaskInfoBuffer), TRC_SUCCESS);
	assert_int_equal_cs(xTraceDiagnosticsInitialize(&pxState->xDiagnosticsBuffer), TRC_SUCCESS);

	return 0;
}

int teardown_Task(void **ppState)
{
	TaskTestState_t* pxState = (TaskTestState_t*)*ppState;
	
	return 0;
}

/******************************************************************************************
*                                        TESTS                                            *
******************************************************************************************/

void test_xTraceTaskInitialize(void **ppState)
{
	TaskTestState_t* pxState = (TaskTestState_t*)*ppState;

	assert_int_equal_cs(xTraceTaskInitialize(0), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceTaskInitialize(&pxState->xTaskInfoBuffer), TRC_SUCCESS);
}

void test_xTraceTaskRegister(void **ppState)
{
	TaskTestState_t* pxState = (TaskTestState_t*)*ppState;
	TraceEntryHandle_t xEntryHandle;
	TraceTaskHandle_t xTaskHandle = 0;
	const char* szSymbol;

	assert_int_equal_cs(xTraceTaskRegister(0, "TEST", 10, &xTaskHandle), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceTaskRegister((void*)0x12345678, "TEST", 10, 0), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceTaskRegister((void*)0x12345678, "TEST", 10, &xTaskHandle), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryFind((void*)0x12345678, &xEntryHandle), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetSymbol(xEntryHandle, &szSymbol), TRC_SUCCESS);

	assert_string_equal(szSymbol, "TEST");

	assert_int_equal_cs(xTraceTaskRegister((void*)0x12345679, 0, 10, &xTaskHandle), TRC_SUCCESS); /* null string */
}

void test_xTraceTaskRegisterWithoutHandle(void** ppState)
{
	TaskTestState_t* pxState = (TaskTestState_t*)*ppState;
	TraceEntryHandle_t xEntryHandle;
	const char* szSymbol;

	assert_int_equal_cs(xTraceTaskRegisterWithoutHandle(0, "TEST", 10), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceTaskRegisterWithoutHandle((void*)0x12345678, "TEST", 10), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryFind((void*)0x12345678, &xEntryHandle), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetSymbol(xEntryHandle, &szSymbol), TRC_SUCCESS);

	assert_string_equal(szSymbol, "TEST");

	assert_int_equal_cs(xTraceTaskRegisterWithoutHandle((void*)0x12345679, 0, 10), TRC_SUCCESS); /* null string */
}

void test_xTraceTaskUnregister(void **ppState)
{
	TaskTestState_t* pxState = (TaskTestState_t*)*ppState;
	TraceTaskHandle_t xTaskHandle = 0;

	assert_int_equal_cs(xTraceTaskUnregister(0, 10), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceTaskRegister((void*)0x12345678, "TEST", 10, &xTaskHandle), TRC_SUCCESS);

	assert_int_equal_cs(xTraceTaskUnregister(xTaskHandle, 10), TRC_SUCCESS);

	assert_int_equal_cs(xTraceTaskUnregister(xTaskHandle, 10), TRC_FAIL); /* No object at that address any longer */
}

void test_xTraceTaskUnregisterWithoutHandle(void** ppState)
{
	TaskTestState_t* pxState = (TaskTestState_t*)*ppState;

	assert_int_equal_cs(xTraceTaskUnregisterWithoutHandle(0, 10), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceTaskUnregisterWithoutHandle((void*)0x12345678, 10), TRC_FAIL); /* No object at that address yet */

	assert_int_equal_cs(xTraceTaskRegisterWithoutHandle((void*)0x12345678, "TEST", 10), TRC_SUCCESS);

	assert_int_equal_cs(xTraceTaskUnregisterWithoutHandle((void*)0x12345678, 10), TRC_SUCCESS);

	assert_int_equal_cs(xTraceTaskUnregisterWithoutHandle((void*)0x12345678, 10), TRC_FAIL); /* No object at that address any longer */
}

void test_xTraceTaskSetName(void** ppState)
{
	TaskTestState_t* pxState = (TaskTestState_t*)*ppState;
	TraceEntryHandle_t xEntryHandle;
	TraceTaskHandle_t xTaskHandle = 0;
	const char* szSymbol;

	assert_int_equal_cs(xTraceTaskSetName(0, "TEST"), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceTaskRegister((void*)0x12345679, "TEST", 10, &xTaskHandle), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryFind((void*)0x12345679, &xEntryHandle), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetSymbol(xEntryHandle, &szSymbol), TRC_SUCCESS);

	assert_string_equal(szSymbol, "TEST");

	assert_int_equal_cs(xTraceTaskSetName(xTaskHandle, 0), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetSymbol(xEntryHandle, &szSymbol), TRC_SUCCESS);

	assert_string_equal(szSymbol, "");

	assert_int_equal_cs(xTraceTaskSetName(xTaskHandle, ""), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetSymbol(xEntryHandle, &szSymbol), TRC_SUCCESS);

	assert_string_equal(szSymbol, "");

	assert_int_equal_cs(xTraceTaskSetName(xTaskHandle, "TEST"), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetSymbol(xEntryHandle, &szSymbol), TRC_SUCCESS);

	assert_string_equal(szSymbol, "TEST");
}

void test_xTraceTaskSetNameWithoutHandle(void **ppState)
{
	TaskTestState_t* pxState = (TaskTestState_t*)*ppState;
	TraceEntryHandle_t xEntryHandle;
	const char* szSymbol;

	assert_int_equal_cs(xTraceTaskSetNameWithoutHandle(0, "TEST"), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceTaskSetNameWithoutHandle((void*)0x12345678, 0), TRC_SUCCESS); /* This will create a task with that address and name */

	assert_int_equal_cs(xTraceEntryFind((void*)0x12345678, &xEntryHandle), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetSymbol(xEntryHandle, &szSymbol), TRC_SUCCESS);

	assert_string_equal(szSymbol, "");

	assert_int_equal_cs(xTraceTaskSetNameWithoutHandle((void*)0x12345678, 0), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetSymbol(xEntryHandle, &szSymbol), TRC_SUCCESS);

	assert_string_equal(szSymbol, "");

	assert_int_equal_cs(xTraceTaskSetNameWithoutHandle((void*)0x12345678, ""), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetSymbol(xEntryHandle, &szSymbol), TRC_SUCCESS);

	assert_string_equal(szSymbol, "");

	assert_int_equal_cs(xTraceTaskSetNameWithoutHandle((void*)0x12345678, "TEST"), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetSymbol(xEntryHandle, &szSymbol), TRC_SUCCESS);

	assert_string_equal(szSymbol, "TEST");
}

void test_xTraceTaskSetPriority(void **ppState)
{
	TaskTestState_t* pxState = (TaskTestState_t*)*ppState;
	TraceEntryHandle_t xEntryHandle;
	TraceTaskHandle_t xTaskHandle = 0;
	TraceUnsignedBaseType_t uxState;
	
	assert_int_equal_cs(xTraceTaskSetPriority(0, 10), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceTaskRegister((void*)0x12345678, "TEST", 10, &xTaskHandle), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryFind((void*)0x12345678, &xEntryHandle), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetState(xEntryHandle, 0, &uxState), TRC_SUCCESS);

	assert_int_equal_cs(uxState, 10);

	assert_int_equal_cs(xTraceTaskSetPriority(xTaskHandle, 20), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetState(xEntryHandle, 0, &uxState), TRC_SUCCESS);

	assert_int_equal_cs(uxState, 20);
}

void test_xTraceTaskSetPriorityWithoutHandle(void** ppState)
{
	TaskTestState_t* pxState = (TaskTestState_t*)*ppState;
	TraceEntryHandle_t xEntryHandle;
	TraceUnsignedBaseType_t uxState;

	assert_int_equal_cs(xTraceTaskSetPriorityWithoutHandle(0, 10), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceTaskSetPriorityWithoutHandle((void*)0x12345678, 10), TRC_FAIL); /* This will create a task with that address and state */

	assert_int_equal_cs(xTraceTaskRegisterWithoutHandle((void*)0x12345678, "TEST", 10), TRC_SUCCESS);
	
	assert_int_equal_cs(xTraceEntryFind((void*)0x12345678, &xEntryHandle), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetState(xEntryHandle, 0, &uxState), TRC_SUCCESS);

	assert_int_equal_cs(uxState, 10);

	assert_int_equal_cs(xTraceTaskSetPriorityWithoutHandle((void*)0x12345678, 20), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetState(xEntryHandle, 0, &uxState), TRC_SUCCESS);

	assert_int_equal_cs(uxState, 20);
}

void test_xTraceTaskSwitch(void **ppState)
{
	TaskTestState_t* pxState = (TaskTestState_t*)*ppState;

	assert_int_equal_cs(xTraceTaskSwitch(0, 10), TRC_SUCCESS); /* 0 is a valid value, so this will actually work */

	assert_int_equal_cs(xTraceTaskSwitch((void*)0x12345678, 10), TRC_SUCCESS);
}

void test_xTraceTaskReady(void** ppState)
{
	TaskTestState_t* pxState = (TaskTestState_t*)*ppState;

	assert_int_equal_cs(xTraceTaskReady(0), TRC_SUCCESS); /* 0 is a valid value, so this will actually work */

	assert_int_equal_cs(xTraceTaskReady((void*)0x12345678), TRC_SUCCESS);
}

void test_xTraceTaskSetGetCurrent(void** ppState)
{
	TaskTestState_t* pxState = (TaskTestState_t*)*ppState;
	void* pvAddress = 0;
	
	assert_int_equal_cs(xTraceTaskGetCurrent(&pvAddress), TRC_SUCCESS);
	assert_int_equal_cs((TraceUnsignedBaseType_t)xTraceTaskGetCurrentReturn(), 2);
	assert_int_equal_cs((TraceUnsignedBaseType_t)pvAddress, (TraceUnsignedBaseType_t)2); /* 2 is default value */

	assert_int_equal_cs(xTraceTaskSetCurrent((void*)(TraceUnsignedBaseType_t)0x12345678), TRC_SUCCESS);
	assert_int_equal_cs(xTraceTaskGetCurrent(&pvAddress), TRC_SUCCESS);
	assert_int_equal_cs((TraceUnsignedBaseType_t)pvAddress, (TraceUnsignedBaseType_t)0x12345678);
	assert_int_equal_cs((TraceUnsignedBaseType_t)xTraceTaskGetCurrentReturn(), (TraceUnsignedBaseType_t)0x12345678);

	assert_int_equal_cs(xTraceTaskSetCurrent((void*)(TraceUnsignedBaseType_t)0x87654321), TRC_SUCCESS);
	assert_int_equal_cs(xTraceTaskGetCurrent(&pvAddress), TRC_SUCCESS);
	assert_int_equal_cs((TraceUnsignedBaseType_t)pvAddress, (TraceUnsignedBaseType_t)0x87654321);
	assert_int_equal_cs((TraceUnsignedBaseType_t)xTraceTaskGetCurrentReturn(), (TraceUnsignedBaseType_t)0x87654321);
}

uint32_t runTaskTests(void)
{
	uint32_t uiCountFailedTests = 0;

	const struct CMUnitTest aTests_trcTask[] =
	{
		cmocka_unit_test_setup_teardown(test_xTraceTaskRegister, setup_Task, teardown_Task),
		cmocka_unit_test_setup_teardown(test_xTraceTaskRegisterWithoutHandle, setup_Task, teardown_Task),
		cmocka_unit_test_setup_teardown(test_xTraceTaskUnregister, setup_Task, teardown_Task),
		cmocka_unit_test_setup_teardown(test_xTraceTaskUnregisterWithoutHandle, setup_Task, teardown_Task),
		cmocka_unit_test_setup_teardown(test_xTraceTaskSetName, setup_Task, teardown_Task),
		cmocka_unit_test_setup_teardown(test_xTraceTaskSetNameWithoutHandle, setup_Task, teardown_Task),
		cmocka_unit_test_setup_teardown(test_xTraceTaskSetPriority, setup_Task, teardown_Task),
		cmocka_unit_test_setup_teardown(test_xTraceTaskSetPriorityWithoutHandle, setup_Task, teardown_Task),
		cmocka_unit_test_setup_teardown(test_xTraceTaskSwitch, setup_Task, teardown_Task),
		cmocka_unit_test_setup_teardown(test_xTraceTaskReady, setup_Task, teardown_Task),
		cmocka_unit_test_setup_teardown(test_xTraceTaskSetGetCurrent, setup_Task, teardown_Task),
	};

	uiCountFailedTests += cmocka_run_group_tests(aTests_trcTask, group_setup_Task, group_teardown_Task);
	
	return uiCountFailedTests;
}
