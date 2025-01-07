#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <Runnable_UnitTest.h>
#include <trcRecorder.h>

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
	TraceExtensionData_t xExtensionData;
} StringTestState_t;

StringTestState_t xStringTestState;
volatile uint32_t MOCK_RecorderInitialized = 1;
uint32_t MOCK_uiCriticalSection = 0;
uint32_t MOCK_uiCriticalSectionExpectedValue = 0;
uint8_t MOCK_tmpBuffer[TRC_MAX_BLOB_SIZE];
uint8_t MOCK_streamPortBuffer[TRC_MAX_BLOB_SIZE];
uint32_t MOCK_streamPortMaxTransferSize = 0xFFFFFFFF;

void* MOCK_TASK_pvCurrentTask = (void*)0x12345678;

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
	assert_int_equal_cs(xTraceExtensionInitialize(&pxState->xExtensionData), TRC_SUCCESS);

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

void test_xTraceRunnableRegister(void** ppState)
{
	TraceRunnableHandle_t xRunnableHandle = 0;
	TraceEntryHandle_t xEntryHandle = 0;
	uint32_t uiCountBefore, uiCountAfter, uiOptions = 0;
	char* szTestString = "test";
	const char* szStr = (void*)0;
	void* pvCurrentTask = (void*)0;
	
	assert_int_equal_cs(xTraceRunnableRegister(0, TRC_RUNNABLE_REGISTER_METHOD_USE_HANDLE_ADDRESS , &xRunnableHandle), TRC_FAIL); /* null test */
	assert_int_equal_cs(xTraceRunnableRegister(szTestString, TRC_RUNNABLE_REGISTER_METHOD_USE_HANDLE_ADDRESS, 0), TRC_FAIL); /* null test */

	/* Do NOT store in entry table */
	xRunnableHandle = 0;
	assert_int_equal(xTraceEntryGetCount(&uiCountBefore), TRC_SUCCESS);
	assert_int_equal_cs_create_data1(xTraceRunnableRegister(szTestString, TRC_RUNNABLE_REGISTER_METHOD_USE_HANDLE_ADDRESS, &xRunnableHandle), TRC_SUCCESS,
		PSF_EVENT_RUNNABLE_REGISTER,
		(TraceUnsignedBaseType_t)xRunnableHandle,
		szTestString,
		strlen(szTestString) + 1);	/* We add null termination to length */
	assert_int_equal_cs(xTraceEntryFind((void*)xRunnableHandle, &xEntryHandle), TRC_FAIL);
	assert_int_equal((TraceUnsignedBaseType_t)xRunnableHandle, (TraceUnsignedBaseType_t)&xRunnableHandle);
	assert_int_equal(xTraceEntryGetCount(&uiCountAfter), TRC_SUCCESS);
	assert_int_equal(uiCountBefore, uiCountAfter);

	/* Do NOT store in entry table */
	xRunnableHandle = 0;
	assert_int_equal(xTraceEntryGetCount(&uiCountBefore), TRC_SUCCESS);
	assert_int_equal_cs_create_data1(xTraceRunnableRegister(szTestString, TRC_RUNNABLE_REGISTER_METHOD_USE_STRING_ADDRESS, &xRunnableHandle), TRC_SUCCESS,
		PSF_EVENT_RUNNABLE_REGISTER,
		(TraceUnsignedBaseType_t)xRunnableHandle,
		szTestString,
		strlen(szTestString) + 1);	/* We add null termination to length */
	assert_int_equal_cs(xTraceEntryFind((void*)xRunnableHandle, &xEntryHandle), TRC_FAIL);
	assert_int_equal((TraceUnsignedBaseType_t)xRunnableHandle, (TraceUnsignedBaseType_t)szTestString);
	assert_int_equal(xTraceEntryGetCount(&uiCountAfter), TRC_SUCCESS);
	assert_int_equal(uiCountBefore, uiCountAfter);

	/* Store in entry table */
	xRunnableHandle = 0;
	assert_int_equal(xTraceEntryGetCount(&uiCountBefore), TRC_SUCCESS);
	assert_int_equal_cs_create_data1(xTraceRunnableRegister(szTestString, TRC_RUNNABLE_REGISTER_METHOD_USE_ENTRY_TABLE, &xRunnableHandle), TRC_SUCCESS,
		PSF_EVENT_RUNNABLE_REGISTER,
		(TraceUnsignedBaseType_t)xRunnableHandle,
		szTestString,
		strlen(szTestString) + 1);	/* We add null termination to length */
	assert_int_equal_cs(xTraceEntryFind((void*)xRunnableHandle, &xEntryHandle), TRC_SUCCESS);
	assert_int_equal(xTraceEntryGetOptions(xEntryHandle, &uiOptions), TRC_SUCCESS);
	assert_int_equal(uiOptions, TRC_ENTRY_OPTION_RUNNABLE);
	assert_int_equal_cs(xTraceEntryGetSymbol(xEntryHandle, &szStr), TRC_SUCCESS);
	assert_int_not_equal((TraceUnsignedBaseType_t)szStr, (TraceUnsignedBaseType_t)szTestString);
	assert_string_equal(szStr, szTestString);
	assert_int_equal_cs(xTraceEntryGetState(xEntryHandle, 0, (TraceUnsignedBaseType_t*)&pvCurrentTask), TRC_SUCCESS);
	assert_int_equal((TraceUnsignedBaseType_t)pvCurrentTask, (TraceUnsignedBaseType_t)MOCK_TASK_pvCurrentTask);
	assert_int_equal(xTraceEntryGetCount(&uiCountAfter), TRC_SUCCESS);
	assert_int_equal(uiCountBefore + 1, uiCountAfter);

	/* Already registered, still sent */
	assert_int_equal(xTraceEntryGetCount(&uiCountBefore), TRC_SUCCESS);
	assert_int_equal_cs_create_data1(xTraceRunnableRegister(szTestString, TRC_RUNNABLE_REGISTER_METHOD_USE_ENTRY_TABLE, &xRunnableHandle), TRC_SUCCESS,
		PSF_EVENT_RUNNABLE_REGISTER,
		(TraceUnsignedBaseType_t)xRunnableHandle,
		szTestString,
		strlen(szTestString) + 1);	/* We add null termination to length */
	assert_int_equal(xTraceEntryGetCount(&uiCountAfter), TRC_SUCCESS);
	assert_int_equal(uiCountBefore, uiCountAfter);
}

