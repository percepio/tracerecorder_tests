#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <EventVerification.h>
#include <EventVerification_UnitTest.h>
#include <trcRecorder.h>

int group_setup_EventVerification(void** ppState);
int group_teardown_EventVerification(void** ppState);
int setup_EventVerification(void** ppState);
int teardown_EventVerification(void** ppState);

void test_xTraceEventExpect(void** ppState);

/******************************************************************************************
*                                        TEST STATE                                       *
******************************************************************************************/

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

int group_setup_EventVerification(void **ppState)
{
	*ppState = &xEventTestState;

	assert_int_equal(sizeof(TraceUnsignedBaseType_t), sizeof(void*));
	assert_int_equal(sizeof(TraceUnsignedBaseType_t), sizeof(TraceBaseType_t));

	return 0;
}

int group_teardown_EventVerification(void **ppState)
{
	*ppState = 0;
	
	return 0;
}

int setup_EventVerification(void **ppState)
{
	EventTestState_t* pxState = (EventTestState_t*)*ppState;

	assert_int_equal_cs(xTraceDiagnosticsInitialize(&pxState->xDiagnosticsBuffer), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEventInitialize(&xEventTestState.xEventDataBuffer), TRC_SUCCESS);

	return 0;
}

int teardown_EventVerification(void **ppState)
{
	EventTestState_t* pxState = (EventTestState_t*)*ppState;

	return 0;
}

/******************************************************************************************
*                                        TESTS                                            *
******************************************************************************************/

