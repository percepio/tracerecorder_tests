#ifndef EXTENSION_UNIT_TEST_H
#define EXTENSION_UNIT_TEST_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************************
*                                        SETUP / TEARDOWN                                 *
******************************************************************************************/

int group_setup_Extensions(void **ppState);

int group_teardown_Extensions(void **ppState);

int setup_emptyExtensions(void **ppState);

int setup_twoExtensions(void **ppState);

int teardown_extensions(void **ppState);

/******************************************************************************************
*                                        TESTS                                            *
******************************************************************************************/

void test_xTraceExtensionCreate(void **ppState);

void test_xTraceExtensionGetBaseEventId(void **ppState);

void test_xTraceExtensionGetEventId(void **ppState);

void test_xTraceExtensionGetConfigName(void **ppState);

uint32_t runExtensionTests(void);

#ifdef __cplusplus
}
#endif

#endif /* EXTENSION_UNIT_TEST_H */