void test_xTraceRunnableStart(void** ppState)
{
	TraceRunnableHandle_t xRunnableHandle = 0;
	char* szTestString = "test";

	assert_int_equal_cs(xTraceRunnableRegister(szTestString, TRC_RUNNABLE_REGISTER_METHOD_USE_ENTRY_TABLE, &xRunnableHandle), TRC_SUCCESS);
	
	assert_int_equal_cs_create1(xTraceRunnableStart(xRunnableHandle), TRC_SUCCESS, PSF_EVENT_RUNNABLE_START, (TraceUnsignedBaseType_t)xRunnableHandle);
}

void test_xTraceRunnableStop(void** ppState)
{
	TraceRunnableHandle_t xRunnableHandle = 0;
	char* szTestString = "test";

	assert_int_equal_cs(xTraceRunnableRegister(szTestString, TRC_RUNNABLE_REGISTER_METHOD_USE_ENTRY_TABLE, &xRunnableHandle), TRC_SUCCESS);

	assert_int_equal_cs_create0(xTraceRunnableStop(), TRC_SUCCESS, PSF_EVENT_RUNNABLE_STOP);
}

void test_xTraceRunnableRegisterStaticSet(void** ppState)
{
	TraceRunnableStaticSetHandle_t xRunnableSetHandle = 0;
	TraceEntryHandle_t xEntryHandle = 0;
	uint32_t uiCountBefore = 0, uiCountAfter = 0, uiOptions = 0;
	char* szTestString = "test";
	char* szStr = (void*)0;
	void* pvCurrentTask = (void*)0;

	assert_int_equal_cs(xTraceRunnableRegisterStaticSet(0, 1, 0, 0, 5, &xRunnableSetHandle), TRC_FAIL); /* null test */
	assert_int_equal_cs(xTraceRunnableRegisterStaticSet("", 1, 0, 0, 5, &xRunnableSetHandle), TRC_FAIL); /* empty string test */
	assert_int_equal_cs(xTraceRunnableRegisterStaticSet(szTestString, 1, 0, 0, 0, &xRunnableSetHandle), TRC_FAIL); /* null test */
	assert_int_equal_cs(xTraceRunnableRegisterStaticSet(szTestString, 1, 0, 0, 5, 0), TRC_FAIL); /* null test */
	
	assert_int_equal_cs_create_data4(xTraceRunnableRegisterStaticSet(szTestString, 1, 2, 3, 5, &xRunnableSetHandle), TRC_SUCCESS,
		PSF_EVENT_EXTENSION_CREATE,
		(TraceUnsignedBaseType_t)xRunnableSetHandle,
		1 << 24 | 2 << 16 | 3,
		(TRC_EVENT_LAST_ID + 1) + 0,
		5,
		szTestString,
		strlen(szTestString) + 1);	/* Runnable Sets are Extensions */	/* We add null termination to length */
	
	assert_int_equal_cs_create_data4(xTraceRunnableRegisterStaticSet(szTestString, 4, 5, 6, 5, &xRunnableSetHandle), TRC_SUCCESS,
		PSF_EVENT_EXTENSION_CREATE,
		(TraceUnsignedBaseType_t)xRunnableSetHandle,
		4 << 24 | 5 << 16 | 6,
		(TRC_EVENT_LAST_ID + 1) + 5,
		5,
		szTestString,
		strlen(szTestString) + 1);	/* Runnable Sets are Extensions */	/* We add null termination to length */
}

