#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <MultiCoreEventBuffer_UnitTest.h>
#include <trcRecorder.h>

TraceMultiCoreEventBuffer_t xMultiCoreEventBuffer;

typedef struct {
	uint16_t EventID;
	uint16_t EventCount;
	uint32_t TS;
} BaseEvent;

typedef struct {
	BaseEvent base;
	TraceUnsignedBaseType_t param1;
} EventWithParam_1;

typedef struct {
	BaseEvent base;
	TraceUnsignedBaseType_t param1;
	TraceUnsignedBaseType_t param2;
} EventWithParam_2;

typedef struct {
	BaseEvent base;
	TraceUnsignedBaseType_t param1;
	TraceUnsignedBaseType_t param2;
	TraceUnsignedBaseType_t param3;
} EventWithParam_3;

typedef struct {
	BaseEvent base;
	TraceUnsignedBaseType_t param1;
	TraceUnsignedBaseType_t param2;
	TraceUnsignedBaseType_t param3;
	TraceUnsignedBaseType_t param4;
} EventWithParam_4;

typedef struct {
	BaseEvent base;
	TraceUnsignedBaseType_t param1;
	TraceUnsignedBaseType_t param2;
	TraceUnsignedBaseType_t param3;
	TraceUnsignedBaseType_t param4;
	TraceUnsignedBaseType_t param5;
} EventWithParam_5;


/******************************************************************************************
*                                        TEST STATE                                       *
******************************************************************************************/

typedef struct MultiCoreEventBufferTestState
{
	/* Pointer to Buffer */
	void *pvMultiCoreEventBuffer;
	uint8_t pvMultiCoreEventBufferBuffer[TRC_CFG_CORE_COUNT * 10 * sizeof(EventWithParam_2) +
		(TRC_CFG_CORE_COUNT * sizeof(TraceEventBuffer_t)) + 10];
} MultiCoreEventBufferTestState_t;

MultiCoreEventBufferTestState_t xMultiCoreEventBufferTestState;

uint32_t MOCK_uiTestTimestampCounter = 0;
volatile uint32_t MOCK_RecorderEnabled = 1;
volatile uint32_t MOCK_RecorderInitialized = 1;
uint32_t MOCK_uiCurrentCore = 0;
uint8_t MOCK_tmpBuffer[TRC_MAX_BLOB_SIZE];
uint8_t MOCK_streamPortBuffer[TRC_MAX_BLOB_SIZE];
int32_t MOCK_iISRNesting = -1;
uint32_t MOCK_uiCriticalSection = 0;
uint32_t MOCK_uiCriticalSectionExpectedValue = 0;
uint32_t MOCK_streamPortMaxTransferSize = 0xFFFFFFFF;

/******************************************************************************************
*                                        SETUP / TEARDOWN                                 *
******************************************************************************************/

int group_setup_MultiCoreEventBuffer(void **ppState)
{
	xMultiCoreEventBufferTestState.pvMultiCoreEventBuffer = &xMultiCoreEventBuffer;
	*ppState = &xMultiCoreEventBufferTestState;

	assert_int_equal(sizeof(TraceUnsignedBaseType_t), sizeof(void*));
	assert_int_equal(sizeof(TraceUnsignedBaseType_t), sizeof(TraceBaseType_t));

	return 0;
}

int group_teardown_MultiCoreEventBuffer(void **ppState)
{
	MultiCoreEventBufferTestState_t* pxState = (MultiCoreEventBufferTestState_t*)*ppState;

	for (uint32_t i = 0; i < TRC_CFG_CORE_COUNT; i++) {
		pxState->pvMultiCoreEventBuffer = 0;
	}

	return 0;
}

int setup_freeMultiCoreEventBuffer(void **ppState)
{
	MultiCoreEventBufferTestState_t* pxState = (MultiCoreEventBufferTestState_t*)*ppState;
	
	assert_int_equal_cs(xTraceMultiCoreEventBufferInitialize(pxState->pvMultiCoreEventBuffer, TRC_EVENT_BUFFER_OPTION_OVERWRITE,
		pxState->pvMultiCoreEventBufferBuffer, sizeof(pxState->pvMultiCoreEventBufferBuffer)), TRC_SUCCESS);

	return 0;
}

int setup_skipMultiCoreEventBuffer(void** ppState)
{
	MultiCoreEventBufferTestState_t* pxState = (MultiCoreEventBufferTestState_t*)*ppState;

	assert_int_equal_cs(xTraceMultiCoreEventBufferInitialize(pxState->pvMultiCoreEventBuffer, TRC_EVENT_BUFFER_OPTION_SKIP,
		pxState->pvMultiCoreEventBufferBuffer, sizeof(pxState->pvMultiCoreEventBufferBuffer)), TRC_SUCCESS);

	return 0;
}

int teardown_MultiCoreEventBuffer(void **ppState)
{
	return 0;
}

/******************************************************************************************
*                                        TESTS                                            *
******************************************************************************************/

