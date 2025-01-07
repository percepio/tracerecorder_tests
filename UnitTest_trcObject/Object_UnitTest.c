#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <Object_UnitTest.h>
#include <trcRecorder.h>

int group_setup_Object(void** ppState);
int group_teardown_Object(void** ppState);
int setup_Object(void** ppState);
int teardown_Object(void** ppState);

void test_xTraceObjectCreate(void** ppState);
void test_xTraceObjectCreateWithoutHandle(void** ppState);
void test_xTraceObjectDelete(void** ppState);
void test_xTraceObjectDeleteWithoutHandle(void** ppState);
void test_xTraceObjectSetName(void** ppState);
void test_xTraceObjectSetNameWithoutHandle(void** ppState);
void test_xTraceObjectSetState(void** ppState);
void test_xTraceObjectSetStateWithoutHandle(void** ppState);
void test_xTraceObjectSetSpecificState(void** ppState);
void test_xTraceObjectSetSpecficiStateWithoutHandle(void** ppState);
void test_xTraceObjectSetOptions(void** ppState);
void test_xTraceObjectSetOptionsWithoutHandle(void** ppState);

/******************************************************************************************
*                                        TEST STATE                                       *
******************************************************************************************/

typedef struct ObjectTestState
{
	/* Test Objects */
	TraceEntryTable_t xEntryTableBuffer;
	TraceEntryIndexTable_t xEntryIndexTableBuffer;
	TraceDiagnosticsData_t xDiagnosticsBuffer;
} ObjectTestState_t;

ObjectTestState_t xObjectTestState;

uint32_t uiTraceSystemState = TRC_STATE_IN_APPLICATION;
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

int group_setup_Object(void **ppState)
{
	*ppState = &xObjectTestState;

	assert_int_equal(sizeof(TraceUnsignedBaseType_t), sizeof(void*));
	assert_int_equal(sizeof(TraceUnsignedBaseType_t), sizeof(TraceBaseType_t));

	return 0;
}

int group_teardown_Object(void **ppState)
{
	*ppState = 0;
	
	return 0;
}

int setup_Object(void **ppState)
{
	ObjectTestState_t* pxState = (ObjectTestState_t*)*ppState;

	assert_int_equal_cs(xTraceEntryTableInitialize(&xObjectTestState.xEntryTableBuffer), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEntryIndexTableInitialize(&pxState->xEntryIndexTableBuffer), TRC_SUCCESS);
	assert_int_equal_cs(xTraceDiagnosticsInitialize(&pxState->xDiagnosticsBuffer), TRC_SUCCESS);

	return 0;
}

int teardown_Object(void **ppState)
{
	ObjectTestState_t* pxState = (ObjectTestState_t*)*ppState;

	return 0;
}

/******************************************************************************************
*                                        TESTS                                            *
******************************************************************************************/

void test_xTraceObjectCreate(void** ppState)
{
	void* pvObject = (void*)0x12345678;
	TraceObjectHandle_t xObjectHandle = 0;
	TraceEntryHandle_t xEntryHandle = 0;
	TraceUnsignedBaseType_t uxState;
	const char* szName;

	assert_int_equal_cs(xTraceObjectRegister(1, 0, "TEST", 0x87654321, &xObjectHandle), TRC_SUCCESS); /* null test, this is OK */

	assert_int_equal_asserts_enabled(xTraceObjectRegister(1, pvObject, "TEST", 0x87654321, 0), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceEntryFind(pvObject, &xEntryHandle), TRC_FAIL);
	
	assert_int_equal_cs(xTraceObjectRegister(1, pvObject, "TEST", 0x87654321, &xObjectHandle), TRC_SUCCESS);

	assert_int_not_equal((TraceUnsignedBaseType_t)xObjectHandle, (TraceUnsignedBaseType_t)0);

	assert_int_equal_cs(xTraceEntryFind(pvObject, &xEntryHandle), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetSymbol(xEntryHandle, &szName), TRC_SUCCESS);

	assert_string_equal(szName, "TEST");

	assert_int_equal_cs(xTraceEntryGetState(xEntryHandle, 0, &uxState), TRC_SUCCESS);

	assert_int_equal_cs(uxState, 0x87654321);

	assert_int_equal_cs(xTraceObjectRegister(1, pvObject, 0, 0x87654321, &xObjectHandle), TRC_SUCCESS); /* null string */
}

