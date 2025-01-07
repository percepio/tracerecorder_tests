#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <StaticBuffer_UnitTest.h>
#include <trcRecorder.h>

int group_teardown_StaticBuffer(void** ppState);
int group_setup_StaticBuffer(void** ppState);
int setup_StaticBuffer(void** ppState);
int teardown_StaticBuffer(void** ppState);

void test_xTraceStaticBufferInitialize(void** ppState);
void test_xTraceStaticBufferGet(void** ppState);

/******************************************************************************************
*                                        TEST STATE                                       *
******************************************************************************************/

typedef struct StaticBufferTestState
{
	TraceStaticBufferTable_t xStaticBuffer;
} StaticBufferTestState_t;

StaticBufferTestState_t xStaticBufferTestState;
volatile uint32_t MOCK_RecorderEnabled = 1;
volatile uint32_t MOCK_RecorderInitialized = 1;
uint32_t MOCK_uiCurrentCore = 0;
int32_t MOCK_iISRNesting = -1;
uint32_t MOCK_uiCriticalSection = 0;
uint32_t MOCK_uiCriticalSectionExpectedValue = 0;
uint8_t MOCK_tmpBuffer[TRC_MAX_BLOB_SIZE];
uint8_t MOCK_streamPortBuffer[TRC_MAX_BLOB_SIZE];
uint32_t MOCK_streamPortMaxTransferSize = 0xFFFFFFFF;

/******************************************************************************************
*                                        SETUP / TEARDOWN                                 *
******************************************************************************************/

int group_setup_StaticBuffer(void **ppState)
{
	*ppState = &xStaticBufferTestState;

	assert_int_equal(sizeof(TraceUnsignedBaseType_t), sizeof(void*));
	assert_int_equal(sizeof(TraceUnsignedBaseType_t), sizeof(TraceBaseType_t));

	return 0;
}

int group_teardown_StaticBuffer(void **ppState)
{
	StaticBufferTestState_t* pxState = (StaticBufferTestState_t*)*ppState;

	return 0;
}

int setup_StaticBuffer(void **ppState)
{
	StaticBufferTestState_t* pxState = (StaticBufferTestState_t*)*ppState;
	
	assert_int_equal_cs(xTraceStaticBufferInitialize(&xStaticBufferTestState.xStaticBuffer), TRC_SUCCESS);

	return 0;
}

int teardown_StaticBuffer(void **ppState)
{
	return 0;
}

/******************************************************************************************
*                                        TESTS                                            *
******************************************************************************************/

void test_xTraceStaticBufferInitialize(void** ppState)
{
	StaticBufferTestState_t* pxState = (StaticBufferTestState_t*)*ppState;

	assert_int_equal_asserts_enabled(xTraceStaticBufferInitialize(0), TRC_FAIL); /* null test */

	assert_int_equal_cs(xTraceStaticBufferInitialize(&xStaticBufferTestState.xStaticBuffer), TRC_SUCCESS);
}

void test_xTraceStaticBufferGet(void** ppState)
{
	StaticBufferTestState_t* pxState = (StaticBufferTestState_t*)*ppState;
	void* pvData;
	uint32_t i, j;

	assert_int_equal_asserts_enabled(xTraceStaticBufferGet(0), TRC_FAIL); /* null test */
	for (i = 0; i < (TRC_CFG_CORE_COUNT); i++)
	{
		MOCK_uiCurrentCore = i;
		for (j = 0; j < (TRC_CFG_MAX_ISR_NESTING) + 1; j++)
		{
			MOCK_iISRNesting = (int32_t)j - 1;
			assert_int_equal_cs(xTraceStaticBufferGet(&pvData), TRC_SUCCESS);
			assert_int_equal_cs((TraceUnsignedBaseType_t)pvData, (TraceUnsignedBaseType_t)&pxTraceStaticBufferTable->coreDummyEvents[i].dummyEvents[j]);
		}
	}
	MOCK_uiCurrentCore = 0;
	MOCK_iISRNesting = -1;
}

void TRACE_ASSERTS_DISABLED(void** ppState)
{
}

uint32_t runStaticBufferTests(void)
{
	uint32_t uiCountFailedTests = 0;

	const struct CMUnitTest aTests_trcStaticBuffer[] =
	{
#if (TRC_CFG_USE_TRACE_ASSERT == 0)
		cmocka_unit_test(TRACE_ASSERTS_DISABLED),
#endif
		cmocka_unit_test(test_xTraceStaticBufferInitialize),
		cmocka_unit_test_setup_teardown(test_xTraceStaticBufferGet, setup_StaticBuffer, teardown_StaticBuffer),
#if (TRC_CFG_USE_TRACE_ASSERT == 0)
		cmocka_unit_test(TRACE_ASSERTS_DISABLED),
#endif
	};

	uiCountFailedTests += cmocka_run_group_tests(aTests_trcStaticBuffer, group_setup_StaticBuffer, group_teardown_StaticBuffer);
	
	return uiCountFailedTests;
}
