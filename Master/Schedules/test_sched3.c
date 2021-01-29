schedule_length = 50;
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
sender_of_flow[1] = 5;
receiver_of_flow[1] = 1;
sender_of_flow[2] = 4;
receiver_of_flow[2] = 1;
#if TSCH_TTL_BASED_RETRANSMISSIONS
  first_tx_slot_in_flow[0][0] = 9;
  last_tx_slot_in_flow[0][0] = 10;
  first_tx_slot_in_flow[0][1] = 34;
  last_tx_slot_in_flow[0][1] = 35;
  first_tx_slot_in_flow[1][0] = 4;
  last_tx_slot_in_flow[1][0] = 7;
#endif /* TSCH_TTL_BASED_RETRANSMISSIONS */
if (node_id == 4){
  own_transmission_flow = 2;
  is_sender = 1;
  own_receiver = 1;
  schedule_length = 50;
  const scheduled_link_t add_link[] = {
    {  2,  1,  4,  0 },
    {  2,  1,  5,  0 },
    {  2,  1,  6,  0 },
  };
  hash_map_insert(&forward_to, 2, 2);
  const uint8_t cha_idx[] = {0, };
  const uint8_t cha_idx_to[] = {2, };
  add_links(add_link, 3, cha_idx, cha_idx_to, 1);
  destination.u8[NODE_ID_INDEX] = 2;
  tsch_queue_update_time_source(&destination);
  max_transmissions[1] = 3;
  #if !TSCH_TTL_BASED_RETRANSMISSIONS
    sending_slots[0] = 4;
    sending_slots[1] = 5;
    sending_slots[2] = 6;
    num_sending_slots = 3;
  #endif /* !TSCH_TTL_BASED_RETRANSMISSIONS */
} else if (node_id == 2){
  const scheduled_link_t add_link[] = {
    {  2,  3,  5,  0 },
    {  2,  3,  6,  0 },
    {  2,  1,  7,  0 },
    {  2,  2,  4,  0 },
  };
  hash_map_insert(&forward_to, 2, 1);
  const uint8_t cha_idx[] = {0, 3, };
  const uint8_t cha_idx_to[] = {1, 4, };
  add_links(add_link, 4, cha_idx, cha_idx_to, 2);
  destination.u8[NODE_ID_INDEX] = 1;
  tsch_queue_update_time_source(&destination);
  max_transmissions[1] = 3;
} else if (node_id == 1){
  const scheduled_link_t add_link[] = {
    {  2,  2,  5,  0 },
    {  2,  2,  6,  0 },
    {  2,  2,  7,  0 },
    {  1,  2,  9,  0 },
    {  1,  2, 10,  0 },
    {  1,  2, 34,  0 },
    {  1,  2, 35,  0 },
  };
  const uint8_t cha_idx[] = {0, 3, };
  const uint8_t cha_idx_to[] = {2, 5, };
  add_links(add_link, 7, cha_idx, cha_idx_to, 2);
} else if (node_id == 5){
  own_transmission_flow = 1;
  is_sender = 1;
  own_receiver = 1;
  schedule_length = 25;
  const scheduled_link_t add_link[] = {
    {  1,  1,  9,  0 },
    {  1,  1, 10,  0 },
    {  1,  1, 34,  0 },
    {  1,  1, 35,  0 },
  };
  hash_map_insert(&forward_to, 1, 1);
  const uint8_t cha_idx[] = {0, };
  const uint8_t cha_idx_to[] = {1, };
  add_links(add_link, 4, cha_idx, cha_idx_to, 1);
  destination.u8[NODE_ID_INDEX] = 1;
  tsch_queue_update_time_source(&destination);
  max_transmissions[0] = 2;
  #if !TSCH_TTL_BASED_RETRANSMISSIONS
    sending_slots[0] = 9;
    sending_slots[1] = 10;
    num_sending_slots = 2;
  #endif /* !TSCH_TTL_BASED_RETRANSMISSIONS */
} else if (node_id == 3){
  destination.u8[NODE_ID_INDEX] = 1;
  tsch_queue_update_time_source(&destination);
}
