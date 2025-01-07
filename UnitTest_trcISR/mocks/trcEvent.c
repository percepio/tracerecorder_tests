/*
* trcEvent.c mock verification
*/

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <trcEvent.h>
#include <trcRecorder.h>

TraceUnsignedBaseType_t MOCK_EVENT_CREATE_uiEventCode = 0;
TraceUnsignedBaseType_t MOCK_EVENT_CREATE_uxValue0 = 0;
TraceUnsignedBaseType_t MOCK_EVENT_CREATE_uxValue1 = 0;
TraceUnsignedBaseType_t MOCK_EVENT_CREATE_uxValue2 = 0;
TraceUnsignedBaseType_t MOCK_EVENT_CREATE_uxValue3 = 0;
TraceUnsignedBaseType_t MOCK_EVENT_CREATE_uxValue4 = 0;
TraceUnsignedBaseType_t MOCK_EVENT_CREATE_uxValue5 = 0;
void* MOCK_EVENT_CREATE_DATA_pxBuffer;
TraceUnsignedBaseType_t MOCK_EVENT_CREATE_DATA_uxSize;

/* VERIFY MOCK DATA FUNCTIONS */

void prvClearEventData(void)
{
	MOCK_EVENT_CREATE_uiEventCode = 0;
	MOCK_EVENT_CREATE_uxValue0 = 0;
	MOCK_EVENT_CREATE_uxValue1 = 0;
	MOCK_EVENT_CREATE_uxValue2 = 0;
	MOCK_EVENT_CREATE_uxValue3 = 0;
	MOCK_EVENT_CREATE_uxValue4 = 0;
	MOCK_EVENT_CREATE_uxValue5 = 0;
	MOCK_EVENT_CREATE_DATA_pxBuffer = (void*)0;
	MOCK_EVENT_CREATE_DATA_uxSize = 0;
}

int prvVerifyEventCreate(TraceUnsignedBaseType_t uiEventCode, TraceUnsignedBaseType_t v0, TraceUnsignedBaseType_t v1, TraceUnsignedBaseType_t v2, TraceUnsignedBaseType_t v3, TraceUnsignedBaseType_t v4, TraceUnsignedBaseType_t v5, void* pxBuffer, TraceUnsignedBaseType_t uxSize)
{
	assert_int_equal_cs(MOCK_EVENT_CREATE_uiEventCode, uiEventCode);
	assert_int_equal_cs(MOCK_EVENT_CREATE_uxValue0, v0);
	assert_int_equal_cs(MOCK_EVENT_CREATE_uxValue1, v1);
	assert_int_equal_cs(MOCK_EVENT_CREATE_uxValue2, v2);
	assert_int_equal_cs(MOCK_EVENT_CREATE_uxValue3, v3);
	assert_int_equal_cs(MOCK_EVENT_CREATE_uxValue4, v4);
	assert_int_equal_cs(MOCK_EVENT_CREATE_uxValue5, v5);
	assert_memory_equal(MOCK_EVENT_CREATE_DATA_pxBuffer, pxBuffer, uxSize);
	assert_int_equal_cs(MOCK_EVENT_CREATE_DATA_uxSize, uxSize);

	return 0;
}
