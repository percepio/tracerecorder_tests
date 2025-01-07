/*
* trcError.h mock
*/

#ifndef TRC_ERROR_H
#define TRC_ERROR_H

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

#include <trcTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TraceErrorBuffer
{
	uint32_t buffer[1];
} TraceErrorBuffer_t;

#define xTraceErrorInitialize(pxBuffer) (TRC_SUCCESS)
#define xTraceWarning(uiErrorCode) ((void)uiErrorCode, TRC_SUCCESS)
#define xTraceError(uiErrorCode) ((void)uiErrorCode, TRC_SUCCESS)
#define xTraceErrorGetLast(pszError) ((void*)pszError, TRC_SUCCESS)
#define xTraceErrorClear() (TRC_SUCCESS)
#define xTraceErrorCheckStatus() (TRC_SUCCESS)

#ifdef __cplusplus
}
#endif

#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

#endif /* (TRC_USE_TRACEALYZER_RECORDER == 1) */

#endif /* TRC_ERROR_H*/
