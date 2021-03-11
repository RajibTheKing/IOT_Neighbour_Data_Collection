void generated_schedule(void){
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
receiver_of_flow[1] = 2;
sender_of_flow[2] = 9;
receiver_of_flow[2] = 8;
sender_of_flow[3] = 11;
receiver_of_flow[3] = 10;
#if TSCH_TTL_BASED_RETRANSMISSIONS
  first_tx_slot_in_flow[0][0] = 30;
  last_tx_slot_in_flow[0][0] = 39;
  first_tx_slot_in_flow[1][0] = 30;
  last_tx_slot_in_flow[1][0] = 33;
  first_tx_slot_in_flow[2][0] = 32;
  last_tx_slot_in_flow[2][0] = 39;
#endif /* TSCH_TTL_BASED_RETRANSMISSIONS */
if (node_id == 5){
  own_transmission_flow = 1;
  is_sender = 1;
  own_receiver = 2;
  schedule_length = 40;
  const scheduled_link_t add_link[] = {
    {  1,  1, 30,  0 },
    {  1,  1, 31,  0 },
    {  1,  1, 38,  1 },
  };
  hash_map_insert(&forward_to, 1, 11);
  const uint8_t cha_idx[] = {0, };
  const uint8_t cha_idx_to[] = {11, };
  add_links(add_link, 3, cha_idx, cha_idx_to, 1);
  max_transmissions[0] = 3;
  #if !TSCH_TTL_BASED_RETRANSMISSIONS
    sending_slots[0] = 30;
    sending_slots[1] = 31;
    sending_slots[2] = 38;
    num_sending_slots = 3;
  #endif /* !TSCH_TTL_BASED_RETRANSMISSIONS */
} else if (node_id == 11){
  own_transmission_flow = 3;
  is_sender = 1;
  own_receiver = 10;
  schedule_length = 40;
  const scheduled_link_t add_link[] = {
    {  1,  3, 31,  0 },
    {  3,  1, 32,  0 },
    {  3,  1, 33,  0 },
    {  3,  1, 34,  0 },
    {  3,  1, 35,  0 },
    {  3,  1, 36,  0 },
    {  1,  3, 38,  1 },
    {  1,  1, 39,  1 },
    {  1,  2, 30,  0 },
  };
  hash_map_insert(&forward_to, 1, 2);
  hash_map_insert(&forward_to, 3, 2);
  const uint8_t cha_idx[] = {0, 8, };
  const uint8_t cha_idx_to[] = {2, 5, };
  add_links(add_link, 9, cha_idx, cha_idx_to, 2);
  max_transmissions[0] = 3;
  max_transmissions[2] = 5;
  #if !TSCH_TTL_BASED_RETRANSMISSIONS
    sending_slots[0] = 32;
    sending_slots[1] = 33;
    sending_slots[2] = 34;
    sending_slots[3] = 35;
    sending_slots[4] = 36;
    num_sending_slots = 5;
  #endif /* !TSCH_TTL_BASED_RETRANSMISSIONS */
} else if (node_id == 9){
  own_transmission_flow = 2;
  is_sender = 1;
  own_receiver = 8;
  schedule_length = 40;
  const scheduled_link_t add_link[] = {
    {  2,  1, 30,  1 },
    {  2,  1, 31,  1 },
    {  2,  1, 32,  1 },
  };
  hash_map_insert(&forward_to, 2, 6);
  const uint8_t cha_idx[] = {0, };
  const uint8_t cha_idx_to[] = {6, };
  add_links(add_link, 3, cha_idx, cha_idx_to, 1);
  max_transmissions[1] = 3;
  #if !TSCH_TTL_BASED_RETRANSMISSIONS
    sending_slots[0] = 30;
    sending_slots[1] = 31;
    sending_slots[2] = 32;
    num_sending_slots = 3;
  #endif /* !TSCH_TTL_BASED_RETRANSMISSIONS */
} else if (node_id == 6){
  const scheduled_link_t add_link[] = {
    {  2,  3, 31,  1 },
    {  2,  3, 32,  1 },
    {  2,  1, 33,  1 },
    {  2,  2, 30,  1 },
  };
  hash_map_insert(&forward_to, 2, 8);
  const uint8_t cha_idx[] = {0, 3, };
  const uint8_t cha_idx_to[] = {8, 9, };
  add_links(add_link, 4, cha_idx, cha_idx_to, 2);
  max_transmissions[1] = 3;
} else if (node_id == 2){
  const scheduled_link_t add_link[] = {
    {  3,  3, 33,  0 },
    {  3,  3, 34,  0 },
    {  3,  3, 35,  0 },
    {  3,  3, 36,  0 },
    {  3,  1, 37,  0 },
    {  1,  2, 31,  0 },
    {  3,  2, 32,  0 },
    {  1,  2, 38,  1 },
    {  1,  2, 39,  1 },
  };
  hash_map_insert(&forward_to, 3, 1);
  const uint8_t cha_idx[] = {0, 5, };
  const uint8_t cha_idx_to[] = {1, 11, };
  add_links(add_link, 9, cha_idx, cha_idx_to, 2);
  max_transmissions[2] = 5;
} else if (node_id == 8){
  const scheduled_link_t add_link[] = {
    {  3,  2, 34,  0 },
    {  2,  2, 31,  1 },
    {  2,  2, 32,  1 },
    {  2,  2, 33,  1 },
    {  3,  3, 35,  0 },
    {  3,  3, 36,  0 },
    {  3,  3, 37,  0 },
    {  3,  3, 38,  0 },
    {  3,  1, 39,  0 },
  };
  hash_map_insert(&forward_to, 3, 10);
  const uint8_t cha_idx[] = {0, 1, 4, };
  const uint8_t cha_idx_to[] = {1, 6, 10, };
  add_links(add_link, 9, cha_idx, cha_idx_to, 3);
  max_transmissions[2] = 5;
} else if (node_id == 1){
  const scheduled_link_t add_link[] = {
    {  3,  2, 33,  0 },
    {  3,  3, 34,  0 },
    {  3,  3, 35,  0 },
    {  3,  3, 36,  0 },
    {  3,  3, 37,  0 },
    {  3,  1, 38,  0 },
  };
  hash_map_insert(&forward_to, 3, 8);
  const uint8_t cha_idx[] = {0, 1, };
  const uint8_t cha_idx_to[] = {2, 8, };
  add_links(add_link, 6, cha_idx, cha_idx_to, 2);
  max_transmissions[2] = 5;
} else if (node_id == 10){
  const scheduled_link_t add_link[] = {
    {  3,  2, 35,  0 },
    {  3,  2, 36,  0 },
    {  3,  2, 37,  0 },
    {  3,  2, 38,  0 },
    {  3,  2, 39,  0 },
  };
  const uint8_t cha_idx[] = {0, };
  const uint8_t cha_idx_to[] = {8, };
  add_links(add_link, 5, cha_idx, cha_idx_to, 1);
}
max_slot_frame_used = 2;
const uint8_t available_slot_matrix[] = {
     0,  0,  0,  0,  0, 
     1,  1,  0, 29,  0, 
     2,  2,  0, 28,  0, 
     3,  3,  0, 27,  0, 
     4,  4,  0, 26,  0, 
     5,  5,  0, 25,  0, 
     6,  6,  0, 24,  0, 
     7,  7,  0, 23,  0, 
     8,  8,  0, 22,  0, 
     9,  9,  0, 21,  0, 
    10, 10,  0, 20,  0, 
    11, 11,  0, 19,  0, 
};
copy_available_slot_matrix(available_slot_matrix);
}