void test_xTraceObjectCreateWithoutHandle(void** ppState)
{
	void* pvObject = (void*)0x12345678;
	TraceEntryHandle_t xEntryHandle = 0;
	TraceUnsignedBaseType_t uxState;
	const char* szName;

	assert_int_equal_cs(xTraceObjectRegisterWithoutHandle(1, 0, "TEST", 0x87654321), TRC_SUCCESS); /* null test, this is OK */

	assert_int_equal_cs(xTraceEntryFind(pvObject, &xEntryHandle), TRC_FAIL);

	assert_int_equal_cs(xTraceObjectRegisterWithoutHandle(1, pvObject, "TEST", 0x87654321), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryFind(pvObject, &xEntryHandle), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetSymbol(xEntryHandle, &szName), TRC_SUCCESS);

	assert_string_equal(szName, "TEST");

	assert_int_equal_cs(xTraceEntryGetState(xEntryHandle, 0, &uxState), TRC_SUCCESS);

	assert_int_equal_cs(uxState, 0x87654321);

	assert_int_equal_cs(xTraceObjectRegisterWithoutHandle(1, pvObject, 0, 0x87654321), TRC_SUCCESS); /* null string */
}

void test_xTraceObjectDelete(void** ppState)
{
	void* pvObject = (void*)0x12345678;
	TraceObjectHandle_t xObjectHandle = 0;

	assert_int_equal_asserts_enabled(xTraceObjectUnregister(0, 1, 0x87654321), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceObjectRegister(1, pvObject, "TEST", 0x87654321, &xObjectHandle), TRC_SUCCESS);

	assert_int_equal_cs(xTraceObjectUnregister(xObjectHandle, 1, 0x87654321), TRC_SUCCESS);

	assert_int_equal_cs(xTraceObjectUnregister(xObjectHandle, 1, 0x87654321), TRC_FAIL);
}

void test_xTraceObjectDeleteWithoutHandle(void** ppState)
{
	void* pvObject = (void*)0x12345678;

	assert_int_equal_asserts_enabled(xTraceObjectUnregisterWithoutHandle(1, 0, 0x87654321), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceObjectUnregisterWithoutHandle(1, (void*)0x12345678, 0x87654321), TRC_FAIL);

	assert_int_equal_cs(xTraceObjectRegisterWithoutHandle(1, pvObject, "TEST", 0x87654321), TRC_SUCCESS);

	assert_int_equal_cs(xTraceObjectUnregisterWithoutHandle(1, (void*)0x12345678, 0x87654321), TRC_SUCCESS);

	assert_int_equal_cs(xTraceObjectUnregisterWithoutHandle(1, (void*)0x12345678, 0x87654321), TRC_FAIL);
}

void test_xTraceObjectSetName(void** ppState)
{
	void* pvObject = (void*)0x12345678;
	TraceEntryHandle_t xEntryHandle;
	TraceObjectHandle_t xObjectHandle;
	const char* szName;

	assert_int_equal_asserts_enabled(xTraceObjectSetName(0, ""), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceObjectRegister(1, pvObject, "TEST", 0, &xObjectHandle), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryFind(pvObject, &xEntryHandle), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetSymbol(xEntryHandle, &szName), TRC_SUCCESS);

	assert_string_equal(szName, "TEST");

	assert_int_equal_cs(xTraceObjectSetName(xObjectHandle, 0), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryFind(pvObject, &xEntryHandle), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetSymbol(xEntryHandle, &szName), TRC_SUCCESS);

	assert_string_equal(szName, "");

	assert_int_equal_cs(xTraceObjectSetName(xObjectHandle, "0x87654321"), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetSymbol(xEntryHandle, &szName), TRC_SUCCESS);

	assert_string_equal(szName, "0x87654321");
}

void test_xTraceObjectSetNameWithoutHandle(void** ppState)
{
	void* pvObject = (void*)0x12345678;
	TraceEntryHandle_t xEntryHandle;
	const char* szName;

	assert_int_equal_asserts_enabled(xTraceObjectSetNameWithoutHandle(0, ""), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceObjectRegisterWithoutHandle(1, pvObject, "TEST", 0), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryFind(pvObject, &xEntryHandle), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetSymbol(xEntryHandle, &szName), TRC_SUCCESS);

	assert_string_equal(szName, "TEST");

	assert_int_equal_cs(xTraceObjectSetNameWithoutHandle(pvObject, 0), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetSymbol(xEntryHandle, &szName), TRC_SUCCESS);

	assert_string_equal(szName, "");

	assert_int_equal_cs(xTraceObjectSetNameWithoutHandle(pvObject, "0x87654321"), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetSymbol(xEntryHandle, &szName), TRC_SUCCESS);

	assert_string_equal(szName, "0x87654321");
}

