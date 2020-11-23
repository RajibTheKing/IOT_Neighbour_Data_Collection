schedule_length = 48;
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
sender_of_flow[1] = 1;
receiver_of_flow[1] = 3;
sender_of_flow[2] = 2;
receiver_of_flow[2] = 4;
#if TSCH_TTL_BASED_RETRANSMISSIONS
  first_tx_slot_in_flow[0][0] = 2;
  last_tx_slot_in_flow[0][0] = 2;
  first_tx_slot_in_flow[0][1] = 26;
  last_tx_slot_in_flow[0][1] = 26;
  first_tx_slot_in_flow[1][0] = 0;
  last_tx_slot_in_flow[1][0] = 1;
#endif /* TSCH_TTL_BASED_RETRANSMISSIONS */
if (node_id == 2){
  own_transmission_flow = 2;
  is_sender = 1;
  own_receiver = 4;
  schedule_length = 48;
  const scheduled_link_t add_link[] = {
    {  2,  1,  0,  0 },
    {  2,  1,  1,  0 },
  };
  hash_map_insert(&forward_to, 2, 4);
  const uint8_t cha_idx[] = {0, };
  const uint8_t cha_idx_to[] = {4, };
  add_links(add_link, 2, cha_idx, cha_idx_to, 1);
  destination.u8[NODE_ID_INDEX] = 1;
  tsch_queue_update_time_source(&destination);
  max_transmissions[1] = 2;
  #if !TSCH_TTL_BASED_RETRANSMISSIONS
    sending_slots[0] = 0;
    sending_slots[1] = 1;
    num_sending_slots = 2;
  #endif /* !TSCH_TTL_BASED_RETRANSMISSIONS */
} else if (node_id == 4){
  const scheduled_link_t add_link[] = {
    {  2,  2,  0,  0 },
    {  2,  2,  1,  0 },
  };
  const uint8_t cha_idx[] = {0, };
  const uint8_t cha_idx_to[] = {2, };
  add_links(add_link, 2, cha_idx, cha_idx_to, 1);
  destination.u8[NODE_ID_INDEX] = 1;
  tsch_queue_update_time_source(&destination);
} else if (node_id == 1){
  own_transmission_flow = 1;
  is_sender = 1;
  own_receiver = 3;
  schedule_length = 24;
  const scheduled_link_t add_link[] = {
    {  1,  1,  2,  0 },
    {  1,  1, 26,  0 },
  };
  hash_map_insert(&forward_to, 1, 3);
  const uint8_t cha_idx[] = {0, };
  const uint8_t cha_idx_to[] = {3, };
  add_links(add_link, 2, cha_idx, cha_idx_to, 1);
  max_transmissions[0] = 1;
  #if !TSCH_TTL_BASED_RETRANSMISSIONS
    sending_slots[0] = 2;
    num_sending_slots = 1;
  #endif /* !TSCH_TTL_BASED_RETRANSMISSIONS */
} else if (node_id == 3){
  const scheduled_link_t add_link[] = {
    {  1,  2,  2,  0 },
    {  1,  2, 26,  0 },
  };
  const uint8_t cha_idx[] = {0, };
  const uint8_t cha_idx_to[] = {1, };
  add_links(add_link, 2, cha_idx, cha_idx_to, 1);
  destination.u8[NODE_ID_INDEX] = 1;
  tsch_queue_update_time_source(&destination);
} else if (node_id == 5){
  destination.u8[NODE_ID_INDEX] = 1;
  tsch_queue_update_time_source(&destination);
}
