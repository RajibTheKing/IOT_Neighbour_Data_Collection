/**
 * \file
 *         Neighbor discovery
 * \author
 *         Oliver Harms <oha@informatik.uni-kiel.de>
 *
 */

// includes
  #include "contiki.h"
  #include "net/master-routing/master-routing.h"

  #include <string.h>

/* Log configuration */
  #include "sys/log.h"
  #include "sys/node-id.h"
  #define LOG_MODULE "App"
  #define LOG_LEVEL LOG_LEVEL_INFO

  static int counter = 1;
/*---------------------------------------------------------------------------*/
PROCESS(master_neighbor_discovery_process, "Master neighbor discovery example");
AUTOSTART_PROCESSES(&master_neighbor_discovery_process);

/*---------------------------------------------------------------------------*/
//void input_callback(const void *data, uint16_t len,
//  const linkaddr_t *src, const linkaddr_t *dest)
//{
//  if(len == sizeof(unsigned)) {
//    LOG_INFO("Received %u from ", *(unsigned *)data);
//    LOG_INFO_LLADDR(src);
//    LOG_INFO_("\n");
//  }
//}
/*---------------------------------------------------------------------------*/

static uint16_t adv_counter = 0;
static uint8_t actual_data_sending_interval = 0;

PROCESS_THREAD(master_neighbor_discovery_process, ev, data)
{
  static struct etimer periodic_timer;
  //uint8_t success;

  PROCESS_BEGIN();

  /* Initialize Master */
  init_master_routing();
  //master_routing_set_input_callback(input_callback);

  etimer_set(&periodic_timer, CLOCK_SECOND);

  do {
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
      etimer_reset(&periodic_timer);
  } while(!master_routing_configured());


  if (node_is_sender()){
    actual_data_sending_interval = 60 + (node_id * 2);

    while(1){
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
      counter += 1;
      //success =

      if(counter % 2 == 0)
      {
        master_routing_sendto(NULL, 0, 0);
      }
      
      //LOG_INFO("Success: %u", success);
      

      if(counter % 15 ==0 && node_id == 1 && counter% actual_data_sending_interval != 0){
        master_routing_send_advertisement_sendto( 100, ++adv_counter, node_id);
      }

      if(counter % actual_data_sending_interval == 0 && node_id != 1){
        master_routing_send_actual_data();
      }
      etimer_reset(&periodic_timer);
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/