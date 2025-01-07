#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <EventBuffer_UnitTest.h>
#include <trcRecorder.h>

#define TRC_EVENT_SET_PARAM_COUNT(id, n) ((uint16_t)(((uint16_t)(id)) | ((((uint16_t)(n)) & 0xF) << 12)))

TraceEventBuffer_t xEventBuffer;

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

int group_teardown_EventBuffer(void** ppState);
int group_setup_EventBuffer(void** ppState);
int setup_freeEventBuffer(void** ppState);
int setup_skipEventBuffer(void** ppState);
int teardown_eventBuffer(void** ppState);

void test_xTraceEventBufferInitialize(void** ppState);
void test_xTraceEventBufferAllocOverwrite(void** ppState);
void test_xTraceEventBufferFreePush(void** ppState);
void test_xTraceEventBufferSkipPush(void** ppState);
void test_xTraceEventBufferTransferChunk(void** ppState);
void test_xTraceEventBufferTransferAll_Normal(void** ppState);
void test_xTraceEventBufferTransferAll_Partial(void** ppState);
void test_xTraceEventBufferTransferAll_NormalWrapping(void** ppState);
void test_xTraceEventBufferTransferAll_PartialWrapping(void** ppState);
void test_xTraceEventBufferClear(void** ppState);

/******************************************************************************************
*                                        TEST STATE                                       *
******************************************************************************************/

typedef struct EventBufferTestState
{
	/* Pointer to Buffer */
	void *pvEvtBuffer;
	uint8_t pvEvtBufferBuffer[116];
} EventBufferTestState_t;

EventBufferTestState_t xEvtRingBufferTestState;

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

int group_setup_EventBuffer(void **ppState)
{
	xEvtRingBufferTestState.pvEvtBuffer = &xEventBuffer;
	*ppState = &xEvtRingBufferTestState;

	assert_int_equal(sizeof(TraceUnsignedBaseType_t), sizeof(void*));
	assert_int_equal(sizeof(TraceUnsignedBaseType_t), sizeof(TraceBaseType_t));

	return 0;
}

int group_teardown_EventBuffer(void **ppState)
{
	EventBufferTestState_t* pxState = (EventBufferTestState_t*)*ppState;

	pxState->pvEvtBuffer = 0;

	return 0;
}

int setup_freeEventBuffer(void **ppState)
{
	EventBufferTestState_t* pxState = (EventBufferTestState_t*)*ppState;
	
	assert_int_equal_cs(xTraceEventBufferInitialize(pxState->pvEvtBuffer, TRC_EVENT_BUFFER_OPTION_OVERWRITE,
		pxState->pvEvtBufferBuffer, sizeof(pxState->pvEvtBufferBuffer)), TRC_SUCCESS);

	return 0;
}

int setup_skipEventBuffer(void** ppState)
{
	EventBufferTestState_t* pxState = (EventBufferTestState_t*)*ppState;

	assert_int_equal_cs(xTraceEventBufferInitialize(pxState->pvEvtBuffer, TRC_EVENT_BUFFER_OPTION_SKIP,
		pxState->pvEvtBufferBuffer, sizeof(pxState->pvEvtBufferBuffer)), TRC_SUCCESS);

	return 0;
}

int teardown_eventBuffer(void **ppState)
{
	return 0;
}

/******************************************************************************************
*                                        TESTS                                            *
******************************************************************************************/

void test_xTraceEventBufferInitialize(void **ppState)
{
	EventBufferTestState_t* pxState = (EventBufferTestState_t*)*ppState;

	assert_int_equal_cs(xTraceEventBufferInitialize(0, 0x0, pxState->pvEvtBufferBuffer, sizeof(pxState->pvEvtBufferBuffer)), TRC_FAIL); /* null test */
	assert_int_equal_cs(xTraceEventBufferInitialize(pxState->pvEvtBuffer, 0x0, 0, sizeof(pxState->pvEvtBufferBuffer)), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceEventBufferInitialize(pxState->pvEvtBuffer, 0x0, pxState->pvEvtBufferBuffer, sizeof(pxState->pvEvtBufferBuffer)), TRC_SUCCESS);
}

