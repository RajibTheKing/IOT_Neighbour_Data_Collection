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
hash_map_insert(&last_received_relayed_packet_of_flow, 1, 0);
hash_map_insert(&last_received_relayed_packet_of_flow, 2, 0);
sender_of_flow[1] = 9;
receiver_of_flow[1] = 8;
sender_of_flow[2] = 4;
receiver_of_flow[2] = 2;
#if TSCH_TTL_BASED_RETRANSMISSIONS
  first_tx_slot_in_flow[0][0] = 33;
  last_tx_slot_in_flow[0][0] = 39;
  first_tx_slot_in_flow[1][0] = 37;
  last_tx_slot_in_flow[1][0] = 39;
#endif /* TSCH_TTL_BASED_RETRANSMISSIONS */
if (node_id == 9){
  own_transmission_flow = 1;
  is_sender = 1;
  own_receiver = 8;
  schedule_length = 40;
  const scheduled_link_t add_link[] = {
    {  1,  1, 33,  0 },
    {  1,  1, 34,  0 },
    {  1,  1, 35,  0 },
    {  1,  1, 36,  0 },
    {  1,  1, 37,  0 },
  };
  hash_map_insert(&forward_to, 1, 7);
  const uint8_t cha_idx[] = {0, };
  const uint8_t cha_idx_to[] = {7, };
  add_links(add_link, 5, cha_idx, cha_idx_to, 1);
  max_transmissions[0] = 5;
  #if !TSCH_TTL_BASED_RETRANSMISSIONS
    sending_slots[0] = 33;
    sending_slots[1] = 34;
    sending_slots[2] = 35;
    sending_slots[3] = 36;
    sending_slots[4] = 37;
    num_sending_slots = 5;
  #endif /* !TSCH_TTL_BASED_RETRANSMISSIONS */
} else if (node_id == 7){
  const scheduled_link_t add_link[] = {
    {  1,  3, 34,  0 },
    {  1,  3, 35,  0 },
    {  1,  3, 36,  0 },
    {  1,  3, 37,  0 },
    {  1,  1, 38,  0 },
    {  1,  2, 33,  0 },
  };
  hash_map_insert(&forward_to, 1, 1);
  const uint8_t cha_idx[] = {0, 5, };
  const uint8_t cha_idx_to[] = {1, 9, };
  add_links(add_link, 6, cha_idx, cha_idx_to, 2);
  max_transmissions[0] = 5;
} else if (node_id == 1){
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
  const uint8_t cha_idx_to[] = {1, };
  add_links(add_link, 5, cha_idx, cha_idx_to, 1);
} else if (node_id == 4){
  own_transmission_flow = 2;
  is_sender = 1;
  own_receiver = 2;
  schedule_length = 40;
  const scheduled_link_t add_link[] = {
    {  2,  1, 37,  1 },
    {  2,  1, 38,  1 },
    {  2,  1, 39,  1 },
  };
  hash_map_insert(&forward_to, 2, 2);
  const uint8_t cha_idx[] = {0, };
  const uint8_t cha_idx_to[] = {2, };
  add_links(add_link, 3, cha_idx, cha_idx_to, 1);
  max_transmissions[1] = 3;
  #if !TSCH_TTL_BASED_RETRANSMISSIONS
    sending_slots[0] = 37;
    sending_slots[1] = 38;
    sending_slots[2] = 39;
    num_sending_slots = 3;
  #endif /* !TSCH_TTL_BASED_RETRANSMISSIONS */
} else if (node_id == 2){
  const scheduled_link_t add_link[] = {
    {  2,  2, 37,  1 },
    {  2,  2, 38,  1 },
    {  2,  2, 39,  1 },
  };
  const uint8_t cha_idx[] = {0, };
  const uint8_t cha_idx_to[] = {4, };
  add_links(add_link, 3, cha_idx, cha_idx_to, 1);
}
max_slot_frame_used = 2;
const uint8_t available_slot_matrix[] = {
     0,  0,  0,  0,  0, 
     1,  1,  0, 32,  0, 
     2,  2,  0, 31,  0, 
     3,  3,  0, 30,  0, 
     4,  4,  0, 29,  0, 
     5,  5,  0, 28,  0, 
     6,  6,  0, 27,  0, 
     7,  7,  0, 26,  0, 
     8,  8,  0, 25,  0, 
     9,  9,  0, 24,  0, 
    10, 10,  0, 23,  0, 
};
copy_available_slot_matrix(available_slot_matrix);
}
