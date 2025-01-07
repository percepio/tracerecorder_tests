#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdarg.h>
#include <Print_UnitTest.h>
#include <trcRecorder.h>

int group_setup_Print(void** ppState);
int group_teardown_Print(void** ppState);
int setup_Print(void** ppState);
int teardown_Print(void** ppState);

void test_xTracePrintInitialize(void** ppState);
void test_xTracePrint(void** ppState);
void test_xTracePrintF(void** ppState);
void test_xTraceConsoleChannelPrintF(void** ppState);
void test_xTraceVPrintF_helper1(int start, ...);
void test_xTraceVPrintF_helper2(va_list vl);
void test_xTraceVPrintF(void** ppState);

/******************************************************************************************
*                                        TEST STATE                                       *
******************************************************************************************/

typedef struct PrintTestState
{
	/* Test Objects */
	TraceEntryTable_t xEntryTableBuffer;
	TraceEntryIndexTable_t xEntryIndexTableBuffer;
	TracePrintData_t xPrintBuffer;
	TraceDiagnosticsData_t xDiagnosticsBuffer;
} PrintTestState_t;

PrintTestState_t xPrintTestState;
volatile uint32_t MOCK_RecorderInitialized = 1;
uint32_t MOCK_uiCriticalSection = 0;
uint32_t MOCK_uiCriticalSectionExpectedValue = 0;
uint8_t MOCK_tmpBuffer[TRC_MAX_BLOB_SIZE];
uint8_t MOCK_streamPortBuffer[TRC_MAX_BLOB_SIZE];
uint32_t MOCK_streamPortMaxTransferSize = 0xFFFFFFFF;

/* VERIFY EVENT DATA */

#define assert_int_equal_cs_print0(a, b, e, p, sz, sl) assert_int_equal_cs_create_data1(a, b, e, p, sz, sl)
#define assert_int_equal_cs_print1(a, b, e, p, v1, sz, sl) assert_int_equal_cs_create_data2(a, b, e, p, v1, sz, sl)
#define assert_int_equal_cs_print2(a, b, e, p, v1, v2, sz, sl) assert_int_equal_cs_create_data3(a, b, e, p, v1, v2, sz, sl)
#define assert_int_equal_cs_print3(a, b, e, p, v1, v2, v3, sz, sl) assert_int_equal_cs_create_data4(a, b, e, p, v1, v2, v3, sz, sl)
#define assert_int_equal_cs_print4(a, b, e, p, v1, v2, v3, v4, sz, sl) assert_int_equal_cs_create_data5(a, b, e, p, v1,v2, v3, v4, sz, sl)
#define assert_int_equal_cs_print5(a, b, e, p, v1, v2, v3, v4, v5, sz, sl) assert_int_equal_cs_create_data6(a, b, e, p, v1, v2, v3, v4, v5, sz, sl)
#define assert_int_equal_cs_console(a, b, e, sz, sl) assert_int_equal_cs_print0(a, b, e, xPrintTestState.xPrintBuffer.consoleChannel, sz, sl)
#define assert_int_equal_cs_consolef(a, b, e, sz, sl, v) assert_int_equal_cs_print1(a, b, e, xPrintTestState.xPrintBuffer.consoleChannel, v, sz, sl)
#define assert_int_equal_cs_vprintf(a, b, e, p, sz, sl, v) assert_int_equal_cs_print1(a, b, e, p, v, sz, sl)

/******************************************************************************************
*                                        SETUP / TEARDOWN                                 *
******************************************************************************************/

int group_setup_Print(void **ppState)
{
	*ppState = &xPrintTestState;

	assert_int_equal(sizeof(TraceUnsignedBaseType_t), sizeof(void*));
	assert_int_equal(sizeof(TraceUnsignedBaseType_t), sizeof(TraceBaseType_t));

	return 0;
}

int group_teardown_Print(void **ppState)
{
	*ppState = 0;
	
	return 0;
}