void test_xTraceEventBufferAllocOverwrite(void** ppState)
{
	EventBufferTestState_t* pxState = (EventBufferTestState_t*)*ppState;

	uint8_t auiData[128] = { 0 };
	int32_t iBytesWritten = 0;
	uint32_t uiHead;
	uint32_t uiTail;
	uint32_t uiSlack = 0;
	EventWithParam_1 xDummyEventWithParam1 = { 0 };
	EventWithParam_2 xDummyEventWithParam2 = { 0 };
	TraceEventBuffer_t* pxEventBuffer = ((TraceEventBuffer_t*)(pxState->pvEvtBuffer));
	xDummyEventWithParam1.base.EventID = TRC_EVENT_SET_PARAM_COUNT(0, 1);
	xDummyEventWithParam2.base.EventID = TRC_EVENT_SET_PARAM_COUNT(0, 2);

	assert_int_equal_cs(xTraceEventBufferAlloc(pxEventBuffer, 32, 0), TRC_FAIL); /* null test */
	assert_int_equal_cs(xTraceEventBufferAlloc(0, 32, (void**)&auiData), TRC_FAIL); /* null test */
	assert_int_equal_cs(xTraceEventBufferAlloc(pxEventBuffer, pxEventBuffer->uiSize * 2, (void**)&auiData), TRC_FAIL); /* size test */

	for (uint32_t i = 0; i < pxEventBuffer->uiSize / sizeof(xDummyEventWithParam1); i++) {
		/* Store head and tail for later error checks */
		uiHead = pxEventBuffer->uiHead;
		uiTail = pxEventBuffer->uiTail;

		/* Allocate buffer entry, this should always succeed */
		assert_int_equal_cs(xTraceEventBufferAlloc(pxEventBuffer, sizeof(xDummyEventWithParam1), (void**)&auiData), TRC_SUCCESS);

		/* Verify that next head is what we expect */
		assert_int_equal_cs(pxEventBuffer->uiNextHead, uiHead + sizeof(xDummyEventWithParam1));

		/* */
		assert_int_equal_cs(xTraceEventBufferAllocCommit(pxEventBuffer, 0, sizeof(xDummyEventWithParam1), &iBytesWritten), TRC_SUCCESS);

		/* Verify that head is what we expect */
		assert_int_equal_cs(pxEventBuffer->uiHead, uiHead + sizeof(xDummyEventWithParam1));

		/* Head and tail should not become equal */
		assert_int_not_equal(pxEventBuffer->uiHead, pxEventBuffer->uiTail);
	}

	/* Check wrapping behavior */
	assert_int_equal_cs(xTraceEventBufferAlloc(pxEventBuffer, sizeof(xDummyEventWithParam1), (void**)&auiData), TRC_SUCCESS);
	assert_int_equal_cs(pxEventBuffer->uiHead, 0);
	assert_int_not_equal(pxEventBuffer->uiHead, pxEventBuffer->uiTail);
	assert_int_equal_cs(pxEventBuffer->uiNextHead, sizeof(xDummyEventWithParam1));
	assert_int_equal_cs(xTraceEventBufferAllocCommit(pxEventBuffer, 0, sizeof(xDummyEventWithParam1), &iBytesWritten), TRC_SUCCESS);
	assert_int_equal_cs(pxEventBuffer->uiHead, sizeof(xDummyEventWithParam1));
	assert_int_not_equal(pxEventBuffer->uiHead, pxEventBuffer->uiTail);

	for (uint32_t i = 0; i < pxEventBuffer->uiSize / sizeof(xDummyEventWithParam1) - 1; i++) {
		/* Store head and tail for later error checks */
		uiHead = pxEventBuffer->uiHead;
		uiTail = pxEventBuffer->uiTail;

		/* Allocate buffer entry, this should always succeed */
		assert_int_equal_cs(xTraceEventBufferAlloc(pxEventBuffer, sizeof(xDummyEventWithParam1), (void**)&auiData), TRC_SUCCESS);

		/* Verify that next head is what we expect */
		assert_int_equal_cs(pxEventBuffer->uiNextHead, uiHead + sizeof(xDummyEventWithParam1));

		assert_int_not_equal(pxEventBuffer->uiHead, pxEventBuffer->uiTail);

		/* */
		assert_int_equal_cs(xTraceEventBufferAllocCommit(pxEventBuffer, 0, sizeof(xDummyEventWithParam1), &iBytesWritten), TRC_SUCCESS);

		/* Verify that head is what we expect */
		assert_int_equal_cs(pxEventBuffer->uiHead, uiHead + sizeof(xDummyEventWithParam1));

		assert_int_not_equal(pxEventBuffer->uiHead, pxEventBuffer->uiTail);
	}
}

