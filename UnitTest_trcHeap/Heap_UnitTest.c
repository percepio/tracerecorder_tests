#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <Heap_UnitTest.h>
#include <trcRecorder.h>

int group_teardown_Heap(void** ppState);
int group_setup_Heap(void** ppState);
int setup_Heap(void** ppState);
int teardown_Heap(void** ppState);

void test_xTraceHeapCreate(void** ppState);
void test_xTraceHeapAlloc(void** ppState);
void test_xTraceHeapFree(void** ppState);
void test_xTraceHeapGetSetCurrent(void** ppState);
void test_xTraceHeapGetSetHighWaterMark(void** ppState);
void test_xTraceHeapGetSetMax(void** ppState);

/******************************************************************************************
*                                        TEST STATE                                       *
******************************************************************************************/

typedef struct HeapTestState
{
	/* Pointer to Buffer */
	TraceEntryTable_t xEntryTableBuffer;
	TraceEntryIndexTable_t xEntryIndexTableBuffer;
	TraceDiagnosticsData_t xDiagnosticsBuffer;
	
	/* Pointer to Buffer */
	TraceHeapHandle_t xHeapHandle;
	char* szName;
} HeapTestState_t;

HeapTestState_t xHeapTestState;

uint32_t uiTestTimestampCounter = 0;
volatile uint32_t MOCK_RecorderEnabled = 1;
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

int group_setup_Heap(void **ppState)
{
	*ppState = &xHeapTestState;
	xHeapTestState.szName = "TEST_HEAP";

	assert_int_equal(sizeof(TraceUnsignedBaseType_t), sizeof(void*));
	assert_int_equal(sizeof(TraceUnsignedBaseType_t), sizeof(TraceBaseType_t));

	return 0;
}

int group_teardown_Heap(void **ppState)
{
	return 0;
}

int setup_Heap(void **ppState)
{
	HeapTestState_t* pxState = (HeapTestState_t*)*ppState;

	assert_int_equal_cs(xTraceEntryTableInitialize(&pxState->xEntryTableBuffer), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEntryIndexTableInitialize(&pxState->xEntryIndexTableBuffer), TRC_SUCCESS);
	assert_int_equal_cs(xTraceDiagnosticsInitialize(&pxState->xDiagnosticsBuffer), TRC_SUCCESS);
	
	assert_int_equal_cs(xTraceHeapCreate(pxState->szName, 0, 0, 100, &pxState->xHeapHandle), TRC_SUCCESS);

	return 0;
}

int teardown_Heap(void **ppState)
{
	HeapTestState_t* pxState = (HeapTestState_t*)*ppState;

	pxState->xHeapHandle = 0;
	
	return 0;
}

/******************************************************************************************
*                                        TESTS                                            *
******************************************************************************************/

void test_xTraceHeapCreate(void **ppState)
{
	TraceUnsignedBaseType_t uxCurrent, uxHighWaterMark, uxMax;
	HeapTestState_t* pxState = (HeapTestState_t*)*ppState;

	assert_int_equal_cs(xTraceHeapCreate(0, 0, 0, 0, &pxState->xHeapHandle), TRC_SUCCESS); /* null test */
	assert_int_equal_cs(xTraceHeapCreate(pxState->szName, 0, 0, 0, 0), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceHeapCreate(pxState->szName, 10, 50, 90, &pxState->xHeapHandle), TRC_SUCCESS);

	assert_int_equal_cs(xTraceHeapGetCurrent(pxState->xHeapHandle, &uxCurrent), TRC_SUCCESS);
	assert_int_equal_cs(uxCurrent, 10);

	assert_int_equal_cs(xTraceHeapGetHighWaterMark(pxState->xHeapHandle, &uxHighWaterMark), TRC_SUCCESS);
	assert_int_equal_cs(uxHighWaterMark, 50);

	assert_int_equal_cs(xTraceHeapGetMax(pxState->xHeapHandle, &uxMax), TRC_SUCCESS);
	assert_int_equal_cs(uxMax, 90);
}

