/*
* trcISR.h mock
*/

#ifndef TRC_ISR_H
#define TRC_ISR_H

extern int32_t MOCK_iISRNesting;

#define xTraceGetCurrentISRNesting(puiValue) (*puiValue = MOCK_iISRNesting, TRC_SUCCESS)

#define xTraceISRGetCurrentNesting xTraceGetCurrentISRNesting

#define xTraceISRGetCurrentNestingReturned() (MOCK_iISRNesting)

#endif /* TRC_ISR_H */
