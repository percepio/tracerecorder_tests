#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <Event_UnitTest.h>
#include <trcRecorder.h>

static TraceUnsignedBaseType_t prvIs64bit(void);

static TraceUnsignedBaseType_t prvIs64bit(void)
{
	return sizeof(TraceUnsignedBaseType_t) == 8;
}

int group_setup_Event(void** ppState);
int group_teardown_Event(void** ppState);
int setup_Event(void** ppState);
int teardown_Event(void** ppState);

void test_xTraceEventInitialize(void** ppState);
void test_xTraceEventCreate0(void** ppState);
void test_xTraceEventCreate1(void** ppState);
void test_xTraceEventCreate2(void** ppState);
void test_xTraceEventCreate3(void** ppState);
void test_xTraceEventCreate4(void** ppState);
void test_xTraceEventCreate5(void** ppState);
void test_xTraceEventCreate6(void** ppState);
void test_xTraceEventCreateData0(void** ppState);
void test_xTraceEventCreateData1(void** ppState);
void test_xTraceEventCreateData2(void** ppState);
void test_xTraceEventCreateData3(void** ppState);
void test_xTraceEventCreateData4(void** ppState);
void test_xTraceEventCreateData5(void** ppState);
void test_xTraceEventCreateData6(void** ppState);
void test_xTraceEventGetSize(void** ppState);

/******************************************************************************************
*                                        TEST STATE                                       *
******************************************************************************************/

typedef struct {
	uint16_t EventID;
	uint16_t EventCount;
	uint32_t TS;
} BaseEvent;

typedef struct EventTestState
{
	/* Test Objects */
	TraceDiagnosticsData_t xDiagnosticsBuffer;
	TraceEventDataTable_t xEventDataBuffer;
} EventTestState_t;

EventTestState_t xEventTestState;

uint32_t MOCK_uiTestTimestampCounter = 0;

volatile uint32_t MOCK_RecorderEnabled = 1;
volatile uint32_t MOCK_RecorderInitialized = 1;
uint8_t MOCK_tmpBuffer[TRC_MAX_BLOB_SIZE];
uint8_t MOCK_streamPortBuffer[TRC_MAX_BLOB_SIZE];
uint32_t MOCK_uiCurrentCore = 0;
int32_t MOCK_iISRNesting = -1;
uint32_t MOCK_uiCriticalSection = 0;
uint32_t MOCK_uiCriticalSectionExpectedValue = 0;
uint32_t MOCK_streamPortMaxTransferSize = 0xFFFFFFFF;

/******************************************************************************************
*                                        SETUP / TEARDOWN                                 *
******************************************************************************************/

int group_setup_Event(void **ppState)
{
	*ppState = &xEventTestState;

	assert_int_equal(sizeof(TraceUnsignedBaseType_t), sizeof(void*));
	assert_int_equal(sizeof(TraceUnsignedBaseType_t), sizeof(TraceBaseType_t));

	return 0;
}

int group_teardown_Event(void **ppState)
{
	*ppState = 0;
	
	return 0;
}

int setup_Event(void **ppState)
{
	uint32_t i = 0;
	EventTestState_t* pxState = (EventTestState_t*)*ppState;

	assert_int_equal_cs(xTraceDiagnosticsInitialize(&pxState->xDiagnosticsBuffer), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEventInitialize(&xEventTestState.xEventDataBuffer), TRC_SUCCESS);

	for (i = 0; i < TRC_MAX_BLOB_SIZE; i++)
	{
		MOCK_tmpBuffer[i] = 0;
		MOCK_streamPortBuffer[i] = 0;
	}

	return 0;
}

int teardown_Event(void **ppState)
{
	EventTestState_t* pxState = (EventTestState_t*)*ppState;

	(void)pxState;

	return 0;
}

/******************************************************************************************
*                                        TESTS                                            *
******************************************************************************************/

void test_xTraceEventInitialize(void** ppState)
{
	assert_int_equal_asserts_enabled(xTraceEventInitialize(0), TRC_FAIL); /* null test */
	
	assert_int_equal_cs(xTraceEventInitialize(&xEventTestState.xEventDataBuffer), TRC_SUCCESS);
}

void test_xTraceEventCreate0(void** ppState)
{
	TraceUnsignedBaseType_t* pxParams = (TraceUnsignedBaseType_t*)&MOCK_streamPortBuffer[sizeof(TraceEvent0_t)];
	uint32_t uiSize = 0;

	MOCK_uiCriticalSection = 0;
	MOCK_uiCriticalSectionExpectedValue = 0;
	MOCK_uiTestTimestampCounter = 0x12345678;
	assert_int_equal_cs(xTraceEventCreate0(100), TRC_SUCCESS);

	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[0] & 0xFFF, 100);										/* event code */
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, 8 + 0 * sizeof(TraceUnsignedBaseType_t));
	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[1], 1);												/* event counter */
	assert_int_equal_cs(((uint32_t*)MOCK_streamPortBuffer)[1], 0x12345678);									/* timestamp */

	assert_int_equal_cs(pxParams[0], 0);
	assert_int_equal_cs(pxParams[1], 0);
	assert_int_equal_cs(pxParams[2], 0);
	assert_int_equal_cs(pxParams[3], 0);
	assert_int_equal_cs(pxParams[4], 0);
	assert_int_equal_cs(pxParams[5], 0);
	assert_int_equal_cs(pxParams[6], 0);

	MOCK_uiTestTimestampCounter = 0x87654321;
	assert_int_equal_cs(xTraceEventCreate0(200), TRC_SUCCESS);

	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[0] & 0xFFF, 200);										/* event code */
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, 8 + 0 * sizeof(TraceUnsignedBaseType_t));
	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[1], 2);												/* event counter */
	assert_int_equal_cs(((uint32_t*)MOCK_streamPortBuffer)[1], 0x87654321);									/* timestamp */

	assert_int_equal_cs(pxParams[0], 0);
	assert_int_equal_cs(pxParams[1], 0);
	assert_int_equal_cs(pxParams[2], 0);
	assert_int_equal_cs(pxParams[3], 0);
	assert_int_equal_cs(pxParams[4], 0);
	assert_int_equal_cs(pxParams[5], 0);
	assert_int_equal_cs(pxParams[6], 0);
}

