/*
* trcEvent.h mock
*/

#ifndef TRC_EVENT_H
#define TRC_EVENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <trcTypes.h>

extern TraceUnsignedBaseType_t MOCK_EVENT_CREATE_uiEventCode;
extern TraceUnsignedBaseType_t MOCK_EVENT_CREATE_uxValue0;
extern TraceUnsignedBaseType_t MOCK_EVENT_CREATE_uxValue1;
extern TraceUnsignedBaseType_t MOCK_EVENT_CREATE_uxValue2;
extern TraceUnsignedBaseType_t MOCK_EVENT_CREATE_uxValue3;
extern TraceUnsignedBaseType_t MOCK_EVENT_CREATE_uxValue4;
extern TraceUnsignedBaseType_t MOCK_EVENT_CREATE_uxValue5;
extern void* MOCK_EVENT_CREATE_DATA_pxBuffer;
extern TraceUnsignedBaseType_t MOCK_EVENT_CREATE_DATA_uxSize;

#define xTraceEventCreate0(_uiEventCode) (MOCK_EVENT_CREATE_uiEventCode = (_uiEventCode), TRC_SUCCESS)
#define xTraceEventCreate1(_uiEventCode, _uxParam1) (MOCK_EVENT_CREATE_uiEventCode = (_uiEventCode), MOCK_EVENT_CREATE_uxValue0 = (_uxParam1), TRC_SUCCESS)
#define xTraceEventCreate2(_uiEventCode, _uxParam1, _uxParam2) (MOCK_EVENT_CREATE_uiEventCode = (_uiEventCode), MOCK_EVENT_CREATE_uxValue0 = (_uxParam1), MOCK_EVENT_CREATE_uxValue1 = (_uxParam2), TRC_SUCCESS)
#define xTraceEventCreate3(_uiEventCode, _uxParam1, _uxParam2, _uxParam3) (MOCK_EVENT_CREATE_uiEventCode = (_uiEventCode), MOCK_EVENT_CREATE_uxValue0 = (_uxParam1), MOCK_EVENT_CREATE_uxValue1 = (_uxParam2), MOCK_EVENT_CREATE_uxValue2 = (_uxParam3), TRC_SUCCESS)
#define xTraceEventCreate4(_uiEventCode, _uxParam1, _uxParam2, _uxParam3, _uxParam4) (MOCK_EVENT_CREATE_uiEventCode = (_uiEventCode), MOCK_EVENT_CREATE_uxValue0 = (_uxParam1), MOCK_EVENT_CREATE_uxValue1 = (_uxParam2), MOCK_EVENT_CREATE_uxValue2 = (_uxParam3), MOCK_EVENT_CREATE_uxValue3 = (_uxParam4), TRC_SUCCESS)
#define xTraceEventCreate5(_uiEventCode, _uxParam1, _uxParam2, _uxParam3, _uxParam4, _uxParam5) (MOCK_EVENT_CREATE_uiEventCode = (_uiEventCode), MOCK_EVENT_CREATE_uxValue0 = (_uxParam1), MOCK_EVENT_CREATE_uxValue1 = (_uxParam2), MOCK_EVENT_CREATE_uxValue2 = (_uxParam3), MOCK_EVENT_CREATE_uxValue3 = (_uxParam4), MOCK_EVENT_CREATE_uxValue4 = (_uxParam5), TRC_SUCCESS)
#define xTraceEventCreate6(_uiEventCode, _uxParam1, _uxParam2, _uxParam3, _uxParam4, _uxParam5, _uxParam6) (MOCK_EVENT_CREATE_uiEventCode = (_uiEventCode), MOCK_EVENT_CREATE_uxValue0 = (_uxParam1), MOCK_EVENT_CREATE_uxValue1 = (_uxParam2), MOCK_EVENT_CREATE_uxValue2 = (_uxParam3), MOCK_EVENT_CREATE_uxValue3 = (_uxParam4), MOCK_EVENT_CREATE_uxValue4 = (_uxParam5), MOCK_EVENT_CREATE_uxValue5 = (_uxParam6), TRC_SUCCESS)
#define xTraceEventCreateData0(_uiEventCode, _pxBuffer, _uxSize) (MOCK_EVENT_CREATE_uiEventCode = (_uiEventCode), MOCK_EVENT_CREATE_DATA_pxBuffer = (_pxBuffer), MOCK_EVENT_CREATE_DATA_uxSize = (_uxSize), TRC_SUCCESS)
#define xTraceEventCreateData1(_uiEventCode, _uxParam1, _pxBuffer, _uxSize) (MOCK_EVENT_CREATE_uiEventCode = (_uiEventCode), (MOCK_EVENT_CREATE_uxValue0) = (_uxParam1), MOCK_EVENT_CREATE_DATA_pxBuffer = (_pxBuffer), MOCK_EVENT_CREATE_DATA_uxSize = (_uxSize), TRC_SUCCESS)
#define xTraceEventCreateData2(_uiEventCode, _uxParam1, _uxParam2, _pxBuffer, _uxSize) (MOCK_EVENT_CREATE_uiEventCode = (_uiEventCode), (MOCK_EVENT_CREATE_uxValue0) = (_uxParam1), (MOCK_EVENT_CREATE_uxValue1) = (_uxParam2), MOCK_EVENT_CREATE_DATA_pxBuffer = (_pxBuffer), MOCK_EVENT_CREATE_DATA_uxSize = (_uxSize), TRC_SUCCESS)
#define xTraceEventCreateData3(_uiEventCode, _uxParam1, _uxParam2, _uxParam3, _pxBuffer, _uxSize) (MOCK_EVENT_CREATE_uiEventCode = (_uiEventCode), (MOCK_EVENT_CREATE_uxValue0) = (_uxParam1), (MOCK_EVENT_CREATE_uxValue1) = (_uxParam2), (MOCK_EVENT_CREATE_uxValue2) = (_uxParam3), MOCK_EVENT_CREATE_DATA_pxBuffer = (_pxBuffer), MOCK_EVENT_CREATE_DATA_uxSize = (_uxSize), TRC_SUCCESS)
#define xTraceEventCreateData4(_uiEventCode, _uxParam1, _uxParam2, _uxParam3, _uxParam4, _pxBuffer, _uxSize) (MOCK_EVENT_CREATE_uiEventCode = (_uiEventCode), (MOCK_EVENT_CREATE_uxValue0) = (_uxParam1), (MOCK_EVENT_CREATE_uxValue1) = (_uxParam2), (MOCK_EVENT_CREATE_uxValue2) = (_uxParam3), (MOCK_EVENT_CREATE_uxValue3) = (_uxParam4), MOCK_EVENT_CREATE_DATA_pxBuffer = (_pxBuffer), MOCK_EVENT_CREATE_DATA_uxSize = (_uxSize), TRC_SUCCESS)
#define xTraceEventCreateData5(_uiEventCode, _uxParam1, _uxParam2, _uxParam3, _uxParam4, _uxParam5, _pxBuffer, _uxSize) (MOCK_EVENT_CREATE_uiEventCode = (_uiEventCode), (MOCK_EVENT_CREATE_uxValue0) = (_uxParam1), (MOCK_EVENT_CREATE_uxValue1) = (_uxParam2), (MOCK_EVENT_CREATE_uxValue2) = (_uxParam3), (MOCK_EVENT_CREATE_uxValue3) = (_uxParam4), (MOCK_EVENT_CREATE_uxValue4) = (_uxParam5), MOCK_EVENT_CREATE_DATA_pxBuffer = (_pxBuffer), MOCK_EVENT_CREATE_DATA_uxSize = (_uxSize), TRC_SUCCESS)
#define xTraceEventCreateData6(_uiEventCode, _uxParam1, _uxParam2, _uxParam3, _uxParam4, _uxParam5, _uxParam6, _pxBuffer, _uxSize) (MOCK_EVENT_CREATE_uiEventCode = (_uiEventCode), (MOCK_EVENT_CREATE_uxValue0) = (_uxParam1), (MOCK_EVENT_CREATE_uxValue1) = (_uxParam2), (MOCK_EVENT_CREATE_uxValue2) = (_uxParam3), (MOCK_EVENT_CREATE_uxValue3) = (_uxParam4), (MOCK_EVENT_CREATE_uxValue4) = (_uxParam5), (MOCK_EVENT_CREATE_uxValue5) = (_uxParam6), MOCK_EVENT_CREATE_DATA_pxBuffer = (_pxBuffer), MOCK_EVENT_CREATE_DATA_uxSize = (_uxSize), TRC_SUCCESS)
#define xTraceEventGetPayload(xEventHandle, uiOffset, uiSize, ppvData) ((void)(xEventHandle), (void)(uiOffset), (void)(uiSize), (ppvData) != 0 ? *(ppvData) = (void*)0x12345678 : 0, (ppvData) != 0 ? TRC_SUCCESS : TRC_FAIL)
#define xTraceEventPayloadRemaining(xEventHandle, puiValue) ((void)(xEventHandle), (puiValue) != 0 ? *(puiValue) = 0x12345678 : 0, (puiValue) != 0 ? TRC_SUCCESS : TRC_FAIL)
#define xTraceEventPayloadUsed(xEventHandle, puiValue) ((void)(xEventHandle), (puiValue) != 0 ? *(puiValue) = 0x12345678 : 0, (puiValue) != 0 ? TRC_SUCCESS : TRC_FAIL)
#define xTraceEventPayloadSize(xEventHandle, puiValue) ((void)(xEventHandle), (puiValue) != 0 ? *(puiValue) = 0x12345678 : 0, (puiValue) != 0 ? TRC_SUCCESS : TRC_FAIL)
#define xTraceEventGetSize(pvAddress, puiSize) ((void*)(pvAddress), (puiSize) != 0 ? *(puiSize) = 0x10 : 0, (puiSize) != 0 ? TRC_SUCCESS : TRC_FAIL)

