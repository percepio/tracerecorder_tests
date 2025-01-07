#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <EntryTable_UnitTest.h>
#include <trcRecorder.h>

int group_setup_EntryTable(void** ppState);
int group_teardown_EntryTable(void** ppState);
int setup_emptyEntryTable(void** ppState);
int setup_entryTableWithOneEntry(void** ppState);
int teardown_entryTable(void** ppState);

void test_xTraceEntryTableInitialize(void** ppState);
void test_xTraceEntryCreate(void** ppState);
void test_xTraceEntryCreateWithAddress(void** ppState);
void test_xTraceEntryDelete(void** ppState);
void test_xTraceEntryGetAddress(void** ppState);
void test_xTraceEntrySetGetState(void** ppState);
void test_xTraceEntrySetClearGetOptions(void** ppState);
void test_xTraceEntrySetGetSymbol(void** ppState);
void test_xTraceEntryGetEntryAtIndex(void** ppState);
void test_xTraceEntryFind(void** ppState);
void test_xTraceEntryCount(void** ppState);
void test_xTraceDiagnosticsGetSet(void** ppState);

/******************************************************************************************
*                                        TEST STATE                                       *
******************************************************************************************/

typedef struct EntryTableTestState
{
	/* Buffer */
	TraceEntryTable_t xBuffer;
	TraceEntryIndexTable_t xIndexBuffer;
	TraceDiagnosticsData_t xDiagnosticsBuffer;

	/* Test Objects */
	TraceEntryHandle_t xEntryHandle;
} EntryTableTestState_t;

EntryTableTestState_t xEntryTableTestState;
volatile uint32_t MOCK_RecorderInitialized = 1;
uint32_t MOCK_uiCriticalSection = 0;
uint32_t MOCK_uiCriticalSectionExpectedValue = 0;
uint8_t MOCK_tmpBuffer[TRC_MAX_BLOB_SIZE];
uint8_t MOCK_streamPortBuffer[TRC_MAX_BLOB_SIZE];
uint32_t MOCK_streamPortMaxTransferSize = 0xFFFFFFFF;

/******************************************************************************************
*                                        SETUP / TEARDOWN                                 *
******************************************************************************************/

int group_setup_EntryTable(void **ppState)
{
	*ppState = &xEntryTableTestState;

	assert_int_equal(sizeof(TraceUnsignedBaseType_t), sizeof(void*));
	assert_int_equal(sizeof(TraceUnsignedBaseType_t), sizeof(TraceBaseType_t));

	return 0;
}

int group_teardown_EntryTable(void **ppState)
{
	EntryTableTestState_t* pxState = (EntryTableTestState_t*)*ppState;

	return 0;
}

int setup_emptyEntryTable(void **ppState)
{
	EntryTableTestState_t* pxState = (EntryTableTestState_t*)*ppState;

	assert_true(TRC_ENTRY_TABLE_SLOTS > 1);

	assert_int_equal_cs(xTraceEntryTableInitialize(&pxState->xBuffer), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEntryIndexTableInitialize(&pxState->xIndexBuffer), TRC_SUCCESS);
	assert_int_equal_cs(xTraceDiagnosticsInitialize(&pxState->xDiagnosticsBuffer), TRC_SUCCESS);

	pxState->xEntryHandle = 0;

	return 0;
}

int setup_entryTableWithOneEntry(void **ppState)
{
	void *pvAddress;
	TraceUnsignedBaseType_t uxState;
	uint32_t i;
	const char *szSymbol = 0;
	EntryTableTestState_t* pxState = (EntryTableTestState_t*)*ppState;

	assert_true(TRC_ENTRY_TABLE_SLOTS > 0);

	assert_int_equal_cs(setup_emptyEntryTable(ppState), 0);

	assert_int_equal_cs(xTraceEntryCreate(&pxState->xEntryHandle), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEntryGetAddress(pxState->xEntryHandle, &pvAddress), TRC_SUCCESS);
	assert_int_equal_cs((TraceUnsignedBaseType_t)pvAddress, (TraceUnsignedBaseType_t)pxState->xEntryHandle); /* Temporary address has been set */
	for (i = 0; i < TRC_ENTRY_TABLE_STATE_COUNT; i++)
	{
		assert_int_equal_cs(xTraceEntryGetState(pxState->xEntryHandle, i, &uxState), TRC_SUCCESS);
		assert_int_equal_cs(uxState, 0);
	}
	assert_int_equal_cs(xTraceEntryGetSymbol(pxState->xEntryHandle, &szSymbol), TRC_SUCCESS);
	assert_string_equal(szSymbol, "");

	return 0;
}

