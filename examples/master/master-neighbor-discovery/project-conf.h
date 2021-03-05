#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_

#undef MASTER_MSG_LENGTH
#define MASTER_MSG_LENGTH 60

#undef  TESTBED
#define TESTBED TESTBED_COOJA // default: TESTBED_COOJA OR TESTBED_KIEL

#define NUM_COOJA_NODES 9

#undef NBR_TABLE_CONF_MAX_NEIGHBORS
#define NBR_TABLE_CONF_MAX_NEIGHBORS 10

#undef TSCH_SCHEDULE_CONF_MAX_LINKS
#define TSCH_SCHEDULE_CONF_MAX_LINKS 10

//TX-POWER:
#undef MASTER_CONF_CC2538_TX_POWER
#define MASTER_CONF_CC2538_TX_POWER 3 // default: -3

#ifdef CONTIKI_TARGET_ZOUL
#undef CC2538_RF_CONF_TX_POWER
    #define CC2538_RF_CONF_TX_POWER 0xD5 // default: 0xA1 // 0x88: -7 dBm; 0xA1: -3 dBm; 0xB0: -1 dBm; 0xB6: 0 dBm; 0xC5: 1 dBm; 0xD5: 3 dBm; 0xED: 5 dBm; 0xFF: 7 dBm
#endif

#define TSCH_CONF_WITH_CENTRAL_SCHEDULING    0
#define TSCH_CONF_FLOW_BASED_QUEUES          0
#define TSCH_CONF_TTL_BASED_RETRANSMISSIONS  0

#if TESTBED == TESTBED_COOJA
    #define MASTER_TSCH_COORDINATOR 0x01
#else
    #define MASTER_TSCH_COORDINATOR 0x08
#endif

#undef TSCH_CONF_DEFAULT_HOPPING_SEQUENCE
#define TSCH_CONF_DEFAULT_HOPPING_SEQUENCE TSCH_HOPPING_SEQUENCE_4_4

#define LOG_CONF_LEVEL_MAC LOG_LEVEL_INFO

#include "net/master-routing/master-project-conf.h"


// #undef MASTER_CONF_SCHEDULE
// #define MASTER_CONF_SCHEDULE "../../../Master/Schedules/loglistener_24.02.2021.c"
// #define TSCH_SCHEDULE_CONF_MAX_SLOTFRAMES 3

#endif /* PROJECT_CONF_H_ */