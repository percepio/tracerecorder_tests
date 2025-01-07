#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <Diagnostics_UnitTest.h>
#include <trcRecorder.h>

int group_setup_Diagnostics(void** ppState);
int group_teardown_Diagnostics(void** ppState);
int setup_Diagnostics(void** ppState);
int teardown_Diagnostics(void** ppState);

void test_xTraceDiagnosticsInitialize(void** ppState);
void test_xTraceDiagnosticsGetSet(void** ppState);
void test_xTraceDiagnosticsSetIfHigherLower(void** ppState);
void test_xTraceDiagnosticsIncreaseDecrease(void** ppState);
void test_xTraceDiagnosticsCheckStatus(void** ppState);

/******************************************************************************************
*                                        TEST STATE                                       *
******************************************************************************************/

typedef struct DiagnosticsTestState
{
	/* Buffer */
	TraceDiagnosticsData_t xDiagnosticsBuffer;
} DiagnosticsTestState_t;

DiagnosticsTestState_t xDiagnosticsTestState;

uint32_t uiTestMaxBytesTruncated = 0;
uint32_t MOCK_uiStackMonitorNoSlots = 0;
volatile uint32_t MOCK_RecorderInitialized = 1;
uint32_t MOCK_uiCriticalSection = 0;
uint32_t MOCK_uiCriticalSectionExpectedValue = 0;
uint8_t MOCK_tmpBuffer[TRC_MAX_BLOB_SIZE];
uint8_t MOCK_streamPortBuffer[TRC_MAX_BLOB_SIZE];
uint32_t MOCK_streamPortMaxTransferSize = 0xFFFFFFFF;

/******************************************************************************************
*                                        SETUP / TEARDOWN                                 *
******************************************************************************************/

int group_setup_Diagnostics(void **ppState)
{
	*ppState = &xDiagnosticsTestState;

	assert_int_equal(sizeof(TraceUnsignedBaseType_t), sizeof(void*));
	assert_int_equal(sizeof(TraceUnsignedBaseType_t), sizeof(TraceBaseType_t));

	return 0;
}

int group_teardown_Diagnostics(void **ppState)
{
	DiagnosticsTestState_t* pxState = (DiagnosticsTestState_t*)*ppState;

	return 0;
}

int setup_Diagnostics(void **ppState)
{
	DiagnosticsTestState_t* pxState = (DiagnosticsTestState_t*)*ppState;

	assert_int_equal_cs(xTraceDiagnosticsInitialize(&pxState->xDiagnosticsBuffer), TRC_SUCCESS);

	return 0;
}

int teardown_Diagnostics(void **ppState)
{
	DiagnosticsTestState_t* pxState = (DiagnosticsTestState_t*)*ppState;

	return 0;
}

/******************************************************************************************
*                                        TESTS                                            *
******************************************************************************************/

void test_xTraceDiagnosticsInitialize(void **ppState)
{
	DiagnosticsTestState_t* pxState = (DiagnosticsTestState_t*)*ppState;

	assert_int_equal_cs(xTraceDiagnosticsInitialize(0), TRC_FAIL);
	assert_int_equal_cs(xTraceDiagnosticsInitialize(&pxState->xDiagnosticsBuffer), TRC_SUCCESS);
}