int teardown_entryTable(void **ppState)
{
	EntryTableTestState_t* pxState = (EntryTableTestState_t*)*ppState;

	pxState->xEntryHandle = 0;

	return 0;
}

/******************************************************************************************
*                                        TESTS                                            *
******************************************************************************************/

void test_xTraceEntryTableInitialize(void **ppState)
{
	EntryTableTestState_t* pxState = (EntryTableTestState_t*)*ppState;

	assert_int_equal_asserts_enabled(xTraceEntryTableInitialize(0), TRC_FAIL); /* null test */
	assert_int_equal_asserts_enabled(xTraceEntryIndexTableInitialize(0), TRC_FAIL);
	assert_int_equal_cs(xTraceEntryTableInitialize(&pxState->xBuffer), TRC_SUCCESS);
}

void test_xTraceEntryCreate(void **ppState)
{
	void *pvAddress;
	TraceUnsignedBaseType_t uxState;
	const char *szSymbol = 0;
	int i;
	EntryTableTestState_t* pxState = (EntryTableTestState_t*)*ppState;

	assert_int_equal_asserts_enabled(xTraceEntryCreate(0), TRC_FAIL); /* null test */

	for (i = 0; i < TRC_ENTRY_TABLE_SLOTS; i++) /* fill up entire table */
	{
		assert_int_equal_cs(xTraceEntryCreate(&pxState->xEntryHandle), TRC_SUCCESS);
	}

	assert_int_equal_cs(xTraceEntryCreate(&pxState->xEntryHandle), TRC_FAIL); /* no slots left */

	assert_int_equal_cs(xTraceEntryGetAddress(pxState->xEntryHandle, &pvAddress), TRC_SUCCESS);
	assert_int_equal_cs((TraceUnsignedBaseType_t)pvAddress, (TraceUnsignedBaseType_t)pxState->xEntryHandle); /* Temporary address has been set */
	for (i = 0; i < TRC_ENTRY_TABLE_STATE_COUNT; i++)
	{
		assert_int_equal_cs(xTraceEntryGetState(pxState->xEntryHandle, i, &uxState), TRC_SUCCESS);
		assert_int_equal_cs(uxState, 0);
	}
	assert_int_equal_cs(xTraceEntryGetSymbol(pxState->xEntryHandle, &szSymbol), TRC_SUCCESS);
	assert_string_equal(szSymbol, "");
}

void test_xTraceEntryCreateWithAddress(void** ppState)
{
	void* pvAddress;
	TraceUnsignedBaseType_t uxState;
	const char* szSymbol = 0;
	int i;
	EntryTableTestState_t* pxState = (EntryTableTestState_t*)*ppState;

	assert_int_equal_asserts_enabled(xTraceEntryCreateWithAddress(0, &pxState->xEntryHandle), TRC_FAIL); /* null test */

	assert_int_equal_asserts_enabled(xTraceEntryCreateWithAddress((void*)0x12345678, 0), TRC_FAIL); /* null test */

	for (i = 0; i < TRC_ENTRY_TABLE_SLOTS; i++) /* fill up entire table */
	{
		assert_int_equal_cs(xTraceEntryCreateWithAddress((void*)(0x12345678 + i * sizeof(TraceUnsignedBaseType_t)), &pxState->xEntryHandle), TRC_SUCCESS);
		
		assert_int_equal_cs(xTraceEntryGetAddress(pxState->xEntryHandle, &pvAddress), TRC_SUCCESS);
		assert_int_equal_cs((TraceUnsignedBaseType_t)(0x12345678 + i * sizeof(TraceUnsignedBaseType_t)), (TraceUnsignedBaseType_t)pvAddress);
	}

	assert_int_equal_cs(xTraceEntryCreateWithAddress((void*)0x12345678 , &pxState->xEntryHandle), TRC_FAIL); /* no slots left */

	for (i = 0; i < TRC_ENTRY_TABLE_STATE_COUNT; i++)
	{
		assert_int_equal_cs(xTraceEntryGetState(pxState->xEntryHandle, i, &uxState), TRC_SUCCESS);
		assert_int_equal_cs(uxState, 0);
	}
	
	assert_int_equal_cs(xTraceEntryGetSymbol(pxState->xEntryHandle, &szSymbol), TRC_SUCCESS);
	assert_string_equal(szSymbol, "");
}

