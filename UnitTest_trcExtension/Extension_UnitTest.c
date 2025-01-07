#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <Extension_UnitTest.h>
#include <trcRecorder.h>

/******************************************************************************************
*                                        TEST STATE                                       *
******************************************************************************************/

typedef struct ExtensionsTestState
{
	/* Pointer to Buffer */
	TraceEntryTable_t xEntryTableBuffer;
	TraceEntryIndexTable_t xEntryIndexTableBuffer;
	TraceDiagnosticsData_t xDiagnosticsBuffer;
	TraceExtensionData_t xExtensionBuffer;
	
	/* Test Objects */
	TraceExtensionHandle_t xExtension1Handle, xExtension2Handle;

	/* Test Parameters */
	const char *szExt1Name, *szExt2Name;
	uint32_t uiExt1Major, uiExt1Minor, uiExt1Patch;
	uint32_t uiExt2Major, uiExt2Minor, uiExt2Patch;
	uint32_t uiExt1EventCount, uiExt2EventCount;
	uint32_t uiExt1LocalEventId, uiExt2LocalEventId;
	uint32_t uiOriginalExtensionBaseId;
} ExtensionsTestState_t;

ExtensionsTestState_t xExtensionsTestState;
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

int group_setup_Extensions(void **ppState)
{
	xExtensionsTestState.xExtension1Handle = 0;
	xExtensionsTestState.xExtension2Handle = 0;
	xExtensionsTestState.szExt1Name = "EXT1";
	xExtensionsTestState.szExt2Name = "EXT2";
	xExtensionsTestState.uiOriginalExtensionBaseId = 0;
	xExtensionsTestState.uiExt1Major = 1;
	xExtensionsTestState.uiExt1Minor = 2;
	xExtensionsTestState.uiExt1Patch = 3;
	xExtensionsTestState.uiExt2Major = 4;
	xExtensionsTestState.uiExt2Minor = 5;
	xExtensionsTestState.uiExt2Patch = 6;
	xExtensionsTestState.uiExt1EventCount = 5;
	xExtensionsTestState.uiExt2EventCount = 10;
	xExtensionsTestState.uiExt1LocalEventId = 3;
	xExtensionsTestState.uiExt2LocalEventId = 6;
	*ppState = &xExtensionsTestState;

	assert_int_equal(sizeof(TraceUnsignedBaseType_t), sizeof(void*));
	assert_int_equal(sizeof(TraceUnsignedBaseType_t), sizeof(TraceBaseType_t));

	return 0;
}

int group_teardown_Extensions(void **ppState)
{
	ExtensionsTestState_t* pxState = (ExtensionsTestState_t*)*ppState;

	pxState->uiOriginalExtensionBaseId = 0;
	pxState->xExtension1Handle = 0;
	pxState->xExtension2Handle = 0;
	pxState->szExt1Name = "";
	pxState->szExt2Name = "";
	pxState->uiExt1EventCount = 0;
	pxState->uiExt2EventCount = 0;
	pxState->uiExt1LocalEventId = 0;
	pxState->uiExt2LocalEventId = 0;
	pxState->uiExt1Major = 0;
	pxState->uiExt1Minor = 0;
	pxState->uiExt1Patch = 0;
	pxState->uiExt2Major = 0;
	pxState->uiExt2Minor = 0;
	pxState->uiExt2Patch = 0;

	return 0;
}

int setup_emptyExtensions(void** ppState)
{
	ExtensionsTestState_t* pxState = (ExtensionsTestState_t*)*ppState;
	
	assert_int_equal_cs(xTraceEntryTableInitialize(&pxState->xEntryTableBuffer), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEntryIndexTableInitialize(&pxState->xEntryIndexTableBuffer), TRC_SUCCESS);
	assert_int_equal_cs(xTraceDiagnosticsInitialize(&pxState->xDiagnosticsBuffer), TRC_SUCCESS);
	assert_int_equal_cs(xTraceExtensionInitialize(&pxState->xExtensionBuffer), TRC_SUCCESS);

	pxState->uiOriginalExtensionBaseId = (uint32_t)pxState->xExtensionBuffer.uxNextFreeExtensionEventId;

	return 0;
}

int setup_twoExtensions(void **ppState)
{
	ExtensionsTestState_t* pxState = (ExtensionsTestState_t*)*ppState;

	assert_int_equal_cs(setup_emptyExtensions(ppState), 0);

	assert_int_equal_cs(xTraceExtensionCreate(pxState->szExt1Name, pxState->uiExt1Major, pxState->uiExt1Minor, pxState->uiExt1Patch, pxState->uiExt1EventCount, &pxState->xExtension1Handle), TRC_SUCCESS);
	assert_int_equal_cs(xTraceExtensionCreate(pxState->szExt2Name, pxState->uiExt2Major, pxState->uiExt2Minor, pxState->uiExt2Patch, pxState->uiExt2EventCount, &pxState->xExtension2Handle), TRC_SUCCESS);

	return 0;
}

int teardown_extensions(void **ppState)
{
	ExtensionsTestState_t* pxState = (ExtensionsTestState_t*)*ppState;

	pxState->xExtensionBuffer.uxNextFreeExtensionEventId = (TraceUnsignedBaseType_t)pxState->uiOriginalExtensionBaseId;

	pxState->xExtension1Handle = 0;
	pxState->xExtension2Handle = 0;

	return 0;
}

/******************************************************************************************
*                                        TESTS                                            *
******************************************************************************************/

