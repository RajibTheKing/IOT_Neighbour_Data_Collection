#include "strength-vector.h"

void onReceivedNewBeacon(Beacon receivedBeacon)
{
    int currentSequence = receivedBeacon.seq;
    int nodeID = receivedBeacon.nodeID;

    if(highestSequence < currentSequence){
        highestSequence = currentSequence;
    }

    if(highestNodeID < nodeID){
        highestNodeID = nodeID;
    }

    receivedCounter[nodeID]++;

    strength_vector[nodeID] = (receivedCounter[nodeID] * 1.0) / (highestSequence * 1.0);

    debug_purpose++;
    if(debug_purpose  % 30 == 0){
        printf("TheKing--> receivedCounter(%d): ", highestSequence);
        int i;
        for(i = 0; i<= highestNodeID; i++){
            printf("%d ", receivedCounter[i]);
        }
        printf("\n");
    }
}