void test_xTraceMultiCoreEventBufferInitialize(void **ppState)
{
	MultiCoreEventBufferTestState_t* pxState = (MultiCoreEventBufferTestState_t*)*ppState;

	assert_int_equal_cs(xTraceMultiCoreEventBufferInitialize(0, 0x0, pxState->pvMultiCoreEventBufferBuffer, sizeof(pxState->pvMultiCoreEventBufferBuffer)), TRC_FAIL); /* null test */
	assert_int_equal_cs(xTraceMultiCoreEventBufferInitialize(pxState->pvMultiCoreEventBuffer, 0x0, 0, sizeof(pxState->pvMultiCoreEventBufferBuffer)), TRC_FAIL); /* null test */
	assert_int_equal_cs(xTraceMultiCoreEventBufferInitialize(pxState->pvMultiCoreEventBuffer, 0x0, pxState->pvMultiCoreEventBufferBuffer,
		TRC_CFG_CORE_COUNT - 1), TRC_FAIL); /* size test */

	assert_int_equal_cs(xTraceMultiCoreEventBufferInitialize(pxState->pvMultiCoreEventBuffer, 0x0, pxState->pvMultiCoreEventBufferBuffer,
		sizeof(pxState->pvMultiCoreEventBufferBuffer)), TRC_SUCCESS);
}

void test_xTraceMultiCoreEventBufferFreePush(void** ppState)
{
	MultiCoreEventBufferTestState_t* pxState = (MultiCoreEventBufferTestState_t*)*ppState;

	int32_t iBytesWritten;
	EventWithParam_1 xDummyEventWithParam1 = { 0 };
	EventWithParam_2 xDummyEventWithParam2 = { 0 };
	xDummyEventWithParam1.base.EventID = ((1 & 0xF) << 12);
	xDummyEventWithParam2.base.EventID = ((1 & 0xF) << 12);

	/* Test that the failure conditions hold for all cores in the buffer. */
	for (uint32_t coreId = 0; coreId < TRC_CFG_CORE_COUNT; coreId++)
	{
		MOCK_uiCurrentCore = coreId;
		assert_int_equal_cs(xTraceMultiCoreEventBufferPush(pxState->pvMultiCoreEventBuffer, 0, 0, &iBytesWritten), TRC_FAIL); /* null test */
		assert_int_equal_cs(xTraceMultiCoreEventBufferPush(0, (void*)&xDummyEventWithParam1, sizeof(xDummyEventWithParam1), &iBytesWritten), TRC_FAIL); /* null test */
		assert_int_equal_cs(xTraceMultiCoreEventBufferPush(pxState->pvMultiCoreEventBuffer, (void*)&xDummyEventWithParam1, sizeof(xDummyEventWithParam1), 0), TRC_FAIL); /* null test */
		assert_int_equal_cs(xTraceMultiCoreEventBufferPush(pxState->pvMultiCoreEventBuffer, (void*)&xDummyEventWithParam1, 0xFFFFFFFF, &iBytesWritten), TRC_FAIL); /* size test */
		assert_int_equal_cs(((TraceMultiCoreEventBuffer_t*)pxState->pvMultiCoreEventBuffer)->xEventBuffer[coreId]->uiHead, 0);
		assert_int_equal_cs(((TraceMultiCoreEventBuffer_t*)pxState->pvMultiCoreEventBuffer)->xEventBuffer[coreId]->uiTail, 0);
	}

	/* Test that free buffer allow us to push data for all cores. */
	for (uint32_t coreId = 0; coreId < TRC_CFG_CORE_COUNT; coreId++)
	{
		MOCK_uiCurrentCore = coreId;
		for (uint32_t i = 0; i < (((TraceMultiCoreEventBuffer_t*)pxState->pvMultiCoreEventBuffer)->xEventBuffer[coreId]->uiSize / sizeof(EventWithParam_2)) * 2 + 1; i++)
		{
			assert_int_equal_cs(xTraceMultiCoreEventBufferPush(pxState->pvMultiCoreEventBuffer, (void*)&xDummyEventWithParam2, sizeof(EventWithParam_2), &iBytesWritten), TRC_SUCCESS);
			assert_int_equal_cs(iBytesWritten, sizeof(EventWithParam_2));
		}
	}

	/* Test that buffer is not empty */
	for (uint32_t coreId = 0; coreId < TRC_CFG_CORE_COUNT; coreId++)
	{
		assert_int_not_equal(((TraceMultiCoreEventBuffer_t*)pxState->pvMultiCoreEventBuffer)->xEventBuffer[coreId]->uiFree,
			((TraceMultiCoreEventBuffer_t*)pxState->pvMultiCoreEventBuffer)->xEventBuffer[coreId]->uiSize);
		assert_int_not_equal(((TraceMultiCoreEventBuffer_t*)pxState->pvMultiCoreEventBuffer)->xEventBuffer[coreId]->uiHead, 0);
		assert_int_not_equal(((TraceMultiCoreEventBuffer_t*)pxState->pvMultiCoreEventBuffer)->xEventBuffer[coreId]->uiTail, 0);
	}

	/* Test that buffer is cleared when calling clear */
	assert_int_equal_cs(xTraceMultiCoreEventBufferClear(0), TRC_FAIL);
	assert_int_equal_cs(xTraceMultiCoreEventBufferClear(((TraceMultiCoreEventBuffer_t*)pxState->pvMultiCoreEventBuffer)), TRC_SUCCESS);

	for (uint32_t coreId = 0; coreId < TRC_CFG_CORE_COUNT; coreId++)
	{
		assert_int_equal_cs(((TraceMultiCoreEventBuffer_t*)pxState->pvMultiCoreEventBuffer)->xEventBuffer[coreId]->uiFree,
			((TraceMultiCoreEventBuffer_t*)pxState->pvMultiCoreEventBuffer)->xEventBuffer[coreId]->uiSize);
		assert_int_equal_cs(((TraceMultiCoreEventBuffer_t*)pxState->pvMultiCoreEventBuffer)->xEventBuffer[coreId]->uiHead, 0);
		assert_int_equal_cs(((TraceMultiCoreEventBuffer_t*)pxState->pvMultiCoreEventBuffer)->xEventBuffer[coreId]->uiTail, 0);
	}
}

