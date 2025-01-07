#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <StateMachine_UnitTest.h>
#include <trcRecorder.h>

int group_setup_StateMachine(void** ppState);
int group_teardown_StateMachine(void** ppState);
int setup_emptyStateMachine(void** ppState);
int setup_all(void** ppState);
int setup_twoStatesTwoMachines(void** ppState);
int teardown_stateMachine(void** ppState);

void test_xTraceStateMachineCreate(void** ppState);
void test_xTraceStateMachineStateCreate(void** ppState);
void test_xTraceStateMachineSetState(void** ppState);

/******************************************************************************************
*                                        TEST STATE                                       *
******************************************************************************************/

typedef struct StateMachineTestState
{
	/* Pointer to Buffer */
	TraceEntryTable_t xEntryTableBuffer;
	TraceEntryIndexTable_t xEntryIndexTableBuffer;
	TraceDiagnosticsData_t xDiagnosticsBuffer;

	/* Test Objects */
	TraceStateMachineHandle_t xMachine1Handle, xMachine2Handle;
	TraceStateMachineStateHandle_t xState11Handle, xState12Handle;
	TraceStateMachineStateHandle_t xState21Handle, xState22Handle;

	/* Test Parameters */
	const char *szState11Name, *szState12Name;
	const char* szState21Name, * szState22Name;
	const char *szMachine1Name, *szMachine2Name;
} StateMachineTestState_t;

StateMachineTestState_t xStateMachineTestState;
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

int group_setup_StateMachine(void **ppState)
{
	xStateMachineTestState.xMachine1Handle = 0;
	xStateMachineTestState.szMachine1Name = "TEST_STATE_MACHINE_1";
	xStateMachineTestState.xMachine2Handle = 0;
	xStateMachineTestState.szMachine2Name = "TEST_STATE_MACHINE_2";
	xStateMachineTestState.xState11Handle = 0;
	xStateMachineTestState.szState11Name = "TEST_STATE_MACHINE_1_STATE_1";
	xStateMachineTestState.xState12Handle = 0;
	xStateMachineTestState.szState12Name = "TEST_STATE_MACHINE_1_STATE_2";
	xStateMachineTestState.xState21Handle = 0;
	xStateMachineTestState.szState21Name = "TEST_STATE_MACHINE_2_STATE_1";
	xStateMachineTestState.xState22Handle = 0;
	xStateMachineTestState.szState22Name = "TEST_STATE_MACHINE_2_STATE_2";
	*ppState = &xStateMachineTestState;

	assert_int_equal(sizeof(TraceUnsignedBaseType_t), sizeof(void*));
	assert_int_equal(sizeof(TraceUnsignedBaseType_t), sizeof(TraceBaseType_t));

	return 0;
}

int group_teardown_StateMachine(void **ppState)
{
	StateMachineTestState_t* pxState = (StateMachineTestState_t*)*ppState;

	pxState->xMachine1Handle = 0;
	pxState->szMachine1Name = "";
	pxState->xMachine2Handle = 0;
	pxState->szMachine2Name = "";
	pxState->xState11Handle = 0;
	pxState->szState11Name = "";
	pxState->xState12Handle = 0;
	pxState->szState12Name = "";
	pxState->xState21Handle = 0;
	pxState->szState21Name = "";
	pxState->xState22Handle = 0;
	pxState->szState22Name = "";

	return 0;
}

int setup_emptyStateMachine(void **ppState)
{
	StateMachineTestState_t* pxState = (StateMachineTestState_t*)*ppState;

	assert_int_equal_cs(xTraceEntryTableInitialize(&pxState->xEntryTableBuffer), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEntryIndexTableInitialize(&pxState->xEntryIndexTableBuffer), TRC_SUCCESS);
	assert_int_equal_cs(xTraceDiagnosticsInitialize(&pxState->xDiagnosticsBuffer), TRC_SUCCESS);

	return 0;
}

int setup_twoStatesTwoMachines(void **ppState)
{
	StateMachineTestState_t* pxState = (StateMachineTestState_t*)*ppState;

	assert_int_equal_cs(setup_emptyStateMachine(ppState), 0);

	assert_int_equal_cs(xTraceStateMachineCreate(pxState->szMachine1Name, &pxState->xMachine1Handle), TRC_SUCCESS);
	assert_int_equal_cs(xTraceStateMachineCreate(pxState->szMachine2Name, &pxState->xMachine2Handle), TRC_SUCCESS);

	return 0;
}

int setup_all(void** ppState)
{
	StateMachineTestState_t* pxState = (StateMachineTestState_t*)*ppState;

	assert_int_equal_cs(setup_twoStatesTwoMachines(ppState), 0);

	assert_int_equal_cs(xTraceStateMachineStateCreate(pxState->xMachine1Handle, pxState->szState11Name, &pxState->xState11Handle), TRC_SUCCESS);
	assert_int_equal_cs(xTraceStateMachineStateCreate(pxState->xMachine1Handle, pxState->szState12Name, &pxState->xState12Handle), TRC_SUCCESS);
	assert_int_equal_cs(xTraceStateMachineStateCreate(pxState->xMachine2Handle, pxState->szState21Name, &pxState->xState21Handle), TRC_SUCCESS);
	assert_int_equal_cs(xTraceStateMachineStateCreate(pxState->xMachine2Handle, pxState->szState22Name, &pxState->xState22Handle), TRC_SUCCESS);

	return 0;
}

