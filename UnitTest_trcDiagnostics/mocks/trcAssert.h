/*
* trcAssert.h mock
*/

#ifndef TRC_ASSERT_H
#define TRC_ASSERT_H

#if (TRC_CFG_USE_TRACE_ASSERT == 1)

#define TRC_ASSERT(__condition) if (! (__condition)) { return TRC_FAIL; }

#define TRC_ASSERT_ALWAYS_EVALUATE TRC_ASSERT

#define TRC_ASSERT_CUSTOM_ON_FAIL(__condition, __custom_on_fail) if (!(__condition)) { __custom_on_fail; }

#define TRC_ASSERT_CUSTOM_ON_FAIL_ALWAYS_EVALUATE TRC_ASSERT_CUSTOM_ON_FAIL

#define TRC_ASSERT_EQUAL_SIZE(x, y) if (sizeof(x) != sizeof(y)) { return TRC_FAIL; }

#else

#define TRC_ASSERT(__condition) 

#define TRC_ASSERT_ALWAYS_EVALUATE(__condition) (__condition)

#define TRC_ASSERT_CUSTOM_ON_FAIL(__condition, __custom_on_fail) 

#define TRC_ASSERT_CUSTOM_ON_FAIL_ALWAYS_EVALUATE(__condition, __custom_on_fail) (__condition)

#define TRC_ASSERT_EQUAL_SIZE(x, y) 

#endif

#define xTraceAssertInitialize(pxBuffer) ((void)pxBuffer, TRC_SUCCESS)

#define xTraceAssertGet(pxFileNameStringHandle, puxLineNumber) ((void)pxFileNameStringHandle, (void)puxLineNumber, TRC_FAIL)

#endif /* TRC_ASSERT_H*/
