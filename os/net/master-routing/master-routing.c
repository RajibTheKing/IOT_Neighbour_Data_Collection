/*
 * Copyright (c) 2011, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 */

/**
 * \file
 *         Implementation of the Master Routing module.
 * \author
 *         Oliver Harms <oha@informatik.uni-kiel.de>
 *
 */

/**
 * \addtogroup master-routing
 * @{
 */

#include "contiki.h"
#include "master-routing.h"
#include "dev/leds.h"
#include "net/netstack.h"
#include "net/packetbuf.h"
#include "net/mac/tsch/tsch.h"
#include "net/mac/tsch/tsch-schedule.h"
#include "net/mac/tsch/tsch-private.h"
#include "sys/testbed.h"
#include "sys/hash-map.h"
#include "node-id.h"
#include "net/master-net/master-net.h"
#include "commonHeader.h"
#include "strength-vector.c"

#include <string.h>
#include <stdlib.h>

#include <stdio.h>

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "MASTER-R"
#define LOG_LEVEL LOG_LEVEL_INFO

//MASTER_SLOTFRAME_LENGTH

/** Master's routing packet with "header" */
typedef struct __attribute__((packed)) master_routing_packet_t
{
  uint8_t packet_type;
  uint8_t flow_number; //use as sender in case of neighbor discovery
  uint16_t packet_number;
#if TSCH_TTL_BASED_RETRANSMISSIONS && defined(MASTER_SCHEDULE)
  uint16_t ttl_slot_number;
  uint16_t earliest_tx_slot;
#endif /* TSCH_TTL_BASED_RETRANSMISSIONS && defined(MASTER_SCHEDULE) */
  uint16_t advertisement_seq;
  float strengthToMaster;
  uint8_t bestNode;
  uint8_t data_len;
  uint8_t data[50];
} master_routing_packet_t;

/* structure used by Master (Python) */
typedef struct __attribute__((packed))
{
  const uint16_t slotframe_handle;
  const uint16_t send_receive;
  const uint16_t timeslot;
  const uint16_t channel_offset;
} scheduled_link_t;

static master_routing_packet_t mrp; //masternet_routing_packet (mrp)
static uint8_t rcvd_actual_data_counter = 0;

#ifdef MASTER_SCHEDULE
static hash_table_t forward_to;                           //forward to next node, later not needed anymore //TODO: different hash_table sizes?, or size of flow!
static hash_table_t last_received_relayed_packet_of_flow; //for routing layer duplicate detection
#if TSCH_TTL_BASED_RETRANSMISSIONS
    // including macro MAX_TX_SIZE, which determines the maximal number of timeslots to send
#include "scheduling_tx_macros.h"
static uint16_t first_tx_slot_in_flow[MASTER_NUM_FLOWS][MAX_TX_SIZE];
static uint16_t last_tx_slot_in_flow[MASTER_NUM_FLOWS][MAX_TX_SIZE];
// static uint16_t last_sent_packet_asn = 0; // to be used only by sender
static uint16_t last_sent_packet_ttl = 0;
#else
static uint16_t sending_slots[MAX_NUMBER_TRANSMISSIONS];
static uint16_t num_sending_slots;
#endif /* TSCH_TTL_BASED_RETRANSMISSIONS */

static uint16_t max_transmissions[MASTER_NUM_FLOWS];
static uint16_t schedule_length;

#endif /* MASTER_SCHEDULE */

static const uint8_t minimal_routing_packet_size = sizeof(master_routing_packet_t) - MASTER_MSG_LENGTH;
static const uint8_t maximal_routing_packet_size = sizeof(master_routing_packet_t);

static uint16_t own_packet_number = 0;
static uint8_t own_receiver; //TODO: check if still needed, or if receiver_of_flow can be used
static uint8_t own_transmission_flow = 0;
static uint8_t is_sender = 0;

#ifdef MASTER_SCHEDULE
// scheduled with Master
static struct tsch_slotframe *sf[MASTER_NUM_FLOWS + 1]; //1 sf per flow + EB-sf
static uint8_t receiver_of_flow[MASTER_NUM_FLOWS + 1];
static uint8_t sender_of_flow[MASTER_NUM_FLOWS + 1];
#else
// Neighbor Discovery for Master
static struct tsch_slotframe *sf[4]; //2 sf for ND + EB-sf
#endif /* MASTER_SCHEDULE */

static master_packetbuf_config_t sent_packet_configuration;

static struct ctimer install_schedule_timer;
static uint8_t started = 0;
static uint8_t is_configured = 0;

static master_routing_input_callback current_callback = NULL;

static uint16_t max_advertisement_seq = 0;
static uint8_t bestNode = 0;
static struct tsch_link *best_tsch_shedule_link = NULL;
static float max_master_strength = 0;
//static master_routing_packet_t history;

static uint8_t history_ad_seq_array[NUM_COOJA_NODES];

static uint8_t flag_to_stop_ad = 0;

/*-------------------------- Routing configuration --------------------------*/