void test_xTraceEventCreate1(void** ppState)
{
	TraceUnsignedBaseType_t* pxParams = (TraceUnsignedBaseType_t*)&MOCK_streamPortBuffer[sizeof(TraceEvent0_t)];
	uint32_t uiSize = 0;

	MOCK_uiCriticalSection = 0;
	MOCK_uiCriticalSectionExpectedValue = 0;
	MOCK_uiTestTimestampCounter = 0x12345678;
	assert_int_equal_cs(xTraceEventCreate1(101, 102), TRC_SUCCESS);

	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[0] & 0xFFF, 101);										/* event code */
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, 8 + 1 * sizeof(TraceUnsignedBaseType_t));
	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[1], 1);												/* event counter */
	assert_int_equal_cs(((uint32_t*)MOCK_streamPortBuffer)[1], 0x12345678);									/* timestamp */

	assert_int_equal_cs(pxParams[0], 102);
	assert_int_equal_cs(pxParams[1], 0);
	assert_int_equal_cs(pxParams[2], 0);
	assert_int_equal_cs(pxParams[3], 0);
	assert_int_equal_cs(pxParams[4], 0);
	assert_int_equal_cs(pxParams[5], 0);
	assert_int_equal_cs(pxParams[6], 0);

	MOCK_uiTestTimestampCounter = 0x87654321;
	assert_int_equal_cs(xTraceEventCreate1(201, 202), TRC_SUCCESS);

	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[0] & 0xFFF, 201);										/* event code */
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, 8 + 1 * sizeof(TraceUnsignedBaseType_t));
	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[1], 2);												/* event counter */
	assert_int_equal_cs(((uint32_t*)MOCK_streamPortBuffer)[1], 0x87654321);									/* timestamp */

	assert_int_equal_cs(pxParams[0], 202);
	assert_int_equal_cs(pxParams[1], 0);
	assert_int_equal_cs(pxParams[2], 0);
	assert_int_equal_cs(pxParams[3], 0);
	assert_int_equal_cs(pxParams[4], 0);
	assert_int_equal_cs(pxParams[5], 0);
	assert_int_equal_cs(pxParams[6], 0);
}

void test_xTraceEventCreate2(void** ppState)
{
	TraceUnsignedBaseType_t* pxParams = (TraceUnsignedBaseType_t*)&MOCK_streamPortBuffer[sizeof(TraceEvent0_t)];
	uint32_t uiSize = 0;

	MOCK_uiCriticalSection = 0;
	MOCK_uiCriticalSectionExpectedValue = 0;
	MOCK_uiTestTimestampCounter = 0x12345678;
	assert_int_equal_cs(xTraceEventCreate2(103, 104, 105), TRC_SUCCESS);

	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[0] & 0xFFF, 103);										/* event code */
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, 8 + 2 * sizeof(TraceUnsignedBaseType_t));
	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[1], 1);												/* event counter */
	assert_int_equal_cs(((uint32_t*)MOCK_streamPortBuffer)[1], 0x12345678);									/* timestamp */

	assert_int_equal_cs(pxParams[0], 104);
	assert_int_equal_cs(pxParams[1], 105);
	assert_int_equal_cs(pxParams[2], 0);
	assert_int_equal_cs(pxParams[3], 0);
	assert_int_equal_cs(pxParams[4], 0);
	assert_int_equal_cs(pxParams[5], 0);
	assert_int_equal_cs(pxParams[6], 0);

	MOCK_uiTestTimestampCounter = 0x87654321;
	assert_int_equal_cs(xTraceEventCreate2(203, 204, 205), TRC_SUCCESS);

	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[0] & 0xFFF, 203);										/* event code */
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, 8 + 2 * sizeof(TraceUnsignedBaseType_t));
	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[1], 2);												/* event counter */
	assert_int_equal_cs(((uint32_t*)MOCK_streamPortBuffer)[1], 0x87654321);									/* timestamp */

	assert_int_equal_cs(pxParams[0], 204);
	assert_int_equal_cs(pxParams[1], 205);
	assert_int_equal_cs(pxParams[2], 0);
	assert_int_equal_cs(pxParams[3], 0);
	assert_int_equal_cs(pxParams[4], 0);
	assert_int_equal_cs(pxParams[5], 0);
	assert_int_equal_cs(pxParams[6], 0);
}

void test_xTraceEventCreate3(void** ppState)
{
	TraceUnsignedBaseType_t* pxParams = (TraceUnsignedBaseType_t*)&MOCK_streamPortBuffer[sizeof(TraceEvent0_t)];
	uint32_t uiSize = 0;

	MOCK_uiCriticalSection = 0;
	MOCK_uiCriticalSectionExpectedValue = 0;
	MOCK_uiTestTimestampCounter = 0x12345678;
	assert_int_equal_cs(xTraceEventCreate3(106, 107, 108, 109), TRC_SUCCESS);

	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[0] & 0xFFF, 106);										/* event code */
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, 8 + 3 * sizeof(TraceUnsignedBaseType_t));
	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[1], 1);												/* event counter */
	assert_int_equal_cs(((uint32_t*)MOCK_streamPortBuffer)[1], 0x12345678);									/* timestamp */

	assert_int_equal_cs(pxParams[0], 107);
	assert_int_equal_cs(pxParams[1], 108);
	assert_int_equal_cs(pxParams[2], 109);
	assert_int_equal_cs(pxParams[3], 0);
	assert_int_equal_cs(pxParams[4], 0);
	assert_int_equal_cs(pxParams[5], 0);
	assert_int_equal_cs(pxParams[6], 0);

	MOCK_uiTestTimestampCounter = 0x87654321;
	assert_int_equal_cs(xTraceEventCreate3(206, 207, 208, 209), TRC_SUCCESS);

	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[0] & 0xFFF, 206);										/* event code */
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, 8 + 3 * sizeof(TraceUnsignedBaseType_t));
	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[1], 2);												/* event counter */
	assert_int_equal_cs(((uint32_t*)MOCK_streamPortBuffer)[1], 0x87654321);									/* timestamp */

	assert_int_equal_cs(pxParams[0], 207);
	assert_int_equal_cs(pxParams[1], 208);
	assert_int_equal_cs(pxParams[2], 209);
	assert_int_equal_cs(pxParams[3], 0);
	assert_int_equal_cs(pxParams[4], 0);
	assert_int_equal_cs(pxParams[5], 0);
	assert_int_equal_cs(pxParams[6], 0);
}

