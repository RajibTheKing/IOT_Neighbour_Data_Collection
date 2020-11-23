#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_

#undef MASTER_MSG_LENGTH
#define MASTER_MSG_LENGTH 64

#undef  TESTBED
#define TESTBED TESTBED_COOJA // default: TESTBED_COOJA OR TESTBED_KIEL

#undef MASTER_CONF_SCHEDULE
#define MASTER_CONF_SCHEDULE "../../../Master/Schedules/test_sched2.c"

#define NUM_COOJA_NODES 5

#undef NBR_TABLE_CONF_MAX_NEIGHBORS
#define NBR_TABLE_CONF_MAX_NEIGHBORS 10

// #undef TSCH_SCHEDULE_CONF_MAX_LINKS //use output of Scheduler
// #define TSCH_SCHEDULE_CONF_MAX_LINKS 12

//TX-POWER: // probably only one of the two options below needed
#undef MASTER_CONF_CC2538_TX_POWER
#define MASTER_CONF_CC2538_TX_POWER 3 // default: -3

#ifdef CONTIKI_TARGET_ZOUL
#undef CC2538_RF_CONF_TX_POWER
    #define CC2538_RF_CONF_TX_POWER 0xD5 // default: 0xA1 // 0x88: -7 dBm; 0xA1: -3 dBm; 0xB0: -1 dBm; 0xB6: 0 dBm; 0xC5: 1 dBm; 0xD5: 3 dBm; 0xED: 5 dBm; 0xFF: 7 dBm
#endif

// #define MASTER_TSCH_COORDINATOR 0x05

#undef TSCH_CONF_DEFAULT_HOPPING_SEQUENCE
#define TSCH_CONF_DEFAULT_HOPPING_SEQUENCE TSCH_HOPPING_SEQUENCE_4_4

// including necessary macros for MASTER, like MASTER_MAX_SLOTFRAMES and TSCH_SCHEDULE_CONF_MAX_LINKS
#include "scheduling_macros.h"
#include "net/master-routing/master-project-conf.h"

// #define LOG_CONF_LEVEL_MAC LOG_LEVEL_DBG // default: LOG_LEVEL_INFO
// #define LOG_CONF_LEVEL_FRAMER LOG_LEVEL_INFO
#endif /* PROJECT_CONF_H_ */