void test_xTraceHeapAlloc(void **ppState)
{
	HeapTestState_t* pxState = (HeapTestState_t*)*ppState;
	TraceUnsignedBaseType_t uxCurrent;

	assert_int_equal_cs(xTraceHeapAlloc(0, (void*)0x12345678, 10), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceHeapAlloc(pxState->xHeapHandle, (void*)0x12345678, 10), TRC_SUCCESS);
	assert_int_equal_cs(xTraceHeapGetCurrent(pxState->xHeapHandle, &uxCurrent), TRC_SUCCESS);
	assert_int_equal_cs(uxCurrent, 10);
	assert_int_equal_cs(xTraceHeapAlloc(pxState->xHeapHandle, (void*)0x12345678, 10), TRC_SUCCESS);
	assert_int_equal_cs(xTraceHeapGetCurrent(pxState->xHeapHandle, &uxCurrent), TRC_SUCCESS);
	assert_int_equal_cs(uxCurrent, 20);
	assert_int_equal_cs(xTraceHeapAlloc(pxState->xHeapHandle, 0, 10), TRC_SUCCESS); /* null address failed alloc */
	assert_int_equal_cs(xTraceHeapGetCurrent(pxState->xHeapHandle, &uxCurrent), TRC_SUCCESS);
	assert_int_equal_cs(uxCurrent, 20);
}

void test_xTraceHeapFree(void **ppState)
{
	HeapTestState_t* pxState = (HeapTestState_t*)*ppState;
	TraceUnsignedBaseType_t uxCurrent;
	
	assert_int_equal_cs(xTraceHeapFree(0, (void*)0x12345678, 50), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceHeapAlloc(pxState->xHeapHandle, (void*)0x12345678, 50), TRC_SUCCESS);
	
	assert_int_equal_cs(xTraceHeapFree(pxState->xHeapHandle, (void*)0x12345678, 10), TRC_SUCCESS);
	assert_int_equal_cs(xTraceHeapGetCurrent(pxState->xHeapHandle, &uxCurrent), TRC_SUCCESS);
	assert_int_equal_cs(uxCurrent, 40);

	assert_int_equal_cs(xTraceHeapFree(pxState->xHeapHandle, (void*)0x12345678, 10), TRC_SUCCESS);
	assert_int_equal_cs(xTraceHeapGetCurrent(pxState->xHeapHandle, &uxCurrent), TRC_SUCCESS);
	assert_int_equal_cs(uxCurrent, 30);

	assert_int_equal_cs(xTraceHeapFree(pxState->xHeapHandle, 0, 10), TRC_SUCCESS); /* null address failed free */
	assert_int_equal_cs(xTraceHeapGetCurrent(pxState->xHeapHandle, &uxCurrent), TRC_SUCCESS);
	assert_int_equal_cs(uxCurrent, 30);

	assert_int_equal_cs(xTraceHeapFree(pxState->xHeapHandle, (void*)0x12345678, 30), TRC_SUCCESS);
	assert_int_equal_cs(xTraceHeapGetCurrent(pxState->xHeapHandle, &uxCurrent), TRC_SUCCESS);
	assert_int_equal_cs(uxCurrent, 0);
}

void test_xTraceHeapGetSetCurrent(void **ppState)
{
	HeapTestState_t* pxState = (HeapTestState_t*)*ppState;
	TraceUnsignedBaseType_t uxCurrent;
	
	assert_int_equal_cs(xTraceHeapGetCurrent(0, &uxCurrent), TRC_FAIL); /* null test */
	assert_int_equal_cs(xTraceHeapGetCurrent(pxState->xHeapHandle, 0), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceHeapSetCurrent(0, 200), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceHeapGetCurrent(pxState->xHeapHandle, &uxCurrent), TRC_SUCCESS);
	assert_int_equal_cs(uxCurrent, 0);

	assert_int_equal_cs(xTraceHeapAlloc(pxState->xHeapHandle, (void*)0x12345678, 50), TRC_SUCCESS);
	
	assert_int_equal_cs(xTraceHeapGetCurrent(pxState->xHeapHandle, &uxCurrent), TRC_SUCCESS);
	assert_int_equal_cs(uxCurrent, 50);

	assert_int_equal_cs(xTraceHeapSetCurrent(pxState->xHeapHandle, 200), TRC_SUCCESS);
	assert_int_equal_cs(xTraceHeapGetCurrent(pxState->xHeapHandle, &uxCurrent), TRC_SUCCESS);
	assert_int_equal_cs(uxCurrent, 200);
}

