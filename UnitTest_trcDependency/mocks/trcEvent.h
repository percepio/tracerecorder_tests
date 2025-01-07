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

extern TraceEventHandle_t MOCK_EVENT_BEGIN_xEventHandle;
extern TraceEventHandle_t MOCK_EVENT_END_xEventHandle;
extern TraceUnsignedBaseType_t MOCK_EVENT_CREATE0_uiEventCode;
extern TraceUnsignedBaseType_t MOCK_EVENT_CREATE1_uiEventCode;
extern TraceUnsignedBaseType_t MOCK_EVENT_CREATE2_uiEventCode;
extern TraceUnsignedBaseType_t MOCK_EVENT_CREATE3_uiEventCode;
extern TraceUnsignedBaseType_t MOCK_EVENT_CREATE4_uiEventCode;
extern TraceUnsignedBaseType_t MOCK_EVENT_CREATE5_uiEventCode;
extern TraceUnsignedBaseType_t MOCK_EVENT_CREATE6_uiEventCode;

#define xTraceEventCreate0(_uiEventCode) (MOCK_EVENT_CREATE0_uiEventCode = (_uiEventCode), TRC_SUCCESS)
#define xTraceEventCreate1(_uiEventCode, _uxParam1) (MOCK_EVENT_CREATE1_uiEventCode = (_uiEventCode), (void)(_uxParam1), TRC_SUCCESS)
#define xTraceEventCreate2(_uiEventCode, _uxParam1, _uxParam2) (MOCK_EVENT_CREATE2_uiEventCode = (_uiEventCode), (void)(_uxParam1), (void)(_uxParam2), TRC_SUCCESS)
#define xTraceEventCreate3(_uiEventCode, _uxParam1, _uxParam2, _uxParam3) (MOCK_EVENT_CREATE3_uiEventCode = (_uiEventCode), (void)(_uxParam1), (void)(_uxParam2), (void)(_uxParam3), TRC_SUCCESS)
#define xTraceEventCreate4(_uiEventCode, _uxParam1, _uxParam2, _uxParam3, _uxParam4) (MOCK_EVENT_CREATE4_uiEventCode = (_uiEventCode), (void)(_uxParam1), (void)(_uxParam2), (void)(_uxParam3), (void)(_uxParam4), TRC_SUCCESS)
#define xTraceEventCreate5(_uiEventCode, _uxParam1, _uxParam2, _uxParam3, _uxParam4, _uxParam5) (MOCK_EVENT_CREATE5_uiEventCode = (_uiEventCode), (void)(_uxParam1), (void)(_uxParam2), (void)(_uxParam3), (void)(_uxParam4), (void)(_uxParam5), TRC_SUCCESS)
#define xTraceEventCreate6(_uiEventCode, _uxParam1, _uxParam2, _uxParam3, _uxParam4, _uxParam5, _uxParam6) (MOCK_EVENT_CREATE6_uiEventCode = (_uiEventCode), (void)(_uxParam1), (void)(_uxParam2), (void)(_uxParam3), (void)(_uxParam4), (void)(_uxParam5), (void)(_uxParam6), TRC_SUCCESS)
#define xTraceEventCreateData0(_uiEventCode, _pxBuffer, _ulSize) (MOCK_EVENT_CREATE0_uiEventCode = (_uiEventCode), (void)(_pxBuffer), (void)(_ulSize), TRC_SUCCESS)
#define xTraceEventCreateData1(_uiEventCode, _uxParam1, _pxBuffer, _ulSize) (MOCK_EVENT_CREATE1_uiEventCode = (_uiEventCode), (void)(_uxParam1), (void)(_pxBuffer), (void)(_ulSize), TRC_SUCCESS)
#define xTraceEventCreateData2(_uiEventCode, _uxParam1, _uxParam2, _pxBuffer, _ulSize) (MOCK_EVENT_CREATE2_uiEventCode = (_uiEventCode), (void)(_uxParam1), (void)(_uxParam2), (void)(_pxBuffer), (void)(_ulSize), TRC_SUCCESS)
#define xTraceEventCreateData3(_uiEventCode, _uxParam1, _uxParam2, _uxParam3, _pxBuffer, _ulSize) (MOCK_EVENT_CREATE3_uiEventCode = (_uiEventCode), (void)(_uxParam1), (void)(_uxParam2), (void)(_uxParam3), (void)(_pxBuffer), (void)(_ulSize), TRC_SUCCESS)
#define xTraceEventCreateData4(_uiEventCode, _uxParam1, _uxParam2, _uxParam3, _uxParam4, _pxBuffer, _ulSize) (MOCK_EVENT_CREATE4_uiEventCode = (_uiEventCode), (void)(_uxParam1), (void)(_uxParam2), (void)(_uxParam3), (void)(_uxParam4), (void)(_pxBuffer), (void)(_ulSize), TRC_SUCCESS)
#define xTraceEventCreateData5(_uiEventCode, _uxParam1, _uxParam2, _uxParam3, _uxParam4, _uxParam5, _pxBuffer, _ulSize) (MOCK_EVENT_CREATE5_uiEventCode = (_uiEventCode), (void)(_uxParam1), (void)(_uxParam2), (void)(_uxParam3), (void)(_uxParam4), (void)(_uxParam5), (void)(_pxBuffer), (void)(_ulSize), TRC_SUCCESS)
#define xTraceEventCreateData6(_uiEventCode, _uxParam1, _uxParam2, _uxParam3, _uxParam4, _uxParam5, _uxParam6, _pxBuffer, _ulSize) (MOCK_EVENT_CREATE6_uiEventCode = (_uiEventCode), (void)(_uxParam1), (void)(_uxParam2), (void)(_uxParam3), (void)(_uxParam4), (void)(_uxParam5), (void)(_uxParam6), (void)(_pxBuffer), (void)(_ulSize), TRC_SUCCESS)
#define xTraceEventBeginOffline(uiEventCode, uiPayloadSize, pxEventHandle) ((void)(uiEventCode), (void)(uiPayloadSize), (pxEventHandle) != 0 ? *(pxEventHandle) = MOCK_EVENT_BEGIN_xEventHandle = (TraceEventHandle_t)0x12345678 : 0, (pxEventHandle) != 0 ? TRC_SUCCESS : TRC_FAIL)
#define xTraceEventBegin xTraceEventBeginOffline
#define xTraceEventEndOffline(xEventHandle) (MOCK_EVENT_END_xEventHandle = (xEventHandle), TRC_SUCCESS)
#define xTraceEventEnd xTraceEventEndOffline
#define xTraceEventGetPayload(xEventHandle, uiOffset, uiSize, ppvData) ((void)(xEventHandle), (void)(uiOffset), (void)(uiSize), (ppvData) != 0 ? *(ppvData) = (void*)0x12345678 : 0, (ppvData) != 0 ? TRC_SUCCESS : TRC_FAIL)
#define xTraceEventPayloadRemaining(xEventHandle, puiValue) ((void)(xEventHandle), (puiValue) != 0 ? *(puiValue) = 0x12345678 : 0, (puiValue) != 0 ? TRC_SUCCESS : TRC_FAIL)
#define xTraceEventPayloadUsed(xEventHandle, puiValue) ((void)(xEventHandle), (puiValue) != 0 ? *(puiValue) = 0x12345678 : 0, (puiValue) != 0 ? TRC_SUCCESS : TRC_FAIL)
#define xTraceEventPayloadSize(xEventHandle, puiValue) ((void)(xEventHandle), (puiValue) != 0 ? *(puiValue) = 0x12345678 : 0, (puiValue) != 0 ? TRC_SUCCESS : TRC_FAIL)
#define xTraceEventGetSize(pvAddress, puiSize) ((void*)(pvAddress), (puiSize) != 0 ? *(puiSize) = 0x10 : 0, (puiSize) != 0 ? TRC_SUCCESS : TRC_FAIL)
#define xTraceEventAddData(xEventHandle, puiData, uiSize) ((void)(xEventHandle), (void)(puiData), (void)(uiSize), TRC_SUCCESS)
#define xTraceEventAddString(xEventHandle, szString, uiLength) ((void)(xEventHandle), (void)(szString), (void)(uiLength), TRC_SUCCESS)
#define xTraceEventAddPointer(xEventHandle, value) ((void)(xEventHandle), (void)(value), TRC_SUCCESS)
#define xTraceEventAddUnsignedBaseType(xEventHandle, value) ((void)(xEventHandle), (void)(value), TRC_SUCCESS)
#define xTraceEventAdd32(xEventHandle, value) ((void)(xEventHandle), (void)(value), TRC_SUCCESS)
#define xTraceEventAdd16(xEventHandle, value) ((void)(xEventHandle), (void)(value), TRC_SUCCESS)
#define xTraceEventAdd8(xEventHandle, value) ((void)(xEventHandle), (void)(value), TRC_SUCCESS)

#ifdef __cplusplus
}
#endif

#endif /* TRC_EVENT_H */
