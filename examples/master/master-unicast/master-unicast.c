/**
 * \file
 *         Unicast communication
 * \author
 *         Oliver Harms <oha@informatik.uni-kiel.de>
 *
 */

// includes
  #include "contiki.h"
  #include "net/master-routing/master-routing.h"
  #include "node-id.h"
  #include "lib/random.h"

  #include <string.h>

/* Log configuration */
  #include "sys/log.h"
  #define LOG_MODULE "App"
  #define LOG_LEVEL LOG_LEVEL_INFO


/*---------------------------------------------------------------------------*/
PROCESS(master_unicast_process, "Master unicast example");
PROCESS(master_neighbor_discovery_process, "Master neighbor discovery example");
AUTOSTART_PROCESSES(&master_neighbor_discovery_process);

/*---------------------------------------------------------------------------*/
void input_callback(const void *data, uint16_t len, uint8_t src, uint8_t dest)
{
  LOG_INFO("Received %u bytes from node %u\n", len, src);
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(master_unicast_process, ev, data)
{
  static struct etimer periodic_timer;
  static uint8_t payload[MASTER_MSG_LENGTH];
  static uint8_t own_receiver;
  //uint8_t success;

  PROCESS_BEGIN();

  /* Initialize Master */
  init_master_routing();
  master_routing_set_input_callback(input_callback);

  LOG_INFO("node_id: %u\n", node_id); 

  etimer_set(&periodic_timer, CLOCK_SECOND);

  do {
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
      etimer_reset(&periodic_timer);
  } while(!master_routing_configured());

  if (node_is_sender()){
    own_receiver = get_node_receiver();
    etimer_set(&periodic_timer, random_rand()%(CLOCK_SECOND));
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
// #   if CONTIKI_TARGET_SKY
//     // etimer_set(&periodic_timer, CLOCK_SECOND / 40); // 1.025*CLOCK_SECOND
    // including send interval for each sending node
    #include "scheduling_send_interval.c"
// #   else
//     etimer_set(&periodic_timer, 1.01*CLOCK_SECOND);
// #   endif
    while(1){
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
      etimer_reset(&periodic_timer);
      
      uint8_t payload_index;
      for (payload_index = 0; payload_index < MASTER_MSG_LENGTH; ++payload_index){
        payload[payload_index] = (uint8_t)(random_rand() >> 8);
      }

      //success = 
      master_routing_sendto(&payload, MASTER_MSG_LENGTH, own_receiver);
      //LOG_INFO("Success: %u", success);
    }
  } else {
    while (1){
      PROCESS_YIELD();
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

static int counter = 1;
static uint16_t adv_counter = 0;
static uint8_t actual_data_sending_interval = 0;

PROCESS_THREAD(master_neighbor_discovery_process, ev, data)
{
  static struct etimer periodic_timer;
  static uint8_t payload[MASTER_MSG_LENGTH];
  static uint8_t own_receiver;
  //uint8_t success;

  PROCESS_BEGIN();

  /* Initialize Master */
  init_master_routing();
  master_routing_set_input_callback(input_callback);

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

      /*if(counter % 1 == 0)
      {
        master_routing_sendto(NULL, 0, 0);
      }*/
      
      //LOG_INFO("Success: %u", success);

    if( counter > 120 && counter % 13 == 0){
      own_receiver = get_node_receiver();
      uint8_t payload_index;
      for (payload_index = 0; payload_index < MASTER_MSG_LENGTH; ++payload_index){
        payload[payload_index] = (uint8_t)(random_rand() >> 8);
      }
      master_routing_sendto(&payload, MASTER_MSG_LENGTH, own_receiver);
    }
      

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