void test_xTraceObjectSetState(void** ppState)
{
	void* pvObject = (void*)0x12345678;
	TraceEntryHandle_t xEntryHandle;
	TraceObjectHandle_t xObjectHandle;
	TraceUnsignedBaseType_t uxState;

	assert_int_equal_asserts_enabled(xTraceObjectSetState(0, 0x87654321), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceObjectRegister(1, pvObject, "TEST", 0x12345678, &xObjectHandle), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryFind(pvObject, &xEntryHandle), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetState(xEntryHandle, 0, &uxState), TRC_SUCCESS);

	assert_int_equal_cs(uxState, 0x12345678);

	assert_int_equal_cs(xTraceObjectSetState(xObjectHandle, 0x87654321), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetState(xEntryHandle, 0, &uxState), TRC_SUCCESS);

	assert_int_equal_cs(uxState, 0x87654321);

	assert_int_equal_cs(xTraceObjectSetState(xObjectHandle, 0x88888888), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetState(xEntryHandle, 0, &uxState), TRC_SUCCESS);

	assert_int_equal_cs(uxState, 0x88888888);
}

void test_xTraceObjectSetStateWithoutHandle(void** ppState)
{
	void* pvObject = (void*)0x12345678;
	TraceEntryHandle_t xEntryHandle;
	TraceUnsignedBaseType_t uxState;

	assert_int_equal_asserts_enabled(xTraceObjectSetStateWithoutHandle(0, 0x87654321), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceObjectRegisterWithoutHandle(1, pvObject, "TEST", 0x12345678), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryFind(pvObject, &xEntryHandle), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetState(xEntryHandle, 0, &uxState), TRC_SUCCESS);

	assert_int_equal_cs(uxState, 0x12345678);

	assert_int_equal_cs(xTraceObjectSetStateWithoutHandle(pvObject, 0x87654321), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetState(xEntryHandle, 0, &uxState), TRC_SUCCESS);

	assert_int_equal_cs(uxState, 0x87654321);
}

void test_xTraceObjectSetSpecificState(void** ppState)
{
	void* pvObject = (void*)0x12345678;
	TraceEntryHandle_t xEntryHandle;
	TraceObjectHandle_t xObjectHandle;
	TraceUnsignedBaseType_t uxState;

	assert_int_equal_asserts_enabled(xTraceObjectSetSpecificState(0, 1, 0x87654321), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceObjectRegister(1, pvObject, "TEST", 0x12345678, &xObjectHandle), TRC_SUCCESS);

	assert_int_equal_cs(xTraceObjectSetSpecificState(xObjectHandle, -1, 0x87654321), TRC_FAIL); /* index out of range */

	assert_int_equal_cs(xTraceObjectSetSpecificState(xObjectHandle, 0xFFFF, 0x87654321), TRC_FAIL); /* index out of range */

	assert_int_equal_cs(xTraceEntryFind(pvObject, &xEntryHandle), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetState(xEntryHandle, 1, &uxState), TRC_SUCCESS);

	assert_int_equal_cs(uxState, 0);

	assert_int_equal_cs(xTraceObjectSetSpecificState(xObjectHandle, 1, 0x87654321), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetState(xEntryHandle, 1, &uxState), TRC_SUCCESS);

	assert_int_equal_cs(uxState, 0x87654321);


	assert_int_equal_cs(xTraceEntryGetState(xEntryHandle, 2, &uxState), TRC_SUCCESS);

	assert_int_equal_cs(uxState, 0);
	
	assert_int_equal_cs(xTraceObjectSetSpecificState(xObjectHandle, 2, 0x88888888), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetState(xEntryHandle, 2, &uxState), TRC_SUCCESS);

	assert_int_equal_cs(uxState, 0x88888888);
}

void test_xTraceObjectSetSpecificStateWithoutHandle(void** ppState)
{
	void* pvObject = (void*)0x12345678;
	TraceEntryHandle_t xEntryHandle;
	TraceUnsignedBaseType_t uxState;

	assert_int_equal_asserts_enabled(xTraceObjectSetSpecificStateWithoutHandle(0, 1, 0x87654321), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceObjectRegisterWithoutHandle(1, pvObject, "TEST", 0x12345678), TRC_SUCCESS);
	
	assert_int_equal_cs(xTraceObjectSetSpecificStateWithoutHandle(pvObject, 1, 0x87654321), TRC_FAIL); /* not created yet */

	assert_int_equal_cs(xTraceObjectSetSpecificStateWithoutHandle(pvObject, -1, 0x87654321), TRC_FAIL); /* index out of range */

	assert_int_equal_cs(xTraceObjectSetSpecificStateWithoutHandle(pvObject, 0xFFFF, 0x87654321), TRC_FAIL); /* index out of range */

	assert_int_equal_cs(xTraceEntryFind(pvObject, &xEntryHandle), TRC_SUCCESS);

	
	assert_int_equal_cs(xTraceEntryGetState(xEntryHandle, 1, &uxState), TRC_SUCCESS);

	assert_int_equal_cs(uxState, 0);

	assert_int_equal_cs(xTraceObjectSetSpecificStateWithoutHandle(pvObject, 1, 0x87654321), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetState(xEntryHandle, 1, &uxState), TRC_SUCCESS);

	assert_int_equal_cs(uxState, 0x87654321);

	
	assert_int_equal_cs(xTraceEntryGetState(xEntryHandle, 2, &uxState), TRC_SUCCESS);

	assert_int_equal_cs(uxState, 0);

	assert_int_equal_cs(xTraceObjectSetSpecificStateWithoutHandle(pvObject, 2, 0x88888888), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetState(xEntryHandle, 2, &uxState), TRC_SUCCESS);

	assert_int_equal_cs(uxState, 0x88888888);
}

