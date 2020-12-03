#include "contiki.h"
#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#include <string.h>
#include <stdio.h> /* For printf() */

/* Log configuration */
#include "sys/log.h"
#include "sys/node-id.h"
#include "commonHeader.h"
#include "strength-vector.c"

#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

/* Configuration */
#define SEND_INTERVAL (5 * CLOCK_SECOND)

int sequence_number = 0;
static Beacon beaconData;
static Beacon received_beaconData;


/*---------------------------------------------------------------------------*/
PROCESS(nullnet_example_process, "NullNet broadcast example");
AUTOSTART_PROCESSES(&nullnet_example_process);

/*---------------------------------------------------------------------------*/
void input_callback(const void *data, uint16_t len,
  const linkaddr_t *src, const linkaddr_t *dest)
{
  memcpy(&received_beaconData, data, len);

  printf("TheKing--> received: (seq, node) = (%d, %d)\n", received_beaconData.seq, received_beaconData.nodeID);

  onReceivedNewBeacon(received_beaconData);

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

    memcpy(nullnet_buf, &beaconData, sizeof(Beacon));
    nullnet_len = sizeof(Beacon);

    NETSTACK_NETWORK.output(NULL);
    etimer_reset(&periodic_timer);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