int teardown_stateMachine(void **ppState)
{
	StateMachineTestState_t* pxState = (StateMachineTestState_t*)*ppState;

	pxState->xMachine1Handle = 0;
	pxState->xMachine2Handle = 0;
	pxState->xState11Handle = 0;
	pxState->xState12Handle = 0;
	pxState->xState21Handle = 0;
	pxState->xState22Handle = 0;

	return 0;
}

/******************************************************************************************
*                                        TESTS                                            *
******************************************************************************************/

void test_xTraceStateMachineCreate(void** ppState)
{
	StateMachineTestState_t* pxState = (StateMachineTestState_t*)*ppState;

	assert_int_equal_cs(xTraceStateMachineCreate(0, &pxState->xMachine1Handle), TRC_SUCCESS); /* null test */
	assert_int_equal_cs(xTraceStateMachineCreate(pxState->szMachine1Name, 0), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceStateMachineCreate(pxState->szMachine1Name, &pxState->xMachine1Handle), TRC_SUCCESS);
	assert_int_equal_cs(xTraceStateMachineCreate(pxState->szMachine2Name, &pxState->xMachine2Handle), TRC_SUCCESS);

	assert_int_not_equal((TraceUnsignedBaseType_t)pxState->xMachine1Handle, (TraceUnsignedBaseType_t)pxState->xMachine2Handle);
}

void test_xTraceStateMachineStateCreate(void **ppState)
{
	StateMachineTestState_t* pxState = (StateMachineTestState_t*)*ppState;

	assert_int_equal_cs(xTraceStateMachineStateCreate(0, pxState->szState11Name, &pxState->xState11Handle), TRC_FAIL); /* null test */
	assert_int_equal_cs(xTraceStateMachineStateCreate(pxState->xMachine1Handle, 0, &pxState->xState11Handle), TRC_SUCCESS); /* null test */
	assert_int_equal_cs(xTraceStateMachineStateCreate(pxState->xMachine1Handle, pxState->szState11Name, 0), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceStateMachineStateCreate(pxState->xMachine1Handle, pxState->szState11Name, &pxState->xState11Handle), TRC_SUCCESS);
	assert_int_equal_cs(xTraceStateMachineStateCreate(pxState->xMachine1Handle, pxState->szState12Name, &pxState->xState12Handle), TRC_SUCCESS);
	assert_int_not_equal((TraceUnsignedBaseType_t)pxState->xState11Handle, (TraceUnsignedBaseType_t)pxState->xState12Handle);

	assert_int_equal_cs(xTraceStateMachineStateCreate(pxState->xMachine2Handle, pxState->szState21Name, &pxState->xState21Handle), TRC_SUCCESS);
	assert_int_equal_cs(xTraceStateMachineStateCreate(pxState->xMachine2Handle, pxState->szState22Name, &pxState->xState22Handle), TRC_SUCCESS);
	assert_int_not_equal((TraceUnsignedBaseType_t)pxState->xState21Handle, (TraceUnsignedBaseType_t)pxState->xState22Handle);
}

void test_xTraceStateMachineSetState(void **ppState)
{
	StateMachineTestState_t* pxState = (StateMachineTestState_t*)*ppState;

	assert_int_equal_cs(xTraceStateMachineSetState(0, pxState->xState11Handle), TRC_FAIL);
	assert_int_equal_cs(xTraceStateMachineSetState(pxState->xMachine1Handle, 0), TRC_FAIL);

	assert_int_equal_cs(xTraceStateMachineSetState(pxState->xMachine1Handle, pxState->xState11Handle), TRC_SUCCESS);
	assert_int_equal_cs(xTraceStateMachineSetState(pxState->xMachine1Handle, pxState->xState12Handle), TRC_SUCCESS);
	assert_int_equal_cs(xTraceStateMachineSetState(pxState->xMachine1Handle, pxState->xState21Handle), TRC_FAIL); /* State was created for state machine 2 */
	assert_int_equal_cs(xTraceStateMachineSetState(pxState->xMachine1Handle, pxState->xState22Handle), TRC_FAIL); /* State was created for state machine 2 */

	assert_int_equal_cs(xTraceStateMachineSetState(pxState->xMachine2Handle, pxState->xState11Handle), TRC_FAIL); /* State was created for state machine 1 */
	assert_int_equal_cs(xTraceStateMachineSetState(pxState->xMachine2Handle, pxState->xState12Handle), TRC_FAIL); /* State was created for state machine 1 */
	assert_int_equal_cs(xTraceStateMachineSetState(pxState->xMachine2Handle, pxState->xState21Handle), TRC_SUCCESS);
	assert_int_equal_cs(xTraceStateMachineSetState(pxState->xMachine2Handle, pxState->xState22Handle), TRC_SUCCESS);
}

uint32_t runStateMachineTests(void)
{
	uint32_t uiCountFailedTests = 0;

	const struct CMUnitTest aTests_trcStateMachine[] =
	{
		cmocka_unit_test_setup_teardown(test_xTraceStateMachineCreate, setup_emptyStateMachine, teardown_stateMachine),
		cmocka_unit_test_setup_teardown(test_xTraceStateMachineStateCreate, setup_twoStatesTwoMachines, teardown_stateMachine),
		cmocka_unit_test_setup_teardown(test_xTraceStateMachineSetState, setup_all, teardown_stateMachine),
	};

	uiCountFailedTests += cmocka_run_group_tests(aTests_trcStateMachine, group_setup_StateMachine, group_teardown_StateMachine);

	return uiCountFailedTests;
}