void test_xTraceEventCreate4(void** ppState)
{
	TraceUnsignedBaseType_t* pxParams = (TraceUnsignedBaseType_t*)&MOCK_streamPortBuffer[sizeof(TraceEvent0_t)];
	uint32_t uiSize = 0;
	
	MOCK_uiCriticalSection = 0;
	MOCK_uiCriticalSectionExpectedValue = 0;
	MOCK_uiTestTimestampCounter = 0x12345678;
	assert_int_equal_cs(xTraceEventCreate4(110, 111, 112, 113, 114), TRC_SUCCESS);

	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[0] & 0xFFF, 110);										/* event code */
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, 8 + 4 * sizeof(TraceUnsignedBaseType_t));
	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[1], 1);												/* event counter */
	assert_int_equal_cs(((uint32_t*)MOCK_streamPortBuffer)[1], 0x12345678);									/* timestamp */

	assert_int_equal_cs(pxParams[0], 111);
	assert_int_equal_cs(pxParams[1], 112);
	assert_int_equal_cs(pxParams[2], 113);
	assert_int_equal_cs(pxParams[3], 114);
	assert_int_equal_cs(pxParams[4], 0);
	assert_int_equal_cs(pxParams[5], 0);
	assert_int_equal_cs(pxParams[6], 0);

	MOCK_uiTestTimestampCounter = 0x87654321;
	assert_int_equal_cs(xTraceEventCreate4(210, 211, 212, 213, 214), TRC_SUCCESS);

	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[0] & 0xFFF, 210);										/* event code */
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, 8 + 4 * sizeof(TraceUnsignedBaseType_t));
	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[1], 2);												/* event counter */
	assert_int_equal_cs(((uint32_t*)MOCK_streamPortBuffer)[1], 0x87654321);									/* timestamp */

	assert_int_equal_cs(pxParams[0], 211);
	assert_int_equal_cs(pxParams[1], 212);
	assert_int_equal_cs(pxParams[2], 213);
	assert_int_equal_cs(pxParams[3], 214);
	assert_int_equal_cs(pxParams[4], 0);
	assert_int_equal_cs(pxParams[5], 0);
	assert_int_equal_cs(pxParams[6], 0);
}

void test_xTraceEventCreate5(void** ppState)
{
	TraceUnsignedBaseType_t* pxParams = (TraceUnsignedBaseType_t*)&MOCK_streamPortBuffer[sizeof(TraceEvent0_t)];
	uint32_t uiSize = 0;

	MOCK_uiCriticalSection = 0;
	MOCK_uiCriticalSectionExpectedValue = 0;
	MOCK_uiTestTimestampCounter = 0x12345678;
	assert_int_equal_cs(xTraceEventCreate5(115, 116, 117, 118, 119, 120), TRC_SUCCESS);

	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[0] & 0xFFF, 115);										/* event code */
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, 8 + 5 * sizeof(TraceUnsignedBaseType_t));
	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[1], 1);												/* event counter */
	assert_int_equal_cs(((uint32_t*)MOCK_streamPortBuffer)[1], 0x12345678);									/* timestamp */

	assert_int_equal_cs(pxParams[0], 116);
	assert_int_equal_cs(pxParams[1], 117);
	assert_int_equal_cs(pxParams[2], 118);
	assert_int_equal_cs(pxParams[3], 119);
	assert_int_equal_cs(pxParams[4], 120);
	assert_int_equal_cs(pxParams[5], 0);
	assert_int_equal_cs(pxParams[6], 0);

	MOCK_uiTestTimestampCounter = 0x87654321;
	assert_int_equal_cs(xTraceEventCreate5(215, 216, 217, 218, 219, 220), TRC_SUCCESS);

	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[0] & 0xFFF, 215);										/* event code */
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, 8 + 5 * sizeof(TraceUnsignedBaseType_t));
	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[1], 2);												/* event counter */
	assert_int_equal_cs(((uint32_t*)MOCK_streamPortBuffer)[1], 0x87654321);									/* timestamp */

	assert_int_equal_cs(pxParams[0], 216);
	assert_int_equal_cs(pxParams[1], 217);
	assert_int_equal_cs(pxParams[2], 218);
	assert_int_equal_cs(pxParams[3], 219);
	assert_int_equal_cs(pxParams[4], 220);
	assert_int_equal_cs(pxParams[5], 0);
	assert_int_equal_cs(pxParams[6], 0);
}

void test_xTraceEventCreate6(void** ppState)
{
	TraceUnsignedBaseType_t* pxParams = (TraceUnsignedBaseType_t*)&MOCK_streamPortBuffer[sizeof(TraceEvent0_t)];
	uint32_t uiSize = 0;

	MOCK_uiCriticalSection = 0;
	MOCK_uiCriticalSectionExpectedValue = 0;
	MOCK_uiTestTimestampCounter = 0x12345678;
	assert_int_equal_cs(xTraceEventCreate6(121, 122, 123, 124, 125, 126, 127), TRC_SUCCESS);

	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[0] & 0xFFF, 121);										/* event code */
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, 8 + 6 * sizeof(TraceUnsignedBaseType_t));
	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[1], 1);												/* event counter */
	assert_int_equal_cs(((uint32_t*)MOCK_streamPortBuffer)[1], 0x12345678);									/* timestamp */

	assert_int_equal_cs(pxParams[0], 122);
	assert_int_equal_cs(pxParams[1], 123);
	assert_int_equal_cs(pxParams[2], 124);
	assert_int_equal_cs(pxParams[3], 125);
	assert_int_equal_cs(pxParams[4], 126);
	assert_int_equal_cs(pxParams[5], 127);
	assert_int_equal_cs(pxParams[6], 0);

	MOCK_uiTestTimestampCounter = 0x87654321;
	assert_int_equal_cs(xTraceEventCreate6(221, 222, 223, 224, 225, 226, 227), TRC_SUCCESS);

	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[0] & 0xFFF, 221);										/* event code */
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, 8 + 6 * sizeof(TraceUnsignedBaseType_t));
	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[1], 2);												/* event counter */
	assert_int_equal_cs(((uint32_t*)MOCK_streamPortBuffer)[1], 0x87654321);									/* timestamp */

	assert_int_equal_cs(pxParams[0], 222);
	assert_int_equal_cs(pxParams[1], 223);
	assert_int_equal_cs(pxParams[2], 224);
	assert_int_equal_cs(pxParams[3], 225);
	assert_int_equal_cs(pxParams[4], 226);
	assert_int_equal_cs(pxParams[5], 227);
	assert_int_equal_cs(pxParams[6], 0);
}