void test_xTraceEventBufferFreePush(void** ppState)
{
	EventBufferTestState_t* pxState = (EventBufferTestState_t*)*ppState;

	uint32_t uiBytesWritten = 0;
	EventWithParam_1 xDummyEventWithParam1 = { 0 };
	EventWithParam_2 xDummyEventWithParam2 = { 0 };
	TraceEventBuffer_t* pxEventBuffer = ((TraceEventBuffer_t*)(pxState->pvEvtBuffer));
	xDummyEventWithParam1.base.EventID = TRC_EVENT_SET_PARAM_COUNT(0, 1);
	xDummyEventWithParam2.base.EventID = TRC_EVENT_SET_PARAM_COUNT(0, 2);

	assert_int_equal_cs(xTraceEventBufferPush(pxState->pvEvtBuffer, 0, 0, &uiBytesWritten), TRC_FAIL); /* null test */
	assert_int_equal_cs(xTraceEventBufferPush(0, (void*)&xDummyEventWithParam1, sizeof(xDummyEventWithParam1), &uiBytesWritten), TRC_FAIL); /* null test */
	assert_int_equal_cs(xTraceEventBufferPush(pxState->pvEvtBuffer, (void*)&xDummyEventWithParam1, 0xFFFFFFFF, &uiBytesWritten), TRC_FAIL); /* size test */
	assert_int_equal_cs(xTraceEventBufferPush(pxState->pvEvtBuffer, (void*)&xDummyEventWithParam1, sizeof(xDummyEventWithParam1), 0), TRC_FAIL); /* null test */
	assert_int_equal_cs(pxEventBuffer->uiHead, 0); /* Head should not have moved */
	assert_int_equal_cs(pxEventBuffer->uiTail, 0); /* Tail should not have moved */
	assert_int_equal_cs(pxEventBuffer->uiFree, pxEventBuffer->uiSize); /* Free space should be equal to size */
	assert_int_equal_cs(pxEventBuffer->uiOptions, TRC_EVENT_BUFFER_OPTION_OVERWRITE); /* The option should be free on full */

	uint32_t uiCurrEventCode = 0;
	uint32_t uiLastEventCode = 0;
	uint32_t uiPrevTail = pxEventBuffer->uiTail;
	uint32_t uiEventId = 0;
	uint32_t uiEventSize = 0;

	/* In free mode we should always be able to push the number of bytes we wanted. */
	for (uint32_t i = 0; i < (((TraceEventBuffer_t*)pxState->pvEvtBuffer)->uiSize / sizeof(EventWithParam_1)) * 2; i++) {
		xDummyEventWithParam1.base.EventID = TRC_EVENT_SET_PARAM_COUNT(uiCurrEventCode++, 1);

		assert_int_equal_cs(xTraceEventBufferPush(pxState->pvEvtBuffer, (void*)&xDummyEventWithParam1, sizeof(EventWithParam_1), &uiBytesWritten), TRC_SUCCESS);
		assert_int_equal_cs(uiBytesWritten, sizeof(EventWithParam_1));

		/* Check if tail moved */
		if (pxEventBuffer->uiTail != uiPrevTail) {
			/* Event removed, last event code updated */
			uiLastEventCode++;
		}

		/* Update previous tail */
		uiPrevTail = pxEventBuffer->uiTail;
	}

	/* Check that those events that are in the buffer after writing with free matches what we expect */
	for (uint32_t i = uiLastEventCode; i < uiCurrEventCode; i++)
	{
		uiEventId = *((uint16_t*)(&pxEventBuffer->puiBuffer[pxEventBuffer->uiTail]));

		assert_int_equal_cs(uiEventId, TRC_EVENT_SET_PARAM_COUNT(i, 1));

		/* Move tail, we do not have to check against head here since uiLastEventCode and uiCurrentEventCode sets
		 * the bounds of the buffer.
		 */
		pxEventBuffer->uiTail = (pxEventBuffer->uiTail + (sizeof(BaseEvent) + (sizeof(TraceUnsignedBaseType_t) * (uiEventId >> 12)))) % pxEventBuffer->uiSize;
	}
}