void test_xTraceEventExpect(void** ppState)
{
	TraceEventHandle_t xEventHandle = 0;
	TraceUnsignedBaseType_t line = 0;
	TraceUnsignedBaseType_t paramOffset = sizeof(TraceEvent0_t) + sizeof(void*) + sizeof(TraceUnsignedBaseType_t) + sizeof(TraceUnsignedBaseType_t);

	MOCK_uiCriticalSectionExpectedValue = 0;

	TraceEvent1_t* te1 = (TraceEvent1_t*)MOCK_streamPortBuffer;
	TraceEvent2_t* te2 = (TraceEvent2_t*)MOCK_streamPortBuffer;
	TraceEvent3_t* te3 = (TraceEvent3_t*)MOCK_streamPortBuffer;
	TraceEvent4_t* te4 = (TraceEvent4_t*)MOCK_streamPortBuffer;
	TraceEvent5_t* te5 = (TraceEvent5_t*)MOCK_streamPortBuffer;
	TraceEvent6_t* te6 = (TraceEvent6_t*)MOCK_streamPortBuffer;
	
	assert_int_equal_cs(xTraceEventExpectNever(1234), TRC_SUCCESS); line = __LINE__;
	assert_int_equal_cs(te1->EventID & 0xFFFu, PSF_EVENT_EXPECT_EMPTY);
	assert_int_equal_cs(te1->uxParams[0], 1234);

	assert_int_equal_cs(xTraceEventExpectEmpty((void*)0x1EADBEEF), TRC_SUCCESS); line = __LINE__;
	assert_int_equal_cs(te2->EventID & 0xFFFu, PSF_EVENT_EXPECT_EMPTY);
	assert_int_equal_cs(te2->uxParams[0], 0x1EADBEEF);
	assert_int_equal_cs(te2->uxParams[1], line);

	xTraceEventExpect_0((void*)0x01, 0x11); line = __LINE__;
	assert_int_equal_cs(te3->EventID & 0xFFFu, PSF_EVENT_EXPECT);
	assert_int_equal_cs(te3->uxParams[0], 0x01);
	assert_int_equal_cs(te3->uxParams[1], line);
	assert_int_equal_cs(te3->uxParams[2], 0x11);

	xTraceEventExpect_Handle_0((void*)0x02, 0x12, (void*)0x22); line = __LINE__;
	assert_int_equal_cs(te4->EventID & 0xFFFu, PSF_EVENT_EXPECT);
	assert_int_equal_cs(te4->uxParams[0], 0x02);
	assert_int_equal_cs(te4->uxParams[1], line);
	assert_int_equal_cs(te4->uxParams[2], 0x12);
	assert_int_equal_cs(te4->uxParams[3], 0x22);

	xTraceEventExpect_1((void*)0x03, 0x13, 0x23); line = __LINE__;
	assert_int_equal_cs(te4->EventID & 0xFFFu, PSF_EVENT_EXPECT);
	assert_int_equal_cs(te4->uxParams[0], 0x03);
	assert_int_equal_cs(te4->uxParams[1], line);
	assert_int_equal_cs(te4->uxParams[2], 0x13);
	assert_int_equal_cs(te4->uxParams[3], 0x23);

	xTraceEventExpect_Handle_1((void*)0x04, 0x14, (void*)0x24, 0x34); line = __LINE__;
	assert_int_equal_cs(te5->EventID & 0xFFFu, PSF_EVENT_EXPECT);
	assert_int_equal_cs(te5->uxParams[0], 0x04);
	assert_int_equal_cs(te5->uxParams[1], line);
	assert_int_equal_cs(te5->uxParams[2], 0x14);
	assert_int_equal_cs(te5->uxParams[3], 0x24);
	assert_int_equal_cs(te5->uxParams[4], 0x34);

	xTraceEventExpect_2((void*)0x05, 0x15, 0x25, 0x35); line = __LINE__;
	assert_int_equal_cs(te5->EventID & 0xFFFu, PSF_EVENT_EXPECT);
	assert_int_equal_cs(te5->uxParams[0], 0x05);
	assert_int_equal_cs(te5->uxParams[1], line);
	assert_int_equal_cs(te5->uxParams[2], 0x15);
	assert_int_equal_cs(te5->uxParams[3], 0x25);
	assert_int_equal_cs(te5->uxParams[4], 0x35);

	xTraceEventExpect_Handle_2((void*)0x06, 0x16, (void*)0x26, 0x36, 0x46); line = __LINE__;
	assert_int_equal_cs(te6->EventID & 0xFFFu, PSF_EVENT_EXPECT);
	assert_int_equal_cs(te6->uxParams[0], 0x06);
	assert_int_equal_cs(te6->uxParams[1], line);
	assert_int_equal_cs(te6->uxParams[2], 0x16);
	assert_int_equal_cs(te6->uxParams[3], 0x26);
	assert_int_equal_cs(te6->uxParams[4], 0x36);
	assert_int_equal_cs(te6->uxParams[5], 0x46);

	xTraceEventExpect_3((void*)0x07, 0x17, 0x27, 0x37, 0x47); line = __LINE__;
	assert_int_equal_cs(te6->EventID & 0xFFFu, PSF_EVENT_EXPECT);
	assert_int_equal_cs(te6->uxParams[0], 0x07);
	assert_int_equal_cs(te6->uxParams[1], line);
	assert_int_equal_cs(te6->uxParams[2], 0x17);
	assert_int_equal_cs(te6->uxParams[3], 0x27);
	assert_int_equal_cs(te6->uxParams[4], 0x37);
	assert_int_equal_cs(te6->uxParams[5], 0x47);
}

void TRACE_ASSERTS_DISABLED(void** ppState)
{
}

uint32_t runEventVerificationTests(void)
{
	uint32_t uiCountFailedTests = 0;

	const struct CMUnitTest aTests_EventVerification[] =
	{
#if (TRC_CFG_USE_TRACE_ASSERT == 0)
		cmocka_unit_test(TRACE_ASSERTS_DISABLED),
#endif
		cmocka_unit_test_setup_teardown(test_xTraceEventExpect, setup_EventVerification, teardown_EventVerification),
#if (TRC_CFG_USE_TRACE_ASSERT == 0)
		cmocka_unit_test(TRACE_ASSERTS_DISABLED),
#endif
	};

	uiCountFailedTests += cmocka_run_group_tests(aTests_EventVerification, group_setup_EventVerification, group_teardown_EventVerification);

	return uiCountFailedTests;
}
