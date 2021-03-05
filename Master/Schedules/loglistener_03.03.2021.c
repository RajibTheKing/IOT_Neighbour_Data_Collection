schedule_length = 40;
sf[1] = tsch_schedule_get_slotframe_by_handle(1);
if (sf[1]){
  tsch_schedule_remove_slotframe(sf[1]);
}
sf[1] = tsch_schedule_add_slotframe(1, schedule_length);
hash_map_insert(&last_received_relayed_packet_of_flow, 1, 0);
sender_of_flow[1] = 3;
receiver_of_flow[1] = 6;
#if TSCH_TTL_BASED_RETRANSMISSIONS
  first_tx_slot_in_flow[0][0] = 36;
  last_tx_slot_in_flow[0][0] = 39;
#endif /* TSCH_TTL_BASED_RETRANSMISSIONS */
if (node_id == 3){
  own_transmission_flow = 1;
  is_sender = 1;
  own_receiver = 6;
  schedule_length = 40;
  const scheduled_link_t add_link[] = {
    {  1,  1, 36,  0 },
    {  1,  1, 37,  0 },
    {  1,  1, 38,  0 },
  };
  hash_map_insert(&forward_to, 1, 1);
  const uint8_t cha_idx[] = {0, };
  const uint8_t cha_idx_to[] = {1, };
  add_links(add_link, 3, cha_idx, cha_idx_to, 1);
  max_transmissions[0] = 3;
  #if !TSCH_TTL_BASED_RETRANSMISSIONS
    sending_slots[0] = 36;
    sending_slots[1] = 37;
    sending_slots[2] = 38;
    num_sending_slots = 3;
  #endif /* !TSCH_TTL_BASED_RETRANSMISSIONS */
} else if (node_id == 1){
  const scheduled_link_t add_link[] = {
    {  1,  2, 36,  0 },
    {  1,  3, 37,  0 },
    {  1,  3, 38,  0 },
    {  1,  1, 39,  0 },
  };
  hash_map_insert(&forward_to, 1, 6);
  const uint8_t cha_idx[] = {0, 1, };
  const uint8_t cha_idx_to[] = {3, 6, };
  add_links(add_link, 4, cha_idx, cha_idx_to, 2);
  max_transmissions[0] = 3;
} else if (node_id == 6){
  const scheduled_link_t add_link[] = {
    {  1,  2, 37,  0 },
    {  1,  2, 38,  0 },
    {  1,  2, 39,  0 },
  };
  const uint8_t cha_idx[] = {0, };
  const uint8_t cha_idx_to[] = {1, };
  add_links(add_link, 3, cha_idx, cha_idx_to, 1);
}
max_slot_frame_used = 1;
