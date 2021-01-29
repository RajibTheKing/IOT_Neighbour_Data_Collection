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
  first_tx_slot_in_flow[0][0] = 2;
  last_tx_slot_in_flow[0][0] = 6;
  first_tx_slot_in_flow[0][1] = 27;
  last_tx_slot_in_flow[0][1] = 30;
  first_tx_slot_in_flow[1][0] = 0;
  last_tx_slot_in_flow[1][0] = 3;
#endif /* TSCH_TTL_BASED_RETRANSMISSIONS */
if (node_id == 4){
  own_transmission_flow = 2;
  is_sender = 1;
  own_receiver = 1;
  schedule_length = 50;
  const scheduled_link_t add_link[] = {
    {  2,  1,  0,  0 },
    {  2,  1,  1,  0 },
    {  2,  1,  2,  0 },
  };
  hash_map_insert(&forward_to, 2, 2);
  const uint8_t cha_idx[] = {0, };
  const uint8_t cha_idx_to[] = {2, };
  add_links(add_link, 3, cha_idx, cha_idx_to, 1);
  destination.u8[NODE_ID_INDEX] = 2;
  tsch_queue_update_time_source(&destination);
  max_transmissions[1] = 3;
  #if !TSCH_TTL_BASED_RETRANSMISSIONS
    sending_slots[0] = 0;
    sending_slots[1] = 1;
    sending_slots[2] = 2;
    num_sending_slots = 3;
  #endif /* !TSCH_TTL_BASED_RETRANSMISSIONS */
} else if (node_id == 2){
  const scheduled_link_t add_link[] = {
    {  2,  3,  1,  0 },
    {  2,  3,  2,  0 },
    {  2,  1,  3,  0 },
    {  2,  2,  0,  0 },
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
    {  2,  2,  1,  0 },
    {  2,  2,  2,  0 },
    {  2,  2,  3,  0 },
    {  1,  2,  4,  0 },
    {  1,  2,  5,  0 },
    {  1,  2,  6,  0 },
    {  1,  2, 28,  0 },
    {  1,  2, 29,  0 },
    {  1,  2, 30,  0 },
  };
  const uint8_t cha_idx[] = {0, 3, };
  const uint8_t cha_idx_to[] = {2, 3, };
  add_links(add_link, 9, cha_idx, cha_idx_to, 2);
} else if (node_id == 5){
  own_transmission_flow = 1;
  is_sender = 1;
  own_receiver = 1;
  schedule_length = 25;
  const scheduled_link_t add_link[] = {
    {  1,  1,  2,  1 },
    {  1,  1,  4,  0 },
    {  1,  1,  5,  0 },
    {  1,  1, 27,  0 },
    {  1,  1, 28,  0 },
    {  1,  1, 29,  0 },
  };
  hash_map_insert(&forward_to, 1, 3);
  const uint8_t cha_idx[] = {0, };
  const uint8_t cha_idx_to[] = {3, };
  add_links(add_link, 6, cha_idx, cha_idx_to, 1);
  destination.u8[NODE_ID_INDEX] = 3;
  tsch_queue_update_time_source(&destination);
  max_transmissions[0] = 3;
  #if !TSCH_TTL_BASED_RETRANSMISSIONS
    sending_slots[0] = 2;
    sending_slots[1] = 4;
    sending_slots[2] = 5;
    num_sending_slots = 3;
  #endif /* !TSCH_TTL_BASED_RETRANSMISSIONS */
} else if (node_id == 3){
  const scheduled_link_t add_link[] = {
    {  1,  3,  4,  0 },
    {  1,  3,  5,  0 },
    {  1,  1,  6,  0 },
    {  1,  3, 28,  0 },
    {  1,  3, 29,  0 },
    {  1,  1, 30,  0 },
    {  1,  2,  2,  1 },
    {  1,  2, 27,  0 },
  };
  hash_map_insert(&forward_to, 1, 1);
  const uint8_t cha_idx[] = {0, 6, };
  const uint8_t cha_idx_to[] = {1, 5, };
  add_links(add_link, 8, cha_idx, cha_idx_to, 2);
  destination.u8[NODE_ID_INDEX] = 1;
  tsch_queue_update_time_source(&destination);
  max_transmissions[0] = 3;
}