void test_xTraceEventBufferSkipPush(void** ppState)
{
	EventBufferTestState_t* pxState = (EventBufferTestState_t*)*ppState;

	uint32_t uiBytesWritten = 0;
	EventWithParam_1 xDummyEventWithParam1 = { 0 };
	EventWithParam_2 xDummyEventWithParam2 = { 0 };
	TraceEventBuffer_t* pxEventBuffer = ((TraceEventBuffer_t*)(pxState->pvEvtBuffer));
	xDummyEventWithParam1.base.EventID = ((1 & 0xF) << 12);
	xDummyEventWithParam2.base.EventID = ((2 & 0xF) << 12);

	assert_int_equal_cs(xTraceEventBufferPush(pxState->pvEvtBuffer, 0, 0, &uiBytesWritten), TRC_FAIL); /* null test */
	assert_int_equal_cs(xTraceEventBufferPush(0, (void*)&xDummyEventWithParam1, sizeof(xDummyEventWithParam1), &uiBytesWritten), TRC_FAIL); /* null test */
	assert_int_equal_cs(xTraceEventBufferPush(pxState->pvEvtBuffer, (void*)&xDummyEventWithParam1, 0xFFFFFFFF, &uiBytesWritten), TRC_FAIL); /* size test */
	assert_int_equal_cs(xTraceEventBufferPush(pxState->pvEvtBuffer, (void*)&xDummyEventWithParam1, sizeof(xDummyEventWithParam1), 0), TRC_FAIL); /* null test */
	assert_int_equal_cs(pxEventBuffer->uiHead, 0); /* Head should not have moved */
	assert_int_equal_cs(pxEventBuffer->uiTail, 0); /* Tail should not have moved */
	assert_int_equal_cs(pxEventBuffer->uiFree, pxEventBuffer->uiSize); /* Free space should be equal to size */
	assert_int_equal_cs(pxEventBuffer->uiOptions, TRC_EVENT_BUFFER_OPTION_SKIP); /* The option should be free on full */

	uint32_t uiCurrEventCode = 0;
	uint32_t uiLastEventCode = 0;
	uint32_t uiPrevTail = pxEventBuffer->uiTail;
	uint32_t uiEventId = 0;
	uint32_t uiEventSize = 0;

	/* Test that skip buffer allow us to push data until full. */
	for (uint32_t i = 0; i < (((TraceEventBuffer_t*)pxState->pvEvtBuffer)->uiSize - sizeof(uint32_t)) / sizeof(EventWithParam_2); i++)
	{
		xDummyEventWithParam2.base.EventID = TRC_EVENT_SET_PARAM_COUNT(uiCurrEventCode++, 2);

		assert_int_equal_cs(xTraceEventBufferPush(pxState->pvEvtBuffer, (void*)&xDummyEventWithParam2, sizeof(EventWithParam_2), &uiBytesWritten), TRC_SUCCESS);
		assert_int_equal_cs(uiBytesWritten, sizeof(EventWithParam_2));

		/* Check if tail moved */
		if (pxEventBuffer->uiTail != uiPrevTail) {
			/* Event removed, last event code updated */
			uiLastEventCode++;
		}

		/* Update previous tail */
		uiPrevTail = pxEventBuffer->uiTail;
	}

	/* Check buffer not empty */
	assert_int_not_equal(pxEventBuffer->uiHead, pxEventBuffer->uiTail);

	/* Test that skip buffer does not allow us to push data when full */
	for (uint32_t i = 0; i < ((TraceEventBuffer_t*)pxState->pvEvtBuffer)->uiSize / sizeof(EventWithParam_2); i++) {
		assert_int_equal_cs(xTraceEventBufferPush(pxState->pvEvtBuffer, (void*)&xDummyEventWithParam2, sizeof(EventWithParam_2), &uiBytesWritten), TRC_SUCCESS);
		assert_int_equal_cs(uiBytesWritten, 0);
	}

	/* Check that those events that are in the buffer after writing with skip matches what we expect.
	 * this also flushes the buffer. */
	for (uint32_t i = uiLastEventCode; i < uiCurrEventCode; i++) {
		uiEventId = *((uint16_t*)(&pxEventBuffer->puiBuffer[pxEventBuffer->uiTail]));

		assert_int_equal_cs(uiEventId, TRC_EVENT_SET_PARAM_COUNT(i, 2));

		/* Move tail, we do not have to check against head here since uiLastEventCode and uiCurrentEventCode sets
		 * the bounds of the buffer.
		 */
		pxEventBuffer->uiTail = (pxEventBuffer->uiTail + (sizeof(BaseEvent) + (sizeof(TraceUnsignedBaseType_t) * (uiEventId >> 12)))) % pxEventBuffer->uiSize;
	}

	/* Head and tail should be equal (buffer empty) */
	assert_int_equal_cs(pxEventBuffer->uiHead, pxEventBuffer->uiTail);

	/* Reset for new run */
	uiCurrEventCode = 0;
	uiLastEventCode = 0;
	uiPrevTail = pxEventBuffer->uiTail;

	/* Test that skip buffer allow us to push data until full when head and tail start from a non-zero location (post flush). */
	for (uint32_t i = 0; i < (((TraceEventBuffer_t*)pxState->pvEvtBuffer)->uiSize - sizeof(TraceUnsignedBaseType_t)) / sizeof(EventWithParam_2); i++) {
		xDummyEventWithParam2.base.EventID = TRC_EVENT_SET_PARAM_COUNT(uiCurrEventCode++, 2);

		assert_int_equal_cs(xTraceEventBufferPush(pxState->pvEvtBuffer, (void*)&xDummyEventWithParam2, sizeof(EventWithParam_2), &uiBytesWritten), TRC_SUCCESS);
		assert_int_equal_cs(uiBytesWritten, sizeof(EventWithParam_2));

		/* Check if tail moved */
		if (pxEventBuffer->uiTail != uiPrevTail) {
			/* Event removed, last event code updated */
			uiLastEventCode++;
		}

		/* Update previous tail */
		uiPrevTail = pxEventBuffer->uiTail;
	}

	/* Check buffer not empty */
	assert_int_not_equal(pxEventBuffer->uiHead, pxEventBuffer->uiTail);

	/* Test that skip buffer does not allow us to push data when full */
	for (uint32_t i = 0; i < ((TraceEventBuffer_t*)pxState->pvEvtBuffer)->uiSize / sizeof(EventWithParam_2); i++) {
		assert_int_equal_cs(xTraceEventBufferPush(pxState->pvEvtBuffer, (void*)&xDummyEventWithParam2, sizeof(EventWithParam_2), &uiBytesWritten), TRC_SUCCESS);
		assert_int_equal_cs(uiBytesWritten, 0);
	}

	/* Check that those events that are in the buffer after writing with skip matches what we expect.
	 * this also flushes the buffer. */
	for (uint32_t i = uiLastEventCode; i < uiCurrEventCode; i++) {
		uiEventId = *((uint16_t*)(&pxEventBuffer->puiBuffer[pxEventBuffer->uiTail]));

		assert_int_equal_cs(uiEventId, TRC_EVENT_SET_PARAM_COUNT(i, 2));

		/* Move tail, we do not have to check against head here since uiLastEventCode and uiCurrentEventCode sets
		 * the bounds of the buffer.
		 */
		pxEventBuffer->uiTail = (pxEventBuffer->uiTail + (sizeof(BaseEvent) + (sizeof(TraceUnsignedBaseType_t) * (uiEventId >> 12)))) % pxEventBuffer->uiSize;
	}
}

