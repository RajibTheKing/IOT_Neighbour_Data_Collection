
#ifndef STRENGTH_VECTOR
#define STRENGTH_VECTOR

#include "commonHeader.h"




double strength_vector[MAX_EXPECTED_NODES_IN_NETWORK];
int receivedCounter[MAX_EXPECTED_NODES_IN_NETWORK];

int debug_purpose = 0;

int highestSequence = 0;
int highestNodeID = 0;

extern void onReceivedNewBeacon(Beacon receivedBeacon);

#endif