void test_xTraceMultiCoreEventBufferSkipPush(void** ppState)
{
	MultiCoreEventBufferTestState_t* pxState = (MultiCoreEventBufferTestState_t*)*ppState;

	uint32_t uiBytesWritten = 0;
	EventWithParam_1 xDummyEventWithParam1 = { 0 };
	EventWithParam_2 xDummyEventWithParam2 = { 0 };
	xDummyEventWithParam1.base.EventID = ((1 & 0xF) << 12);
	xDummyEventWithParam2.base.EventID = ((2 & 0xF) << 12);

	/* Test that the failure conditions hold for all cores in the buffer. */
	for (uint32_t coreId = 0; coreId < TRC_CFG_CORE_COUNT; coreId++)
	{
		MOCK_uiCurrentCore = coreId;
		assert_int_equal_cs(xTraceMultiCoreEventBufferPush(pxState->pvMultiCoreEventBuffer, 0, 0, &uiBytesWritten), TRC_FAIL); /* null test */
		assert_int_equal_cs(xTraceMultiCoreEventBufferPush(0, (void*)&xDummyEventWithParam1, sizeof(xDummyEventWithParam1), &uiBytesWritten), TRC_FAIL); /* null test */
		assert_int_equal_cs(xTraceMultiCoreEventBufferPush(pxState->pvMultiCoreEventBuffer, (void*)&xDummyEventWithParam1, 0xFFFFFFFF, &uiBytesWritten), TRC_FAIL); /* size test */
	}

	/* Test that skip buffer allow us to push data until full (for all cores). */
	for (uint32_t coreId = 0; coreId < TRC_CFG_CORE_COUNT; coreId++)
	{
		MOCK_uiCurrentCore = coreId;
		for (uint32_t i = 0; i < ((((TraceMultiCoreEventBuffer_t*)pxState->pvMultiCoreEventBuffer)->xEventBuffer[coreId]->uiSize - sizeof(uint32_t)) / sizeof(EventWithParam_2)); i++) {
			assert_int_equal_cs(xTraceMultiCoreEventBufferPush(pxState->pvMultiCoreEventBuffer, (void*)&xDummyEventWithParam2, sizeof(EventWithParam_2), &uiBytesWritten), TRC_SUCCESS);
			assert_int_equal_cs(uiBytesWritten, sizeof(EventWithParam_2));
		}
	}

	/* Test that skip buffer does not allow us to push data while full (for all cores). */
	for (uint32_t coreId = 0; coreId < TRC_CFG_CORE_COUNT; coreId++)
	{
		MOCK_uiCurrentCore = coreId;
		for (uint32_t i = 0; i < (((TraceMultiCoreEventBuffer_t*)pxState->pvMultiCoreEventBuffer)->xEventBuffer[coreId]->uiSize / sizeof(EventWithParam_2)) * 2; i++) {
			assert_int_equal_cs(xTraceMultiCoreEventBufferPush(pxState->pvMultiCoreEventBuffer, (void*)&xDummyEventWithParam2, sizeof(EventWithParam_2), &uiBytesWritten), TRC_SUCCESS);
			assert_int_equal_cs(uiBytesWritten, 0);
		}
	}
}

uint32_t runMultiCoreEventBufferTests(void)
{
	uint32_t uiCountFailedTests = 0;

	const struct CMUnitTest aTests_trcMultiCoreEventBuffer[] =
	{
		cmocka_unit_test(test_xTraceMultiCoreEventBufferInitialize),
		cmocka_unit_test_setup_teardown(test_xTraceMultiCoreEventBufferFreePush, setup_freeMultiCoreEventBuffer, teardown_MultiCoreEventBuffer),
		cmocka_unit_test_setup_teardown(test_xTraceMultiCoreEventBufferSkipPush, setup_skipMultiCoreEventBuffer, teardown_MultiCoreEventBuffer),
	};

	uiCountFailedTests += cmocka_run_group_tests(aTests_trcMultiCoreEventBuffer, group_setup_MultiCoreEventBuffer, group_teardown_MultiCoreEventBuffer);
	
	return uiCountFailedTests;
}