void test_xTraceEntryDelete(void **ppState)
{
	EntryTableTestState_t* pxState = (EntryTableTestState_t*)*ppState;
	/* Make sure the single entry that setup created can be deleted */

	assert_int_equal_asserts_enabled(xTraceEntryDelete(0), TRC_FAIL); /* null test */

	assert_int_equal_asserts_enabled(xTraceEntryDelete((TraceEntryHandle_t)(TraceUnsignedBaseType_t)0xDEADBEEF), TRC_FAIL); /* "random" value */
	assert_int_equal_asserts_enabled(xTraceEntryDelete((TraceEntryHandle_t)((TraceUnsignedBaseType_t)&pxState->xBuffer - 4)), TRC_FAIL); /* 4 bytes before table start */
	assert_int_equal_asserts_enabled(xTraceEntryDelete((TraceEntryHandle_t)((TraceUnsignedBaseType_t)&pxState->xBuffer + sizeof(TraceUnsignedBaseType_t) * 2)), TRC_FAIL); /* Two TraceUnsignedBaseType_t after table start, still outside entries */
	assert_int_equal_asserts_enabled(xTraceEntryDelete((TraceEntryHandle_t)((TraceUnsignedBaseType_t)&pxState->xBuffer + sizeof(pxState->xBuffer))), TRC_FAIL); /* at end of table */
	assert_int_equal_cs(xTraceEntryDelete(pxState->xEntryHandle), TRC_SUCCESS);
}

void test_xTraceEntryGetAddress(void **ppState)
{
	/* Make sure the entry that setup created can be modified */
	void *pvTestAddress = (void*)(TraceUnsignedBaseType_t)0xDEADBEEF;
	void *pvAddress;
	TraceEntryHandle_t xEntryHandle;
	EntryTableTestState_t* pxState = (EntryTableTestState_t*)*ppState;

	assert_int_equal_asserts_enabled(xTraceEntryGetAddress(0, &pvAddress), TRC_FAIL); /* null test */
	assert_int_equal_asserts_enabled(xTraceEntryGetAddress(pxState->xEntryHandle, 0), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceEntryGetAddress(pxState->xEntryHandle, &pvAddress), TRC_SUCCESS);
	assert_int_equal_cs((TraceUnsignedBaseType_t)pvAddress, (TraceUnsignedBaseType_t)pxState->xEntryHandle); /* Temporary address has been set */

	assert_int_equal_cs(xTraceEntryCreateWithAddress(pvTestAddress, &xEntryHandle), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEntryGetAddress(xEntryHandle, &pvAddress), TRC_SUCCESS);
	assert_int_equal_cs((TraceUnsignedBaseType_t)pvAddress, (TraceUnsignedBaseType_t)pvTestAddress);
}

void test_xTraceEntrySetGetState(void **ppState)
{
	/* Make sure the entry that setup created can be modified */
	TraceUnsignedBaseType_t uxTestState = 0xDEADBEEF;
	TraceUnsignedBaseType_t uxState, i;
	EntryTableTestState_t* pxState = (EntryTableTestState_t*)*ppState;

	assert_int_equal_asserts_enabled(xTraceEntryGetState(0, 0, &uxState), TRC_FAIL); /* null test */
	assert_int_equal_asserts_enabled(xTraceEntryGetState(pxState->xEntryHandle, 0, 0), TRC_FAIL); /* null test */
	assert_int_equal_asserts_enabled(xTraceEntryGetState(pxState->xEntryHandle, -1, 0), TRC_FAIL); /* negative index test */
	assert_int_equal_asserts_enabled(xTraceEntryGetState(pxState->xEntryHandle, TRC_ENTRY_TABLE_STATE_COUNT, 0), TRC_FAIL); /* out of bounds index test */

	assert_int_equal_asserts_enabled(xTraceEntrySetState(0, 0, uxTestState), TRC_FAIL); /* null test */
	assert_int_equal_asserts_enabled(xTraceEntrySetState(pxState->xEntryHandle, -1, uxTestState), TRC_FAIL); /* negative index test */
	assert_int_equal_asserts_enabled(xTraceEntrySetState(pxState->xEntryHandle, TRC_ENTRY_TABLE_STATE_COUNT, uxTestState), TRC_FAIL); /* out of bounds index test */

	for (i = 0; i < TRC_ENTRY_TABLE_STATE_COUNT; i++)
	{
		assert_int_equal_cs(xTraceEntryGetState(pxState->xEntryHandle, i, &uxState), TRC_SUCCESS);
		assert_int_equal_cs(uxState, 0);

		uxState = xTraceEntryGetStateReturn(pxState->xEntryHandle, i);
		assert_int_equal_cs(uxState, 0);
	}

	for (i = 0; i < TRC_ENTRY_TABLE_STATE_COUNT; i++)
	{
		assert_int_equal_cs(xTraceEntrySetState(pxState->xEntryHandle, i, uxTestState + i), TRC_SUCCESS);
	}

	for (i = 0; i < TRC_ENTRY_TABLE_STATE_COUNT; i++)
	{
		assert_int_equal_cs(xTraceEntryGetState(pxState->xEntryHandle, i, &uxState), TRC_SUCCESS);
		assert_int_equal_cs(uxState, (uint64_t)uxTestState + (TraceUnsignedBaseType_t)i);

		uxState = xTraceEntryGetStateReturn(pxState->xEntryHandle, i);
		assert_int_equal_cs(uxState, (uint64_t)uxTestState + (TraceUnsignedBaseType_t)i);
	}
}

