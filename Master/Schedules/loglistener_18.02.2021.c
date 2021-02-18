schedule_length = 286;
sf[1] = tsch_schedule_get_slotframe_by_handle(1);
if (sf[1]){
  tsch_schedule_remove_slotframe(sf[1]);
}
sf[1] = tsch_schedule_add_slotframe(1, schedule_length);
sf[2] = tsch_schedule_get_slotframe_by_handle(2);
if (sf[2]){
  tsch_schedule_remove_slotframe(sf[2]);
}
sf[2] = tsch_schedule_add_slotframe(2, schedule_length);
hash_map_insert(&last_received_relayed_packet_of_flow, 1, 0);
hash_map_insert(&last_received_relayed_packet_of_flow, 2, 0);
sender_of_flow[1] = 2;
receiver_of_flow[1] = 1;
sender_of_flow[2] = 1;
receiver_of_flow[2] = 2;
#if TSCH_TTL_BASED_RETRANSMISSIONS
  first_tx_slot_in_flow[0][0] = 0;
  last_tx_slot_in_flow[0][0] = 1;
  first_tx_slot_in_flow[0][1] = 22;
  last_tx_slot_in_flow[0][1] = 23;
  first_tx_slot_in_flow[0][2] = 44;
  last_tx_slot_in_flow[0][2] = 45;
  first_tx_slot_in_flow[0][3] = 66;
  last_tx_slot_in_flow[0][3] = 67;
  first_tx_slot_in_flow[0][4] = 88;
  last_tx_slot_in_flow[0][4] = 89;
  first_tx_slot_in_flow[0][5] = 110;
  last_tx_slot_in_flow[0][5] = 111;
  first_tx_slot_in_flow[0][6] = 133;
  last_tx_slot_in_flow[0][6] = 134;
  first_tx_slot_in_flow[0][7] = 154;
  last_tx_slot_in_flow[0][7] = 155;
  first_tx_slot_in_flow[0][8] = 176;
  last_tx_slot_in_flow[0][8] = 177;
  first_tx_slot_in_flow[0][9] = 198;
  last_tx_slot_in_flow[0][9] = 199;
  first_tx_slot_in_flow[0][10] = 220;
  last_tx_slot_in_flow[0][10] = 221;
  first_tx_slot_in_flow[0][11] = 242;
  last_tx_slot_in_flow[0][11] = 243;
  first_tx_slot_in_flow[0][12] = 264;
  last_tx_slot_in_flow[0][12] = 265;
  first_tx_slot_in_flow[1][0] = 2;
  last_tx_slot_in_flow[1][0] = 3;
  first_tx_slot_in_flow[1][1] = 27;
  last_tx_slot_in_flow[1][1] = 28;
  first_tx_slot_in_flow[1][2] = 53;
  last_tx_slot_in_flow[1][2] = 54;
  first_tx_slot_in_flow[1][3] = 79;
  last_tx_slot_in_flow[1][3] = 80;
  first_tx_slot_in_flow[1][4] = 105;
  last_tx_slot_in_flow[1][4] = 106;
  first_tx_slot_in_flow[1][5] = 131;
  last_tx_slot_in_flow[1][5] = 132;
  first_tx_slot_in_flow[1][6] = 157;
  last_tx_slot_in_flow[1][6] = 158;
  first_tx_slot_in_flow[1][7] = 183;
  last_tx_slot_in_flow[1][7] = 184;
  first_tx_slot_in_flow[1][8] = 209;
  last_tx_slot_in_flow[1][8] = 210;
  first_tx_slot_in_flow[1][9] = 235;
  last_tx_slot_in_flow[1][9] = 236;
  first_tx_slot_in_flow[1][10] = 261;
  last_tx_slot_in_flow[1][10] = 262;