void test_xTraceRunnableStartStatic(void** ppState)
{
	TraceRunnableStaticSetHandle_t xRunnableSetHandle = 0;

	assert_int_equal_cs(xTraceRunnableRegisterStaticSet("RUNNABLES", 1, 0, 0, 5, &xRunnableSetHandle), TRC_SUCCESS);
	
	assert_int_equal_cs_create0(xTraceRunnableStartStatic(xRunnableSetHandle, 0), TRC_SUCCESS, (TRC_EVENT_LAST_ID + 1) + 0);

	assert_int_equal_cs_create0(xTraceRunnableStartStatic(xRunnableSetHandle, 5), TRC_SUCCESS, (TRC_EVENT_LAST_ID + 1) + 5);
}

void test_xTraceRunnableStopStatic(void** ppState)
{
	TraceRunnableStaticSetHandle_t xRunnableSetHandle = 0;

	assert_int_equal_cs(xTraceRunnableRegisterStaticSet("RUNNABLES", 1, 0, 0, 5, &xRunnableSetHandle), TRC_SUCCESS);
	
	assert_int_equal_cs_create0(xTraceRunnableStopStatic(), TRC_SUCCESS, PSF_EVENT_RUNNABLE_STOP);

	assert_int_equal_cs_create0(xTraceRunnableStopStatic(), TRC_SUCCESS, PSF_EVENT_RUNNABLE_STOP);
}

uint32_t runRunnableTests(void)
{
	uint32_t uiCountFailedTests = 0;

	const struct CMUnitTest aTests_trcRunnable[] =
	{
		cmocka_unit_test_setup_teardown(test_xTraceRunnableRegister, setup_Runnable, teardown_Runnable),
		cmocka_unit_test_setup_teardown(test_xTraceRunnableStart, setup_Runnable, teardown_Runnable),
		cmocka_unit_test_setup_teardown(test_xTraceRunnableStop, setup_Runnable, teardown_Runnable),
		cmocka_unit_test_setup_teardown(test_xTraceRunnableRegisterStaticSet, setup_Runnable, teardown_Runnable),
		cmocka_unit_test_setup_teardown(test_xTraceRunnableStartStatic, setup_Runnable, teardown_Runnable),
		cmocka_unit_test_setup_teardown(test_xTraceRunnableStopStatic, setup_Runnable, teardown_Runnable),
	};

	uiCountFailedTests += cmocka_run_group_tests(aTests_trcRunnable, group_setup_Runnable, group_teardown_Runnable);

	return uiCountFailedTests;
}