void test_xTraceEntrySetClearGetOptions(void **ppState)
{
	/* Make sure the entry that setup created can be modified */
	uint32_t uiOptions;
	EntryTableTestState_t* pxState = (EntryTableTestState_t*)*ppState;

	assert_int_equal_asserts_enabled(xTraceEntrySetOptions(0, 0), TRC_FAIL); /* null test */
	assert_int_equal_asserts_enabled(xTraceEntryClearOptions(0, 0), TRC_FAIL); /* null test */
	assert_int_equal_asserts_enabled(xTraceEntryGetOptions(0, &uiOptions), TRC_FAIL); /* null test */
	assert_int_equal_asserts_enabled(xTraceEntryGetOptions(pxState->xEntryHandle, 0), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceEntryGetOptions(pxState->xEntryHandle, &uiOptions), TRC_SUCCESS);
	assert_int_equal_cs(uiOptions, 0x00000000);
	
	assert_int_equal_cs(xTraceEntrySetOptions(pxState->xEntryHandle, 0x00000001), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEntryGetOptions(pxState->xEntryHandle, &uiOptions), TRC_SUCCESS);
	assert_int_equal_cs(uiOptions, 0x00000001);

	assert_int_equal_cs(xTraceEntrySetOptions(pxState->xEntryHandle, 0x80080000), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEntryGetOptions(pxState->xEntryHandle, &uiOptions), TRC_SUCCESS);
	assert_int_equal_cs(uiOptions, 0x80080001);

	assert_int_equal_cs(xTraceEntryClearOptions(pxState->xEntryHandle, 0x00080000), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEntryGetOptions(pxState->xEntryHandle, &uiOptions), TRC_SUCCESS);
	assert_int_equal_cs(uiOptions, 0x80000001);

	assert_int_equal_cs(xTraceEntryClearOptions(pxState->xEntryHandle, 0x80080000), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEntryGetOptions(pxState->xEntryHandle, &uiOptions), TRC_SUCCESS);
	assert_int_equal_cs(uiOptions, 0x00000001);

	assert_int_equal_cs(xTraceEntryClearOptions(pxState->xEntryHandle, 0x00000001), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEntryGetOptions(pxState->xEntryHandle, &uiOptions), TRC_SUCCESS);
	assert_int_equal_cs(uiOptions, 0x00000000);
}