void test_xTraceEventCreateData0(void** ppState)
{
	MOCK_uiCriticalSection = 0;
	MOCK_uiCriticalSectionExpectedValue = 0;
	MOCK_uiTestTimestampCounter = 0x12345678;
	uint8_t aBuffer[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	uint8_t bBuffer[] = { 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
	uint8_t cBuffer[] = { 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1 };
	uint32_t i;
	TraceUnsignedBaseType_t* pxParams = (TraceUnsignedBaseType_t*)&MOCK_streamPortBuffer[sizeof(TraceEvent0_t)];
	uint8_t* pData = (uint8_t*)&pxParams[0];
	uint32_t uiSize = 0;

	assert_int_equal_cs(xTraceEventCreateData0(100, (TraceUnsignedBaseType_t*)aBuffer, sizeof(aBuffer)), TRC_SUCCESS);

	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[0] & 0xFFF, 100);										/* event code */
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, 8 + 0 * sizeof(TraceUnsignedBaseType_t) + ((sizeof(aBuffer) + sizeof(TraceUnsignedBaseType_t) - 1) / sizeof(TraceUnsignedBaseType_t)) * sizeof(TraceUnsignedBaseType_t));	/* Param count should be set to cover entire data, therefor we "ceil align" */
	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[1], 1);												/* event counter */
	assert_int_equal_cs(((uint32_t*)MOCK_streamPortBuffer)[1], 0x12345678);										/* timestamp */

	for (i = 0; i < sizeof(aBuffer); i++)
	{
		assert_int_equal_cs(pData[i], aBuffer[i]);
	}

	MOCK_uiTestTimestampCounter = 0x87654321;
	assert_int_equal_cs(xTraceEventCreateData0(200, (TraceUnsignedBaseType_t*)bBuffer, sizeof(bBuffer)), TRC_SUCCESS);

	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[0] & 0xFFF, 200);										/* event code */
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, 8 + 0 * sizeof(TraceUnsignedBaseType_t) + ((sizeof(bBuffer) + sizeof(TraceUnsignedBaseType_t) - 1) / sizeof(TraceUnsignedBaseType_t)) * sizeof(TraceUnsignedBaseType_t));	/* Param count should be set to cover entire data, therefor we "ceil align" */
	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[1], 2);													/* event counter */
	assert_int_equal_cs(((uint32_t*)MOCK_streamPortBuffer)[1], 0x87654321);										/* timestamp */

	for (i = 0; i < sizeof(bBuffer); i++)
	{
		assert_int_equal_cs(pData[i], bBuffer[i]);
	}

	MOCK_uiTestTimestampCounter = 0x12341234;
	assert_int_equal_cs(xTraceEventCreateData0(300, (TraceUnsignedBaseType_t*)cBuffer, sizeof(cBuffer)), TRC_SUCCESS);

	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[0] & 0xFFF, 300);										/* event code */
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, (TRC_MAX_BLOB_SIZE));					/* Params should be capped by size! */
	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[1], 3);													/* event counter */
	assert_int_equal_cs(((uint32_t*)MOCK_streamPortBuffer)[1], 0x12341234);										/* timestamp */

	for (i = 0; i < (TRC_MAX_BLOB_SIZE - sizeof(TraceEvent0_t)); i++)
	{
		assert_int_equal_cs(pData[i], cBuffer[i]);
	}
}

void test_xTraceEventCreateData1(void** ppState)
{
	MOCK_uiCriticalSection = 0;
	MOCK_uiCriticalSectionExpectedValue = 0;
	MOCK_uiTestTimestampCounter = 0x12345678;
	uint8_t aBuffer[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	uint8_t bBuffer[] = { 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
	uint8_t cBuffer[] = { 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1 };
	uint32_t i;
	TraceUnsignedBaseType_t* pxParams = (TraceUnsignedBaseType_t*)&MOCK_streamPortBuffer[sizeof(TraceEvent0_t)];
	uint8_t* pData = (uint8_t*)&pxParams[1];
	uint32_t uiSize = 0;

	assert_int_equal_cs(xTraceEventCreateData1(101, 102, (TraceUnsignedBaseType_t*)aBuffer, sizeof(aBuffer)), TRC_SUCCESS);

	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[0] & 0xFFF, 101);										/* event code */
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, 8 + 1 * sizeof(TraceUnsignedBaseType_t) + ((sizeof(aBuffer) + sizeof(TraceUnsignedBaseType_t) - 1) / sizeof(TraceUnsignedBaseType_t)) * sizeof(TraceUnsignedBaseType_t));	/* Param count should be set to cover entire data, therefor we "ceil align" */
	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[1], 1);													/* event counter */
	assert_int_equal_cs(((uint32_t*)MOCK_streamPortBuffer)[1], 0x12345678);										/* timestamp */

	assert_int_equal_cs(pxParams[0], 102);

	for (i = 0; i < sizeof(aBuffer); i++)
	{
		assert_int_equal_cs(pData[i], aBuffer[i]);
	}

	MOCK_uiTestTimestampCounter = 0x87654321;
	assert_int_equal_cs(xTraceEventCreateData1(201, 202, (TraceUnsignedBaseType_t*)bBuffer, sizeof(bBuffer)), TRC_SUCCESS);

	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[0] & 0xFFF, 201);										/* event code */
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, 8 + 1 * sizeof(TraceUnsignedBaseType_t) + ((sizeof(bBuffer) + sizeof(TraceUnsignedBaseType_t) - 1) / sizeof(TraceUnsignedBaseType_t)) * sizeof(TraceUnsignedBaseType_t));	/* Param count should be set to cover entire data, therefor we "ceil align" */
	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[1], 2);													/* event counter */
	assert_int_equal_cs(((uint32_t*)MOCK_streamPortBuffer)[1], 0x87654321);										/* timestamp */

	assert_int_equal_cs(pxParams[0], 202);

	for (i = 0; i < sizeof(bBuffer); i++)
	{
		assert_int_equal_cs(pData[i], bBuffer[i]);
	}

	MOCK_uiTestTimestampCounter = 0x12341234;
	assert_int_equal_cs(xTraceEventCreateData1(301, 302, (TraceUnsignedBaseType_t*)cBuffer, sizeof(cBuffer)), TRC_SUCCESS);

	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[0] & 0xFFF, 301);										/* event code */
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, (TRC_MAX_BLOB_SIZE));					/* Params should be capped by size! */
	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[1], 3);													/* event counter */
	assert_int_equal_cs(((uint32_t*)MOCK_streamPortBuffer)[1], 0x12341234);										/* timestamp */

	assert_int_equal_cs(pxParams[0], 302);

	for (i = 0; i < (TRC_MAX_BLOB_SIZE - sizeof(TraceEvent0_t) - 1 * sizeof(TraceUnsignedBaseType_t)); i++)
	{
		assert_int_equal_cs(pData[i], cBuffer[i]);
	}
}