int setup_Print(void **ppState)
{
	PrintTestState_t* pxState = (PrintTestState_t*)*ppState;
	assert_int_equal(MOCK_uiCriticalSection, MOCK_uiCriticalSectionExpectedValue);
	assert_int_equal_cs(xTraceEntryTableInitialize(&xPrintTestState.xEntryTableBuffer), TRC_SUCCESS);
	assert_int_equal_cs(xTraceEntryIndexTableInitialize(&pxState->xEntryIndexTableBuffer), TRC_SUCCESS);
	assert_int_equal_cs(xTraceDiagnosticsInitialize(&pxState->xDiagnosticsBuffer), TRC_SUCCESS);
	assert_int_equal_cs(xTracePrintInitialize(&xPrintTestState.xPrintBuffer), TRC_SUCCESS);

	return 0;
}

int teardown_Print(void **ppState)
{
	PrintTestState_t* pxState = (PrintTestState_t*)*ppState;

	return 0;
}

/******************************************************************************************
*                                        TESTS                                            *
******************************************************************************************/

void test_xTracePrintInitialize(void** ppState)
{
	assert_int_equal_cs(xTracePrintInitialize(0), TRC_FAIL); /* null test */
	
	assert_int_equal_cs(xTracePrintInitialize(&xPrintTestState.xPrintBuffer), TRC_SUCCESS);
}

void test_xTracePrint(void** ppState)
{
	traceString channel;
	
	char* szTestString = "test";
	assert_int_equal_cs(xTraceStringRegister("TEST_CHANNEL", &channel), TRC_SUCCESS);

	assert_int_equal_cs_print0(xTracePrint(channel, (char*)0), TRC_SUCCESS, PSF_EVENT_USER_EVENT + 1, channel, "", 1); /* null test */
	
	assert_int_equal_cs_print0(xTracePrint(0, szTestString), TRC_SUCCESS, PSF_EVENT_USER_EVENT + 1, xPrintTestState.xPrintBuffer.defaultChannel, szTestString, strlen(szTestString) + 1);	// null channel will result in default channel

	assert_int_equal_cs_print0(xTracePrint(channel, szTestString), TRC_SUCCESS, PSF_EVENT_USER_EVENT + 1, channel, szTestString, strlen(szTestString) + 1);
}