void test_xTraceEntrySetGetSymbol(void **ppState)
{
	const char *szTest = "TEST_NAME";
	char szTestTruncate[] = "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789";
	const char *szSymbol = 0;
	EntryTableTestState_t* pxState = (EntryTableTestState_t*)*ppState;

	assert_int_equal_asserts_enabled(xTraceEntryGetSymbol(0, &szSymbol), TRC_FAIL); /* null test */
	assert_int_equal_asserts_enabled(xTraceEntryGetSymbol(pxState->xEntryHandle, 0), TRC_FAIL); /* null test */

	assert_int_equal_asserts_enabled(xTraceEntrySetSymbol(0, szTest, strlen(szTest)), TRC_FAIL); /* null test */
	assert_int_equal_asserts_enabled(xTraceEntrySetSymbol(pxState->xEntryHandle, 0, strlen(szTest)), TRC_SUCCESS); /* null test */

	assert_int_equal_cs(xTraceEntrySetSymbol(pxState->xEntryHandle, szTest, strlen(szTest)), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEntryGetSymbol(pxState->xEntryHandle, &szSymbol), TRC_SUCCESS);
	assert_string_equal(szSymbol, szTest);

	assert_int_equal_cs(xTraceEntrySetSymbol(pxState->xEntryHandle, szTestTruncate, strlen(szTestTruncate)), TRC_SUCCESS);	/* test with too long test string */
	assert_int_equal_cs(xTraceEntryGetSymbol(pxState->xEntryHandle, &szSymbol), TRC_SUCCESS);
	assert_int_equal_cs(strlen(szSymbol), TRC_ENTRY_TABLE_SYMBOL_LENGTH);				/* ensure truncated */

	assert_string_not_equal(szSymbol, szTestTruncate);	/* szSymbol has been truncated */
	szTestTruncate[TRC_ENTRY_TABLE_SYMBOL_LENGTH] = 0;		/* truncate test string to max length */
	assert_string_equal(szSymbol, szTestTruncate);		/* test */

	assert_int_equal_cs(xTraceEntrySetSymbol(pxState->xEntryHandle, szTestTruncate, strlen(szTestTruncate)), TRC_SUCCESS); /* run test again with max length test string */
	assert_int_equal_cs(xTraceEntryGetSymbol(pxState->xEntryHandle, &szSymbol), TRC_SUCCESS);
	assert_string_equal(szSymbol, szTestTruncate);
}

void test_xTraceEntryGetEntryAtIndex(void **ppState)
{
	uint32_t i;
	uint32_t found = 0;
	void *pvAddress;
	EntryTableTestState_t* pxState = (EntryTableTestState_t*)*ppState;
	TraceEntryHandle_t xEntryHandle;
	
	assert_int_equal_asserts_enabled(xTraceEntryGetAtIndex(-1, 0), TRC_FAIL);	/* index out of range test */
	assert_int_equal_asserts_enabled(xTraceEntryGetAtIndex(TRC_ENTRY_TABLE_SLOTS, 0), TRC_FAIL);	/* index out of range test */
	assert_int_equal_asserts_enabled(xTraceEntryGetAtIndex(0, 0), TRC_FAIL);	/* null test */

	for (i = 0; i < (TRC_ENTRY_TABLE_SLOTS); i++)
	{
		xEntryHandle = 0;
		assert_int_equal_cs(xTraceEntryGetAtIndex(i, &xEntryHandle), TRC_SUCCESS);
		assert_int_equal_cs(xTraceEntryGetAddress(xEntryHandle, &pvAddress), TRC_SUCCESS);
		if (pvAddress != 0)
		{
			found++;
		}
	}

	assert_int_equal_cs(found, 1);
	found = 0;
	
	assert_int_equal_cs(xTraceEntryCreate(&pxState->xEntryHandle), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEntryCreate(&pxState->xEntryHandle), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEntryCreate(&pxState->xEntryHandle), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEntryCreate(&pxState->xEntryHandle), TRC_SUCCESS);

	for (i = 0; i < (TRC_ENTRY_TABLE_SLOTS); i++)
	{
		xEntryHandle = 0;
		assert_int_equal_cs(xTraceEntryGetAtIndex(i, &xEntryHandle), TRC_SUCCESS);
		assert_int_equal_cs(xTraceEntryGetAddress(xEntryHandle, &pvAddress), TRC_SUCCESS);
		if (pvAddress != 0)
		{
			found++;
		}
	}

	assert_int_equal_cs(found, 5);
}

void test_xTraceEntryFind(void** ppState)
{
	TraceEntryHandle_t h1, h2, h3;
	TraceEntryHandle_t h4, h5, h6;

	assert_int_equal_asserts_enabled(xTraceEntryFind(0, &h1), TRC_FAIL); /* null test */

	assert_int_equal_asserts_enabled(xTraceEntryFind((void*)0x10000000, 0), TRC_FAIL); /* null test */
	
	assert_int_equal_cs(xTraceEntryCreateWithAddress((void*)0x10000000, &h1), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEntryCreateWithAddress((void*)0x10000001, &h2), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEntryCreate(&h3), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryFind((void*)0x10000000, &h4), TRC_SUCCESS);
	assert_int_equal_cs((TraceUnsignedBaseType_t)h1, (TraceUnsignedBaseType_t)h4);
	assert_int_equal_cs(xTraceEntryFind((void*)0x10000001, &h5), TRC_SUCCESS);
	assert_int_equal_cs((TraceUnsignedBaseType_t)h2, (TraceUnsignedBaseType_t)h5);
	assert_int_equal_cs(xTraceEntryFind((void*)h3, &h6), TRC_SUCCESS);
	assert_int_equal_cs((TraceUnsignedBaseType_t)h3, (TraceUnsignedBaseType_t)h6);
}