void test_xTraceEventCreateData2(void** ppState)
{
	MOCK_uiCriticalSection = 0;
	MOCK_uiCriticalSectionExpectedValue = 0;
	MOCK_uiTestTimestampCounter = 0x12345678;
	uint8_t aBuffer[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	uint8_t bBuffer[] = { 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
	uint8_t cBuffer[] = { 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1 };
	uint32_t i;
	TraceUnsignedBaseType_t* pxParams = (TraceUnsignedBaseType_t*)&MOCK_streamPortBuffer[sizeof(TraceEvent0_t)];
	uint8_t* pData = (uint8_t*)&pxParams[2];
	uint32_t uiSize = 0;

	assert_int_equal_cs(xTraceEventCreateData2(103, 104, 105, (TraceUnsignedBaseType_t*)aBuffer, sizeof(aBuffer)), TRC_SUCCESS);

	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[0] & 0xFFF, 103);										/* event code */
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, 8 + 2 * sizeof(TraceUnsignedBaseType_t) + ((sizeof(aBuffer) + sizeof(TraceUnsignedBaseType_t) - 1) / sizeof(TraceUnsignedBaseType_t)) * sizeof(TraceUnsignedBaseType_t));	/* Param count should be set to cover entire data, therefor we "ceil align" */
	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[1], 1);													/* event counter */
	assert_int_equal_cs(((uint32_t*)MOCK_streamPortBuffer)[1], 0x12345678);										/* timestamp */

	assert_int_equal_cs(pxParams[0], 104);
	assert_int_equal_cs(pxParams[1], 105);

	for (i = 0; i < sizeof(aBuffer); i++)
	{
		assert_int_equal_cs(pData[i], aBuffer[i]);
	}

	MOCK_uiTestTimestampCounter = 0x87654321;
	assert_int_equal_cs(xTraceEventCreateData2(203, 204, 205, (TraceUnsignedBaseType_t*)bBuffer, sizeof(bBuffer)), TRC_SUCCESS);

	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[0] & 0xFFF, 203);										/* event code */
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, 8 + 2 * sizeof(TraceUnsignedBaseType_t) + ((sizeof(bBuffer) + sizeof(TraceUnsignedBaseType_t) - 1) / sizeof(TraceUnsignedBaseType_t)) * sizeof(TraceUnsignedBaseType_t));	/* Param count should be set to cover entire data, therefor we "ceil align" */
	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[1], 2);													/* event counter */
	assert_int_equal_cs(((uint32_t*)MOCK_streamPortBuffer)[1], 0x87654321);										/* timestamp */

	assert_int_equal_cs(pxParams[0], 204);
	assert_int_equal_cs(pxParams[1], 205);

	for (i = 0; i < sizeof(bBuffer); i++)
	{
		assert_int_equal_cs(pData[i], bBuffer[i]);
	}

	MOCK_uiTestTimestampCounter = 0x12341234;
	assert_int_equal_cs(xTraceEventCreateData2(303, 304, 305, (TraceUnsignedBaseType_t*)cBuffer, sizeof(cBuffer)), TRC_SUCCESS);

	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[0] & 0xFFF, 303);										/* event code */
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, (TRC_MAX_BLOB_SIZE));					/* Params should be capped by size! */
	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[1], 3);													/* event counter */
	assert_int_equal_cs(((uint32_t*)MOCK_streamPortBuffer)[1], 0x12341234);										/* timestamp */

	assert_int_equal_cs(pxParams[0], 304);
	assert_int_equal_cs(pxParams[1], 305);

	for (i = 0; i < (TRC_MAX_BLOB_SIZE - sizeof(TraceEvent0_t) - 2 * sizeof(TraceUnsignedBaseType_t)); i++)
	{
		assert_int_equal_cs(pData[i], cBuffer[i]);
	}
}

void test_xTraceEventCreateData3(void** ppState)
{
	MOCK_uiCriticalSection = 0;
	MOCK_uiCriticalSectionExpectedValue = 0;
	MOCK_uiTestTimestampCounter = 0x12345678;
	uint8_t aBuffer[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	uint8_t bBuffer[] = { 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
	uint8_t cBuffer[] = { 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1 };
	uint32_t i;
	TraceUnsignedBaseType_t* pxParams = (TraceUnsignedBaseType_t*)&MOCK_streamPortBuffer[sizeof(TraceEvent0_t)];
	uint8_t* pData = (uint8_t*)&pxParams[3];
	uint32_t uiSize = 0;

	assert_int_equal_cs(xTraceEventCreateData3(106, 107, 108, 109, (TraceUnsignedBaseType_t*)aBuffer, sizeof(aBuffer)), TRC_SUCCESS);

	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[0] & 0xFFF, 106);										/* event code */
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, 8 + 3 * sizeof(TraceUnsignedBaseType_t) + ((sizeof(aBuffer) + sizeof(TraceUnsignedBaseType_t) - 1) / sizeof(TraceUnsignedBaseType_t)) * sizeof(TraceUnsignedBaseType_t));	/* Param count should be set to cover entire data, therefor we "ceil align" */
	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[1], 1);													/* event counter */
	assert_int_equal_cs(((uint32_t*)MOCK_streamPortBuffer)[1], 0x12345678);										/* timestamp */

	assert_int_equal_cs(pxParams[0], 107);
	assert_int_equal_cs(pxParams[1], 108);
	assert_int_equal_cs(pxParams[2], 109);

	for (i = 0; i < sizeof(aBuffer); i++)
	{
		assert_int_equal_cs(pData[i], aBuffer[i]);
	}

	MOCK_uiTestTimestampCounter = 0x87654321;
	assert_int_equal_cs(xTraceEventCreateData3(206, 207, 208, 209, (TraceUnsignedBaseType_t*)bBuffer, sizeof(bBuffer)), TRC_SUCCESS);

	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[0] & 0xFFF, 206);										/* event code */
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, 8 + 3 * sizeof(TraceUnsignedBaseType_t) + ((sizeof(bBuffer) + sizeof(TraceUnsignedBaseType_t) - 1) / sizeof(TraceUnsignedBaseType_t)) * sizeof(TraceUnsignedBaseType_t));	/* Param count should be set to cover entire data, therefor we "ceil align" */
	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[1], 2);													/* event counter */
	assert_int_equal_cs(((uint32_t*)MOCK_streamPortBuffer)[1], 0x87654321);										/* timestamp */

	assert_int_equal_cs(pxParams[0], 207);
	assert_int_equal_cs(pxParams[1], 208);
	assert_int_equal_cs(pxParams[2], 209);

	for (i = 0; i < sizeof(bBuffer); i++)
	{
		assert_int_equal_cs(pData[i], bBuffer[i]);
	}

	MOCK_uiTestTimestampCounter = 0x12341234;
	assert_int_equal_cs(xTraceEventCreateData3(306, 307, 308, 309, (TraceUnsignedBaseType_t*)cBuffer, sizeof(cBuffer)), TRC_SUCCESS);

	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[0] & 0xFFF, 306);										/* event code */
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, (TRC_MAX_BLOB_SIZE));					/* Params should be capped by size! */
	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[1], 3);													/* event counter */
	assert_int_equal_cs(((uint32_t*)MOCK_streamPortBuffer)[1], 0x12341234);										/* timestamp */

	assert_int_equal_cs(pxParams[0], 307);
	assert_int_equal_cs(pxParams[1], 308);
	assert_int_equal_cs(pxParams[2], 309);
	
	for (i = 0; i < (TRC_MAX_BLOB_SIZE - sizeof(TraceEvent0_t) - 3 * sizeof(TraceUnsignedBaseType_t)); i++)
	{
		assert_int_equal_cs(pData[i], cBuffer[i]);
	}
}