/* helper macros */
#define assert_int_equal_cs_create0(a, b, c) prvClearEventData(); assert_int_equal_cs(a, b); prvVerifyEventCreate(c, 0, 0, 0, 0, 0, 0, (void*)0, 0)
#define assert_int_equal_cs_create1(a, b, c, v0) prvClearEventData(); assert_int_equal_cs(a, b); prvVerifyEventCreate(c, (TraceUnsignedBaseType_t)(v0), 0, 0, 0, 0, 0, (void*)0, 0)
#define assert_int_equal_cs_create2(a, b, c, v0, v1) prvClearEventData(); assert_int_equal_cs(a, b); prvVerifyEventCreate(c, (TraceUnsignedBaseType_t)(v0), (TraceUnsignedBaseType_t)(v1), 0, 0, 0, 0, (void*)0, 0)
#define assert_int_equal_cs_create3(a, b, c, v0, v1, v2) prvClearEventData(); assert_int_equal_cs(a, b); prvVerifyEventCreate(c, (TraceUnsignedBaseType_t)(v0), (TraceUnsignedBaseType_t)(v1), (TraceUnsignedBaseType_t)(v2), 0, 0, 0, (void*)0, 0)
#define assert_int_equal_cs_create4(a, b, c, v0, v1, v2, v3) prvClearEventData(); assert_int_equal_cs(a, b); prvVerifyEventCreate(c, (TraceUnsignedBaseType_t)(v0), (TraceUnsignedBaseType_t)(v1), (TraceUnsignedBaseType_t)(v2), (TraceUnsignedBaseType_t)(v3), 0, 0, (void*)0, 0)
#define assert_int_equal_cs_create5(a, b, c, v0, v1, v2, v3, v4) prvClearEventData(); assert_int_equal_cs(a, b); prvVerifyEventCreate(c, (TraceUnsignedBaseType_t)(v0), (TraceUnsignedBaseType_t)(v1), (TraceUnsignedBaseType_t)(v2), (TraceUnsignedBaseType_t)(v3), (TraceUnsignedBaseType_t)(v4), 0, (void*)0, 0)
#define assert_int_equal_cs_create6(a, b, c, v0, v1, v2, v3, v4, v5) prvClearEventData(); assert_int_equal_cs(a, b); prvVerifyEventCreate(c, (TraceUnsignedBaseType_t)(v0), (TraceUnsignedBaseType_t)(v1), (TraceUnsignedBaseType_t)(v2), (TraceUnsignedBaseType_t)(v3), (TraceUnsignedBaseType_t)(v4), (TraceUnsignedBaseType_t)(v5), (void*)0, 0)

