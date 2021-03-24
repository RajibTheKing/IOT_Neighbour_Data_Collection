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


/*initialize  variables */
static int counter = 1;
static uint16_t adv_counter = 0;
static int actual_data_sending_interval = 0;




/*---------------------------------------------------------------------------*/
PROCESS(master_neighbor_discovery_and_unicast_process, "Master neighbor discovery + Unicast example");
AUTOSTART_PROCESSES(&master_neighbor_discovery_and_unicast_process);

/*---------------------------------------------------------------------------*/
void input_callback(const void *data, uint16_t len, uint8_t src, uint8_t dest)
{
  LOG_INFO("Received %u bytes from node %u\n", len, src);
}

PROCESS_THREAD(master_neighbor_discovery_and_unicast_process, ev, data)
{
  static struct etimer periodic_timer;
  static uint8_t payload[MASTER_MSG_LENGTH];
  static uint8_t own_receiver;
  // uint8_t success;

  PROCESS_BEGIN();

  /* Initialize Master */
  init_master_routing();
  master_routing_set_input_callback(input_callback);

  etimer_set(&periodic_timer, CLOCK_SECOND);


  do {
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
      etimer_reset(&periodic_timer);
  } while(!master_routing_configured());


  
    actual_data_sending_interval = node_id; //60 + (node_id*2);

    while(1){
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
      counter += 1; // counter  increases in each clock second

      // after 4 * 60  = 240 seconds  generated schedule for unicast operation will be initialized
      if (counter==240){               
          apply_generated_schedule();
      }
      // if the node is sender and after 240 seconds unicast flow will start  sending  data
      if (node_is_sender()){
        if( counter > 240){
          own_receiver = get_node_receiver();
          uint8_t payload_index;
          for (payload_index = 0; payload_index < MASTER_MSG_LENGTH; ++payload_index){
            payload[payload_index] = (uint8_t)(random_rand() >> 8);
          }
          payload[8] = 0; //pos 8 is used for packet type
          master_routing_sendto(&payload, MASTER_MSG_LENGTH, own_receiver);
        }
      }
      
      // if node is coordinator every 15 seconds it will send advertisement also this initiates advertisement  
      if(counter % 15 ==0 && node_id == 1 ) { 
        master_routing_send_advertisement_sendto( 100, ++adv_counter, node_id);
      }

      // every node except corrdinator will send strength vector to the best node   
      if(counter == actual_data_sending_interval && node_id != 1){
        master_routing_send_actual_data();
        actual_data_sending_interval += NUM_COOJA_NODES;
      }
      etimer_reset(&periodic_timer);
    }
  

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/