void test_xTracePrintF(void** ppState)
{
	traceString channel;

	assert_int_equal_cs(xTraceStringRegister("TEST_CHANNEL", &channel), TRC_SUCCESS);

	assert_int_equal_cs_print1(xTracePrintF(channel, 0), TRC_SUCCESS, PSF_EVENT_USER_EVENT + 1, channel, 0, "", 1); /* null test */

	char* szFormat = "test";
	assert_int_equal_cs_print1(xTracePrintF(0, szFormat), TRC_SUCCESS, PSF_EVENT_USER_EVENT + 1, xPrintTestState.xPrintBuffer.defaultChannel, 0, szFormat, strlen(szFormat) + 1);

	szFormat = "test %%";
	assert_int_equal_cs_print1(xTracePrintF(0, szFormat), TRC_SUCCESS, PSF_EVENT_USER_EVENT + 1, xPrintTestState.xPrintBuffer.defaultChannel, 0, szFormat, strlen(szFormat) + 1);

	szFormat = "test %";
	assert_int_equal_cs_print1(xTracePrintF(0, szFormat), TRC_SUCCESS, PSF_EVENT_USER_EVENT + 1, xPrintTestState.xPrintBuffer.defaultChannel, 0, szFormat, strlen(szFormat) + 1);

	szFormat = "test %d";
	assert_int_equal_cs_print1(xTracePrintF(0, szFormat, (TraceUnsignedBaseType_t)0x12345678), TRC_SUCCESS, PSF_EVENT_USER_EVENT + 2, xPrintTestState.xPrintBuffer.defaultChannel, (TraceUnsignedBaseType_t)0x12345678, szFormat, strlen(szFormat) + 1);

	szFormat = "test";
	assert_int_equal_cs_print1(xTracePrintF(channel, szFormat), TRC_SUCCESS, PSF_EVENT_USER_EVENT + 1, channel, 0, szFormat, strlen(szFormat) + 1);

	szFormat = "test \"%%d, -1\": %d";
	assert_int_equal_cs_print1(xTracePrintF(channel, szFormat, (TraceUnsignedBaseType_t)-1), TRC_SUCCESS, PSF_EVENT_USER_EVENT + 2, channel, (TraceUnsignedBaseType_t)-1, szFormat, strlen(szFormat) + 1);

	szFormat = "test \"%%d, 1\": %d";
	assert_int_equal_cs_print1(xTracePrintF(channel, szFormat, (TraceUnsignedBaseType_t)1), TRC_SUCCESS, PSF_EVENT_USER_EVENT + 2, channel, (TraceUnsignedBaseType_t)1, szFormat, strlen(szFormat) + 1);

	szFormat = "test \"%%08d, -2\": %08d";
	assert_int_equal_cs_print1(xTracePrintF(channel, szFormat, (TraceUnsignedBaseType_t)-2), TRC_SUCCESS, PSF_EVENT_USER_EVENT + 2, channel, (TraceUnsignedBaseType_t)-2, szFormat, strlen(szFormat) + 1);

	szFormat = "test \"%%08d, 2\": %08d";
	assert_int_equal_cs_print1(xTracePrintF(channel, szFormat, (TraceUnsignedBaseType_t)2), TRC_SUCCESS, PSF_EVENT_USER_EVENT + 2, channel, (TraceUnsignedBaseType_t)2, szFormat, strlen(szFormat) + 1);

	szFormat = "test \"%%04u, -3\": %04u";
	assert_int_equal_cs_print1(xTracePrintF(channel, szFormat, (TraceUnsignedBaseType_t)-3), TRC_SUCCESS, PSF_EVENT_USER_EVENT + 2, channel, (TraceUnsignedBaseType_t)-3, szFormat, strlen(szFormat) + 1);

	szFormat = "test \"%%04u, 3\": %04u";
	assert_int_equal_cs_print1(xTracePrintF(channel, szFormat, (TraceUnsignedBaseType_t)3), TRC_SUCCESS, PSF_EVENT_USER_EVENT + 2, channel, (TraceUnsignedBaseType_t)3, szFormat, strlen(szFormat) + 1);

	szFormat = "test \"0x%%x, -4\": 0x%x";
	assert_int_equal_cs_print1(xTracePrintF(channel, szFormat, (TraceUnsignedBaseType_t)-4), TRC_SUCCESS, PSF_EVENT_USER_EVENT + 2, channel, (TraceUnsignedBaseType_t)-4, szFormat, strlen(szFormat) + 1);

	szFormat = "test \"0x%%x, 4\": 0x%x";
	assert_int_equal_cs_print1(xTracePrintF(channel, szFormat, (TraceUnsignedBaseType_t)4), TRC_SUCCESS, PSF_EVENT_USER_EVENT + 2, channel, (TraceUnsignedBaseType_t)4, szFormat, strlen(szFormat) + 1);

	szFormat = "test \"0x%%06x, -5\": 0x%06x";
	assert_int_equal_cs_print1(xTracePrintF(channel, szFormat, (TraceUnsignedBaseType_t)-5), TRC_SUCCESS, PSF_EVENT_USER_EVENT + 2, channel, (TraceUnsignedBaseType_t)-5, szFormat, strlen(szFormat) + 1);

	szFormat = "test \"0x%%06x, 5\": 0x%06x";
	assert_int_equal_cs_print1(xTracePrintF(channel, szFormat, (TraceUnsignedBaseType_t)5), TRC_SUCCESS, PSF_EVENT_USER_EVENT + 2, channel, (TraceUnsignedBaseType_t)5, szFormat, strlen(szFormat) + 1);

	szFormat = "test \"0x%%X, -6\": 0x%X";
	assert_int_equal_cs_print1(xTracePrintF(channel, szFormat, (TraceUnsignedBaseType_t)-6), TRC_SUCCESS, PSF_EVENT_USER_EVENT + 2, channel, (TraceUnsignedBaseType_t)-6, szFormat, strlen(szFormat) + 1);

	szFormat = "test \"0x%%X, 6\": 0x%X";
	assert_int_equal_cs_print1(xTracePrintF(channel, szFormat, (TraceUnsignedBaseType_t)6), TRC_SUCCESS, PSF_EVENT_USER_EVENT + 2, channel, (TraceUnsignedBaseType_t)6, szFormat, strlen(szFormat) + 1);

	szFormat = "test \"0x%%08X, -7\": 0x%08X";
	assert_int_equal_cs_print1(xTracePrintF(channel, szFormat, (TraceUnsignedBaseType_t)-7), TRC_SUCCESS, PSF_EVENT_USER_EVENT + 2, channel, (TraceUnsignedBaseType_t)-7, szFormat, strlen(szFormat) + 1);

	szFormat = "test \"0x%%08X, 7\": 0x%08X";
	assert_int_equal_cs_print1(xTracePrintF(channel, szFormat, (TraceUnsignedBaseType_t)7), TRC_SUCCESS, PSF_EVENT_USER_EVENT + 2, channel, (TraceUnsignedBaseType_t)7, szFormat, strlen(szFormat) + 1);

	szFormat = "test \"%%s, 0\": %s";
	assert_int_equal_cs_print1(xTracePrintF(channel, szFormat, (TraceUnsignedBaseType_t)0), TRC_SUCCESS, PSF_EVENT_USER_EVENT + 2, channel, (TraceUnsignedBaseType_t)0, szFormat, strlen(szFormat) + 1);

	szFormat = "test \"%%s, channel\": %s";
	assert_int_equal_cs_print1(xTracePrintF(channel, szFormat, channel), TRC_SUCCESS, PSF_EVENT_USER_EVENT + 2, channel, (TraceUnsignedBaseType_t)channel, szFormat, strlen(szFormat) + 1);

	szFormat = "test \"%%d, -1; %%s, channel\": %d; %s";
	assert_int_equal_cs_print2(xTracePrintF(channel, szFormat, (TraceUnsignedBaseType_t)-1, channel), TRC_SUCCESS, PSF_EVENT_USER_EVENT + 3, channel, (TraceUnsignedBaseType_t)-1, (TraceUnsignedBaseType_t)channel, szFormat, strlen(szFormat) + 1);

	szFormat = "test \"%%u, -1; %%s, channel\": %u; %s";
	assert_int_equal_cs_print2(xTracePrintF(channel, szFormat, (TraceUnsignedBaseType_t)-1, channel), TRC_SUCCESS, PSF_EVENT_USER_EVENT + 3, channel, (TraceUnsignedBaseType_t)-1, (TraceUnsignedBaseType_t)channel, szFormat, strlen(szFormat) + 1);

	szFormat = "test \"0x%%02x, 1; %%s, channel\": 0x%02x; %s";
	assert_int_equal_cs_print2(xTracePrintF(channel, szFormat, (TraceUnsignedBaseType_t)1, channel), TRC_SUCCESS, PSF_EVENT_USER_EVENT + 3, channel, (TraceUnsignedBaseType_t)1, (TraceUnsignedBaseType_t)channel, szFormat, strlen(szFormat) + 1);

	szFormat = "test \"0x%%04X, 1; %%s, channel\": 0x%04X; %s";
	assert_int_equal_cs_print2(xTracePrintF(channel, szFormat, (TraceUnsignedBaseType_t)1, channel), TRC_SUCCESS, PSF_EVENT_USER_EVENT + 3, channel, (TraceUnsignedBaseType_t)1, (TraceUnsignedBaseType_t)channel, szFormat, strlen(szFormat) + 1);
}

