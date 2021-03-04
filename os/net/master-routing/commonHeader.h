
#ifndef COMMON_HEADER
#define COMMON_HEADER

#include "contiki.h"

#define MAX_EXPECTED_NODES_IN_NETWORK 30
#define MAX_EXPECTED_SIZE_OF_ACTUAL_DATA 40

#define PACKET_TYPE_BEACON 1
#define PACKET_TYPE_ADVERTISEMENT 2
#define PACKET_TYPE_ACTUAL_DATA 3
#define PACKET_TYPE_UNICAST 4

#define MAX_ACTUAL_DATA_LEN 47

typedef struct {
  int packet_type;
  int nodeID;
  int seq;
} Beacon;

typedef struct {
  uint32_t dummy1, dummy2;
  uint8_t packet_type;
  uint8_t nodeId;
  uint16_t seq;
  uint8_t bestNode;
  float strengthToMaster; 
} Advertisement;


typedef struct {
  uint32_t dummy1, dummy2;
  uint8_t packet_type;
  uint8_t nodeId;
  uint8_t dataLen;
  uint8_t data[MAX_ACTUAL_DATA_LEN];
} ActualData;


#endif