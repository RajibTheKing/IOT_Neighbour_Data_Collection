schedule_length = 40;
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
sf[3] = tsch_schedule_get_slotframe_by_handle(3);
if (sf[3]){
  tsch_schedule_remove_slotframe(sf[3]);
}
sf[3] = tsch_schedule_add_slotframe(3, schedule_length);
hash_map_insert(&last_received_relayed_packet_of_flow, 1, 0);
hash_map_insert(&last_received_relayed_packet_of_flow, 2, 0);
hash_map_insert(&last_received_relayed_packet_of_flow, 3, 0);
sender_of_flow[1] = 5;
receiver_of_flow[1] = 8;
sender_of_flow[2] = 3;
receiver_of_flow[2] = 6;
sender_of_flow[3] = 7;
receiver_of_flow[3] = 2;
#if TSCH_TTL_BASED_RETRANSMISSIONS
  first_tx_slot_in_flow[0][0] = 32;
  last_tx_slot_in_flow[0][0] = 39;
  first_tx_slot_in_flow[1][0] = 27;
  last_tx_slot_in_flow[1][0] = 39;
  first_tx_slot_in_flow[2][0] = 24;
  last_tx_slot_in_flow[2][0] = 39;
#endif /* TSCH_TTL_BASED_RETRANSMISSIONS */
if (node_id == 7){
  own_transmission_flow = 3;
  is_sender = 1;
  own_receiver = 2;
  schedule_length = 40;
  const scheduled_link_t add_link[] = {
    {  3,  1, 24,  0 },
    {  3,  1, 25,  0 },
    {  3,  1, 26,  0 },
    {  2,  2, 28,  0 },
    {  3,  1, 32,  1 },
    {  2,  3, 29,  0 },
    {  2,  3, 30,  0 },
    {  2,  3, 31,  0 },
    {  1,  2, 33,  0 },
    {  2,  1, 39,  1 },
    {  1,  3, 34,  0 },
    {  1,  3, 35,  0 },
    {  1,  3, 36,  0 },
    {  1,  3, 37,  0 },
    {  1,  1, 38,  0 },
  };
  hash_map_insert(&forward_to, 3, 1);
  hash_map_insert(&forward_to, 2, 6);
  hash_map_insert(&forward_to, 1, 9);
  const uint8_t cha_idx[] = {0, 5, 10, };
  const uint8_t cha_idx_to[] = {1, 6, 9, };
  add_links(add_link, 15, cha_idx, cha_idx_to, 3);
  max_transmissions[2] = 4;
  max_transmissions[1] = 4;
  max_transmissions[0] = 5;
  #if !TSCH_TTL_BASED_RETRANSMISSIONS
    sending_slots[0] = 24;
    sending_slots[1] = 25;
    sending_slots[2] = 26;
    sending_slots[3] = 32;
    num_sending_slots = 4;
  #endif /* !TSCH_TTL_BASED_RETRANSMISSIONS */
} else if (node_id == 1){
  const scheduled_link_t add_link[] = {
    {  3,  3, 25,  0 },
    {  3,  3, 26,  0 },
    {  2,  2, 27,  0 },
    {  3,  3, 32,  1 },
    {  3,  1, 38,  1 },
    {  3,  2, 24,  0 },
    {  2,  3, 28,  0 },
    {  2,  3, 29,  0 },
    {  2,  3, 30,  0 },
    {  2,  1, 31,  0 },
  };
  hash_map_insert(&forward_to, 3, 3);
  hash_map_insert(&forward_to, 2, 7);
  const uint8_t cha_idx[] = {0, 5, };
  const uint8_t cha_idx_to[] = {3, 7, };
  add_links(add_link, 10, cha_idx, cha_idx_to, 2);
  max_transmissions[2] = 4;
  max_transmissions[1] = 4;
} else if (node_id == 3){
  own_transmission_flow = 2;
  is_sender = 1;
  own_receiver = 6;
  schedule_length = 40;
  const scheduled_link_t add_link[] = {
    {  3,  2, 25,  0 },
    {  2,  1, 27,  0 },
    {  2,  1, 28,  0 },
    {  2,  1, 29,  0 },
    {  2,  1, 30,  0 },
    {  3,  3, 26,  0 },
    {  3,  3, 32,  1 },
    {  3,  3, 38,  1 },
    {  3,  1, 39,  2 },
  };
  hash_map_insert(&forward_to, 2, 1);
  hash_map_insert(&forward_to, 3, 2);
  const uint8_t cha_idx[] = {0, 5, };
  const uint8_t cha_idx_to[] = {1, 2, };
  add_links(add_link, 9, cha_idx, cha_idx_to, 2);
  max_transmissions[1] = 4;
  max_transmissions[2] = 4;
  #if !TSCH_TTL_BASED_RETRANSMISSIONS
    sending_slots[0] = 27;
    sending_slots[1] = 28;
    sending_slots[2] = 29;
    sending_slots[3] = 30;
    num_sending_slots = 4;
  #endif /* !TSCH_TTL_BASED_RETRANSMISSIONS */
} else if (node_id == 2){
  const scheduled_link_t add_link[] = {
    {  3,  2, 26,  0 },
    {  3,  2, 32,  1 },
    {  3,  2, 38,  1 },
    {  3,  2, 39,  2 },
  };
  const uint8_t cha_idx[] = {0, };
  const uint8_t cha_idx_to[] = {3, };
  add_links(add_link, 4, cha_idx, cha_idx_to, 1);
} else if (node_id == 6){
  const scheduled_link_t add_link[] = {
    {  1,  2, 32,  0 },
    {  2,  2, 29,  0 },
    {  2,  2, 30,  0 },
    {  2,  2, 31,  0 },
    {  1,  3, 33,  0 },
    {  1,  3, 34,  0 },
    {  1,  3, 35,  0 },
    {  1,  3, 36,  0 },
    {  1,  1, 37,  0 },
    {  2,  2, 39,  1 },
  };
  hash_map_insert(&forward_to, 1, 7);
  const uint8_t cha_idx[] = {0, 1, };
  const uint8_t cha_idx_to[] = {5, 7, };
  add_links(add_link, 10, cha_idx, cha_idx_to, 2);
  max_transmissions[0] = 5;
} else if (node_id == 5){
  own_transmission_flow = 1;
  is_sender = 1;
  own_receiver = 8;
  schedule_length = 40;
  const scheduled_link_t add_link[] = {
    {  1,  1, 32,  0 },
    {  1,  1, 33,  0 },
    {  1,  1, 34,  0 },
    {  1,  1, 35,  0 },
    {  1,  1, 36,  0 },
  };
  hash_map_insert(&forward_to, 1, 6);
  const uint8_t cha_idx[] = {0, };
  const uint8_t cha_idx_to[] = {6, };
  add_links(add_link, 5, cha_idx, cha_idx_to, 1);
  max_transmissions[0] = 5;
  #if !TSCH_TTL_BASED_RETRANSMISSIONS
    sending_slots[0] = 32;
    sending_slots[1] = 33;
    sending_slots[2] = 34;
    sending_slots[3] = 35;
    sending_slots[4] = 36;
    num_sending_slots = 5;
  #endif /* !TSCH_TTL_BASED_RETRANSMISSIONS */
} else if (node_id == 9){
  const scheduled_link_t add_link[] = {
    {  1,  2, 34,  0 },
    {  1,  3, 35,  0 },
    {  1,  3, 36,  0 },
    {  1,  3, 37,  0 },
    {  1,  3, 38,  0 },
    {  1,  1, 39,  0 },
  };
  hash_map_insert(&forward_to, 1, 8);
  const uint8_t cha_idx[] = {0, 1, };
  const uint8_t cha_idx_to[] = {7, 8, };
  add_links(add_link, 6, cha_idx, cha_idx_to, 2);
  max_transmissions[0] = 5;
} else if (node_id == 8){
  const scheduled_link_t add_link[] = {
    {  1,  2, 35,  0 },
    {  1,  2, 36,  0 },
    {  1,  2, 37,  0 },
    {  1,  2, 38,  0 },
    {  1,  2, 39,  0 },
  };
  const uint8_t cha_idx[] = {0, };
  const uint8_t cha_idx_to[] = {9, };
  add_links(add_link, 5, cha_idx, cha_idx_to, 1);
}

max_slot_frame_used = 3;
