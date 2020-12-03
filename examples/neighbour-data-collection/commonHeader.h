
#ifndef COMMON_HEADER
#define COMMON_HEADER

#define MAX_EXPECTED_NODES_IN_NETWORK 1000

typedef struct {
  int nodeID;
  int seq;
} Beacon;

typedef struct {
  int nodeId;
  float strengthToMaster; 
} Advertisement;


#endif