void test_xTraceEventBufferTransferChunk(void** ppState)
{
	EventBufferTestState_t* pxState = (EventBufferTestState_t*)*ppState;
	int32_t iBytesWritten;
	uint32_t uiTotalBytesWritten = 0;
	uint32_t uiTotalBytesTransfered = 0;
	uint32_t uiHead = 0;
	uint32_t uiTail = 0;

	EventWithParam_1 xDummyEventWithParam1 = { 0 };
	TraceEventBuffer_t* pxEventBuffer = ((TraceEventBuffer_t*)(pxState->pvEvtBuffer));
	xDummyEventWithParam1.base.EventID = ((1 & 0xF) << 12);

	assert_int_equal_cs(xTraceEventBufferTransferChunk(0, 1500, &iBytesWritten), TRC_FAIL); /* null test */
	assert_int_equal_cs(xTraceEventBufferTransferChunk(pxState->pvEvtBuffer, 1500, 0), TRC_FAIL); /* null test */

	/* Push some event to the buffer so we can test transfer */
	for (uint32_t i = 0; i < (pxEventBuffer->uiSize - sizeof(uint32_t)) / sizeof(EventWithParam_1); i++)
	{
		xDummyEventWithParam1.base.EventID = TRC_EVENT_SET_PARAM_COUNT(4012, 1);

		assert_int_equal_cs(xTraceEventBufferPush(pxState->pvEvtBuffer, (void*)&xDummyEventWithParam1, sizeof(EventWithParam_1), &iBytesWritten), TRC_SUCCESS);
		assert_int_equal_cs(iBytesWritten, sizeof(EventWithParam_1));

		uiTotalBytesWritten += iBytesWritten;
	}

	/* Tail should be 0 (skip), Head should not be 0 */
	assert_int_equal_cs(pxEventBuffer->uiTail, 0);
	assert_int_not_equal(pxEventBuffer->uiHead, 0);

	/* Head and tail should not match */
	assert_int_not_equal(pxEventBuffer->uiHead, pxEventBuffer->uiTail);
	
	uiHead = pxEventBuffer->uiHead;
	uiTail = pxEventBuffer->uiTail;

	/* Should succeed */
	for (uint32_t i = 0; i < 3; i++) {
		assert_int_equal_cs(xTraceEventBufferTransferChunk(pxEventBuffer, 32, &iBytesWritten), TRC_SUCCESS);

		uiTotalBytesTransfered += iBytesWritten;
	}

	/* Should succeed */
	assert_int_equal_cs(pxEventBuffer->uiTail, uiTotalBytesTransfered);

	/* Add more events so we can test wrapping transfer */
	for (uint32_t i = 0; i < uiTotalBytesTransfered / sizeof(EventWithParam_1); i++)
	{
		xDummyEventWithParam1.base.EventID = TRC_EVENT_SET_PARAM_COUNT(4012, 1);

		assert_int_equal_cs(xTraceEventBufferPush(pxState->pvEvtBuffer, (void*)&xDummyEventWithParam1, sizeof(EventWithParam_1), &iBytesWritten), TRC_SUCCESS);
		assert_int_equal_cs(iBytesWritten, sizeof(EventWithParam_1));

		uiTotalBytesWritten += iBytesWritten;
	}

	/* Should succeed */
	do {
		if (pxEventBuffer->uiTail > pxEventBuffer->uiHead && (pxEventBuffer->uiSize - pxEventBuffer->uiTail) < 32) {
			/* Store tail for later check, the internal one will be updated */
			uiTail = pxEventBuffer->uiTail;

			/* This should be a partial transfer that succeeds */
			assert_int_equal_cs(xTraceEventBufferTransferChunk(pxEventBuffer, 32, &iBytesWritten), TRC_SUCCESS);

			/* Check that partial write match expected size when wrapping */
			assert_int_equal_cs(iBytesWritten, pxEventBuffer->uiSize - uiTail);

			/* Due to wrapping tail should now be 0 */
			assert_int_equal_cs(pxEventBuffer->uiTail, 0);
		}
		else {
			assert_int_equal_cs(xTraceEventBufferTransferChunk(pxEventBuffer, 32, &iBytesWritten), TRC_SUCCESS);
		}

		uiTotalBytesTransfered += iBytesWritten;
	} while (iBytesWritten > 0);

	/* Verify number of bytes written */
	assert_int_equal_cs(uiTotalBytesWritten, uiTotalBytesTransfered);
}

void test_xTraceEventBufferTransferAll_Normal(void** ppState)
{
	EventBufferTestState_t* pxState = (EventBufferTestState_t*)*ppState;
	int32_t iBytesWritten;
	uint32_t uiTotalBytesWritten = 0;

	EventWithParam_1 xDummyEventWithParam1 = { 0 };
	TraceEventBuffer_t* pxEventBuffer = ((TraceEventBuffer_t*)(pxState->pvEvtBuffer));
	xDummyEventWithParam1.base.EventID = ((1 & 0xF) << 12);

	assert_int_equal_cs(xTraceEventBufferTransferAll(0, &iBytesWritten), TRC_FAIL); /* null test */
	assert_int_equal_cs(xTraceEventBufferTransferAll(pxState->pvEvtBuffer, 0), TRC_FAIL); /* null test */

	/* Push some event to the buffer so we can test transfer */
	for (uint32_t i = 0; i < (pxEventBuffer->uiSize - sizeof(uint32_t)) / sizeof(EventWithParam_1); i++)
	{
		xDummyEventWithParam1.base.EventID = TRC_EVENT_SET_PARAM_COUNT(4012, 1);

		assert_int_equal_cs(xTraceEventBufferPush(pxState->pvEvtBuffer, (void*)&xDummyEventWithParam1, sizeof(EventWithParam_1), &iBytesWritten), TRC_SUCCESS);
		assert_int_equal_cs(iBytesWritten, sizeof(EventWithParam_1));

		uiTotalBytesWritten += iBytesWritten;
	}

	/* Tail should be 0 (skip), Head should not be 0 */
	assert_int_equal_cs(pxEventBuffer->uiTail, 0);
	assert_int_not_equal(pxEventBuffer->uiHead, 0);

	/* Head and tail should not match */
	assert_int_not_equal(pxEventBuffer->uiHead, pxEventBuffer->uiTail);

	/* Should succeed */
	MOCK_streamPortMaxTransferSize = 0xFFFFFFFF;
	assert_int_equal_cs(xTraceEventBufferTransferAll(pxEventBuffer, &iBytesWritten), TRC_SUCCESS);

	/* Head and tail should now match */
	assert_int_equal_cs(pxEventBuffer->uiHead, pxEventBuffer->uiTail);

	/* Verify number of bytes written */
	assert_int_equal_cs(uiTotalBytesWritten, iBytesWritten);
}