#define assert_int_equal_cs_create_data0(a, b, c, p, s) prvClearEventData(); assert_int_equal_cs(a, b); prvVerifyEventCreate(c, 0, 0, 0, 0, 0, 0, (void*)(p), s)
#define assert_int_equal_cs_create_data1(a, b, c, v0, p, s) prvClearEventData(); assert_int_equal_cs(a, b); prvVerifyEventCreate(c, (TraceUnsignedBaseType_t)(v0), 0, 0, 0, 0, 0, (void*)(p), s)
#define assert_int_equal_cs_create_data2(a, b, c, v0, v1, p, s) prvClearEventData(); assert_int_equal_cs(a, b); prvVerifyEventCreate(c, (TraceUnsignedBaseType_t)(v0), (TraceUnsignedBaseType_t)(v1), 0, 0, 0, 0, (void*)(p), s)
#define assert_int_equal_cs_create_data3(a, b, c, v0, v1, v2, p, s) prvClearEventData(); assert_int_equal_cs(a, b); prvVerifyEventCreate(c, (TraceUnsignedBaseType_t)(v0), (TraceUnsignedBaseType_t)(v1), (TraceUnsignedBaseType_t)(v2), 0, 0, 0, (void*)(p), s)
#define assert_int_equal_cs_create_data4(a, b, c, v0, v1, v2, v3, p, s) prvClearEventData(); assert_int_equal_cs(a, b); prvVerifyEventCreate(c, (TraceUnsignedBaseType_t)(v0), (TraceUnsignedBaseType_t)(v1), (TraceUnsignedBaseType_t)(v2), (TraceUnsignedBaseType_t)(v3), 0, 0, (void*)(p), s)
#define assert_int_equal_cs_create_data5(a, b, c, v0, v1, v2, v3, v4, p, s) prvClearEventData(); assert_int_equal_cs(a, b); prvVerifyEventCreate(c, (TraceUnsignedBaseType_t)(v0), (TraceUnsignedBaseType_t)(v1), (TraceUnsignedBaseType_t)(v2), (TraceUnsignedBaseType_t)(v3), (TraceUnsignedBaseType_t)(v4), 0, (void*)(p), s)
#define assert_int_equal_cs_create_data6(a, b, c, v0, v1, v2, v3, v4, v5, p, s) prvClearEventData(); assert_int_equal_cs(a, b); prvVerifyEventCreate(c, (TraceUnsignedBaseType_t)(v0), (TraceUnsignedBaseType_t)(v1), (TraceUnsignedBaseType_t)(v2), (TraceUnsignedBaseType_t)(v3), (TraceUnsignedBaseType_t)(v4), (TraceUnsignedBaseType_t)(v5), (void*)(p), s)

void prvClearEventData(void);
int prvVerifyEventCreate(TraceUnsignedBaseType_t uiEventCode, TraceUnsignedBaseType_t v0, TraceUnsignedBaseType_t v1, TraceUnsignedBaseType_t v2, TraceUnsignedBaseType_t v3, TraceUnsignedBaseType_t v4, TraceUnsignedBaseType_t v5, void* pxBuffer, TraceUnsignedBaseType_t uxSize);

#ifdef __cplusplus
}
#endif

#endif /* TRC_EVENT_H */