void test_xTraceConsoleChannelPrintF(void** ppState)
{
	assert_int_equal_cs_console(xTraceConsoleChannelPrintF(0), TRC_SUCCESS, PSF_EVENT_USER_EVENT + 1, "", 1);

	char* szFormat = "test";
	assert_int_equal_cs_consolef(xTraceConsoleChannelPrintF(szFormat), TRC_SUCCESS, PSF_EVENT_USER_EVENT + 1, szFormat, strlen(szFormat) + 1, 0);

	szFormat = "test %d";
	assert_int_equal_cs_consolef(xTraceConsoleChannelPrintF(szFormat, 1), TRC_SUCCESS, PSF_EVENT_USER_EVENT + 2, szFormat, strlen(szFormat) + 1, 1);
}

traceResult test_xTraceVPrintF_helper(traceString channel, const char* fmt, ...)
{
	traceResult xResult;
	va_list vl;

	va_start(vl, fmt);
	xResult = xTraceVPrintF(channel, fmt, &vl);
	va_end(vl);

	return xResult;
}

void test_xTraceVPrintF(void** ppState)
{
	traceString channel;

	assert_int_equal_cs(xTraceStringRegister("TEST_CHANNEL", &channel), TRC_SUCCESS);

	char* szFormat = "test";
	assert_int_equal_cs_vprintf(test_xTraceVPrintF_helper(0, szFormat), TRC_SUCCESS, PSF_EVENT_USER_EVENT + 1, xPrintTestState.xPrintBuffer.defaultChannel, szFormat, strlen(szFormat) + 1, 0);
	szFormat = "test";
	assert_int_equal_cs_vprintf(test_xTraceVPrintF_helper(channel, szFormat), TRC_SUCCESS, PSF_EVENT_USER_EVENT + 1, channel, szFormat, strlen(szFormat) + 1, 0);
	szFormat = "test %d";
	assert_int_equal_cs_vprintf(test_xTraceVPrintF_helper(channel, szFormat, 1), TRC_SUCCESS, PSF_EVENT_USER_EVENT + 2, channel, szFormat, strlen(szFormat) + 1, 1);
	szFormat = "test %u";
	assert_int_equal_cs_vprintf(test_xTraceVPrintF_helper(channel, szFormat, 2), TRC_SUCCESS, PSF_EVENT_USER_EVENT + 2, channel, szFormat, strlen(szFormat) + 1, 2);
}

