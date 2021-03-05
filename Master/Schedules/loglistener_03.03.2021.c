schedule_length = 40;
sf[1] = tsch_schedule_get_slotframe_by_handle(1);
if (sf[1]){
  tsch_schedule_remove_slotframe(sf[1]);
}
sf[1] = tsch_schedule_add_slotframe(1, schedule_length);
hash_map_insert(&last_received_relayed_packet_of_flow, 1, 0);
sender_of_flow[1] = 7;
receiver_of_flow[1] = 9;
#if TSCH_TTL_BASED_RETRANSMISSIONS
  first_tx_slot_in_flow[0][0] = 38;
  last_tx_slot_in_flow[0][0] = 39;
#endif /* TSCH_TTL_BASED_RETRANSMISSIONS */
if (node_id == 7){
  own_transmission_flow = 1;
  is_sender = 1;
  own_receiver = 9;
  schedule_length = 40;
  const scheduled_link_t add_link[] = {
    {  1,  1, 38,  0 },
    {  1,  1, 39,  0 },
  };
  hash_map_insert(&forward_to, 1, 9);
  const uint8_t cha_idx[] = {0, };
  const uint8_t cha_idx_to[] = {9, };
  add_links(add_link, 2, cha_idx, cha_idx_to, 1);
  max_transmissions[0] = 2;
  #if !TSCH_TTL_BASED_RETRANSMISSIONS
    sending_slots[0] = 38;
    sending_slots[1] = 39;
    num_sending_slots = 2;
  #endif /* !TSCH_TTL_BASED_RETRANSMISSIONS */
} else if (node_id == 9){
  const scheduled_link_t add_link[] = {
    {  1,  2, 38,  0 },
    {  1,  2, 39,  0 },
  };
  const uint8_t cha_idx[] = {0, };
  const uint8_t cha_idx_to[] = {7, };
  add_links(add_link, 2, cha_idx, cha_idx_to, 1);
}
max_slot_frame_used = 1;
