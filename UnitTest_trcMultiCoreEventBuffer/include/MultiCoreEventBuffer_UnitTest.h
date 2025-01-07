#ifndef MULTI_CORE_EVENT_BUFFER_UNIT_TEST_H
#define MULTI_CORE_EVENT_BUFFER_UNIT_TEST_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************************
*                                        SETUP / TEARDOWN                                 *
******************************************************************************************/

int group_teardown_MultiCoreEventBuffer(void **ppState);

int group_setup_MultiCoreEventBuffer(void **ppState);

int setup_MultiCoreEventBuffer(void **ppState);

int teardown_MultiCoreEventBuffer(void **ppState);

/******************************************************************************************
*                                        TESTS                                            *
******************************************************************************************/

void test_xTraceMultiCoreEventBufferInitialize(void **ppState);

uint32_t runMultiCoreEventBufferTests(void);

#ifdef __cplusplus
}
#endif

#endif /* RING_BUFFER_UNIT_TEST_H */