#if MAC_CONF_WITH_TSCH
#if TESTBED == TESTBED_COOJA && CONTIKI_TARGET_SKY
static linkaddr_t coordinator_addr = {{MASTER_TSCH_COORDINATOR, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
#elif TESTBED == TESTBED_FLOCKLAB && CONTIKI_TARGET_SKY
static linkaddr_t coordinator_addr = {{MASTER_TSCH_COORDINATOR, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
#elif TESTBED == TESTBED_KIEL && CONTIKI_TARGET_ZOUL
static linkaddr_t coordinator_addr = {{0x00, 0x12, 0x4B, 0x00, 0x00, 0x00, 0x00, MASTER_TSCH_COORDINATOR}};
#elif TESTBED == TESTBED_KIEL && CONTIKI_TARGET_SKY
static linkaddr_t coordinator_addr = {{MASTER_TSCH_COORDINATOR, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
#elif TESTBED == TESTBED_DESK && CONTIKI_TARGET_ZOUL
static linkaddr_t coordinator_addr = {{0x00, 0x12, 0x4B, 0x00, 0x00, 0x00, 0x00, MASTER_TSCH_COORDINATOR}};
#else
static linkaddr_t coordinator_addr = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, MASTER_TSCH_COORDINATOR}};
#endif
#endif /* MAC_CONF_WITH_TSCH */

/*Deployment node count*/
#ifndef MASTER_SCHEDULE
#if TESTBED == TESTBED_COOJA
static uint8_t deployment_node_count = NUM_COOJA_NODES;
#elif TESTBED == TESTBED_FLOCKLAB
static uint8_t deployment_node_count = 27;
#elif TESTBED == TESTBED_KIEL
static uint8_t deployment_node_count = 20;
#elif TESTBED == TESTBED_DESK
static uint8_t deployment_node_count = 5;
#endif
#endif /* MASTER_SCHEDULE */

/*Destination*/
#ifdef MASTER_SCHEDULE
#if (TESTBED == TESTBED_KIEL || TESTBED == TESTBED_DESK) && CONTIKI_TARGET_ZOUL
static linkaddr_t destination = {{0x00, 0x12, 0x4B, 0x00, 0x00, 0x00, 0x00, 0x00}};
#else
static linkaddr_t destination = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
#endif
#endif /* MASTER_SCHEDULE */

uint8_t
get_destination_index(uint8_t id)
{
#if TESTBED == TESTBED_COOJA
  return id - 1;
#elif TESTBED == TESTBED_FLOCKLAB
  if (id < 5)
  {
    return id - 1;
  }
  else if (id < 9)
  {
    return id - 2;
  }
  else if (id < 12)
  {
    return id - 3;
  }
  else if (id < 21)
  {
    return node_id - 4;
  }
  else if (id < 29)
  {
    return id - 5;
  }
  else
  {
    return id - 7;
  }
#elif TESTBED == TESTBED_KIEL
  if (id < 11)
  {
    return id - 1;
  }
  else
  {
    return id - 2;
  }
#elif TESTBED == TESTBED_DESK
  return id - 1;
#endif
}
/*---------------------------------------------------------------------------*/
#ifdef MASTER_SCHEDULE
static void set_destination_link_addr(uint8_t destination_node_id)
{
  destination.u8[NODE_ID_INDEX] = destination_node_id;
}
#endif /* MASTER_SCHEDULE */
/*---------------------------------------------------------------------------*/
#ifndef MASTER_SCHEDULE
static int
install_discovery_schedule()
{
  LOG_INFO("install discovery schedule\n");
  uint8_t tx_slot;
  uint16_t num_discovery_sending_slots;
  tx_slot = get_destination_index(node_id);

  if (deployment_node_count % 2 == 0)
  {
    num_discovery_sending_slots = deployment_node_count + 1;
  }
  else
  {
    num_discovery_sending_slots = deployment_node_count;
  }
  /* Create slotframe sf1 */
  sf[1] = tsch_schedule_get_slotframe_by_handle(1);
  if (sf[1])
  {
    tsch_schedule_remove_slotframe(sf[1]);
  }
  sf[1] = tsch_schedule_add_slotframe(1, num_discovery_sending_slots);

  sf[2] = tsch_schedule_get_slotframe_by_handle(2);
  if (sf[2])
  {
    tsch_schedule_remove_slotframe(sf[2]);
  }
  sf[2] = tsch_schedule_add_slotframe(2, 1);
  //our own
  sf[3] = tsch_schedule_get_slotframe_by_handle(3);
  if (sf[3])
  {
    tsch_schedule_remove_slotframe(sf[3]);
  }
  sf[3] = tsch_schedule_add_slotframe(3, num_discovery_sending_slots);

  tsch_schedule_add_link(sf[2], LINK_OPTION_RX, LINK_TYPE_NORMAL, &tsch_broadcast_address, 0, 0);
  if (tx_slot < num_discovery_sending_slots)
  {
    tsch_schedule_add_link(sf[1], LINK_OPTION_TX, LINK_TYPE_NORMAL, &tsch_broadcast_address, tx_slot, 0);
    own_transmission_flow = 1;
    is_sender = 1;
    //LOG_INFO("sender\n");
    own_receiver = 0;
    return 1;
  }

  return 0;
}
#endif /* !MASTER_SCHEDULE */
/*---------------------------------------------------------------------------*/
/**
 * \brief Add links to TSCH slot-frames based on calculated schedule
 */
#ifdef MASTER_SCHEDULE
static void
add_links(const scheduled_link_t *links, uint8_t number_links, const uint8_t *change_on_index, const uint8_t *change_on_index_to, uint8_t number_changes)
{
  uint8_t link_idx;
  uint8_t change_idx = 0;
  for (link_idx = 0; link_idx < number_links; ++link_idx)
  {
    if (change_idx < number_changes && link_idx == change_on_index[change_idx])
    {
      destination.u8[NODE_ID_INDEX] = change_on_index_to[change_idx]; //absolute value, therefore no destinations[] needed
      ++change_idx;
    }
    struct tsch_slotframe *sf;
    sf = tsch_schedule_get_slotframe_by_handle(links[link_idx].slotframe_handle);
    tsch_schedule_add_link(sf, links[link_idx].send_receive, LINK_TYPE_NORMAL, &destination, links[link_idx].timeslot, links[link_idx].channel_offset);
  }
  // tsch_schedule_print();
}
#endif /* MASTER_SCHEDULE */
/*---------------------------------------------------------------------------*/
static void
master_install_schedule(void *ptr)
{
  LOG_INFO("install schedule\n");
  tsch_set_eb_period(TSCH_EB_PERIOD);
  tsch_schedule_remove_slotframe(sf[0]);
  sf[0] = tsch_schedule_add_slotframe(0, MASTER_EBSF_PERIOD);
#ifdef MASTER_SCHEDULE
  tsch_schedule_add_link(sf[0], LINK_OPTION_TX | LINK_OPTION_RX | LINK_OPTION_SHARED | LINK_OPTION_TIME_KEEPING, LINK_TYPE_ADVERTISING, &tsch_broadcast_address, 0, 0);
#include MASTER_SCHEDULE
#else
  tsch_schedule_add_link(sf[0], LINK_OPTION_TX | LINK_OPTION_RX, LINK_TYPE_ADVERTISING_ONLY, &tsch_broadcast_address, 0, 0);
  install_discovery_schedule();
  //TODOLIV: log success of schedule installation -> unsuccessful means not large enough sf size
#endif /* MASTER_SCHEDULE */
  is_configured = 1;
  //if MASTER_SCHEDULE -> install schedule, else -> install ND schedule
}
/*---------------------------------------------------------------------------*/
void master_routing_set_input_callback(master_routing_input_callback callback)
{
  if (started == 0)
  {
    init_master_routing();
  }
  current_callback = callback;
}

linkaddr_t getAddressByNodeID(int givenNodeID)
{
  linkaddr_t ans = {{(unsigned char)givenNodeID, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
  return ans;
}

int master_routing_send_advertisement(float strength, uint16_t adv_seq, uint8_t best_node)
{
  //LOG_INFO("ND send\n");
  if (own_transmission_flow != 0)
  {
    uint8_t success = 0;
    mrp.packet_type = PACKET_TYPE_ADVERTISEMENT;
    mrp.flow_number = node_id; // for neighbor discovery: send sender instead of flow-number
    mrp.advertisement_seq = adv_seq;
    mrp.strengthToMaster = strength;
    mrp.bestNode = best_node;
    sent_packet_configuration.max_tx = 1;
    masternet_len = minimal_routing_packet_size;
    //history = mrp;
    history_ad_seq_array[mrp.flow_number] = mrp.advertisement_seq;
    success = NETSTACK_NETWORK.output(NULL);
    LOG_INFO("master advertisement sent;%u;%u;%u;", mrp.flow_number, mrp.packet_type, mrp.advertisement_seq); //sent;<from>;<number>
    printFLoat(mrp.strengthToMaster);
    printf("\n");
    return success;
  }
  else
  {
    LOG_INFO("Node %u is no sender!\n", node_id);
    return 0;
  }
}
//static int counter_ad = 0;

void on_received_advertisement(master_routing_packet_t mrp, const linkaddr_t *src)
{

  if (mrp.bestNode == node_id)
  {
    return;
  }
  float currentStrength = MIN(mrp.strengthToMaster, getStrengthByNodeID(mrp.flow_number));
  int currentSeq = mrp.advertisement_seq;

  if (max_advertisement_seq < currentSeq)
  {
    max_advertisement_seq = currentSeq;

    if (bestNode == mrp.flow_number)
    {
      max_master_strength = currentStrength;
    }
  }

  if (currentSeq >= max_advertisement_seq && max_master_strength < currentStrength)
  {
    max_master_strength = currentStrength;

    if (bestNode != mrp.flow_number)
    {
      bestNode = mrp.flow_number;
      add_link_to_best_node();
    }
    // memcpy(&bestLink, src, sizeof(linkaddr_t));
  }

  // nullnet_buf = (uint8_t *)&advertisementData;
  // advertisementData.packet_type = PACKET_TYPE_ADVERTISEMENT;
  // advertisementData.nodeId = node_id;
  // advertisementData.strengthToMaster = maxMasterStrength;
  // advertisementData.seq = maxSeqforStrength;

  if (history_ad_seq_array[mrp.flow_number] == mrp.advertisement_seq)
  {
    //skip
  }
  else
  {
    if (node_id != 1)
    {
      /*counter_ad += 1;
      printf("advertisement resend node_id=%d counter=%d\n",node_id, counter_ad);*/
      history_ad_seq_array[mrp.flow_number] = mrp.advertisement_seq;
      master_routing_send_advertisement(max_master_strength, max_advertisement_seq, bestNode);
      //history = mrp;

      printf("advertisement Seq: %d, Selected BestNode = %d, Because: His Strength: ", currentSeq, bestNode);
      printFLoat(mrp.strengthToMaster);
      printf(", And MyLink Strength With Him --> ");
      printFLoat(getStrengthByNodeID(mrp.flow_number));
      printf("\n");
    }
  }
}

void send_data_to_master()
{
  if (bestNode > 0)
  {

    int i = 0;

    mrp.packet_type = PACKET_TYPE_ACTUAL_DATA;
    mrp.flow_number = node_id; // for neighbor discovery: send sender instead of flow-number
    uint8_t index = prepareStrengthVectorToSend(mrp.data);

    for (i = 1; i < index; i += 3)
    {
      uint16_t value;
      memcpy(&value, mrp.data + i + 1, sizeof(uint16_t));
      printf("(N,S) = (%d,%d)     ", mrp.data[i], value);
    }
    printf("\n");

    mrp.data[index] = 1;
    mrp.data[index + 1] = node_id;
    index += 2;
    mrp.data_len = index;
    printf("Checking preparaed data: highestNode: %d, index:%d\n", mrp.data[0], index);
    //sent_packet_configuration.max_tx = 1;
    //masternet_len = minimal_routing_packet_size + index;
    //linkaddr_t bestLink = getAddressByNodeID(bestNode);

    // printf("BestLink: ");
    // for (i=0; i < 8; i++)
    // {
    //   printf("%d ", bestLink.u8[i]);
    // }
    // printf("\n");

    // printf("sending actual data inside mrp: ");
    // for(i = 0; i<mrp.data_len; i++){
    //   uint8_t value = mrp.data[i];
    //   printf("%d ", value);
    // }
    // printf("\n");

    //NETSTACK_NETWORK.output(&bestLink);
    unicast_send();

    //NETSTACK_NETWORK.output(&bestLink);
  }
}

void unicast_send()
{

  sent_packet_configuration.max_tx = 1;
  masternet_len = minimal_routing_packet_size + mrp.data_len;
  linkaddr_t bestLink = getAddressByNodeID(bestNode);

  NETSTACK_NETWORK.output(&bestLink);
}

void on_received_actual_data(master_routing_packet_t rcvd_mrp)
{
  uint8_t actualData[50];
  flag_to_stop_ad = 1;
  if (node_id == 1)
  {
    rcvd_actual_data_counter += 1;
    printf("on received actual data: ");
    int i;
    for (i = 0; i < rcvd_mrp.data_len; i++)
    {
      uint8_t value = rcvd_mrp.data[i];
      printf("%d ", value);
    }
    printf("\n");
    //numberofnodes --> n1..n2..n -> datalen--> data
    memcpy(actualData, rcvd_mrp.data, rcvd_mrp.data_len);
    int pos = rcvd_mrp.data[0] * 3 + 1;
    uint8_t numberOfNodes;
    memcpy(&numberOfNodes, actualData + pos, sizeof(uint8_t));
    pos += sizeof(uint8_t);
    //int i;
    printf("Got Actual Data --> Route (Size: %d): ", numberOfNodes + 1);

    for (i = 0; i < numberOfNodes; i++, pos += sizeof(uint8_t))
    {
      uint8_t curNode;
      memcpy(&curNode, actualData + pos, sizeof(uint8_t));
      if (i == 0)
      {
        printf("%d", curNode);
      }
      else
      {
        printf(" --> %d", curNode);
      }
    }
    printf(" --> %d\n", node_id);

    //strength_vector(8): 3 1-1.000,2-1.000,3-.000,4-.750,5-1.000,
    int vc_size = rcvd_mrp.data[0] * 3 + 1;
    uint8_t rcvd_from;
    memcpy(&rcvd_from, rcvd_mrp.data + vc_size + 1, sizeof(uint8_t));
    printf("strength_vector: %d ", rcvd_from);

    for (i = 1; i < vc_size; i += 3)
    {
      uint8_t nodeId = rcvd_mrp.data[i];
      uint16_t value = 0;
      memcpy(&value, rcvd_mrp.data + i + 1, sizeof(uint16_t));
      float strength = value / 10000.0;
      printf("%d-", nodeId);
      printFLoat(strength);
      printf(",");
    }
    printf("\n");

    if (rcvd_actual_data_counter % (highestNodeID -1) == 0)
    {
      //own strength
      printf("strength_vector: %d ", node_id);
      for (i = 1; i <= highestNodeID; i++)
      {
        printf("%d-", i);
        printFLoat(strength_vector[i]);
        printf(",");
      }
      printf("\n");
    }
  }
  else
  {
    //Forwarding
    int i;
    printf("Received actual data from node: %d\n", rcvd_mrp.flow_number);
    //memcpy(actualData, mrp.data, mrp.data_len);

    ///calculating position

    int pos = rcvd_mrp.data[0] * 3 + 1;
    uint8_t numberOfNodes;
    memcpy(&numberOfNodes, rcvd_mrp.data + pos, sizeof(uint8_t));
    numberOfNodes++;
    memcpy(rcvd_mrp.data + pos, &numberOfNodes, sizeof(uint8_t));

    pos += sizeof(uint8_t);
    pos += ((numberOfNodes - 1) * sizeof(uint8_t));

    memcpy(rcvd_mrp.data + pos, &node_id, sizeof(uint8_t));
    rcvd_mrp.data_len = rcvd_mrp.data_len + sizeof(uint8_t);

    printf("modified actual data: ");
    for (i = 0; i < rcvd_mrp.data_len; i++)
    {
      uint8_t value = rcvd_mrp.data[i];
      printf("%d ", value);
    }
    printf("\n");
    printf("Forwarding actualData (node) = (%d) --> to (node) = (%d)\n", node_id, bestNode);

    memcpy(&mrp, &rcvd_mrp, sizeof(rcvd_mrp) - MASTER_MSG_LENGTH + rcvd_mrp.data_len);
    unicast_send();

    //send_data_to_master(actualData, newDatalen);
  }
}
/*---------------------------------------------------------------------------*/
void master_routing_input(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest)
{
  uint8_t type;
  memcpy(&type, data, sizeof(uint8_t));
  printf("Got Some Data len = %d, type = %d\n", len, type);
  /*if(type == PACKET_TYPE_ACTUAL_DATA){
    memcpy(&mrp, data, len);
    LOG_INFO("rcvd actual data %u;%u;%u\n", mrp.packet_type, mrp.flow_number, mrp.data_len);
    on_received_actual_data(mrp);
    
  }*/

  leds_on(LEDS_RED);
  if (len >= minimal_routing_packet_size && len <= maximal_routing_packet_size)
  {
    uint8_t forward_to_upper_layer = 0;

    memcpy(&type, data, sizeof(uint8_t));
    memcpy(&mrp, data, len);
    if (type == PACKET_TYPE_BEACON)
    {
      //nothing
    }
    else if (type == PACKET_TYPE_ADVERTISEMENT /*&& !flag_to_stop_ad*/)
    {
      LOG_INFO("Received advertisement %u;%u;%u;", mrp.packet_type, mrp.flow_number, mrp.advertisement_seq);
      printFLoat(mrp.strengthToMaster);
      printf("\n");
      on_received_advertisement(mrp, src);
      return;
    }
    else if (type == PACKET_TYPE_ACTUAL_DATA)
    {
      memcpy(&mrp, data, len);
      LOG_INFO("rcvd actual data %u;%u;%u\n", mrp.packet_type, mrp.flow_number, mrp.data_len);
      on_received_actual_data(mrp);
      return;
    }
    else
    {
      LOG_INFO("PACKET_TYPE_UNKNOWN\n");
      return;
    }
#ifndef MASTER_SCHEDULE //neighbor discovery
    uint8_t sender = mrp.flow_number;
    LOG_INFO("rcvd;%u;%u;%u;%u;%d\n", node_id, sender, packetbuf_attr(PACKETBUF_ATTR_CHANNEL), mrp.packet_number, (int16_t)packetbuf_attr(PACKETBUF_ATTR_RSSI)); //rcvd;<to>;<from>;<channel>;<number>;<rssi>
    forward_to_upper_layer = 1;
    Beacon receivedBeacon;
    receivedBeacon.nodeID = mrp.flow_number;
    receivedBeacon.seq = mrp.packet_number;
    receivedBeacon.packet_type = PACKET_TYPE_BEACON;
    onReceivedNewBeacon(node_id, receivedBeacon);
#else //normal operation
    uint16_t received_asn = packetbuf_attr(PACKETBUF_ATTR_RECEIVED_ASN);

    // this node is receiver:
    if (node_id == receiver_of_flow[mrp.flow_number])
    {
      if (TSCH_SLOTNUM_LT((uint16_t)hash_map_lookup(&last_received_relayed_packet_of_flow, mrp.flow_number), mrp.packet_number))
      {                                                                                             //if old known one < new one
        hash_map_insert(&last_received_relayed_packet_of_flow, mrp.flow_number, mrp.packet_number); //update last received packet number
        LOG_INFO("rcvd;%u;%u;%u;%u.\n", mrp.packet_number, received_asn, mrp.flow_number, sender_of_flow[mrp.flow_number]);
        forward_to_upper_layer = 1;
      }
      else
      {
        LOG_INFO("received %u at ASN %u duplicate from flow %u, sent by sender %u.\n", mrp.packet_number, received_asn, mrp.flow_number, sender_of_flow[mrp.flow_number]);
      }
    }
    else
    { //forward
      uint8_t next_receiver = hash_map_lookup(&forward_to, mrp.flow_number);
      if (next_receiver != 0)
      {
        if (TSCH_SLOTNUM_LT((uint16_t)hash_map_lookup(&last_received_relayed_packet_of_flow, mrp.flow_number), mrp.packet_number))
        {                                                                                             //if old known one < new one
          hash_map_insert(&last_received_relayed_packet_of_flow, mrp.flow_number, mrp.packet_number); //update last received packet number
          set_destination_link_addr(next_receiver);
#if TSCH_FLOW_BASED_QUEUES
          sent_packet_configuration.flow_number = mrp.flow_number;
#endif /* TSCH_FLOW_BASED_QUEUES */
#if TSCH_TTL_BASED_RETRANSMISSIONS
          if (TSCH_SLOTNUM_LT((uint16_t)tsch_current_asn.ls4b, mrp.ttl_slot_number + 1))
          { // send only if time left for sending - we might already be in the last slot!
            //packetbuf set TTL
            sent_packet_configuration.ttl_slot_number = mrp.ttl_slot_number;
            sent_packet_configuration.earliest_tx_slot = mrp.earliest_tx_slot;
            //set max_transmissions
            sent_packet_configuration.max_tx = (uint16_t)TSCH_SLOTNUM_DIFF16(mrp.ttl_slot_number, (uint16_t)(tsch_current_asn.ls4b - 1)); //(uint16_t) (0xFFFF + 1 + nullnet_routing_packet.ttl_slot_number - (uint16_t) tsch_current_asn.ls4b); //include current asn
            masternet_len = len;                                                                                                          //send same length as received
            NETSTACK_NETWORK.output(&destination);
            LOG_INFO("relay;%u;%u;%u;%u.\n", mrp.packet_number, received_asn, mrp.flow_number, sender_of_flow[mrp.flow_number]);
          }
          else
          {
            LOG_INFO("relay %u at ASN %u from flow %u, sent by sender %u.\n", mrp.packet_number, received_asn, mrp.flow_number, sender_of_flow[mrp.flow_number]);
            LOG_INFO("packet not enqueueing: next ASN %u, received ASN %u, TTL ASN %u\n", (uint16_t)tsch_current_asn.ls4b, received_asn, mrp.ttl_slot_number);
          }
#else
          sent_packet_configuration.max_tx = max_transmissions[mrp.flow_number - 1];
          masternet_len = len; //send same length as received
          NETSTACK_NETWORK.output(&destination);
          LOG_INFO("relay %u at ASN %u from flow %u, sent by sender %u.\n", mrp.packet_number, received_asn, mrp.flow_number, sender_of_flow[mrp.flow_number]);
#endif /* TSCH_TTL_BASED_RETRANSMISSIONS */
        }
        else
        {
          LOG_INFO("received %u duplicate from flow %u, sent by sender %u at ASN %u for relay.\n", mrp.packet_number, mrp.flow_number, sender_of_flow[mrp.flow_number], received_asn);
        }
      }
      else
      {
        LOG_INFO("No routing info for flow %u, sent by sender %u at node %u.\n", mrp.flow_number, sender_of_flow[mrp.flow_number], node_id);
      }
    }
#endif /* !MASTER_SCHEDULE */

    if (forward_to_upper_layer && len > minimal_routing_packet_size)
    {
      //TODO: exchange source by flow-source
      //upper layer input callback (&mrp.data, len-minimal_routing_packet_size)
#ifdef MASTER_SCHEDULE
      current_callback((void *)&mrp.data, len - minimal_routing_packet_size, sender_of_flow[mrp.flow_number], receiver_of_flow[mrp.flow_number]);
#else
      current_callback((void *)&mrp.data, len - minimal_routing_packet_size, src->u8[NODE_ID_INDEX], dest->u8[NODE_ID_INDEX]);
#endif /* MASTER_SCHEDULE */
    }
  }
  leds_off(LEDS_RED);
}
/*---------------------------------------------------------------------------*/
master_packetbuf_config_t
master_routing_sent_configuration()
{
  return sent_packet_configuration;
}
/*---------------------------------------------------------------------------*/
int node_is_sender()
{
  return is_sender;
}
/*---------------------------------------------------------------------------*/
int get_node_receiver()
{
  return own_receiver;
}
/*---------------------------------------------------------------------------*/
int master_routing_configured()
{
  return is_configured;
}
/*---------------------------------------------------------------------------*/
int neighbor_discovery_send(const void *data, uint16_t datalen)
{
  //LOG_INFO("ND send\n");
  if (own_transmission_flow != 0)
  {

    uint8_t success = 0;
    mrp.packet_type = PACKET_TYPE_BEACON;
    mrp.flow_number = node_id; // for neighbor discovery: send sender instead of flow-number
    mrp.packet_number = ++own_packet_number;
    memcpy(mrp.data, data, datalen);
    sent_packet_configuration.max_tx = 1;
    masternet_len = minimal_routing_packet_size + datalen;
    success = NETSTACK_NETWORK.output(NULL);
    LOG_INFO("sent;%u;%u\n", node_id, own_packet_number); //sent;<from>;<number>
    printf("Beacon masterlen=%d\n", masternet_len);
    return success;
  }
  else
  {
    LOG_INFO("Node %u is no sender!\n", node_id);
    return 0;
  }
}

void add_link_to_best_node()
{
  printf("Now Changing best schedule Link\n");
  if (best_tsch_shedule_link != NULL)
  {
    tsch_schedule_remove_link(sf[3], best_tsch_shedule_link);
    best_tsch_shedule_link = NULL;
  }

  linkaddr_t addr = getAddressByNodeID(bestNode);
  uint8_t link_options;
  uint16_t slot_offset = deployment_node_count - node_id; // keeping slot offset as our own Node ID
  uint16_t channel_offset = deployment_node_count - node_id;
  link_options = LINK_OPTION_TX;
  best_tsch_shedule_link = tsch_schedule_add_link(sf[3],
                                                  link_options,
                                                  LINK_TYPE_NORMAL, &addr,
                                                  slot_offset, channel_offset);
}

/*---------------------------------------------------------------------------*/
int master_routing_send(const void *data, uint16_t datalen)
{
#ifndef MASTER_SCHEDULE
  return neighbor_discovery_send(data, datalen);
#else
  if (own_transmission_flow != 0)
  {
    mrp.flow_number = own_transmission_flow;
    mrp.packet_number = ++own_packet_number;
    memcpy(&(mrp.data), data, datalen);

    // get current / next active ASN (tsch_current_asn)
    // get corresponding slotframe slot number (TSCH_ASN_MOD(tsch_current_asn, sf->size))
    struct tsch_slotframe *sf;
    uint16_t sf_size;
    uint16_t current_sf_slot;
    // uint16_t current_asn = (uint16_t)tsch_current_asn.ls4b;
    struct tsch_asn_t current_asn = tsch_current_asn;
    sf = tsch_schedule_get_slotframe_by_handle(own_transmission_flow);
    sf_size = ((uint16_t)((sf->size).val));
    current_sf_slot = TSCH_ASN_MOD(current_asn, sf->size);

#if TSCH_TTL_BASED_RETRANSMISSIONS
    // wrap-arounds if the next timeslot should be the first timeslot in the next hyper-period (round)
    // LOG_INFO("DEBUG: %u;%u;%u;%u;%u.\n", current_sf_slot, (uint16_t)current_asn.ls4b, (uint16_t)tsch_current_asn.ls4b, sf_size, ((uint16_t)tsch_current_asn.ls4b % sf_size));
    int first_next_idx = 0;
    if (current_sf_slot > first_tx_slot_in_flow[own_transmission_flow - 1][sf_size / schedule_length - 1])
    {
      first_next_idx = 0;
    }
    else
    {
      // finds the next index, which stores the next possible timeslot to send
      int index = 0;
      for (/* none */; index < (sf_size / schedule_length); index++)
      {
        if (current_sf_slot <= first_tx_slot_in_flow[own_transmission_flow - 1][index])
        {
          first_next_idx = index;
          break;
        }
      }
    }
    // determines the next timeslot based on the current and the next timeslot stored in first/last_tx_slot_in_flow
    mrp.earliest_tx_slot = (uint16_t)current_asn.ls4b + (sf_size + first_tx_slot_in_flow[own_transmission_flow - 1][first_next_idx] - current_sf_slot) % sf_size; // earliest slot in next slotframe
    mrp.ttl_slot_number = mrp.earliest_tx_slot + (sf_size + last_tx_slot_in_flow[own_transmission_flow - 1][first_next_idx] - first_tx_slot_in_flow[own_transmission_flow - 1][first_next_idx]) % sf_size;
    if (TSCH_SLOTNUM_LT(mrp.earliest_tx_slot, last_sent_packet_ttl))
    { // avoid duplicates in earliest ASN
      --own_packet_number;
      LOG_INFO("Too high sending frequency, try again later\n");
      return 0;
    }
    // last_sent_packet_asn = mrp.earliest_tx_slot;
    last_sent_packet_ttl = mrp.ttl_slot_number;
#endif /* TSCH_TTL_BASED_RETRANSMISSIONS */

    uint8_t next_receiver = hash_map_lookup(&forward_to, mrp.flow_number);
    if (next_receiver != 0)
    {
      set_destination_link_addr(next_receiver);

#if TSCH_FLOW_BASED_QUEUES
      sent_packet_configuration.flow_number = mrp.flow_number;
#endif /* TSCH_FLOW_BASED_QUEUES */

#if TSCH_TTL_BASED_RETRANSMISSIONS
      //packetbuf set TTL
      sent_packet_configuration.ttl_slot_number = mrp.ttl_slot_number;
      sent_packet_configuration.earliest_tx_slot = mrp.earliest_tx_slot;
      //set max_transmissions
      sent_packet_configuration.max_tx = (uint16_t)TSCH_SLOTNUM_DIFF16(mrp.ttl_slot_number, (uint16_t)(current_asn.ls4b - 1)); // (uint16_t) (0xFFFF + 1 + nullnet_routing_packet.ttl_slot_number - nullnet_routing_packet.earliest_tx_slot); // include earliest slot!
#else
      sent_packet_configuration.max_tx = max_transmissions[sent_packet_configuration.flow_number - 1];
#endif /* TSCH_TTL_BASED_RETRANSMISSIONS */
      LOG_INFO("expected max tx: %u\n", sent_packet_configuration.max_tx);

      masternet_len = minimal_routing_packet_size + datalen;
      NETSTACK_NETWORK.output(&destination);

      //print sent data
#if TSCH_TTL_BASED_RETRANSMISSIONS
      LOG_INFO("sent;%u;%u;%u;%u;%u.\n", mrp.packet_number, mrp.earliest_tx_slot, mrp.ttl_slot_number, mrp.flow_number, sender_of_flow[mrp.flow_number]);
#else
      // calculate sending slot based on
      uint8_t tx_slot_idx;
      uint16_t earliest_tx_slot_asn;
      uint16_t earliest_slot_number_offset = 0xFFFF;
      uint16_t local_slot_number_offset;
      for (tx_slot_idx = 0; tx_slot_idx < num_sending_slots; ++tx_slot_idx)
      {
        if (sending_slots[tx_slot_idx] >= current_sf_slot)
        {
          local_slot_number_offset = sending_slots[tx_slot_idx] - current_sf_slot;
        }
        else
        {
          local_slot_number_offset = sending_slots[tx_slot_idx] + sf_size - current_sf_slot;
        }
        if (local_slot_number_offset < earliest_slot_number_offset)
        {
          earliest_slot_number_offset = local_slot_number_offset;
        }
      }
      earliest_tx_slot_asn = (uint16_t)current_asn.ls4b + earliest_slot_number_offset;

      LOG_INFO("sent %u at ASN %u from flow %u, sent by sender %u.\n", mrp.packet_number, earliest_tx_slot_asn, mrp.flow_number, sender_of_flow[mrp.flow_number]);
#endif /* TSCH_TTL_BASED_RETRANSMISSIONS */
    }
    else
    {
      LOG_INFO("No routing info for flow %u, sent by sender %u.\n", mrp.flow_number, sender_of_flow[mrp.flow_number]);
    }
    return 1;
  }
  else
  {
    LOG_INFO("Node %u is no sender!\n", node_id);
    return 0;
  }
#endif /* !MASTER_SCHEDULE */
}
/*---------------------------------------------------------------------------*/
int master_routing_sendto(const void *data, uint16_t datalen, uint8_t receiver)
{
  //LOG_INFO("send length %u to %u", datalen, receiver);
  if (receiver == own_receiver)
  {
    return master_routing_send(data, datalen);
  }
  else
  {
    LOG_INFO("No routing info for receiver %u\n", receiver);
    return 0;
  }
}

/*---------------------------------------------------------------------------*/

int master_routing_send_advertisement_sendto(float strength, uint16_t adv_seq, uint8_t best_node)
{

  max_advertisement_seq = adv_seq;
  bestNode = best_node;
  max_master_strength = strength;
  //modify_schedule();

  return master_routing_send_advertisement(strength, adv_seq, bestNode);

  /*if(!flag_to_stop_ad){
    return master_routing_send_advertisement(strength, adv_seq, bestNode);
  }else{
    return 0;
  }*/
}

int master_routing_send_actual_data()
{
  send_data_to_master();
  return 1;
}
/*---------------------------------------------------------------------------*/
void init_master_routing(void)
{
#if NETSTACK_CONF_WITH_MASTER_NET
  if (started == 0)
  {
#if MAC_CONF_WITH_TSCH
    tsch_set_coordinator(linkaddr_cmp(&coordinator_addr, &linkaddr_node_addr));
    tsch_set_eb_period(CLOCK_SECOND / 4);
#endif /* MAC_CONF_WITH_TSCH */

    /* Initialize MasterNet */
    masternet_buf = (uint8_t *)&mrp;
    masternet_len = sizeof(master_routing_packet_t);

    current_callback = NULL;

    /* Register MasterNet input/config callback */
    masternet_set_input_callback(master_routing_input); //TODOLIV
    masternet_set_config_callback(master_routing_sent_configuration);

    tsch_schedule_remove_all_slotframes();
    sf[0] = tsch_schedule_add_slotframe(0, 1);
    tsch_schedule_add_link(sf[0], LINK_OPTION_TX | LINK_OPTION_RX, LINK_TYPE_ADVERTISING_ONLY, &tsch_broadcast_address, 0, 0);

    //sf[3] = tsch_schedule_add_slotframe(3, 5);
    //tsch_schedule_add_link(sf[3], LINK_OPTION_TX | LINK_OPTION_RX, LINK_TYPE_ADVERTISING_ONLY, &tsch_broadcast_address, 0, 0);

    // started = 1;
    // LOG_INFO("started\n");
    // int i;
    // for (i = 0; i < 5; ++i)
    // {
    //   uint8_t link_options;
    //   linkaddr_t addr;
    //   uint16_t remote_id = i + 1;

    //   addr = getAddressByNodeID(remote_id);

    //   /* Add a unicast cell for each potential neighbor (in Cooja) */
    //   /* Use the same slot offset; the right link will be dynamically selected at runtime based on queue sizes */
    //     uint16_t slot_offset = i;
    //     uint16_t channel_offset = i;
    //   /* Warning: LINK_OPTION_SHARED cannot be configured, as with this schedule
    //  * backoff windows will not be reset correctly! */
    //   link_options = remote_id == node_id ? LINK_OPTION_RX : LINK_OPTION_TX;

    //   tsch_schedule_add_link(sf[0],
    //                          link_options,
    //                          LINK_TYPE_NORMAL, &addr,
    //                          slot_offset, channel_offset);

    // }

    linkaddr_t addr = getAddressByNodeID(node_id);
    uint8_t link_options;
    uint16_t slot_offset = deployment_node_count - node_id; // keeping slot offset as our own Node ID
    uint16_t channel_offset = deployment_node_count - node_id;
    link_options = LINK_OPTION_RX;
    tsch_schedule_add_link(sf[3],
                           link_options,
                           LINK_TYPE_NORMAL, &addr,
                           slot_offset, channel_offset);

    /* wait for end of TSCH initialization phase, timed with MASTER_INIT_PERIOD */
    ctimer_set(&install_schedule_timer, MASTER_INIT_PERIOD, master_install_schedule, NULL);

    set_own_node_id_as_highest(node_id);
  }
#else
  LOG_ERR("can't init master-routing: master-net not configured\n");
#endif /* NETSTACK_CONF_WITH_MASTER_NET */
}
/*---------------------------------------------------------------------------*/
/** @} */

// uint8_t schedule_modified = 0;
//  void modify_schedule()
// {
//   if (!schedule_modified)
//   {
//     schedule_length = 45;
//     sf[4] = tsch_schedule_get_slotframe_by_handle(4);
//     if (sf[4])
//     {
//       tsch_schedule_remove_slotframe(sf[4]);
//     }
//     sf[4] = tsch_schedule_add_slotframe(4, schedule_length);
//     hash_map_insert(&last_received_relayed_packet_of_flow, 1, 0);
//     sender_of_flow[2] = 2;
//     receiver_of_flow[2] = 1;
// #if TSCH_TTL_BASED_RETRANSMISSIONS
//     first_tx_slot_in_flow[0][0] = 19;
//     last_tx_slot_in_flow[0][0] = 20;
// #endif /* TSCH_TTL_BASED_RETRANSMISSIONS */
//     if (node_id == 2)
//     {
//       own_transmission_flow = 1;
//       is_sender = 1;
//       own_receiver = 1;
//       schedule_length = 45;
//       const scheduled_link_t add_link[] = {
//           {2, 1, 19, 0},
//           {2, 1, 20, 0},
//       };
//       hash_map_insert(&forward_to, 2, 1);
//       const uint8_t cha_idx[] = {
//           0,
//       };
//       const uint8_t cha_idx_to[] = {
//           1,
//       };
//       add_links(add_link, 2, cha_idx, cha_idx_to, 1);
//       destination.u8[NODE_ID_INDEX] = 1;
//       tsch_queue_update_time_source(&destination);
//       max_transmissions[0] = 2;
// #if !TSCH_TTL_BASED_RETRANSMISSIONS
//       sending_slots[0] = 19;
//       sending_slots[1] = 20;
//       num_sending_slots = 2;
// #endif /* !TSCH_TTL_BASED_RETRANSMISSIONS */
//     }
//     else if (node_id == 1)
//     {
//       const scheduled_link_t add_link[] = {
//           {1, 2, 19, 0},
//           {1, 2, 20, 0},
//       };
//       const uint8_t cha_idx[] = {
//           0,
//       };
//       const uint8_t cha_idx_to[] = {
//           2,
//       };
//       add_links(add_link, 2, cha_idx, cha_idx_to, 1);
//     }
//     else if (node_id == 3)
//     {
//       destination.u8[NODE_ID_INDEX] = 1;
//       tsch_queue_update_time_source(&destination);
//     }
//     else if (node_id == 4)
//     {
//       destination.u8[NODE_ID_INDEX] = 2;
//       tsch_queue_update_time_source(&destination);
//     }
//     else if (node_id == 5)
//     {
//       destination.u8[NODE_ID_INDEX] = 3;
//       tsch_queue_update_time_source(&destination);
//     }

//     printf("schedule modified: %d\n", node_id);
//     schedule_modified = 1;
//   }
// }