#ifndef TRC_CONFIG_H
#define TRC_CONFIG_H

#define TRC_CFG_CORE_COUNT 2

#define TRC_CFG_BUFFER_SIZE 32

#define TRC_CFG_MAX_ISR_NESTING 16
#define TRC_CFG_ENTRY_SLOTS 10
#define TRC_CFG_ENTRY_SYMBOL_MAX_LENGTH 32

#define TRC_CFG_SCHEDULING_ONLY 0
#define TRC_CFG_INCLUDE_USER_EVENTS 1

#define TRC_CFG_RECORDER_BUFFER_ALLOCATION TRC_RECORDER_BUFFER_ALLOCATION_STATIC
#define TRC_CFG_RECORDER_MODE TRC_RECORDER_MODE_STREAMING

#define TRC_CFG_RECORDER_DATA_ATTRIBUTE 

#define TRC_CFG_USE_TRACE_ASSERT 1

#endif /* TRC_CONFIG_H */