void test_xTraceDiagnosticsGetSet(void **ppState)
{
	DiagnosticsTestState_t* pxState = (DiagnosticsTestState_t*)*ppState;
	TraceUnsignedBaseType_t uxValue = 0;

	assert_int_equal_cs(xTraceDiagnosticsGet((TraceDiagnosticsType_t)-1, &uxValue), TRC_FAIL); /* index out of range */
	assert_int_equal_cs(xTraceDiagnosticsGet((TraceDiagnosticsType_t)TRC_DIAGNOSTICS_COUNT, &uxValue), TRC_FAIL); /* index out of range */

	assert_int_equal_cs(xTraceDiagnosticsSet((TraceDiagnosticsType_t)-1, 1), TRC_FAIL); /* index out of range */
	assert_int_equal_cs(xTraceDiagnosticsSet((TraceDiagnosticsType_t)TRC_DIAGNOSTICS_COUNT, 1), TRC_FAIL); /* index out of range */

	assert_int_equal_cs(xTraceDiagnosticsGet(TRC_DIAGNOSTICS_BLOB_MAX_BYTES_TRUNCATED, &uxValue), TRC_SUCCESS);
	assert_int_equal_cs(uxValue, 0);
	
	assert_int_equal_cs(xTraceDiagnosticsSet(TRC_DIAGNOSTICS_BLOB_MAX_BYTES_TRUNCATED, 1), TRC_SUCCESS);
	assert_int_equal_cs(xTraceDiagnosticsGet(TRC_DIAGNOSTICS_BLOB_MAX_BYTES_TRUNCATED, &uxValue), TRC_SUCCESS);
	assert_int_equal_cs(uxValue, 1);

	assert_int_equal_cs(xTraceDiagnosticsSet(TRC_DIAGNOSTICS_ENTRY_SYMBOL_LONGEST_LENGTH, 2), TRC_SUCCESS);
	assert_int_equal_cs(xTraceDiagnosticsGet(TRC_DIAGNOSTICS_ENTRY_SYMBOL_LONGEST_LENGTH, &uxValue), TRC_SUCCESS);
	assert_int_equal_cs(uxValue, 2);

	assert_int_equal_cs(xTraceDiagnosticsGet(TRC_DIAGNOSTICS_BLOB_MAX_BYTES_TRUNCATED, &uxValue), TRC_SUCCESS); /* Make sure we didn't change the wrong value */
	assert_int_equal_cs(uxValue, 1);
}

void test_xTraceDiagnosticsSetIfHigherLower(void** ppState)
{
	DiagnosticsTestState_t* pxState = (DiagnosticsTestState_t*)*ppState;
	TraceUnsignedBaseType_t uxValue = 0;

	assert_int_equal_cs(xTraceDiagnosticsSetIfHigher((TraceDiagnosticsType_t)-1, 1), TRC_FAIL); /* index out of range */
	assert_int_equal_cs(xTraceDiagnosticsSetIfHigher((TraceDiagnosticsType_t)TRC_DIAGNOSTICS_COUNT, 1), TRC_FAIL); /* index out of range */

	assert_int_equal_cs(xTraceDiagnosticsSetIfLower((TraceDiagnosticsType_t)-1, 1), TRC_FAIL); /* index out of range */
	assert_int_equal_cs(xTraceDiagnosticsSetIfLower((TraceDiagnosticsType_t)TRC_DIAGNOSTICS_COUNT, 1), TRC_FAIL); /* index out of range */

	assert_int_equal_cs(xTraceDiagnosticsGet(TRC_DIAGNOSTICS_ENTRY_SYMBOL_LONGEST_LENGTH, &uxValue), TRC_SUCCESS);
	assert_int_equal_cs(uxValue, 0);

	assert_int_equal_cs(xTraceDiagnosticsSetIfHigher(TRC_DIAGNOSTICS_ENTRY_SYMBOL_LONGEST_LENGTH, 5), TRC_SUCCESS);
	assert_int_equal_cs(xTraceDiagnosticsGet(TRC_DIAGNOSTICS_ENTRY_SYMBOL_LONGEST_LENGTH, &uxValue), TRC_SUCCESS);
	assert_int_equal_cs(uxValue, 5);

	assert_int_equal_cs(xTraceDiagnosticsSetIfHigher(TRC_DIAGNOSTICS_ENTRY_SYMBOL_LONGEST_LENGTH, 3), TRC_SUCCESS);
	assert_int_equal_cs(xTraceDiagnosticsGet(TRC_DIAGNOSTICS_ENTRY_SYMBOL_LONGEST_LENGTH, &uxValue), TRC_SUCCESS);
	assert_int_equal_cs(uxValue, 5);

	assert_int_equal_cs(xTraceDiagnosticsSetIfLower(TRC_DIAGNOSTICS_ENTRY_SYMBOL_LONGEST_LENGTH, 3), TRC_SUCCESS);
	assert_int_equal_cs(xTraceDiagnosticsGet(TRC_DIAGNOSTICS_ENTRY_SYMBOL_LONGEST_LENGTH, &uxValue), TRC_SUCCESS);
	assert_int_equal_cs(uxValue, 3);

	assert_int_equal_cs(xTraceDiagnosticsSetIfLower(TRC_DIAGNOSTICS_ENTRY_SYMBOL_LONGEST_LENGTH, 5), TRC_SUCCESS);
	assert_int_equal_cs(xTraceDiagnosticsGet(TRC_DIAGNOSTICS_ENTRY_SYMBOL_LONGEST_LENGTH, &uxValue), TRC_SUCCESS);
	assert_int_equal_cs(uxValue, 3);
}

