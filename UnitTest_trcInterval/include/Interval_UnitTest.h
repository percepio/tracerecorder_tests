#ifndef INTERVAL_UNIT_TEST_H
#define INTERVAL_UNIT_TEST_H

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>

/******************************************************************************************
*                                        SETUP / TEARDOWN                                 *
******************************************************************************************/

int group_setup_Interval(void **ppState);

int group_teardown_Interval(void **ppState);

int setup_Interval(void **ppState);

int teardown_Interval(void **ppState);

/******************************************************************************************
*                                        TESTS                                            *
******************************************************************************************/

void test_xTraceIntervalCreate(void **ppState);

void test_xTraceIntervalStart(void **ppState);

void test_xTraceIntervalStop(void **ppState);

void test_xTraceIntervalGetState(void **ppState);

uint32_t runIntervalTests(void);

#ifdef __cplusplus
}
#endif

#endif /* INTERVAL_UNIT_TEST_H */
