/*
 * trcTimestamp.h mock
 */

#ifndef TRC_TIMESTAMP_H
#define TRC_TIMESTAMP_H

#include <stdint.h>

extern uint32_t MOCK_uiTestTimestampCounter;

#define xTraceTimestampGet(puiTimestamp) (puiTimestamp != 0 ? (*puiTimestamp = MOCK_uiTestTimestampCounter++, TRC_SUCCESS) : TRC_FAIL)

#define xTraceTimestampGetWraparounds(puiWraparounds) (puiWraparounds != 0 ? TRC_SUCCESS : TRC_FAIL)

#endif /* TRC_TIMESTAMP_H*/