void test_xTraceEventCreateData4(void** ppState)
{
	MOCK_uiCriticalSection = 0;
	MOCK_uiCriticalSectionExpectedValue = 0;
	MOCK_uiTestTimestampCounter = 0x12345678;
	uint8_t aBuffer[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	uint8_t bBuffer[] = { 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
	uint8_t cBuffer[] = { 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1 };
	uint32_t i;
	TraceUnsignedBaseType_t* pxParams = (TraceUnsignedBaseType_t*)&MOCK_streamPortBuffer[sizeof(TraceEvent0_t)];
	uint8_t* pData = (uint8_t*)&pxParams[4];
	uint32_t uiSize = 0;

	assert_int_equal_cs(xTraceEventCreateData4(110, 111, 112, 113, 114, (TraceUnsignedBaseType_t*)aBuffer, sizeof(aBuffer)), TRC_SUCCESS);

	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[0] & 0xFFF, 110);										/* event code */
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, 8 + 4 * sizeof(TraceUnsignedBaseType_t) + ((sizeof(aBuffer) + sizeof(TraceUnsignedBaseType_t) - 1) / sizeof(TraceUnsignedBaseType_t)) * sizeof(TraceUnsignedBaseType_t));	/* Param count should be set to cover entire data, therefor we "ceil align" */
	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[1], 1);													/* event counter */
	assert_int_equal_cs(((uint32_t*)MOCK_streamPortBuffer)[1], 0x12345678);										/* timestamp */

	assert_int_equal_cs(pxParams[0], 111);
	assert_int_equal_cs(pxParams[1], 112);
	assert_int_equal_cs(pxParams[2], 113);
	assert_int_equal_cs(pxParams[3], 114);

	for (i = 0; i < sizeof(aBuffer); i++)
	{
		assert_int_equal_cs(pData[i], aBuffer[i]);
	}

	MOCK_uiTestTimestampCounter = 0x87654321;
	assert_int_equal_cs(xTraceEventCreateData4(210, 211, 212, 213, 214, (TraceUnsignedBaseType_t*)bBuffer, sizeof(bBuffer)), TRC_SUCCESS);

	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[0] & 0xFFF, 210);										/* event code */
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, 8 + 4 * sizeof(TraceUnsignedBaseType_t) + ((sizeof(bBuffer) + sizeof(TraceUnsignedBaseType_t) - 1) / sizeof(TraceUnsignedBaseType_t)) * sizeof(TraceUnsignedBaseType_t));	/* Param count should be set to cover entire data, therefor we "ceil align" */
	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[1], 2);													/* event counter */
	assert_int_equal_cs(((uint32_t*)MOCK_streamPortBuffer)[1], 0x87654321);										/* timestamp */

	assert_int_equal_cs(pxParams[0], 211);
	assert_int_equal_cs(pxParams[1], 212);
	assert_int_equal_cs(pxParams[2], 213);
	assert_int_equal_cs(pxParams[3], 214);

	for (i = 0; i < sizeof(bBuffer); i++)
	{
		assert_int_equal_cs(pData[i], bBuffer[i]);
	}

	MOCK_uiTestTimestampCounter = 0x12341234;
	assert_int_equal_cs(xTraceEventCreateData4(310, 311, 312, 313, 314, (TraceUnsignedBaseType_t*)cBuffer, sizeof(cBuffer)), TRC_SUCCESS);

	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[0] & 0xFFF, 310);										/* event code */
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, (TRC_MAX_BLOB_SIZE));					/* Params should be capped by size! */
	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[1], 3);													/* event counter */
	assert_int_equal_cs(((uint32_t*)MOCK_streamPortBuffer)[1], 0x12341234);										/* timestamp */

	assert_int_equal_cs(pxParams[0], 311);
	assert_int_equal_cs(pxParams[1], 312);
	assert_int_equal_cs(pxParams[2], 313);
	assert_int_equal_cs(pxParams[3], 314);

	for (i = 0; i < (TRC_MAX_BLOB_SIZE - sizeof(TraceEvent0_t) - 4 * sizeof(TraceUnsignedBaseType_t)); i++)
	{
		assert_int_equal_cs(pData[i], cBuffer[i]);
	}
}

