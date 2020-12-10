#include "contiki.h"
#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#include <string.h>
#include <stdio.h> /* For printf() */

/* Log configuration */
#include "sys/log.h"
#include "sys/node-id.h"
#include "commonHeader.h"

#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#include "strength-vector.c"

/* Configuration */
#define SEND_INTERVAL (10 * CLOCK_SECOND)
#define ADVERTISEMENT_INTERVAL (60 * CLOCK_SECOND)

int sequence_number = 0;
static Beacon beaconData;
static Beacon received_beaconData;


static Advertisement advertisementData;
static Advertisement received_advertisementData;
static int counter = 0;
static float maxMasterStrength = -1;
static int bestNode = -1;
static linkaddr_t bestLink;
static int maxSeqforStrength = -1;

static int mainStrengthSeq = 1;


static Advertisement history;
static uint8_t actualData[MAX_EXPECTED_SIZE_OF_ACTUAL_DATA];
static int actualDataLen;

/*---------------------------------------------------------------------------*/
PROCESS(nullnet_example_process, "NullNet broadcast example");
AUTOSTART_PROCESSES(&nullnet_example_process);

//#define MIN(X , Y) (X < Y ? X : Y)

/*---------------------------------------------------------------------------*/
void input_callback(const void *data, uint16_t len,
  const linkaddr_t *src, const linkaddr_t *dest)
{
  int type;
  memcpy(&type, data, sizeof(int));

  if (type == PACKET_TYPE_BEACON){
    memcpy(&received_beaconData, data, len);

    printf("TheKing--> received: (seq, node) = (%d, %d)\n", received_beaconData.seq, received_beaconData.nodeID);

    onReceivedNewBeacon(received_beaconData);
  }
  if (type == PACKET_TYPE_ADVERTISEMENT){
    memcpy(&received_advertisementData, data, len);
    printf("received adv from %d, seq : %d, currentlyMyBestNode: %d\n",received_advertisementData.nodeId, received_advertisementData.seq, bestNode);
    printFLoat(received_advertisementData.strengthToMaster);
    printf("\n");

    float currentStrength = MIN(received_advertisementData.strengthToMaster, getStrengthByNodeID(received_advertisementData.nodeId));
    int currentSeq = received_advertisementData.seq;

    if(maxSeqforStrength < currentSeq)
    {
      maxSeqforStrength = currentSeq;

      if(bestNode == received_advertisementData.nodeId){
        maxMasterStrength = currentStrength;
      }
    }
    
    if(currentSeq >= maxSeqforStrength && maxMasterStrength < currentStrength){
      maxMasterStrength = currentStrength;
      bestNode = received_advertisementData.nodeId;
      memcpy(&bestLink, src, sizeof(linkaddr_t));
      
    }

    
    nullnet_buf = (uint8_t *)&advertisementData;
    advertisementData.packet_type = PACKET_TYPE_ADVERTISEMENT;
    advertisementData.nodeId = node_id;
    advertisementData.strengthToMaster = maxMasterStrength;
    advertisementData.seq = maxSeqforStrength;

    if(history.nodeId == advertisementData.nodeId && history.strengthToMaster == advertisementData.strengthToMaster ){
      //skip
    }else{

      memcpy(nullnet_buf, &advertisementData, sizeof(Advertisement));
      nullnet_len = sizeof(Advertisement);
      NETSTACK_NETWORK.output(NULL);
      history = advertisementData;
      printf("TheKing--> BroadCast advertisement (node) = (%d)\n", advertisementData.nodeId);

      printf("Seq: %d, Selected BestNode = %d, Because: His Strength: ", currentSeq, bestNode);
      printFLoat(received_advertisementData.strengthToMaster);
      printf(", And MyLink Strength With Him --> ");
      printFLoat(getStrengthByNodeID(received_advertisementData.nodeId));
      printf("\n");

    }
    
  }

  if (type == PACKET_TYPE_ACTUAL_DATA) {

    if(node_id == 1){

      memcpy(actualData, data, len);
      int pos = 0;
      pos += sizeof(int);
      int numberOfNodes;
      memcpy(&numberOfNodes, actualData+pos, sizeof(int));
      pos  += sizeof(int);
      int i;

      printf("Got Actual Data --> Route (Size: %d): ", numberOfNodes + 1);
      for(i = 0; i<numberOfNodes; i++, pos += sizeof(int)){
        int curNode;
        memcpy(&curNode, actualData + pos, sizeof(int));
        if(i == 0){
          printf("%d", curNode);
        }else{
          printf(" --> %d", curNode);
        }
        
      }
      printf(" --> %d\n", node_id);


    }else{
      memcpy(actualData, data, len);
      int pos = 0;
      pos += sizeof(int);
      int numberOfNodes;
      memcpy(&numberOfNodes, data+pos, sizeof(int));
      numberOfNodes++;
      memcpy(actualData + pos, &numberOfNodes, sizeof(int));
      pos += sizeof(int);
      pos += ((numberOfNodes-1) * sizeof(int));

      
      memcpy(actualData + pos, &node_id, sizeof(int));
      actualDataLen = len + sizeof(int);

      nullnet_buf = actualData;
      nullnet_len = actualDataLen;
      NETSTACK_NETWORK.output(&bestLink);
      printf("TheKing--> Forwarding actualData (node) = (%d) --> to (node) = (%d)\n", node_id, bestNode);

      
    }

  }
  

}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(nullnet_example_process, ev, data)
{
  static struct etimer periodic_timer;

  PROCESS_BEGIN();

  nullnet_buf = (uint8_t *)&beaconData;

  /* Initialize NullNet function Pointer*/
  nullnet_set_input_callback(input_callback);

  etimer_set(&periodic_timer, SEND_INTERVAL);
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
    
    beaconData.packet_type = PACKET_TYPE_BEACON;
    beaconData.seq = ++sequence_number;
    beaconData.nodeID = node_id;
    printf("TheKing--> Sending (seq, node) = (%d, %d)\n", beaconData.seq, beaconData.nodeID);

    nullnet_buf = (uint8_t *)&beaconData;
    memcpy(nullnet_buf, &beaconData, sizeof(Beacon));
    nullnet_len = sizeof(Beacon);

    NETSTACK_NETWORK.output(NULL);
    counter++;
    if(counter % 5 == 0 && node_id == 1){
      maxMasterStrength = 100.0;
      bestNode = 1;
      //now send Advertisement
      //Starting by Master
      nullnet_buf = (uint8_t *)&advertisementData;
      advertisementData.packet_type = PACKET_TYPE_ADVERTISEMENT;
      advertisementData.nodeId = node_id;
      advertisementData.strengthToMaster = 100.0;
      advertisementData.seq = mainStrengthSeq++;
      memcpy(nullnet_buf, &advertisementData, sizeof(Advertisement));
      nullnet_len = sizeof(Advertisement);
      NETSTACK_NETWORK.output(NULL);
      printf("TheKing--> Sending advertisement (node) = (%d)\n", advertisementData.nodeId);
    }
    else if(node_id != 1 && counter % 5 == 0){
      //this block for All Slaves
      //Send Data if Any reference to Master Available
      if(bestNode != -1){
        //there is a reference found
        //now it's possible to send data to Master
        //4 byte 
        int numberOfNodes = 1;
        int myNodeID = node_id;
        int packet_type = PACKET_TYPE_ACTUAL_DATA;
        int pos = 0;
        memcpy(actualData, &packet_type, sizeof(int));
        pos += sizeof(int);
        memcpy(actualData + pos, &numberOfNodes, sizeof(int));
        pos += sizeof(int);
        memcpy(actualData + pos, &myNodeID, sizeof(int));
        actualDataLen = 3 * sizeof(int);

        nullnet_buf = actualData;
        nullnet_len = actualDataLen;
        NETSTACK_NETWORK.output(&bestLink);
        printf("TheKing--> Sending actualData (node) = (%d) --> to (node) = (%d)\n", myNodeID, bestNode);

      }
    }
    etimer_reset(&periodic_timer);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