void test_xTraceDiagnosticsIncreaseDecrease(void** ppState)
{
	DiagnosticsTestState_t* pxState = (DiagnosticsTestState_t*)*ppState;
	TraceUnsignedBaseType_t uxValue = 0;

	assert_int_equal_cs(xTraceDiagnosticsIncrease((TraceDiagnosticsType_t)-1), TRC_FAIL); /* index out of range */
	assert_int_equal_cs(xTraceDiagnosticsIncrease((TraceDiagnosticsType_t)TRC_DIAGNOSTICS_COUNT), TRC_FAIL); /* index out of range */

	assert_int_equal_cs(xTraceDiagnosticsDecrease((TraceDiagnosticsType_t)-1), TRC_FAIL); /* index out of range */
	assert_int_equal_cs(xTraceDiagnosticsDecrease((TraceDiagnosticsType_t)TRC_DIAGNOSTICS_COUNT), TRC_FAIL); /* index out of range */

	assert_int_equal_cs(xTraceDiagnosticsGet(TRC_DIAGNOSTICS_ENTRY_SYMBOL_LONGEST_LENGTH, &uxValue), TRC_SUCCESS);
	assert_int_equal_cs(uxValue, 0);
	
	assert_int_equal_cs(xTraceDiagnosticsIncrease(TRC_DIAGNOSTICS_ENTRY_SYMBOL_LONGEST_LENGTH), TRC_SUCCESS);

	assert_int_equal_cs(xTraceDiagnosticsGet(TRC_DIAGNOSTICS_ENTRY_SYMBOL_LONGEST_LENGTH, &uxValue), TRC_SUCCESS);
	assert_int_equal_cs(uxValue, 1);
	
	assert_int_equal_cs(xTraceDiagnosticsIncrease(TRC_DIAGNOSTICS_ENTRY_SYMBOL_LONGEST_LENGTH), TRC_SUCCESS);

	assert_int_equal_cs(xTraceDiagnosticsGet(TRC_DIAGNOSTICS_ENTRY_SYMBOL_LONGEST_LENGTH, &uxValue), TRC_SUCCESS);
	assert_int_equal_cs(uxValue, 2);

	assert_int_equal_cs(xTraceDiagnosticsIncrease(TRC_DIAGNOSTICS_ENTRY_SYMBOL_LONGEST_LENGTH), TRC_SUCCESS);

	assert_int_equal_cs(xTraceDiagnosticsGet(TRC_DIAGNOSTICS_ENTRY_SYMBOL_LONGEST_LENGTH, &uxValue), TRC_SUCCESS);
	assert_int_equal_cs(uxValue, 3);

	assert_int_equal_cs(xTraceDiagnosticsIncrease(TRC_DIAGNOSTICS_ENTRY_SYMBOL_LONGEST_LENGTH), TRC_SUCCESS);

	assert_int_equal_cs(xTraceDiagnosticsGet(TRC_DIAGNOSTICS_ENTRY_SYMBOL_LONGEST_LENGTH, &uxValue), TRC_SUCCESS);
	assert_int_equal_cs(uxValue, 4);

	assert_int_equal_cs(xTraceDiagnosticsIncrease(TRC_DIAGNOSTICS_ENTRY_SYMBOL_LONGEST_LENGTH), TRC_SUCCESS);

	assert_int_equal_cs(xTraceDiagnosticsGet(TRC_DIAGNOSTICS_ENTRY_SYMBOL_LONGEST_LENGTH, &uxValue), TRC_SUCCESS);
	assert_int_equal_cs(uxValue, 5);

	assert_int_equal_cs(xTraceDiagnosticsDecrease(TRC_DIAGNOSTICS_ENTRY_SYMBOL_LONGEST_LENGTH), TRC_SUCCESS);

	assert_int_equal_cs(xTraceDiagnosticsGet(TRC_DIAGNOSTICS_ENTRY_SYMBOL_LONGEST_LENGTH, &uxValue), TRC_SUCCESS);
	assert_int_equal_cs(uxValue, 4);

	assert_int_equal_cs(xTraceDiagnosticsDecrease(TRC_DIAGNOSTICS_ENTRY_SYMBOL_LONGEST_LENGTH), TRC_SUCCESS);

	assert_int_equal_cs(xTraceDiagnosticsGet(TRC_DIAGNOSTICS_ENTRY_SYMBOL_LONGEST_LENGTH, &uxValue), TRC_SUCCESS);
	assert_int_equal_cs(uxValue, 3);

	assert_int_equal_cs(xTraceDiagnosticsDecrease(TRC_DIAGNOSTICS_ENTRY_SYMBOL_LONGEST_LENGTH), TRC_SUCCESS);

	assert_int_equal_cs(xTraceDiagnosticsGet(TRC_DIAGNOSTICS_ENTRY_SYMBOL_LONGEST_LENGTH, &uxValue), TRC_SUCCESS);
	assert_int_equal_cs(uxValue, 2);

	assert_int_equal_cs(xTraceDiagnosticsIncrease(TRC_DIAGNOSTICS_ENTRY_SYMBOL_LONGEST_LENGTH), TRC_SUCCESS);

	assert_int_equal_cs(xTraceDiagnosticsGet(TRC_DIAGNOSTICS_ENTRY_SYMBOL_LONGEST_LENGTH, &uxValue), TRC_SUCCESS);
	assert_int_equal_cs(uxValue, 3);

	assert_int_equal_cs(xTraceDiagnosticsDecrease(TRC_DIAGNOSTICS_ENTRY_SYMBOL_LONGEST_LENGTH), TRC_SUCCESS);

	assert_int_equal_cs(xTraceDiagnosticsGet(TRC_DIAGNOSTICS_ENTRY_SYMBOL_LONGEST_LENGTH, &uxValue), TRC_SUCCESS);
	assert_int_equal_cs(uxValue, 2);

	assert_int_equal_cs(xTraceDiagnosticsDecrease(TRC_DIAGNOSTICS_ENTRY_SYMBOL_LONGEST_LENGTH), TRC_SUCCESS);

	assert_int_equal_cs(xTraceDiagnosticsGet(TRC_DIAGNOSTICS_ENTRY_SYMBOL_LONGEST_LENGTH, &uxValue), TRC_SUCCESS);
	assert_int_equal_cs(uxValue, 1);
}