void test_xTracePrintFX(void** ppState)
{
	traceString channel;
	traceString format;

	assert_int_equal_cs(xTraceStringRegister("channel_fixed", &channel), TRC_SUCCESS);
	assert_int_equal_cs(xTraceStringRegister("format", &format), TRC_SUCCESS);

	assert_int_equal_cs_create2(xTracePrintF0(channel, format), TRC_SUCCESS, PSF_EVENT_USER_EVENT_FIXED + 0, channel, format);
	assert_int_equal_cs_create3(xTracePrintF1(channel, format, 1), TRC_SUCCESS, PSF_EVENT_USER_EVENT_FIXED + 1, channel, format, 1);
	assert_int_equal_cs_create4(xTracePrintF2(channel, format, 1, 2), TRC_SUCCESS, PSF_EVENT_USER_EVENT_FIXED + 2, channel, format, 1, 2);
	assert_int_equal_cs_create5(xTracePrintF3(channel, format, 1, 2, 3), TRC_SUCCESS, PSF_EVENT_USER_EVENT_FIXED + 3, channel, format, 1, 2, 3);
	assert_int_equal_cs_create6(xTracePrintF4(channel, format, 1, 2, 3, 4), TRC_SUCCESS, PSF_EVENT_USER_EVENT_FIXED + 4, channel, format, 1, 2, 3, 4);
}

