schedule_length = 22;
sf[1] = tsch_schedule_get_slotframe_by_handle(1);
if (sf[1]){
  tsch_schedule_remove_slotframe(sf[1]);
}
sf[1] = tsch_schedule_add_slotframe(1, schedule_length);
hash_map_insert(&last_received_relayed_packet_of_flow, 1, 0);
sender_of_flow[1] = 2;
receiver_of_flow[1] = 1;
#if TSCH_TTL_BASED_RETRANSMISSIONS
  first_tx_slot_in_flow[0][0] = 0;
  last_tx_slot_in_flow[0][0] = 1;
#endif /* TSCH_TTL_BASED_RETRANSMISSIONS */
if (node_id == 2){
  own_transmission_flow = 1;
  is_sender = 1;
  own_receiver = 1;
  schedule_length = 22;
  const scheduled_link_t add_link[] = {
    {  1,  1,  0,  0 },
    {  1,  1,  1,  0 },
  };
  hash_map_insert(&forward_to, 1, 1);
  const uint8_t cha_idx[] = {0, };
  const uint8_t cha_idx_to[] = {1, };
  add_links(add_link, 2, cha_idx, cha_idx_to, 1);
  destination.u8[NODE_ID_INDEX] = 1;
  tsch_queue_update_time_source(&destination);
  max_transmissions[0] = 2;
  #if !TSCH_TTL_BASED_RETRANSMISSIONS
    sending_slots[0] = 0;
    sending_slots[1] = 1;
    num_sending_slots = 2;
  #endif /* !TSCH_TTL_BASED_RETRANSMISSIONS */
} else if (node_id == 1){
  const scheduled_link_t add_link[] = {
    {  1,  2,  0,  0 },
    {  1,  2,  1,  0 },
  };
  const uint8_t cha_idx[] = {0, };
  const uint8_t cha_idx_to[] = {2, };
  add_links(add_link, 2, cha_idx, cha_idx_to, 1);
}