void test_xTraceHeapGetSetHighWaterMark(void **ppState)
{
	HeapTestState_t* pxState = (HeapTestState_t*)*ppState;
	TraceUnsignedBaseType_t uxHighWaterMark;

	assert_int_equal_cs(xTraceHeapGetHighWaterMark(0, &uxHighWaterMark), TRC_FAIL); /* null test */
	assert_int_equal_cs(xTraceHeapGetHighWaterMark(pxState->xHeapHandle, 0), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceHeapSetHighWaterMark(0, 9000), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceHeapGetHighWaterMark(pxState->xHeapHandle, &uxHighWaterMark), TRC_SUCCESS);
	assert_int_equal_cs(uxHighWaterMark, 0);

	assert_int_equal_cs(xTraceHeapAlloc(pxState->xHeapHandle, (void*)0x12345678, 50), TRC_SUCCESS);

	assert_int_equal_cs(xTraceHeapGetHighWaterMark(pxState->xHeapHandle, &uxHighWaterMark), TRC_SUCCESS);
	assert_int_equal_cs(uxHighWaterMark, 50);

	assert_int_equal_cs(xTraceHeapAlloc(pxState->xHeapHandle, 0, 50), TRC_SUCCESS); /* null address failed alloc */

	assert_int_equal_cs(xTraceHeapGetHighWaterMark(pxState->xHeapHandle, &uxHighWaterMark), TRC_SUCCESS);
	assert_int_equal_cs(uxHighWaterMark, 50);

	assert_int_equal_cs(xTraceHeapFree(pxState->xHeapHandle, (void*)0x12345678, 30), TRC_SUCCESS);

	assert_int_equal_cs(xTraceHeapGetHighWaterMark(pxState->xHeapHandle, &uxHighWaterMark), TRC_SUCCESS);
	assert_int_equal_cs(uxHighWaterMark, 50);

	assert_int_equal_cs(xTraceHeapSetHighWaterMark(pxState->xHeapHandle, 9000), TRC_SUCCESS);
	assert_int_equal_cs(xTraceHeapGetHighWaterMark(pxState->xHeapHandle, &uxHighWaterMark), TRC_SUCCESS);
	assert_int_equal_cs(uxHighWaterMark, 9000);
}

void test_xTraceHeapGetSetMax(void **ppState)
{
	HeapTestState_t* pxState = (HeapTestState_t*)*ppState;
	TraceUnsignedBaseType_t uxMax;

	assert_int_equal_cs(xTraceHeapGetMax(0, &uxMax), TRC_FAIL); /* null test */
	assert_int_equal_cs(xTraceHeapGetMax(pxState->xHeapHandle, 0), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceHeapSetMax(0, 1234), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceHeapGetMax(pxState->xHeapHandle, &uxMax), TRC_SUCCESS);
	assert_int_equal_cs(uxMax, 100);

	assert_int_equal_cs(xTraceHeapSetMax(pxState->xHeapHandle, 1234), TRC_SUCCESS);
	assert_int_equal_cs(xTraceHeapGetMax(pxState->xHeapHandle, &uxMax), TRC_SUCCESS);
	assert_int_equal_cs(uxMax, 1234);
}

uint32_t runHeapTests(void)
{
	uint32_t uiCountFailedTests = 0;

	const struct CMUnitTest aTests_trcHeap[] =
	{
		cmocka_unit_test_setup_teardown(test_xTraceHeapCreate, setup_Heap, teardown_Heap),
		cmocka_unit_test_setup_teardown(test_xTraceHeapAlloc, setup_Heap, teardown_Heap),
		cmocka_unit_test_setup_teardown(test_xTraceHeapFree, setup_Heap, teardown_Heap),
		cmocka_unit_test_setup_teardown(test_xTraceHeapGetSetCurrent, setup_Heap, teardown_Heap),
		cmocka_unit_test_setup_teardown(test_xTraceHeapGetSetHighWaterMark, setup_Heap, teardown_Heap),
		cmocka_unit_test_setup_teardown(test_xTraceHeapGetSetMax, setup_Heap, teardown_Heap),
	};

	uiCountFailedTests += cmocka_run_group_tests(aTests_trcHeap, group_setup_Heap, group_teardown_Heap);
	
	return uiCountFailedTests;
}