#endif /* TSCH_TTL_BASED_RETRANSMISSIONS */
if (node_id == 2){
  own_transmission_flow = 1;
  is_sender = 1;
  own_receiver = 1;
  schedule_length = 22;
  const scheduled_link_t add_link[] = {
    {  1,  1,  0,  0 },
    {  1,  1,  1,  0 },
    {  2,  2,  2,  0 },
    {  2,  2,  3,  0 },
    {  1,  1, 22,  0 },
    {  1,  1, 23,  0 },
    {  2,  2, 27,  0 },
    {  2,  2, 28,  0 },
    {  1,  1, 44,  0 },
    {  1,  1, 45,  0 },
    {  2,  2, 53,  0 },
    {  2,  2, 54,  0 },
    {  1,  1, 66,  0 },
    {  1,  1, 67,  0 },
    {  2,  2, 79,  0 },
    {  2,  2, 80,  0 },
    {  1,  1, 88,  0 },
    {  1,  1, 89,  0 },
    {  2,  2, 105,  0 },
    {  2,  2, 106,  0 },
    {  1,  1, 110,  0 },
    {  1,  1, 111,  0 },
    {  2,  2, 131,  0 },
    {  2,  2, 132,  0 },
    {  1,  1, 133,  0 },
    {  1,  1, 134,  0 },
    {  1,  1, 154,  0 },
    {  1,  1, 155,  0 },
    {  2,  2, 157,  0 },
    {  2,  2, 158,  0 },
    {  1,  1, 176,  0 },
    {  1,  1, 177,  0 },
    {  2,  2, 183,  0 },
    {  2,  2, 184,  0 },
    {  1,  1, 198,  0 },
    {  1,  1, 199,  0 },
    {  2,  2, 209,  0 },
    {  2,  2, 210,  0 },
    {  1,  1, 220,  0 },
    {  1,  1, 221,  0 },
    {  2,  2, 235,  0 },
    {  2,  2, 236,  0 },
    {  1,  1, 242,  0 },
    {  1,  1, 243,  0 },
    {  2,  2, 261,  0 },
    {  2,  2, 262,  0 },
    {  1,  1, 264,  0 },
    {  1,  1, 265,  0 },
  };
  hash_map_insert(&forward_to, 1, 1);
  const uint8_t cha_idx[] = {0, };
  const uint8_t cha_idx_to[] = {1, };
  add_links(add_link, 48, cha_idx, cha_idx_to, 1);
  destination.u8[NODE_ID_INDEX] = 1;
  tsch_queue_update_time_source(&destination);
  max_transmissions[0] = 2;
  #if !TSCH_TTL_BASED_RETRANSMISSIONS
    sending_slots[0] = 0;
    sending_slots[1] = 1;
    num_sending_slots = 2;
  #endif /* !TSCH_TTL_BASED_RETRANSMISSIONS */
} else if (node_id == 1){
  own_transmission_flow = 2;
  is_sender = 1;
  own_receiver = 2;
  schedule_length = 26;
  const scheduled_link_t add_link[] = {
    {  1,  2,  0,  0 },
    {  1,  2,  1,  0 },
    {  2,  1,  2,  0 },
    {  2,  1,  3,  0 },
    {  1,  2, 22,  0 },
    {  1,  2, 23,  0 },
    {  2,  1, 27,  0 },
    {  2,  1, 28,  0 },
    {  1,  2, 44,  0 },
    {  1,  2, 45,  0 },
    {  2,  1, 53,  0 },
    {  2,  1, 54,  0 },
    {  1,  2, 66,  0 },
    {  1,  2, 67,  0 },
    {  2,  1, 79,  0 },
    {  2,  1, 80,  0 },
    {  1,  2, 88,  0 },
    {  1,  2, 89,  0 },
    {  2,  1, 105,  0 },
    {  2,  1, 106,  0 },
    {  1,  2, 110,  0 },
    {  1,  2, 111,  0 },
    {  2,  1, 131,  0 },
    {  2,  1, 132,  0 },
    {  1,  2, 133,  0 },
    {  1,  2, 134,  0 },
    {  1,  2, 154,  0 },
    {  1,  2, 155,  0 },
    {  2,  1, 157,  0 },
    {  2,  1, 158,  0 },
    {  1,  2, 176,  0 },
    {  1,  2, 177,  0 },
    {  2,  1, 183,  0 },
    {  2,  1, 184,  0 },
    {  1,  2, 198,  0 },
    {  1,  2, 199,  0 },
    {  2,  1, 209,  0 },
    {  2,  1, 210,  0 },
    {  1,  2, 220,  0 },
    {  1,  2, 221,  0 },
    {  2,  1, 235,  0 },
    {  2,  1, 236,  0 },
    {  1,  2, 242,  0 },
    {  1,  2, 243,  0 },
    {  2,  1, 261,  0 },
    {  2,  1, 262,  0 },
    {  1,  2, 264,  0 },
    {  1,  2, 265,  0 },
  };
  hash_map_insert(&forward_to, 2, 2);
  const uint8_t cha_idx[] = {0, };
  const uint8_t cha_idx_to[] = {2, };
  add_links(add_link, 48, cha_idx, cha_idx_to, 1);
  max_transmissions[1] = 2;
  #if !TSCH_TTL_BASED_RETRANSMISSIONS
    sending_slots[0] = 2;
    sending_slots[1] = 3;
    num_sending_slots = 2;
  #endif /* !TSCH_TTL_BASED_RETRANSMISSIONS */
} else if (node_id == 3){
  destination.u8[NODE_ID_INDEX] = 1;
  tsch_queue_update_time_source(&destination);
} else if (node_id == 4){
  destination.u8[NODE_ID_INDEX] = 2;
  tsch_queue_update_time_source(&destination);
} else if (node_id == 5){
  destination.u8[NODE_ID_INDEX] = 3;
  tsch_queue_update_time_source(&destination);
}
