/*
* trcPrint.h mock
*/

#ifndef TRC_PRINT_H
#define TRC_PRINT_H

#define xTracePrintInitialize(p) ((void)p, p != 0 ? TRC_SUCCESS : TRC_FAIL)
#define xTracePrint(c, s) ((void)c, (void)s, TRC_SUCCESS)
#define xTracePrintF(c, s, ...) ((void)c, (void)s, TRC_SUCCESS)
#define xTraceConsoleChannelPrintF(s, ...) ((void)s, TRC_SUCCESS)
#define xTraceVPrintF(c, s, v) ((void)c, (void)s, (void)v, TRC_SUCCESS)

#endif /* TRC_PRINT_H */
