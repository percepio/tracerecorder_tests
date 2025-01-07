/*
 * trcStreamingPort.h mock
 */

#ifndef TRC_STREAM_PORT_H
#define TRC_STREAM_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

#define TRC_STREAM_PORT_USE_INTERNAL_BUFFER 0

typedef struct TraceStreamPortBuffer
{
	uint32_t buffer[1];
} TraceStreamPortBuffer_t;

extern uint8_t MOCK_tmpBuffer[TRC_MAX_BLOB_SIZE];
extern uint8_t MOCK_streamPortBuffer[TRC_MAX_BLOB_SIZE];
extern uint32_t MOCK_streamPortMaxTransferSize;

#define xTraceStreamPortInitialize(pxBuffer) ((pxBuffer) != 0 ? TRC_SUCCESS : TRC_FAIL)
#define xTraceStreamPortAllocate(uiSize, ppvData) ((void)(uiSize), (ppvData) != 0 ? *(ppvData) = (void*)&MOCK_tmpBuffer : 0, (ppvData) != 0 ? TRC_SUCCESS : TRC_FAIL)
#define xTraceStreamPortCommit xTraceStreamPortWriteData
#define xTraceStreamPortReadData(pvData, uiSize, piBytesWritten) ((void)(pvData), (piBytesWritten) != 0 ? *(piBytesWritten) = (uiSize) : 0, (piBytesWritten) != 0 ? TRC_SUCCESS : TRC_FAIL)
#define xTraceStreamPortOnBegin() (TRC_SUCCESS)
#define xTraceStreamPortOnEnd() (TRC_SUCCESS)

static traceResult xTraceStreamPortWriteData(void* pvData, uint32_t uiSize, int32_t* piBytesWritten)
{
	uint32_t i = 0;
	uint32_t uiTransferSize = uiSize;

	if (piBytesWritten == 0)
		return TRC_FAIL;

	if (uiTransferSize > MOCK_streamPortMaxTransferSize)
	{
		uiTransferSize = MOCK_streamPortMaxTransferSize;
	}

	for (i = 0; i < uiTransferSize; i++)
	{
		MOCK_streamPortBuffer[i] = ((uint8_t*)pvData)[i];
	}

	*piBytesWritten = uiTransferSize;
	return TRC_SUCCESS;
}

#ifdef __cplusplus
}
#endif

#endif /* TRC_STREAMING_PORT_H */