void test_xTraceEventCreateData5(void** ppState)
{
	MOCK_uiCriticalSection = 0;
	MOCK_uiCriticalSectionExpectedValue = 0;
	MOCK_uiTestTimestampCounter = 0x12345678;
	uint8_t aBuffer[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	uint8_t bBuffer[] = { 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
	uint8_t cBuffer[] = { 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1 };
	uint32_t i;
	TraceUnsignedBaseType_t* pxParams = (TraceUnsignedBaseType_t*)&MOCK_streamPortBuffer[sizeof(TraceEvent0_t)];
	uint8_t* pData = (uint8_t*)&pxParams[5];
	uint32_t uiSize = 0;

	assert_int_equal_cs(xTraceEventCreateData5(115, 116, 117, 118, 119, 120, (TraceUnsignedBaseType_t*)aBuffer, sizeof(aBuffer)), TRC_SUCCESS);

	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[0] & 0xFFF, 115);										/* event code */
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, 8 + 5 * sizeof(TraceUnsignedBaseType_t) + ((sizeof(aBuffer) + sizeof(TraceUnsignedBaseType_t) - 1) / sizeof(TraceUnsignedBaseType_t)) * sizeof(TraceUnsignedBaseType_t));	/* Param count should be set to cover entire data, therefor we "ceil align" */
	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[1], 1);													/* event counter */
	assert_int_equal_cs(((uint32_t*)MOCK_streamPortBuffer)[1], 0x12345678);										/* timestamp */

	assert_int_equal_cs(pxParams[0], 116);
	assert_int_equal_cs(pxParams[1], 117);
	assert_int_equal_cs(pxParams[2], 118);
	assert_int_equal_cs(pxParams[3], 119);
	assert_int_equal_cs(pxParams[4], 120);

	for (i = 0; i < sizeof(aBuffer); i++)
	{
		assert_int_equal_cs(pData[i], aBuffer[i]);
	}

	MOCK_uiTestTimestampCounter = 0x87654321;
	assert_int_equal_cs(xTraceEventCreateData5(215, 216, 217, 218, 219, 220, (TraceUnsignedBaseType_t*)bBuffer, sizeof(bBuffer)), TRC_SUCCESS);

	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[0] & 0xFFF, 215);										/* event code */
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, 8 + 5 * sizeof(TraceUnsignedBaseType_t) + ((sizeof(bBuffer) + sizeof(TraceUnsignedBaseType_t) - 1) / sizeof(TraceUnsignedBaseType_t)) * sizeof(TraceUnsignedBaseType_t));	/* Param count should be set to cover entire data, therefor we "ceil align" */
	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[1], 2);													/* event counter */
	assert_int_equal_cs(((uint32_t*)MOCK_streamPortBuffer)[1], 0x87654321);										/* timestamp */

	assert_int_equal_cs(pxParams[0], 216);
	assert_int_equal_cs(pxParams[1], 217);
	assert_int_equal_cs(pxParams[2], 218);
	assert_int_equal_cs(pxParams[3], 219);
	assert_int_equal_cs(pxParams[4], 220);

	for (i = 0; i < sizeof(bBuffer); i++)
	{
		assert_int_equal_cs(pData[i], bBuffer[i]);
	}

	MOCK_uiTestTimestampCounter = 0x12341234;
	assert_int_equal_cs(xTraceEventCreateData5(315, 316, 317, 318, 319, 320, (TraceUnsignedBaseType_t*)cBuffer, sizeof(cBuffer)), TRC_SUCCESS);

	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[0] & 0xFFF, 315);										/* event code */
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, (TRC_MAX_BLOB_SIZE));					/* Params should be capped by size! */
	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[1], 3);													/* event counter */
	assert_int_equal_cs(((uint32_t*)MOCK_streamPortBuffer)[1], 0x12341234);										/* timestamp */

	assert_int_equal_cs(pxParams[0], 316);
	assert_int_equal_cs(pxParams[1], 317);
	assert_int_equal_cs(pxParams[2], 318);
	assert_int_equal_cs(pxParams[3], 319);
	assert_int_equal_cs(pxParams[4], 320);

	for (i = 0; i < (TRC_MAX_BLOB_SIZE - sizeof(TraceEvent0_t) - 5 * sizeof(TraceUnsignedBaseType_t)); i++)
	{
		assert_int_equal_cs(pData[i], cBuffer[i]);
	}
}

void test_xTraceEventCreateData6(void** ppState)
{
	MOCK_uiCriticalSection = 0;
	MOCK_uiCriticalSectionExpectedValue = 0;
	MOCK_uiTestTimestampCounter = 0x12345678;
	uint8_t aBuffer[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	uint8_t bBuffer[] = { 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
	uint8_t cBuffer[] = { 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1 };
	uint32_t i;
	TraceUnsignedBaseType_t* pxParams = (TraceUnsignedBaseType_t*)&MOCK_streamPortBuffer[sizeof(TraceEvent0_t)];
	uint8_t* pData = (uint8_t*)&pxParams[6];
	uint32_t uiSize = 0;

	assert_int_equal_cs(xTraceEventCreateData6(121, 122, 123, 124, 125, 126, 127, (TraceUnsignedBaseType_t*)aBuffer, sizeof(aBuffer)), TRC_SUCCESS);

	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[0] & 0xFFF, 121);										/* event code */
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, 8 + 6 * sizeof(TraceUnsignedBaseType_t) + ((sizeof(aBuffer) + sizeof(TraceUnsignedBaseType_t) - 1) / sizeof(TraceUnsignedBaseType_t)) * sizeof(TraceUnsignedBaseType_t));	/* Param count should be set to cover entire data, therefor we "ceil align" */
	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[1], 1);													/* event counter */
	assert_int_equal_cs(((uint32_t*)MOCK_streamPortBuffer)[1], 0x12345678);										/* timestamp */

	assert_int_equal_cs(pxParams[0], 122);
	assert_int_equal_cs(pxParams[1], 123);
	assert_int_equal_cs(pxParams[2], 124);
	assert_int_equal_cs(pxParams[3], 125);
	assert_int_equal_cs(pxParams[4], 126);
	assert_int_equal_cs(pxParams[5], 127);

	for (i = 0; i < sizeof(aBuffer); i++)
	{
		assert_int_equal_cs(pData[i], aBuffer[i]);
	}

	MOCK_uiTestTimestampCounter = 0x87654321;
	assert_int_equal_cs(xTraceEventCreateData6(221, 222, 223, 224, 225, 226, 227, (TraceUnsignedBaseType_t*)bBuffer, sizeof(bBuffer)), TRC_SUCCESS);

	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[0] & 0xFFF, 221);										/* event code */
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, 8 + 6 * sizeof(TraceUnsignedBaseType_t) + ((sizeof(bBuffer) + sizeof(TraceUnsignedBaseType_t) - 1) / sizeof(TraceUnsignedBaseType_t)) * sizeof(TraceUnsignedBaseType_t));	/* Param count should be set to cover entire data, therefor we "ceil align" */
	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[1], 2);													/* event counter */
	assert_int_equal_cs(((uint32_t*)MOCK_streamPortBuffer)[1], 0x87654321);										/* timestamp */

	assert_int_equal_cs(pxParams[0], 222);
	assert_int_equal_cs(pxParams[1], 223);
	assert_int_equal_cs(pxParams[2], 224);
	assert_int_equal_cs(pxParams[3], 225);
	assert_int_equal_cs(pxParams[4], 226);
	assert_int_equal_cs(pxParams[5], 227);

	for (i = 0; i < sizeof(bBuffer); i++)
	{
		assert_int_equal_cs(pData[i], bBuffer[i]);
	}

	MOCK_uiTestTimestampCounter = 0x12341234;
	assert_int_equal_cs(xTraceEventCreateData6(321, 322, 323, 324, 325, 326, 327, (TraceUnsignedBaseType_t*)cBuffer, sizeof(cBuffer)), TRC_SUCCESS);

	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[0] & 0xFFF, 321);										/* event code */
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, (TRC_MAX_BLOB_SIZE));					/* Params should be capped by size! */
	assert_int_equal_cs(((uint16_t*)MOCK_streamPortBuffer)[1], 3);													/* event counter */
	assert_int_equal_cs(((uint32_t*)MOCK_streamPortBuffer)[1], 0x12341234);										/* timestamp */

	assert_int_equal_cs(pxParams[0], 322);
	assert_int_equal_cs(pxParams[1], 323);
	assert_int_equal_cs(pxParams[2], 324);
	assert_int_equal_cs(pxParams[3], 325);
	assert_int_equal_cs(pxParams[4], 326);
	assert_int_equal_cs(pxParams[5], 327);

	for (i = 0; i < (TRC_MAX_BLOB_SIZE - sizeof(TraceEvent0_t) - 6 * sizeof(TraceUnsignedBaseType_t)); i++)
	{
		assert_int_equal_cs(pData[i], cBuffer[i]);
	}
}

