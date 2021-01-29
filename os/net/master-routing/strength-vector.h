
#ifndef STRENGTH_VECTOR
#define STRENGTH_VECTOR

#include "commonHeader.h"




float strength_vector[MAX_EXPECTED_NODES_IN_NETWORK];
int receivedCounter[MAX_EXPECTED_NODES_IN_NETWORK];

int debug_purpose = 0;

int highestSequence = 0;
int highestNodeID = 0;

void onReceivedNewBeacon(Beacon receivedBeacon);
void printFLoat(float value);
void ftoa(float n, char* res, int afterpoint);
void reverse(char* str, int len);
int intToStr(int x, char str[], int d);
int myPow(int x, int p);

float getStrengthByNodeID(int nodeID);
#endif