void test_xTraceEntryCount(void** ppState)
{
	uint32_t uiCount;
	TraceEntryHandle_t xEntryHandle;

	assert_int_equal_asserts_enabled(xTraceEntryGetCount(0), TRC_FAIL); /* null test */
	
	assert_int_equal_cs(xTraceEntryGetCount(&uiCount), TRC_SUCCESS);
	assert_int_equal_cs(uiCount, 0);

	assert_int_equal_cs(xTraceEntryCreate(&xEntryHandle), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEntryGetCount(&uiCount), TRC_SUCCESS);
	assert_int_equal_cs(uiCount, 1);

	assert_int_equal_cs(xTraceEntryCreate(&xEntryHandle), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEntryGetCount(&uiCount), TRC_SUCCESS);
	assert_int_equal_cs(uiCount, 2);

	assert_int_equal_cs(xTraceEntryDelete(xEntryHandle), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEntryGetCount(&uiCount), TRC_SUCCESS);
	assert_int_equal_cs(uiCount, 1);
}

void test_xTraceDiagnosticsGetSet(void** ppState)
{
	TraceEntryHandle_t xEntryHandle;
	TraceBaseType_t xLongestSymbolLength;
	TraceBaseType_t xSlotsOverflow;
	uint32_t uiCount;
	uint32_t i;

	assert_int_equal_cs(xTraceDiagnosticsSet(TRC_DIAGNOSTICS_ENTRY_SLOTS_NO_ROOM, 0x12345678), TRC_SUCCESS);
	assert_int_equal_cs(xTraceDiagnosticsGet(TRC_DIAGNOSTICS_ENTRY_SLOTS_NO_ROOM, &xSlotsOverflow), TRC_SUCCESS);
	assert_int_equal_cs(xSlotsOverflow, 0x12345678);
	
	assert_int_equal_cs(xTraceDiagnosticsSet(TRC_DIAGNOSTICS_ENTRY_SLOTS_NO_ROOM, 0), TRC_SUCCESS);

	assert_int_equal_cs(xTraceDiagnosticsSet(TRC_DIAGNOSTICS_ENTRY_SYMBOL_LONGEST_LENGTH, 0x77654321), TRC_SUCCESS);
	assert_int_equal_cs(xTraceDiagnosticsGet(TRC_DIAGNOSTICS_ENTRY_SYMBOL_LONGEST_LENGTH, &xLongestSymbolLength), TRC_SUCCESS);
	assert_int_equal_cs(xLongestSymbolLength, 0x77654321);

	assert_int_equal_cs(xTraceDiagnosticsSet(TRC_DIAGNOSTICS_ENTRY_SLOTS_NO_ROOM, 0), TRC_SUCCESS);
	assert_int_equal_cs(xTraceDiagnosticsSet(TRC_DIAGNOSTICS_ENTRY_SYMBOL_LONGEST_LENGTH, 0), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryCreate(&xEntryHandle), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEntrySetSymbol(xEntryHandle, "0123456789", 10), TRC_SUCCESS);
	assert_int_equal_cs(xTraceDiagnosticsGet(TRC_DIAGNOSTICS_ENTRY_SYMBOL_LONGEST_LENGTH, &xLongestSymbolLength), TRC_SUCCESS);
	assert_int_equal_cs(xLongestSymbolLength, 10);

	assert_int_equal_cs(xTraceEntryCreate(&xEntryHandle), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEntrySetSymbol(xEntryHandle, "01234567890123456789", 20), TRC_SUCCESS);
	assert_int_equal_cs(xTraceDiagnosticsGet(TRC_DIAGNOSTICS_ENTRY_SYMBOL_LONGEST_LENGTH, &xLongestSymbolLength), TRC_SUCCESS);
	assert_int_equal_cs(xLongestSymbolLength, 20);

	assert_int_equal_cs(xTraceDiagnosticsSet(TRC_DIAGNOSTICS_ENTRY_SYMBOL_LONGEST_LENGTH, 0), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryCreate(&xEntryHandle), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEntrySetSymbol(xEntryHandle, "0123456789012345678901234567890123456789", 40), TRC_SUCCESS);
	assert_int_equal_cs(xTraceDiagnosticsGet(TRC_DIAGNOSTICS_ENTRY_SYMBOL_LONGEST_LENGTH, &xLongestSymbolLength), TRC_SUCCESS);
	assert_int_equal_cs(xLongestSymbolLength, 40);

	assert_int_equal_cs(xTraceDiagnosticsSet(TRC_DIAGNOSTICS_ENTRY_SYMBOL_LONGEST_LENGTH, 0), TRC_SUCCESS);

	assert_int_equal_cs(xTraceEntryGetCount(&uiCount), TRC_SUCCESS);
	
	/* Fill up the entry table slots */
	for (i = uiCount; i < (TRC_ENTRY_TABLE_SLOTS); i++)
	{
		assert_int_equal_cs(xTraceEntryCreate(&xEntryHandle), TRC_SUCCESS);
	}
	
	assert_int_equal_cs(xTraceDiagnosticsGet(TRC_DIAGNOSTICS_ENTRY_SLOTS_NO_ROOM, &xSlotsOverflow), TRC_SUCCESS);
	assert_int_equal_cs(xSlotsOverflow, 0);

	/* Overflow by 10! */
	for (i = 0; i < 10; i++)
	{
		assert_int_equal_cs(xTraceEntryCreate(&xEntryHandle), TRC_FAIL);
	}

	assert_int_equal_cs(xTraceDiagnosticsGet(TRC_DIAGNOSTICS_ENTRY_SLOTS_NO_ROOM, &xSlotsOverflow), TRC_SUCCESS);
	assert_int_equal_cs(xSlotsOverflow, 10);
}