void test_xTraceEventGetSize(void** ppState)
{
	uint32_t asd[16];
	uint32_t uiSize = 0;

	MOCK_uiCriticalSection = 0;
	assert_int_equal_asserts_enabled(xTraceEventGetSize(0, &uiSize), TRC_FAIL); /* null test */
	assert_int_equal_asserts_enabled(xTraceEventGetSize((void*)(TraceUnsignedBaseType_t)0xDEADBEEF, 0), TRC_FAIL); /* null test */

	MOCK_uiCriticalSectionExpectedValue = 0;

	assert_int_equal_cs(xTraceEventCreate0(1), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, 8 + 0 * sizeof(TraceUnsignedBaseType_t));

	assert_int_equal_cs(xTraceEventCreate1(1, 2), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, 8 + 1 * sizeof(TraceUnsignedBaseType_t));

	assert_int_equal_cs(xTraceEventCreate2(1, 2, 3), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, 8 + 2 * sizeof(TraceUnsignedBaseType_t));

	assert_int_equal_cs(xTraceEventCreate3(1, 2, 3, 4), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, 8 + 3 * sizeof(TraceUnsignedBaseType_t));

	assert_int_equal_cs(xTraceEventCreate4(1, 2, 3, 4, 5), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, 8 + 4 * sizeof(TraceUnsignedBaseType_t));

	assert_int_equal_cs(xTraceEventCreate5(1, 2, 3, 4, 5, 6), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, 8 + 5 * sizeof(TraceUnsignedBaseType_t));

	assert_int_equal_cs(xTraceEventCreate6(1, 2, 3, 4, 5, 6, 7), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, 8 + 6 * sizeof(TraceUnsignedBaseType_t));

	assert_int_equal_cs(xTraceEventCreateData0(1, (TraceUnsignedBaseType_t*)asd, 2), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, 8 + 0 * sizeof(TraceUnsignedBaseType_t) + sizeof(TraceUnsignedBaseType_t));

	assert_int_equal_cs(xTraceEventCreateData1(1, 2, (TraceUnsignedBaseType_t*)asd, sizeof(TraceUnsignedBaseType_t)), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, 8 + 1 * sizeof(TraceUnsignedBaseType_t) + sizeof(TraceUnsignedBaseType_t));

	assert_int_equal_cs(xTraceEventCreateData2(1, 2, 3, (TraceUnsignedBaseType_t*)asd, 2 * sizeof(TraceUnsignedBaseType_t)), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, 8 + 2 * sizeof(TraceUnsignedBaseType_t) + 2 * sizeof(TraceUnsignedBaseType_t));

	assert_int_equal_cs(xTraceEventCreateData3(1, 2, 3, 4, (TraceUnsignedBaseType_t*)asd, 2 * sizeof(TraceUnsignedBaseType_t)), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, 8 + 3 * sizeof(TraceUnsignedBaseType_t) + 2 * sizeof(TraceUnsignedBaseType_t));

	assert_int_equal_cs(xTraceEventCreateData4(1, 2, 3, 4, 5, (TraceUnsignedBaseType_t*)asd, 1), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, 8 + 4 * sizeof(TraceUnsignedBaseType_t) + sizeof(TraceUnsignedBaseType_t));

	assert_int_equal_cs(xTraceEventCreateData5(1, 2, 3, 4, 5, 6, (TraceUnsignedBaseType_t*)asd, sizeof(TraceUnsignedBaseType_t)), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, 8 + 5 * sizeof(TraceUnsignedBaseType_t) + sizeof(TraceUnsignedBaseType_t));

	assert_int_equal_cs(xTraceEventCreateData6(1, 2, 3, 4, 5, 6, 7, (TraceUnsignedBaseType_t*)asd, 2 * sizeof(TraceUnsignedBaseType_t)), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEventGetSize((void*)MOCK_streamPortBuffer, &uiSize), TRC_SUCCESS);
	assert_int_equal_cs(uiSize, 8 + 6 * sizeof(TraceUnsignedBaseType_t) + 2 * sizeof(TraceUnsignedBaseType_t));
}

void TRACE_ASSERTS_DISABLED(void** ppState)
{
}

uint32_t runEventTests(void)
{
	uint32_t uiCountFailedTests = 0;

	const struct CMUnitTest aTests_trcEvent[] =
	{
#if (TRC_CFG_USE_TRACE_ASSERT == 0)
		cmocka_unit_test(TRACE_ASSERTS_DISABLED),
#endif
		cmocka_unit_test(test_xTraceEventInitialize),
		cmocka_unit_test_setup_teardown(test_xTraceEventCreate0, setup_Event, teardown_Event),
		cmocka_unit_test_setup_teardown(test_xTraceEventCreate1, setup_Event, teardown_Event),
		cmocka_unit_test_setup_teardown(test_xTraceEventCreate2, setup_Event, teardown_Event),
		cmocka_unit_test_setup_teardown(test_xTraceEventCreate3, setup_Event, teardown_Event),
		cmocka_unit_test_setup_teardown(test_xTraceEventCreate4, setup_Event, teardown_Event),
		cmocka_unit_test_setup_teardown(test_xTraceEventCreate5, setup_Event, teardown_Event),
		cmocka_unit_test_setup_teardown(test_xTraceEventCreate6, setup_Event, teardown_Event),
		cmocka_unit_test_setup_teardown(test_xTraceEventCreateData0, setup_Event, teardown_Event),
		cmocka_unit_test_setup_teardown(test_xTraceEventCreateData1, setup_Event, teardown_Event),
		cmocka_unit_test_setup_teardown(test_xTraceEventCreateData2, setup_Event, teardown_Event),
		cmocka_unit_test_setup_teardown(test_xTraceEventCreateData3, setup_Event, teardown_Event),
		cmocka_unit_test_setup_teardown(test_xTraceEventCreateData4, setup_Event, teardown_Event),
		cmocka_unit_test_setup_teardown(test_xTraceEventCreateData5, setup_Event, teardown_Event),
		cmocka_unit_test_setup_teardown(test_xTraceEventCreateData6, setup_Event, teardown_Event),
		cmocka_unit_test_setup_teardown(test_xTraceEventGetSize, setup_Event, teardown_Event),
#if (TRC_CFG_USE_TRACE_ASSERT == 0)
		cmocka_unit_test(TRACE_ASSERTS_DISABLED),
#endif
	};

	uiCountFailedTests += cmocka_run_group_tests(aTests_trcEvent, group_setup_Event, group_teardown_Event);

	return uiCountFailedTests;
}