void test_xTraceObjectSetOptions(void** ppState)
{
	void* pvObject = (void*)0x12345678;
	TraceEntryHandle_t xEntryHandle;
	TraceObjectHandle_t xObjectHandle;
	uint32_t uiMask;
	
	assert_int_equal_asserts_enabled(xTraceObjectSetOptions(0, 0x87654321), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceObjectRegister(1, pvObject, "TEST", 0x12345678, &xObjectHandle), TRC_SUCCESS);

	assert_int_equal_cs(xTraceObjectSetOptions(xObjectHandle, 0x87654321), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryFind(pvObject, &xEntryHandle), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetOptions(xEntryHandle, &uiMask), TRC_SUCCESS);

	assert_int_equal_cs(uiMask, 0x87654321);

	assert_int_equal_cs(xTraceEntryClearOptions(xEntryHandle, uiMask), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetOptions(xEntryHandle, &uiMask), TRC_SUCCESS);

	assert_int_equal_cs(uiMask, 0);

	assert_int_equal_cs(xTraceObjectSetOptions(xObjectHandle, 0x12345678), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetOptions(xEntryHandle, &uiMask), TRC_SUCCESS);

	assert_int_equal_cs(uiMask, 0x12345678);

	assert_int_equal_cs(xTraceEntryClearOptions(xEntryHandle, uiMask), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetOptions(xEntryHandle, &uiMask), TRC_SUCCESS);

	assert_int_equal_cs(uiMask, 0);
}

void test_xTraceObjectSetOptionsWithoutHandle(void** ppState)
{
	void* pvObject = (void*)0x12345678;
	TraceEntryHandle_t xEntryHandle;
	uint32_t uiMask;

	assert_int_equal_asserts_enabled(xTraceObjectSetOptionsWithoutHandle(0, 0x87654321), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceObjectSetOptionsWithoutHandle(pvObject, 0x87654321), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryFind(pvObject, &xEntryHandle), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetOptions(xEntryHandle, &uiMask), TRC_SUCCESS);

	assert_int_equal_cs(uiMask, 0x87654321);

	assert_int_equal_cs(xTraceEntryClearOptions(xEntryHandle, uiMask), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetOptions(xEntryHandle, &uiMask), TRC_SUCCESS);

	assert_int_equal_cs(uiMask, 0);

	assert_int_equal_cs(xTraceObjectSetOptionsWithoutHandle(pvObject, 0x12345678), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetOptions(xEntryHandle, &uiMask), TRC_SUCCESS);

	assert_int_equal_cs(uiMask, 0x12345678);

	assert_int_equal_cs(xTraceEntryClearOptions(xEntryHandle, uiMask), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetOptions(xEntryHandle, &uiMask), TRC_SUCCESS);

	assert_int_equal_cs(uiMask, 0);
}

uint32_t runObjectTests(void)
{
	uint32_t uiCountFailedTests = 0;

	const struct CMUnitTest aTests_trcObject[] =
	{
		cmocka_unit_test_setup_teardown(test_xTraceObjectCreate, setup_Object, teardown_Object),
		cmocka_unit_test_setup_teardown(test_xTraceObjectCreateWithoutHandle, setup_Object, teardown_Object),
		cmocka_unit_test_setup_teardown(test_xTraceObjectDelete, setup_Object, teardown_Object),
		cmocka_unit_test_setup_teardown(test_xTraceObjectDeleteWithoutHandle, setup_Object, teardown_Object),
		cmocka_unit_test_setup_teardown(test_xTraceObjectSetName, setup_Object, teardown_Object),
		cmocka_unit_test_setup_teardown(test_xTraceObjectSetNameWithoutHandle, setup_Object, teardown_Object),
		cmocka_unit_test_setup_teardown(test_xTraceObjectSetState, setup_Object, teardown_Object),
		cmocka_unit_test_setup_teardown(test_xTraceObjectSetStateWithoutHandle, setup_Object, teardown_Object),
		cmocka_unit_test_setup_teardown(test_xTraceObjectSetOptions, setup_Object, teardown_Object),
		cmocka_unit_test_setup_teardown(test_xTraceObjectSetOptionsWithoutHandle, setup_Object, teardown_Object),
	};

	uiCountFailedTests += cmocka_run_group_tests(aTests_trcObject, group_setup_Object, group_teardown_Object);

	return uiCountFailedTests;
}
