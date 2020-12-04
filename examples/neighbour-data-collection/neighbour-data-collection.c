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
static int maxSeqforStrength = -1;

static int mainStrengthSeq = 1;


static Advertisement history;


/*---------------------------------------------------------------------------*/
PROCESS(nullnet_example_process, "NullNet broadcast example");
AUTOSTART_PROCESSES(&nullnet_example_process);

//#define MIN(X , Y) (X < Y ? X : Y)

/*---------------------------------------------------------------------------*/
void input_callback(const void *data, uint16_t len,
  const linkaddr_t *src, const linkaddr_t *dest)
{
  if (len == sizeof(Beacon)){
    memcpy(&received_beaconData, data, len);

    printf("TheKing--> received: (seq, node) = (%d, %d)\n", received_beaconData.seq, received_beaconData.nodeID);

    onReceivedNewBeacon(received_beaconData);
  }
  if (len == sizeof(Advertisement)){
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
    }

    
    nullnet_buf = (uint8_t *)&advertisementData;
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
      nullnet_buf = (uint8_t *)&advertisementData;
      advertisementData.nodeId = node_id;
      advertisementData.strengthToMaster = 100.0;
      advertisementData.seq = mainStrengthSeq++;
      memcpy(nullnet_buf, &advertisementData, sizeof(Advertisement));
      nullnet_len = sizeof(Advertisement);
      NETSTACK_NETWORK.output(NULL);
      printf("TheKing--> Sending advertisement (node) = (%d)\n", advertisementData.nodeId);
    }
    etimer_reset(&periodic_timer);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
