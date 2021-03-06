
#ifndef COMMON_HEADER
#define COMMON_HEADER

#define MAX_EXPECTED_NODES_IN_NETWORK 1000
#define MAX_EXPECTED_SIZE_OF_ACTUAL_DATA 50

#define PACKET_TYPE_BEACON 1
#define PACKET_TYPE_ADVERTISEMENT 2
#define PACKET_TYPE_ACTUAL_DATA 3

typedef struct {
  int packet_type;
  int nodeID;
  int seq;
} Beacon;

typedef struct {
  int packet_type;
  int nodeId;
  int seq;
  float strengthToMaster; 
} Advertisement;


#endif