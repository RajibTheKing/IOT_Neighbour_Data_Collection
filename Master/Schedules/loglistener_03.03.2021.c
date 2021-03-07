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
receiver_of_flow[1] = 10;
sender_of_flow[2] = 4;
receiver_of_flow[2] = 2;
sender_of_flow[3] = 7;
receiver_of_flow[3] = 9;
#if TSCH_TTL_BASED_RETRANSMISSIONS
  first_tx_slot_in_flow[0][0] = 38;
  last_tx_slot_in_flow[0][0] = 39;
  first_tx_slot_in_flow[1][0] = 37;
  last_tx_slot_in_flow[1][0] = 39;
  first_tx_slot_in_flow[2][0] = 38;
  last_tx_slot_in_flow[2][0] = 39;
#endif /* TSCH_TTL_BASED_RETRANSMISSIONS */
if (node_id == 4){
  own_transmission_flow = 2;
  is_sender = 1;
  own_receiver = 2;
  schedule_length = 40;
  const scheduled_link_t add_link[] = {
    {  2,  1, 37,  0 },
    {  2,  1, 38,  0 },
    {  2,  1, 39,  0 },
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
    {  2,  2, 37,  0 },
    {  2,  2, 38,  0 },
    {  2,  2, 39,  0 },
  };
  const uint8_t cha_idx[] = {0, };
  const uint8_t cha_idx_to[] = {4, };
  add_links(add_link, 3, cha_idx, cha_idx_to, 1);
} else if (node_id == 5){
  own_transmission_flow = 1;
  is_sender = 1;
  own_receiver = 10;
  schedule_length = 40;
  const scheduled_link_t add_link[] = {
    {  1,  1, 38,  1 },
    {  1,  1, 39,  1 },
  };
  hash_map_insert(&forward_to, 1, 10);
  const uint8_t cha_idx[] = {0, };
  const uint8_t cha_idx_to[] = {10, };
  add_links(add_link, 2, cha_idx, cha_idx_to, 1);
  max_transmissions[0] = 2;
  #if !TSCH_TTL_BASED_RETRANSMISSIONS
    sending_slots[0] = 38;
    sending_slots[1] = 39;
    num_sending_slots = 2;
  #endif /* !TSCH_TTL_BASED_RETRANSMISSIONS */
} else if (node_id == 10){
  const scheduled_link_t add_link[] = {
    {  1,  2, 38,  1 },
    {  1,  2, 39,  1 },
  };
  const uint8_t cha_idx[] = {0, };
  const uint8_t cha_idx_to[] = {5, };
  add_links(add_link, 2, cha_idx, cha_idx_to, 1);
} else if (node_id == 7){
  own_transmission_flow = 3;
  is_sender = 1;
  own_receiver = 9;
  schedule_length = 40;
  const scheduled_link_t add_link[] = {
    {  3,  1, 38,  2 },
    {  3,  1, 39,  2 },
  };
  hash_map_insert(&forward_to, 3, 9);
  const uint8_t cha_idx[] = {0, };
  const uint8_t cha_idx_to[] = {9, };
  add_links(add_link, 2, cha_idx, cha_idx_to, 1);
  max_transmissions[2] = 2;
  #if !TSCH_TTL_BASED_RETRANSMISSIONS
    sending_slots[0] = 38;
    sending_slots[1] = 39;
    num_sending_slots = 2;
  #endif /* !TSCH_TTL_BASED_RETRANSMISSIONS */
} else if (node_id == 9){
  const scheduled_link_t add_link[] = {
    {  3,  2, 38,  2 },
    {  3,  2, 39,  2 },
  };
  const uint8_t cha_idx[] = {0, };
  const uint8_t cha_idx_to[] = {7, };
  add_links(add_link, 2, cha_idx, cha_idx_to, 1);
}
max_slot_frame_used = 3;
max_channel_used = 4;
const uint8_t used_slot_matrix[] = {
     0,  0,  0,  0,
     0,  0,  0,  0,
     0,  0,  0,  0,
     0,  0,  0,  0,
     0,  0,  0,  0,
     0,  0,  0,  0,
     0,  0,  0,  0,
     0,  0,  0,  0,
     0,  0,  0,  0,
     0,  0,  0,  0,
     0,  0,  0,  0,
     0,  0,  0,  0,
     0,  0,  0,  0,
     0,  0,  0,  0,
     0,  0,  0,  0,
     0,  0,  0,  0,
     0,  0,  0,  0,
     0,  0,  0,  0,
     0,  0,  0,  0,
     0,  0,  0,  0,
     0,  0,  0,  0,
     0,  0,  0,  0,
     0,  0,  0,  0,
     0,  0,  0,  0,
     0,  0,  0,  0,
     0,  0,  0,  0,
     0,  0,  0,  0,
     0,  0,  0,  0,
     0,  0,  0,  0,
     0,  0,  0,  0,
     0,  0,  0,  0,
     0,  0,  0,  0,
     0,  0,  0,  0,
     0,  0,  0,  0,
     0,  0,  0,  0,
     0,  0,  0,  0,
     0,  0,  0,  0,
     1,  0,  0,  0,
     1,  1,  1,  0,
     1,  1,  1,  0,
};
copy_used_slot_matrix(used_slot_matrix);
}