void test_xTraceEventBufferTransferAll_Partial(void** ppState)
{
	EventBufferTestState_t* pxState = (EventBufferTestState_t*)*ppState;
	int32_t iBytesWritten = 0;
	uint32_t uiTotalBytesWritten = 0;
	
	EventWithParam_1 xDummyEventWithParam1 = { 0 };
	TraceEventBuffer_t* pxEventBuffer = ((TraceEventBuffer_t*)(pxState->pvEvtBuffer));
	xDummyEventWithParam1.base.EventID = ((1 & 0xF) << 12);

	assert_int_equal_cs(((TraceEventBuffer_t*)pxState->pvEvtBuffer)->uiSize, 116);

	assert_int_equal_cs(xTraceEventBufferTransferAll(0, &iBytesWritten), TRC_FAIL); /* null test */
	assert_int_equal_cs(xTraceEventBufferTransferAll(pxState->pvEvtBuffer, 0), TRC_FAIL); /* null test */

	/* Push some event to the buffer so we can test transfer */
	for (uint32_t i = 0; i < (pxEventBuffer->uiSize - sizeof(uint32_t)) / sizeof(EventWithParam_1); i++)
	{
		xDummyEventWithParam1.base.EventID = TRC_EVENT_SET_PARAM_COUNT(4012, 1);

		assert_int_equal_cs(xTraceEventBufferPush(pxState->pvEvtBuffer, (void*)&xDummyEventWithParam1, sizeof(EventWithParam_1), &iBytesWritten), TRC_SUCCESS);
		assert_int_equal_cs(iBytesWritten, (int32_t)sizeof(EventWithParam_1));

		uiTotalBytesWritten += (uint32_t)iBytesWritten;
	}

	/* Tail should be 0 (skip), Head should not be 0 */
	assert_int_equal_cs(pxEventBuffer->uiTail, 0);
	assert_int_not_equal(pxEventBuffer->uiHead, 0);

	/* Head and tail should not match */
	assert_int_not_equal(pxEventBuffer->uiHead, pxEventBuffer->uiTail);

	/* 0-50 bytes */
	/* Should succeed */
	MOCK_streamPortMaxTransferSize = 50;
	assert_int_equal_cs(xTraceEventBufferTransferAll(pxEventBuffer, &iBytesWritten), TRC_SUCCESS);

	/* Head and tail should not match */
	assert_int_not_equal(pxEventBuffer->uiHead, pxEventBuffer->uiTail);

	/* Verify number of bytes written */
	assert_int_equal_cs(MOCK_streamPortMaxTransferSize, (uint32_t)iBytesWritten);
	assert_int_equal_cs(pxEventBuffer->uiTail, (uint32_t)iBytesWritten);

	iBytesWritten = 0;

	/* 51-100 bytes */
	/* Should succeed */
	assert_int_equal_cs(xTraceEventBufferTransferAll(pxEventBuffer, &iBytesWritten), TRC_SUCCESS);

	/* Head and tail should not match */
	assert_int_not_equal(pxEventBuffer->uiHead, pxEventBuffer->uiTail);

	/* Verify number of bytes written */
	assert_int_equal_cs(MOCK_streamPortMaxTransferSize, (uint32_t)iBytesWritten);
	assert_int_equal_cs(pxEventBuffer->uiTail, (TraceUnsignedBaseType_t)MOCK_streamPortMaxTransferSize * 2);

	iBytesWritten = 0;

	/* Last time. After this everything should be transferred. */
	/* 101-116 bytes */
	/* Should succeed */
	assert_int_equal_cs(xTraceEventBufferTransferAll(pxEventBuffer, &iBytesWritten), TRC_SUCCESS);

	/* Head and tail should match */
	assert_int_equal(pxEventBuffer->uiHead, pxEventBuffer->uiTail);

	/* Verify number of bytes written */
	assert_int_not_equal(MOCK_streamPortMaxTransferSize, (uint32_t)iBytesWritten);
}

