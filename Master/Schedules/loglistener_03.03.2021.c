void generated_schedule(void){
schedule_length = 30;
sf[1] = tsch_schedule_get_slotframe_by_handle(1);
if (sf[1]){
  tsch_schedule_remove_slotframe(sf[1]);
}
sf[1] = tsch_schedule_add_slotframe(1, schedule_length);
hash_map_insert(&last_received_relayed_packet_of_flow, 1, 0);
sender_of_flow[1] = 4;
receiver_of_flow[1] = 9;
#if TSCH_TTL_BASED_RETRANSMISSIONS
  first_tx_slot_in_flow[0][0] = 21;
  last_tx_slot_in_flow[0][0] = 29;
#endif /* TSCH_TTL_BASED_RETRANSMISSIONS */
if (node_id == 4){
  own_transmission_flow = 1;
  is_sender = 1;
  own_receiver = 9;
  schedule_length = 30;
  const scheduled_link_t add_link[] = {
    {  1,  1, 21,  0 },
    {  1,  1, 22,  0 },
    {  1,  1, 23,  0 },
    {  1,  1, 24,  0 },
    {  1,  1, 25,  0 },
    {  1,  1, 26,  0 },
  };
  hash_map_insert(&forward_to, 1, 2);
  const uint8_t cha_idx[] = {0, };
  const uint8_t cha_idx_to[] = {2, };
  add_links(add_link, 6, cha_idx, cha_idx_to, 1);
  max_transmissions[0] = 6;
  #if !TSCH_TTL_BASED_RETRANSMISSIONS
    sending_slots[0] = 21;
    sending_slots[1] = 22;
    sending_slots[2] = 23;
    sending_slots[3] = 24;
    sending_slots[4] = 25;
    sending_slots[5] = 26;
    num_sending_slots = 6;
  #endif /* !TSCH_TTL_BASED_RETRANSMISSIONS */
} else if (node_id == 2){
  const scheduled_link_t add_link[] = {
    {  1,  3, 22,  0 },
    {  1,  3, 23,  0 },
    {  1,  3, 24,  0 },
    {  1,  3, 25,  0 },
    {  1,  3, 26,  0 },
    {  1,  1, 27,  0 },
    {  1,  2, 21,  0 },
  };
  hash_map_insert(&forward_to, 1, 1);
  const uint8_t cha_idx[] = {0, 6, };
  const uint8_t cha_idx_to[] = {1, 4, };
  add_links(add_link, 7, cha_idx, cha_idx_to, 2);
  max_transmissions[0] = 6;
} else if (node_id == 1){
  const scheduled_link_t add_link[] = {
    {  1,  2, 22,  0 },
    {  1,  3, 23,  0 },
    {  1,  3, 24,  0 },
    {  1,  3, 25,  0 },
    {  1,  3, 26,  0 },
    {  1,  3, 27,  0 },
    {  1,  1, 28,  0 },
  };
  hash_map_insert(&forward_to, 1, 7);
  const uint8_t cha_idx[] = {0, 1, };
  const uint8_t cha_idx_to[] = {2, 7, };
  add_links(add_link, 7, cha_idx, cha_idx_to, 2);
  max_transmissions[0] = 6;
} else if (node_id == 7){
  const scheduled_link_t add_link[] = {
    {  1,  2, 23,  0 },
    {  1,  3, 24,  0 },
    {  1,  3, 25,  0 },
    {  1,  3, 26,  0 },
    {  1,  3, 27,  0 },
    {  1,  3, 28,  0 },
    {  1,  1, 29,  0 },
  };
  hash_map_insert(&forward_to, 1, 9);
  const uint8_t cha_idx[] = {0, 1, };
  const uint8_t cha_idx_to[] = {1, 9, };
  add_links(add_link, 7, cha_idx, cha_idx_to, 2);
  max_transmissions[0] = 6;
} else if (node_id == 9){
  const scheduled_link_t add_link[] = {
    {  1,  2, 24,  0 },
    {  1,  2, 25,  0 },
    {  1,  2, 26,  0 },
    {  1,  2, 27,  0 },
    {  1,  2, 28,  0 },
    {  1,  2, 29,  0 },
  };
  const uint8_t cha_idx[] = {0, };
  const uint8_t cha_idx_to[] = {7, };
  add_links(add_link, 6, cha_idx, cha_idx_to, 1);
}
max_slot_frame_used = 1;
const uint8_t available_slot_matrix[] = {
     0,  0,  0,  0,  0, 
     1,  1,  0, 20,  0, 
     2,  2,  0, 19,  0, 
     3,  3,  0, 18,  0, 
     4,  4,  0, 17,  0, 
     5,  5,  0, 16,  0, 
     6,  6,  0, 15,  0, 
     7,  7,  0, 14,  0, 
     8,  8,  0, 13,  0, 
     9,  9,  0, 12,  0, 
    10, 10,  0, 11,  0, 
};
copy_available_slot_matrix(available_slot_matrix);
}
