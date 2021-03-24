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
sender_of_flow[1] = 5;
receiver_of_flow[1] = 2;
sender_of_flow[2] = 4;
receiver_of_flow[2] = 3;
#if TSCH_TTL_BASED_RETRANSMISSIONS
  first_tx_slot_in_flow[0][0] = 36;
  last_tx_slot_in_flow[0][0] = 39;
  first_tx_slot_in_flow[1][0] = 34;
  last_tx_slot_in_flow[1][0] = 35;
#endif /* TSCH_TTL_BASED_RETRANSMISSIONS */
if (node_id == 4){
  own_transmission_flow = 2;
  is_sender = 1;
  own_receiver = 3;
  schedule_length = 40;
  const scheduled_link_t add_link[] = {
    {  2,  1, 34,  0 },
    {  2,  1, 35,  0 },
  };
  hash_map_insert(&forward_to, 2, 3);
  const uint8_t cha_idx[] = {0, };
  const uint8_t cha_idx_to[] = {3, };
  add_links(add_link, 2, cha_idx, cha_idx_to, 1);
  max_transmissions[1] = 2;
  #if !TSCH_TTL_BASED_RETRANSMISSIONS
    sending_slots[0] = 34;
    sending_slots[1] = 35;
    num_sending_slots = 2;
  #endif /* !TSCH_TTL_BASED_RETRANSMISSIONS */
} else if (node_id == 3){
  const scheduled_link_t add_link[] = {
    {  1,  3, 37,  0 },
    {  1,  3, 38,  0 },
    {  1,  1, 39,  0 },
    {  2,  2, 34,  0 },
    {  2,  2, 35,  0 },
    {  1,  2, 36,  0 },
  };
  hash_map_insert(&forward_to, 1, 2);
  const uint8_t cha_idx[] = {0, 3, 5, };
  const uint8_t cha_idx_to[] = {2, 4, 5, };
  add_links(add_link, 6, cha_idx, cha_idx_to, 3);
  max_transmissions[0] = 3;
} else if (node_id == 5){
  own_transmission_flow = 1;
  is_sender = 1;
  own_receiver = 2;
  schedule_length = 40;
  const scheduled_link_t add_link[] = {
    {  1,  1, 36,  0 },
    {  1,  1, 37,  0 },
    {  1,  1, 38,  0 },
  };
  hash_map_insert(&forward_to, 1, 3);
  const uint8_t cha_idx[] = {0, };
  const uint8_t cha_idx_to[] = {3, };
  add_links(add_link, 3, cha_idx, cha_idx_to, 1);
  max_transmissions[0] = 3;
  #if !TSCH_TTL_BASED_RETRANSMISSIONS
    sending_slots[0] = 36;
    sending_slots[1] = 37;
    sending_slots[2] = 38;
    num_sending_slots = 3;
  #endif /* !TSCH_TTL_BASED_RETRANSMISSIONS */
} else if (node_id == 2){
  const scheduled_link_t add_link[] = {
    {  1,  2, 37,  0 },
    {  1,  2, 38,  0 },
    {  1,  2, 39,  0 },
  };
  const uint8_t cha_idx[] = {0, };
  const uint8_t cha_idx_to[] = {3, };
  add_links(add_link, 3, cha_idx, cha_idx_to, 1);
}
max_slot_frame_used = 2;
const uint8_t available_slot_matrix[] = {
     0,  0,  0,  0,  0, 
     1,  1,  0, 33,  0, 
     2,  2,  0, 32,  0, 
     3,  3,  0, 31,  0, 
     4,  4,  0, 30,  0, 
     5,  5,  0, 29,  0, 
};
copy_available_slot_matrix(available_slot_matrix);
}