void TRACE_ASSERTS_DISABLED(void** ppState)
{
}

uint32_t runEntryTableTests(void)
{
	uint32_t uiCountFailedTests = 0;

	const struct CMUnitTest aTests_trcEntryTable[] =
	{
#if (TRC_CFG_USE_TRACE_ASSERT == 0)
		cmocka_unit_test(TRACE_ASSERTS_DISABLED),
#endif
		cmocka_unit_test(test_xTraceEntryTableInitialize),
		cmocka_unit_test_setup_teardown(test_xTraceEntryCreate, setup_emptyEntryTable, teardown_entryTable),
		cmocka_unit_test_setup_teardown(test_xTraceEntryCreateWithAddress, setup_emptyEntryTable, teardown_entryTable),
		cmocka_unit_test_setup_teardown(test_xTraceEntryDelete, setup_entryTableWithOneEntry, teardown_entryTable),
		cmocka_unit_test_setup_teardown(test_xTraceEntryGetAddress, setup_entryTableWithOneEntry, teardown_entryTable),
		cmocka_unit_test_setup_teardown(test_xTraceEntrySetGetState, setup_entryTableWithOneEntry, teardown_entryTable),
		cmocka_unit_test_setup_teardown(test_xTraceEntrySetClearGetOptions, setup_entryTableWithOneEntry, teardown_entryTable),
		cmocka_unit_test_setup_teardown(test_xTraceEntrySetGetSymbol, setup_entryTableWithOneEntry, teardown_entryTable),
		cmocka_unit_test_setup_teardown(test_xTraceEntryGetEntryAtIndex, setup_entryTableWithOneEntry, teardown_entryTable),
		cmocka_unit_test_setup_teardown(test_xTraceEntryFind, setup_emptyEntryTable, teardown_entryTable),
		cmocka_unit_test_setup_teardown(test_xTraceEntryCount, setup_emptyEntryTable, teardown_entryTable),
		cmocka_unit_test_setup_teardown(test_xTraceDiagnosticsGetSet, setup_emptyEntryTable, teardown_entryTable),
#if (TRC_CFG_USE_TRACE_ASSERT == 0)
		cmocka_unit_test(TRACE_ASSERTS_DISABLED),
#endif
	};

	uiCountFailedTests += cmocka_run_group_tests(aTests_trcEntryTable, group_setup_EntryTable, group_teardown_EntryTable);

	return uiCountFailedTests;
}