void test_xTraceDiagnosticsCheckStatus(void** ppState)
{
	DiagnosticsTestState_t* pxState = (DiagnosticsTestState_t*)*ppState;
	
	assert_int_equal_cs(xTraceDiagnosticsCheckStatus(), TRC_SUCCESS);
}

uint32_t runDiagnosticsTests(void)
{
	uint32_t uiCountFailedTests = 0;

	const struct CMUnitTest aTests_trcDiagnostics[] =
	{
		cmocka_unit_test_setup_teardown(test_xTraceDiagnosticsInitialize, setup_Diagnostics, teardown_Diagnostics),
		cmocka_unit_test_setup_teardown(test_xTraceDiagnosticsGetSet, setup_Diagnostics, teardown_Diagnostics),
		cmocka_unit_test_setup_teardown(test_xTraceDiagnosticsCheckStatus, setup_Diagnostics, teardown_Diagnostics),
		cmocka_unit_test_setup_teardown(test_xTraceDiagnosticsSetIfHigherLower, setup_Diagnostics, teardown_Diagnostics),
		cmocka_unit_test_setup_teardown(test_xTraceDiagnosticsIncreaseDecrease, setup_Diagnostics, teardown_Diagnostics),
	};

	uiCountFailedTests += cmocka_run_group_tests(aTests_trcDiagnostics, group_setup_Diagnostics, group_teardown_Diagnostics);

	return uiCountFailedTests;
}