void test_xTraceEventBufferTransferAll_NormalWrapping(void** ppState)
{
	EventBufferTestState_t* pxState = (EventBufferTestState_t*)*ppState;
	int32_t iBytesWritten = 0;
	uint32_t uiTotalBytesWritten = 0;

	EventWithParam_1 xDummyEventWithParam1 = { 0 };
	TraceEventBuffer_t* pxEventBuffer = ((TraceEventBuffer_t*)(pxState->pvEvtBuffer));
	xDummyEventWithParam1.base.EventID = ((1 & 0xF) << 12);

	assert_int_equal_cs(xTraceEventBufferTransferAll(0, &iBytesWritten), TRC_FAIL); /* null test */
	assert_int_equal_cs(xTraceEventBufferTransferAll(pxState->pvEvtBuffer, 0), TRC_FAIL); /* null test */

	/* Push some event to the buffer so we can test transfer */
	for (uint32_t i = 0; i < (pxEventBuffer->uiSize - sizeof(uint32_t)) / sizeof(EventWithParam_1); i++)
	{
		xDummyEventWithParam1.base.EventID = TRC_EVENT_SET_PARAM_COUNT(4012, 1);

		assert_int_equal_cs(xTraceEventBufferPush(pxState->pvEvtBuffer, (void*)&xDummyEventWithParam1, sizeof(EventWithParam_1), &iBytesWritten), TRC_SUCCESS);
		assert_int_equal_cs(iBytesWritten, (int32_t)sizeof(EventWithParam_1));

		uiTotalBytesWritten += (uint32_t)iBytesWritten;
	}

	/* Tail should be 0 (skip), Head should not be 0 */
	assert_int_equal_cs(pxEventBuffer->uiTail, 0);
	assert_int_not_equal(pxEventBuffer->uiHead, 0);

	/* Head and tail should not match */
	assert_int_not_equal(pxEventBuffer->uiHead, pxEventBuffer->uiTail);

	/* Should succeed */
	MOCK_streamPortMaxTransferSize = 0xFFFFFFFF;
	assert_int_equal_cs(xTraceEventBufferTransferAll(pxEventBuffer, &iBytesWritten), TRC_SUCCESS);

	/* Head and tail should now match */
	assert_int_equal_cs(pxEventBuffer->uiHead, pxEventBuffer->uiTail);

	/* Verify number of bytes written */
	assert_int_equal_cs(uiTotalBytesWritten, iBytesWritten);

	/* NOW we wrap and transfer that */
	assert_int_equal_cs(xTraceEventBufferPush(pxState->pvEvtBuffer, (void*)&xDummyEventWithParam1, sizeof(EventWithParam_1), &iBytesWritten), TRC_SUCCESS);
	assert_int_equal_cs(iBytesWritten, sizeof(EventWithParam_1));

	uiTotalBytesWritten += iBytesWritten;

	/* Make sure we wrapped */
	assert_true(pxEventBuffer->uiHead < pxEventBuffer->uiTail);

	/* Should succeed */
	assert_int_equal_cs(xTraceEventBufferTransferAll(pxEventBuffer, &iBytesWritten), TRC_SUCCESS);

	/* Head and tail should now match */
	assert_int_equal_cs(pxEventBuffer->uiHead, pxEventBuffer->uiTail);

	/* Verify number of bytes written */
	assert_int_equal_cs(iBytesWritten, sizeof(EventWithParam_1));
}

void test_xTraceEventBufferTransferAll_PartialWrapping(void** ppState)
{
	EventBufferTestState_t* pxState = (EventBufferTestState_t*)*ppState;
	int32_t iBytesWritten = 0;
	uint32_t uiTotalBytesWritten = 0;

	EventWithParam_1 xDummyEventWithParam1 = { 0 };
	TraceEventBuffer_t* pxEventBuffer = ((TraceEventBuffer_t*)(pxState->pvEvtBuffer));
	xDummyEventWithParam1.base.EventID = ((1 & 0xF) << 12);

	assert_int_equal_cs(xTraceEventBufferTransferAll(0, &iBytesWritten), TRC_FAIL); /* null test */
	assert_int_equal_cs(xTraceEventBufferTransferAll(pxState->pvEvtBuffer, 0), TRC_FAIL); /* null test */

	/* Push some event to the buffer so we can test transfer */
	for (uint32_t i = 0; i < (pxEventBuffer->uiSize - sizeof(uint32_t)) / sizeof(EventWithParam_1); i++)
	{
		xDummyEventWithParam1.base.EventID = TRC_EVENT_SET_PARAM_COUNT(4012, 1);

		assert_int_equal_cs(xTraceEventBufferPush(pxState->pvEvtBuffer, (void*)&xDummyEventWithParam1, sizeof(EventWithParam_1), &iBytesWritten), TRC_SUCCESS);
		assert_int_equal_cs(iBytesWritten, (int32_t)sizeof(EventWithParam_1));

		uiTotalBytesWritten += (uint32_t)iBytesWritten;
	}

	/* Tail should be 0 (skip), Head should not be 0 */
	assert_int_equal_cs(pxEventBuffer->uiTail, 0);
	assert_int_not_equal(pxEventBuffer->uiHead, 0);

	/* Head and tail should not match */
	assert_int_not_equal(pxEventBuffer->uiHead, pxEventBuffer->uiTail);

	/* Should succeed */
	MOCK_streamPortMaxTransferSize = 50;
	assert_int_equal_cs(xTraceEventBufferTransferAll(pxEventBuffer, &iBytesWritten), TRC_SUCCESS);

	/* Head and tail should not match */
	assert_int_not_equal(pxEventBuffer->uiHead, pxEventBuffer->uiTail);

	/* Verify number of bytes written */
	assert_int_equal_cs(MOCK_streamPortMaxTransferSize, (uint32_t)iBytesWritten);
	assert_int_equal_cs(pxEventBuffer->uiTail, (uint32_t)iBytesWritten);

	/* NOW we wrap and transfer that */
	assert_int_equal_cs(xTraceEventBufferPush(pxState->pvEvtBuffer, (void*)&xDummyEventWithParam1, sizeof(EventWithParam_1), &iBytesWritten), TRC_SUCCESS);
	assert_int_equal_cs(iBytesWritten, (int32_t)sizeof(EventWithParam_1));

	uiTotalBytesWritten += (uint32_t)iBytesWritten;

	/* Make sure we wrapped */
	assert_true(pxEventBuffer->uiHead < pxEventBuffer->uiTail);

	/* Should succeed */
	MOCK_streamPortMaxTransferSize = 50;
	assert_int_equal_cs(xTraceEventBufferTransferAll(pxEventBuffer, &iBytesWritten), TRC_SUCCESS);

	/* Make sure we are still wrapped */
	assert_true(pxEventBuffer->uiHead < pxEventBuffer->uiTail);

	/* Verify number of bytes written */
	assert_int_equal_cs(pxEventBuffer->uiTail, MOCK_streamPortMaxTransferSize * 2);

	/* Should succeed */
	MOCK_streamPortMaxTransferSize = 50;
	assert_int_equal_cs(xTraceEventBufferTransferAll(pxEventBuffer, &iBytesWritten), TRC_SUCCESS);

	/* Head and tail should now match */
	assert_int_equal_cs(pxEventBuffer->uiHead, pxEventBuffer->uiTail);
}