void test_xTracePrintCompactF(void** ppState)
{
	char* szChannel = "channel_compact";
	char* szFormat = "format";
	assert_int_equal_cs_create2(xTracePrintCompactF(szChannel, szFormat), TRC_SUCCESS, PSF_EVENT_USER_EVENT_FIXED + 0, szChannel, szFormat);
	szFormat = "format %u";
	assert_int_equal_cs_create3(xTracePrintCompactF(szChannel, szFormat, 1), TRC_SUCCESS, PSF_EVENT_USER_EVENT_FIXED + 1, szChannel, szFormat, 1);
	szFormat = "format %u %u";
	assert_int_equal_cs_create4(xTracePrintCompactF(szChannel, szFormat, 1, 2), TRC_SUCCESS, PSF_EVENT_USER_EVENT_FIXED + 2, szChannel, szFormat, 1, 2);
	szFormat = "format %u %u %u";
	assert_int_equal_cs_create5(xTracePrintCompactF(szChannel, szFormat, 1, 2, 3), TRC_SUCCESS, PSF_EVENT_USER_EVENT_FIXED + 3, szChannel, szFormat, 1, 2, 3);
	szFormat = "format %u %u %u %u";
	assert_int_equal_cs_create6(xTracePrintCompactF(szChannel, szFormat, 1, 2, 3, 4), TRC_SUCCESS, PSF_EVENT_USER_EVENT_FIXED + 4, szChannel, szFormat, 1, 2, 3, 4);
	szFormat = "format %u %u %u %u %u";
	assert_int_equal_cs(xTracePrintCompactF(szChannel, 	szFormat, 1, 2, 3, 4, 5), TRC_FAIL);
}

void test_xTracePrintCompactFX(void** ppState)
{
	char* szChannel = "channel_compact_fixed";
	char* szFormat = "format";
	assert_int_equal_cs_create2(xTracePrintCompactF0(szChannel, szFormat), TRC_SUCCESS, PSF_EVENT_USER_EVENT_FIXED + 0, szChannel, szFormat);
	szFormat = "format %u";
	assert_int_equal_cs_create3(xTracePrintCompactF1(szChannel, szFormat, 1), TRC_SUCCESS, PSF_EVENT_USER_EVENT_FIXED + 1, szChannel, szFormat, 1);
	szFormat = "format %u %u";
	assert_int_equal_cs_create4(xTracePrintCompactF2(szChannel, szFormat, 1, 2), TRC_SUCCESS, PSF_EVENT_USER_EVENT_FIXED + 2, szChannel, szFormat, 1, 2);
	szFormat = "format %u %u %u";
	assert_int_equal_cs_create5(xTracePrintCompactF3(szChannel, szFormat, 1, 2, 3), TRC_SUCCESS, PSF_EVENT_USER_EVENT_FIXED + 3, szChannel, szFormat, 1, 2, 3);
	szFormat = "format %u %u %u %u";
	assert_int_equal_cs_create6(xTracePrintCompactF4(szChannel, szFormat, 1, 2, 3, 4), TRC_SUCCESS, PSF_EVENT_USER_EVENT_FIXED + 4, szChannel, szFormat, 1, 2, 3, 4);
}

uint32_t runPrintTests(void)
{
	uint32_t uiCountFailedTests = 0;

	const struct CMUnitTest aTests_trcPrint[] =
	{
		cmocka_unit_test_setup_teardown(test_xTracePrintInitialize, setup_Print, teardown_Print),
		cmocka_unit_test_setup_teardown(test_xTracePrint, setup_Print, teardown_Print),
		cmocka_unit_test_setup_teardown(test_xTracePrintF, setup_Print, teardown_Print),
		cmocka_unit_test_setup_teardown(test_xTraceConsoleChannelPrintF, setup_Print, teardown_Print),
		cmocka_unit_test_setup_teardown(test_xTraceVPrintF, setup_Print, teardown_Print),
		cmocka_unit_test_setup_teardown(test_xTracePrintFX, setup_Print, teardown_Print),
		cmocka_unit_test_setup_teardown(test_xTracePrintCompactF, setup_Print, teardown_Print),
		cmocka_unit_test_setup_teardown(test_xTracePrintCompactFX, setup_Print, teardown_Print),
	};

	uiCountFailedTests += cmocka_run_group_tests(aTests_trcPrint, group_setup_Print, group_teardown_Print);

	return uiCountFailedTests;
}