void test_xTraceExtensionCreate(void **ppState)
{
	ExtensionsTestState_t* pxState = (ExtensionsTestState_t*)*ppState;

	assert_int_equal_cs(xTraceExtensionCreate(0, pxState->uiExt1Major, pxState->uiExt1Minor, pxState->uiExt1Patch, pxState->uiExt1EventCount, &pxState->xExtension1Handle), TRC_FAIL); /* null test */
	assert_int_equal_cs(xTraceExtensionCreate("", pxState->uiExt1Major, pxState->uiExt1Minor, pxState->uiExt1Patch, pxState->uiExt1EventCount, &pxState->xExtension1Handle), TRC_FAIL); /* empty string test */
	assert_int_equal_cs(xTraceExtensionCreate(pxState->szExt1Name, pxState->uiExt1Major, pxState->uiExt1Minor, pxState->uiExt1Patch, pxState->uiExt1EventCount, 0), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceExtensionCreate(pxState->szExt1Name, pxState->uiExt1Major, pxState->uiExt1Minor, pxState->uiExt1Patch, pxState->uiExt1EventCount, &pxState->xExtension1Handle), TRC_SUCCESS);
	assert_int_equal_cs(xTraceExtensionCreate(pxState->szExt2Name, pxState->uiExt2Major, pxState->uiExt2Minor, pxState->uiExt2Patch, pxState->uiExt2EventCount, &pxState->xExtension2Handle), TRC_SUCCESS);

	assert_int_not_equal((TraceUnsignedBaseType_t)pxState->xExtension1Handle, (TraceUnsignedBaseType_t)pxState->xExtension2Handle);
}

void test_xTraceExtensionGetBaseEventId(void **ppState)
{
	uint32_t uiExt1BaseEventId, uiExt2BaseEventId;
	ExtensionsTestState_t* pxState = (ExtensionsTestState_t*)*ppState;

	assert_int_equal_cs(xTraceExtensionGetBaseEventId(0, &uiExt1BaseEventId), TRC_FAIL); /* null test */
	assert_int_equal_cs(xTraceExtensionGetBaseEventId(pxState->xExtension1Handle, 0), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceExtensionGetBaseEventId(pxState->xExtension1Handle, &uiExt1BaseEventId), TRC_SUCCESS);
	assert_int_equal_cs(uiExt1BaseEventId, pxState->uiOriginalExtensionBaseId);

	assert_int_equal_cs(xTraceExtensionGetBaseEventId(pxState->xExtension2Handle, &uiExt2BaseEventId), TRC_SUCCESS);
	assert_int_equal_cs(uiExt2BaseEventId, pxState->uiOriginalExtensionBaseId + pxState->uiExt1EventCount);
}

void test_xTraceExtensionGetEventId(void **ppState)
{
	uint32_t uiExt1BaseEventId, uiExt2BaseEventId;
	uint32_t uiExt1EventId, uiExt2EventId;
	ExtensionsTestState_t* pxState = (ExtensionsTestState_t*)*ppState;

	assert_int_equal_cs(xTraceExtensionGetBaseEventId(pxState->xExtension1Handle, &uiExt1BaseEventId), TRC_SUCCESS);
	uiExt1EventId = xTraceExtensionGetEventId(pxState->xExtension1Handle, pxState->uiExt1LocalEventId);
	assert_int_equal_cs(uiExt1EventId, uiExt1BaseEventId + pxState->uiExt1LocalEventId);

	assert_int_equal_cs(xTraceExtensionGetBaseEventId(pxState->xExtension2Handle, &uiExt2BaseEventId), TRC_SUCCESS);
	uiExt2EventId = xTraceExtensionGetEventId(pxState->xExtension2Handle, pxState->uiExt2LocalEventId);
	assert_int_equal_cs(uiExt2EventId, uiExt2BaseEventId + pxState->uiExt2LocalEventId);
}

void test_xTraceExtensionGetConfigName(void **ppState)
{
	const char *szExt1ConfigName = 0;
	const char *szExt2ConfigName = 0;
	ExtensionsTestState_t* pxState = (ExtensionsTestState_t*)*ppState;

	assert_int_equal_cs(xTraceExtensionGetConfigName(0, &szExt1ConfigName), TRC_FAIL); /* null test */
	assert_int_equal_cs(xTraceExtensionGetConfigName(pxState->xExtension1Handle, 0), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceExtensionGetConfigName(pxState->xExtension1Handle, &szExt1ConfigName), TRC_SUCCESS);
	assert_int_equal_cs(xTraceExtensionGetConfigName(pxState->xExtension2Handle, &szExt2ConfigName), TRC_SUCCESS);

	assert_string_equal(szExt1ConfigName, pxState->szExt1Name);
	assert_string_equal(szExt2ConfigName, pxState->szExt2Name);
}

uint32_t runExtensionTests(void)
{
	uint32_t uiCountFailedTests = 0;

	const struct CMUnitTest aTests_trcExtensions[] =
	{
		cmocka_unit_test_setup_teardown(test_xTraceExtensionCreate, setup_emptyExtensions, teardown_extensions),
		cmocka_unit_test_setup_teardown(test_xTraceExtensionGetBaseEventId, setup_twoExtensions, teardown_extensions),
		cmocka_unit_test_setup_teardown(test_xTraceExtensionGetEventId, setup_twoExtensions, teardown_extensions),
		cmocka_unit_test_setup_teardown(test_xTraceExtensionGetConfigName, setup_twoExtensions, teardown_extensions),
	};

	uiCountFailedTests += cmocka_run_group_tests(aTests_trcExtensions, group_setup_Extensions, group_teardown_Extensions);

	return uiCountFailedTests;
}