void test_xTraceEventBufferClear(void** ppState)
{
	EventBufferTestState_t* pxState = (EventBufferTestState_t*)*ppState;
	uint32_t uiBytesWritten = 0;

	EventWithParam_1 xDummyEventWithParam1 = { 0 };
	TraceEventBuffer_t* pxEventBuffer = ((TraceEventBuffer_t*)(pxState->pvEvtBuffer));
	xDummyEventWithParam1.base.EventID = ((1 & 0xF) << 12);

	assert_int_equal_cs(xTraceEventBufferClear(0), TRC_FAIL); /* null test */

	/* Push some event to the buffer so we can test clear */
	for (uint32_t i = 0; i < (pxEventBuffer->uiSize / sizeof(EventWithParam_1)) + 1; i++)
	{
		xDummyEventWithParam1.base.EventID = TRC_EVENT_SET_PARAM_COUNT(4012, 1);

		assert_int_equal_cs(xTraceEventBufferPush(pxState->pvEvtBuffer, (void*)&xDummyEventWithParam1, sizeof(EventWithParam_1), &uiBytesWritten), TRC_SUCCESS);
		assert_int_equal_cs(uiBytesWritten, sizeof(EventWithParam_1));
	}

	/* Head and tail should not be 0 (overwrite) */
	assert_int_not_equal(pxEventBuffer->uiTail, 0);
	assert_int_not_equal(pxEventBuffer->uiHead, 0);

	/* Head and tail should not match */
	assert_int_not_equal(pxEventBuffer->uiHead, pxEventBuffer->uiTail);

	/* Free and size should not match */
	assert_int_not_equal(pxEventBuffer->uiFree, pxEventBuffer->uiSize);

	/* Clear should succeed */
	assert_int_equal_cs(xTraceEventBufferClear(pxEventBuffer), TRC_SUCCESS);

	/* Head and tail should now be 0 */
	assert_int_equal_cs(pxEventBuffer->uiHead, 0);
	assert_int_equal_cs(pxEventBuffer->uiTail, 0);

	/* Free space should now equal size */
	assert_int_equal_cs(pxEventBuffer->uiFree, pxEventBuffer->uiSize);
}

uint32_t runEventBufferTests(void)
{
	uint32_t uiCountFailedTests = 0;

	const struct CMUnitTest aTests_trcEventBuffer[] =
	{
		cmocka_unit_test(test_xTraceEventBufferInitialize),
		cmocka_unit_test_setup_teardown(test_xTraceEventBufferAllocOverwrite, setup_freeEventBuffer, teardown_eventBuffer),
		cmocka_unit_test_setup_teardown(test_xTraceEventBufferFreePush, setup_freeEventBuffer, teardown_eventBuffer),
		cmocka_unit_test_setup_teardown(test_xTraceEventBufferSkipPush, setup_skipEventBuffer, teardown_eventBuffer),
		cmocka_unit_test_setup_teardown(test_xTraceEventBufferTransferChunk, setup_skipEventBuffer, teardown_eventBuffer),
		cmocka_unit_test_setup_teardown(test_xTraceEventBufferTransferAll_Normal, setup_skipEventBuffer, teardown_eventBuffer),
		cmocka_unit_test_setup_teardown(test_xTraceEventBufferTransferAll_Partial, setup_skipEventBuffer, teardown_eventBuffer),
		cmocka_unit_test_setup_teardown(test_xTraceEventBufferTransferAll_NormalWrapping, setup_skipEventBuffer, teardown_eventBuffer),
		cmocka_unit_test_setup_teardown(test_xTraceEventBufferTransferAll_PartialWrapping, setup_skipEventBuffer, teardown_eventBuffer),
		cmocka_unit_test_setup_teardown(test_xTraceEventBufferClear, setup_freeEventBuffer, teardown_eventBuffer),
	};

	uiCountFailedTests += cmocka_run_group_tests(aTests_trcEventBuffer, group_setup_EventBuffer, group_teardown_EventBuffer);
	
	return uiCountFailedTests;
}
