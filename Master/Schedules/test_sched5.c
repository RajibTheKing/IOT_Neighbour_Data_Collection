schedule_length = 450;
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
sf[4] = tsch_schedule_get_slotframe_by_handle(4);
if (sf[4]){
  tsch_schedule_remove_slotframe(sf[4]);
}
sf[4] = tsch_schedule_add_slotframe(4, schedule_length);
hash_map_insert(&last_received_relayed_packet_of_flow, 1, 0);
hash_map_insert(&last_received_relayed_packet_of_flow, 2, 0);
hash_map_insert(&last_received_relayed_packet_of_flow, 3, 0);
hash_map_insert(&last_received_relayed_packet_of_flow, 4, 0);
sender_of_flow[1] = 5;
receiver_of_flow[1] = 1;
sender_of_flow[2] = 4;
receiver_of_flow[2] = 1;
sender_of_flow[3] = 3;
receiver_of_flow[3] = 1;
sender_of_flow[4] = 2;
receiver_of_flow[4] = 1;
#if TSCH_TTL_BASED_RETRANSMISSIONS
  first_tx_slot_in_flow[0][0] = 2;
  last_tx_slot_in_flow[0][0] = 6;
  first_tx_slot_in_flow[0][1] = 27;
  last_tx_slot_in_flow[0][1] = 30;
  first_tx_slot_in_flow[0][2] = 52;
  last_tx_slot_in_flow[0][2] = 56;
  first_tx_slot_in_flow[0][3] = 77;
  last_tx_slot_in_flow[0][3] = 80;
  first_tx_slot_in_flow[0][4] = 102;
  last_tx_slot_in_flow[0][4] = 106;
  first_tx_slot_in_flow[0][5] = 127;
  last_tx_slot_in_flow[0][5] = 130;
  first_tx_slot_in_flow[0][6] = 152;
  last_tx_slot_in_flow[0][6] = 156;
  first_tx_slot_in_flow[0][7] = 177;
  last_tx_slot_in_flow[0][7] = 180;
  first_tx_slot_in_flow[0][8] = 202;
  last_tx_slot_in_flow[0][8] = 207;
  first_tx_slot_in_flow[0][9] = 227;
  last_tx_slot_in_flow[0][9] = 230;
  first_tx_slot_in_flow[0][10] = 252;
  last_tx_slot_in_flow[0][10] = 256;
  first_tx_slot_in_flow[0][11] = 277;
  last_tx_slot_in_flow[0][11] = 280;
  first_tx_slot_in_flow[0][12] = 302;
  last_tx_slot_in_flow[0][12] = 306;
  first_tx_slot_in_flow[0][13] = 327;
  last_tx_slot_in_flow[0][13] = 330;
  first_tx_slot_in_flow[0][14] = 352;
  last_tx_slot_in_flow[0][14] = 356;
  first_tx_slot_in_flow[0][15] = 377;
  last_tx_slot_in_flow[0][15] = 380;
  first_tx_slot_in_flow[0][16] = 402;
  last_tx_slot_in_flow[0][16] = 406;
  first_tx_slot_in_flow[0][17] = 427;
  last_tx_slot_in_flow[0][17] = 430;
  first_tx_slot_in_flow[1][0] = 0;
  last_tx_slot_in_flow[1][0] = 3;
  first_tx_slot_in_flow[1][1] = 50;
  last_tx_slot_in_flow[1][1] = 53;
  first_tx_slot_in_flow[1][2] = 100;
  last_tx_slot_in_flow[1][2] = 103;
  first_tx_slot_in_flow[1][3] = 150;
  last_tx_slot_in_flow[1][3] = 153;
  first_tx_slot_in_flow[1][4] = 201;
  last_tx_slot_in_flow[1][4] = 204;
  first_tx_slot_in_flow[1][5] = 250;
  last_tx_slot_in_flow[1][5] = 253;
  first_tx_slot_in_flow[1][6] = 300;
  last_tx_slot_in_flow[1][6] = 303;
  first_tx_slot_in_flow[1][7] = 350;
  last_tx_slot_in_flow[1][7] = 353;
  first_tx_slot_in_flow[1][8] = 400;
  last_tx_slot_in_flow[1][8] = 403;
  first_tx_slot_in_flow[2][0] = 9;
  last_tx_slot_in_flow[2][0] = 10;
  first_tx_slot_in_flow[2][1] = 39;
  last_tx_slot_in_flow[2][1] = 40;
  first_tx_slot_in_flow[2][2] = 69;
  last_tx_slot_in_flow[2][2] = 70;
  first_tx_slot_in_flow[2][3] = 99;
  last_tx_slot_in_flow[2][3] = 100;
  first_tx_slot_in_flow[2][4] = 131;
  last_tx_slot_in_flow[2][4] = 132;
  first_tx_slot_in_flow[2][5] = 159;
  last_tx_slot_in_flow[2][5] = 160;
  first_tx_slot_in_flow[2][6] = 189;
  last_tx_slot_in_flow[2][6] = 190;
  first_tx_slot_in_flow[2][7] = 219;
  last_tx_slot_in_flow[2][7] = 220;
  first_tx_slot_in_flow[2][8] = 249;
  last_tx_slot_in_flow[2][8] = 250;
  first_tx_slot_in_flow[2][9] = 281;
  last_tx_slot_in_flow[2][9] = 282;
  first_tx_slot_in_flow[2][10] = 309;
  last_tx_slot_in_flow[2][10] = 310;
  first_tx_slot_in_flow[2][11] = 339;
  last_tx_slot_in_flow[2][11] = 340;
  first_tx_slot_in_flow[2][12] = 369;
  last_tx_slot_in_flow[2][12] = 370;
  first_tx_slot_in_flow[2][13] = 399;
  last_tx_slot_in_flow[2][13] = 400;
  first_tx_slot_in_flow[2][14] = 431;
  last_tx_slot_in_flow[2][14] = 432;
  first_tx_slot_in_flow[3][0] = 19;
  last_tx_slot_in_flow[3][0] = 20;
  first_tx_slot_in_flow[3][1] = 64;
  last_tx_slot_in_flow[3][1] = 65;
  first_tx_slot_in_flow[3][2] = 109;
  last_tx_slot_in_flow[3][2] = 110;
  first_tx_slot_in_flow[3][3] = 157;
  last_tx_slot_in_flow[3][3] = 158;
  first_tx_slot_in_flow[3][4] = 199;
  last_tx_slot_in_flow[3][4] = 200;
  first_tx_slot_in_flow[3][5] = 244;
  last_tx_slot_in_flow[3][5] = 245;
  first_tx_slot_in_flow[3][6] = 289;
  last_tx_slot_in_flow[3][6] = 290;
  first_tx_slot_in_flow[3][7] = 334;
  last_tx_slot_in_flow[3][7] = 335;
  first_tx_slot_in_flow[3][8] = 381;
  last_tx_slot_in_flow[3][8] = 382;
  first_tx_slot_in_flow[3][9] = 424;
  last_tx_slot_in_flow[3][9] = 425;
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
    {  2,  1, 50,  0 },
    {  2,  1, 51,  0 },
    {  2,  1, 52,  0 },
    {  2,  1, 100,  1 },
    {  2,  1, 101,  0 },
    {  2,  1, 102,  0 },
    {  2,  1, 150,  0 },
    {  2,  1, 151,  0 },
    {  2,  1, 152,  0 },
    {  2,  1, 201,  0 },
    {  2,  1, 202,  0 },
    {  2,  1, 203,  0 },
    {  2,  1, 250,  1 },
    {  2,  1, 251,  0 },
    {  2,  1, 252,  0 },
    {  2,  1, 300,  0 },
    {  2,  1, 301,  0 },
    {  2,  1, 302,  0 },
    {  2,  1, 350,  0 },
    {  2,  1, 351,  0 },
    {  2,  1, 352,  0 },
    {  2,  1, 400,  1 },
    {  2,  1, 401,  0 },
    {  2,  1, 402,  0 },
  };
  hash_map_insert(&forward_to, 2, 2);
  const uint8_t cha_idx[] = {0, };
  const uint8_t cha_idx_to[] = {2, };
  add_links(add_link, 27, cha_idx, cha_idx_to, 1);
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
  own_transmission_flow = 4;
  is_sender = 1;
  own_receiver = 1;
  schedule_length = 45;
  const scheduled_link_t add_link[] = {
    {  2,  3,  1,  0 },
    {  2,  3,  2,  0 },
    {  2,  1,  3,  0 },
    {  4,  1, 19,  0 },
    {  4,  1, 20,  0 },
    {  2,  3, 51,  0 },
    {  2,  3, 52,  0 },
    {  2,  1, 53,  0 },
    {  4,  1, 64,  0 },
    {  4,  1, 65,  0 },
    {  2,  3, 101,  0 },
    {  2,  3, 102,  0 },
    {  2,  1, 103,  0 },
    {  4,  1, 109,  0 },
    {  4,  1, 110,  0 },
    {  2,  3, 151,  0 },
    {  2,  3, 152,  0 },
    {  2,  1, 153,  0 },
    {  4,  1, 157,  0 },
    {  4,  1, 158,  0 },
    {  4,  1, 199,  0 },
    {  4,  1, 200,  0 },
    {  2,  3, 202,  0 },
    {  2,  3, 203,  0 },
    {  2,  1, 204,  0 },
    {  4,  1, 244,  0 },
    {  4,  1, 245,  0 },
    {  2,  3, 251,  0 },
    {  2,  3, 252,  0 },
    {  2,  1, 253,  0 },
    {  4,  1, 289,  0 },
    {  4,  1, 290,  0 },
    {  2,  3, 301,  0 },
    {  2,  3, 302,  0 },
    {  2,  1, 303,  0 },
    {  4,  1, 334,  0 },
    {  4,  1, 335,  0 },
    {  2,  3, 351,  0 },
    {  2,  3, 352,  0 },
    {  2,  1, 353,  0 },
    {  4,  1, 381,  0 },
    {  4,  1, 382,  0 },
    {  2,  3, 401,  0 },
    {  2,  3, 402,  0 },
    {  2,  1, 403,  0 },
    {  4,  1, 424,  0 },
    {  4,  1, 425,  0 },
    {  2,  2,  0,  0 },
    {  2,  2, 50,  0 },
    {  2,  2, 100,  1 },
    {  2,  2, 150,  0 },
    {  2,  2, 201,  0 },
    {  2,  2, 250,  1 },
    {  2,  2, 300,  0 },
    {  2,  2, 350,  0 },
    {  2,  2, 400,  1 },
  };
  hash_map_insert(&forward_to, 2, 1);
  hash_map_insert(&forward_to, 4, 1);
  const uint8_t cha_idx[] = {0, 47, };
  const uint8_t cha_idx_to[] = {1, 4, };
  add_links(add_link, 56, cha_idx, cha_idx_to, 2);
  destination.u8[NODE_ID_INDEX] = 1;
  tsch_queue_update_time_source(&destination);
  max_transmissions[1] = 3;
  max_transmissions[3] = 2;
  #if !TSCH_TTL_BASED_RETRANSMISSIONS
    sending_slots[0] = 19;
    sending_slots[1] = 20;
    num_sending_slots = 2;
  #endif /* !TSCH_TTL_BASED_RETRANSMISSIONS */
} else if (node_id == 1){
  const scheduled_link_t add_link[] = {
    {  2,  2,  1,  0 },
    {  2,  2,  2,  0 },
    {  2,  2,  3,  0 },
    {  4,  2, 19,  0 },
    {  4,  2, 20,  0 },
    {  2,  2, 51,  0 },
    {  2,  2, 52,  0 },
    {  2,  2, 53,  0 },
    {  4,  2, 64,  0 },
    {  4,  2, 65,  0 },
    {  2,  2, 101,  0 },
    {  2,  2, 102,  0 },
    {  2,  2, 103,  0 },
    {  4,  2, 109,  0 },
    {  4,  2, 110,  0 },
    {  2,  2, 151,  0 },
    {  2,  2, 152,  0 },
    {  2,  2, 153,  0 },
    {  4,  2, 157,  0 },
    {  4,  2, 158,  0 },
    {  4,  2, 199,  0 },
    {  4,  2, 200,  0 },
    {  2,  2, 202,  0 },
    {  2,  2, 203,  0 },
    {  2,  2, 204,  0 },
    {  4,  2, 244,  0 },
    {  4,  2, 245,  0 },
    {  2,  2, 251,  0 },
    {  2,  2, 252,  0 },
    {  2,  2, 253,  0 },
    {  4,  2, 289,  0 },
    {  4,  2, 290,  0 },
    {  2,  2, 301,  0 },
    {  2,  2, 302,  0 },
    {  2,  2, 303,  0 },
    {  4,  2, 334,  0 },
    {  4,  2, 335,  0 },
    {  2,  2, 351,  0 },
    {  2,  2, 352,  0 },
    {  2,  2, 353,  0 },
    {  4,  2, 381,  0 },
    {  4,  2, 382,  0 },
    {  2,  2, 401,  0 },
    {  2,  2, 402,  0 },
    {  2,  2, 403,  0 },
    {  4,  2, 424,  0 },
    {  4,  2, 425,  0 },
    {  1,  2,  4,  0 },
    {  1,  2,  5,  0 },
    {  1,  2,  6,  0 },
    {  3,  2,  9,  0 },
    {  3,  2, 10,  0 },
    {  1,  2, 28,  0 },
    {  1,  2, 29,  0 },
    {  1,  2, 30,  0 },
    {  3,  2, 39,  0 },
    {  3,  2, 40,  0 },
    {  1,  2, 54,  0 },
    {  1,  2, 55,  0 },
    {  1,  2, 56,  0 },
    {  3,  2, 69,  0 },
    {  3,  2, 70,  0 },
    {  1,  2, 78,  0 },
    {  1,  2, 79,  0 },
    {  1,  2, 80,  0 },
    {  3,  2, 99,  0 },
    {  3,  2, 100,  0 },
    {  1,  2, 104,  0 },
    {  1,  2, 105,  0 },
    {  1,  2, 106,  0 },
    {  1,  2, 128,  0 },
    {  1,  2, 129,  0 },
    {  1,  2, 130,  0 },
    {  3,  2, 131,  0 },
    {  3,  2, 132,  0 },
    {  1,  2, 154,  0 },
    {  1,  2, 155,  0 },
    {  1,  2, 156,  0 },
    {  3,  2, 159,  0 },
    {  3,  2, 160,  0 },
    {  1,  2, 178,  0 },
    {  1,  2, 179,  0 },
    {  1,  2, 180,  0 },
    {  3,  2, 189,  0 },
    {  3,  2, 190,  0 },
    {  1,  2, 205,  0 },
    {  1,  2, 206,  0 },
    {  1,  2, 207,  0 },
    {  3,  2, 219,  0 },
    {  3,  2, 220,  0 },
    {  1,  2, 228,  0 },
    {  1,  2, 229,  0 },
    {  1,  2, 230,  0 },
    {  3,  2, 249,  0 },
    {  3,  2, 250,  0 },
    {  1,  2, 254,  0 },
    {  1,  2, 255,  0 },
    {  1,  2, 256,  0 },
    {  1,  2, 278,  0 },
    {  1,  2, 279,  0 },
    {  1,  2, 280,  0 },
    {  3,  2, 281,  0 },
    {  3,  2, 282,  0 },
    {  1,  2, 304,  0 },
    {  1,  2, 305,  0 },
    {  1,  2, 306,  0 },
    {  3,  2, 309,  0 },
    {  3,  2, 310,  0 },
    {  1,  2, 328,  0 },
    {  1,  2, 329,  0 },
    {  1,  2, 330,  0 },
    {  3,  2, 339,  0 },
    {  3,  2, 340,  0 },
    {  1,  2, 354,  0 },
    {  1,  2, 355,  0 },
    {  1,  2, 356,  0 },
    {  3,  2, 369,  0 },
    {  3,  2, 370,  0 },
    {  1,  2, 378,  0 },
    {  1,  2, 379,  0 },
    {  1,  2, 380,  0 },
    {  3,  2, 399,  0 },
    {  3,  2, 400,  0 },
    {  1,  2, 404,  0 },
    {  1,  2, 405,  0 },
    {  1,  2, 406,  0 },
    {  1,  2, 428,  0 },
    {  1,  2, 429,  0 },
    {  1,  2, 430,  0 },
    {  3,  2, 431,  0 },
    {  3,  2, 432,  0 },
  };
  const uint8_t cha_idx[] = {0, 47, };
  const uint8_t cha_idx_to[] = {2, 3, };
  add_links(add_link, 131, cha_idx, cha_idx_to, 2);
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
    {  1,  1, 52,  1 },
    {  1,  1, 54,  0 },
    {  1,  1, 55,  0 },
    {  1,  1, 77,  0 },
    {  1,  1, 78,  0 },
    {  1,  1, 79,  0 },
    {  1,  1, 102,  1 },
    {  1,  1, 104,  0 },
    {  1,  1, 105,  0 },
    {  1,  1, 127,  0 },
    {  1,  1, 128,  0 },
    {  1,  1, 129,  0 },
    {  1,  1, 152,  1 },
    {  1,  1, 154,  0 },
    {  1,  1, 155,  0 },
    {  1,  1, 177,  0 },
    {  1,  1, 178,  0 },
    {  1,  1, 179,  0 },
    {  1,  1, 202,  1 },
    {  1,  1, 205,  0 },
    {  1,  1, 206,  0 },
    {  1,  1, 227,  0 },
    {  1,  1, 228,  0 },
    {  1,  1, 229,  0 },
    {  1,  1, 252,  1 },
    {  1,  1, 254,  0 },
    {  1,  1, 255,  0 },
    {  1,  1, 277,  0 },
    {  1,  1, 278,  0 },
    {  1,  1, 279,  0 },
    {  1,  1, 302,  1 },
    {  1,  1, 304,  0 },
    {  1,  1, 305,  0 },
    {  1,  1, 327,  0 },
    {  1,  1, 328,  0 },
    {  1,  1, 329,  0 },
    {  1,  1, 352,  1 },
    {  1,  1, 354,  0 },
    {  1,  1, 355,  0 },
    {  1,  1, 377,  0 },
    {  1,  1, 378,  0 },
    {  1,  1, 379,  0 },
    {  1,  1, 402,  1 },
    {  1,  1, 404,  0 },
    {  1,  1, 405,  0 },
    {  1,  1, 427,  0 },
    {  1,  1, 428,  0 },
    {  1,  1, 429,  0 },
  };
  hash_map_insert(&forward_to, 1, 3);
  const uint8_t cha_idx[] = {0, };
  const uint8_t cha_idx_to[] = {3, };
  add_links(add_link, 54, cha_idx, cha_idx_to, 1);
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
  own_transmission_flow = 3;
  is_sender = 1;
  own_receiver = 1;
  schedule_length = 30;
  const scheduled_link_t add_link[] = {
    {  1,  3,  4,  0 },
    {  1,  3,  5,  0 },
    {  1,  1,  6,  0 },
    {  3,  1,  9,  0 },
    {  3,  1, 10,  0 },
    {  1,  3, 28,  0 },
    {  1,  3, 29,  0 },
    {  1,  1, 30,  0 },
    {  3,  1, 39,  0 },
    {  3,  1, 40,  0 },
    {  1,  3, 54,  0 },
    {  1,  3, 55,  0 },
    {  1,  1, 56,  0 },
    {  3,  1, 69,  0 },
    {  3,  1, 70,  0 },
    {  1,  3, 78,  0 },
    {  1,  3, 79,  0 },
    {  1,  1, 80,  0 },
    {  3,  1, 99,  0 },
    {  3,  1, 100,  0 },
    {  1,  3, 104,  0 },
    {  1,  3, 105,  0 },
    {  1,  1, 106,  0 },
    {  1,  3, 128,  0 },
    {  1,  3, 129,  0 },
    {  1,  1, 130,  0 },
    {  3,  1, 131,  0 },
    {  3,  1, 132,  0 },
    {  1,  3, 154,  0 },
    {  1,  3, 155,  0 },
    {  1,  1, 156,  0 },
    {  3,  1, 159,  0 },
    {  3,  1, 160,  0 },
    {  1,  3, 178,  0 },
    {  1,  3, 179,  0 },
    {  1,  1, 180,  0 },
    {  3,  1, 189,  0 },
    {  3,  1, 190,  0 },
    {  1,  3, 205,  0 },
    {  1,  3, 206,  0 },
    {  1,  1, 207,  0 },
    {  3,  1, 219,  0 },
    {  3,  1, 220,  0 },
    {  1,  3, 228,  0 },
    {  1,  3, 229,  0 },
    {  1,  1, 230,  0 },
    {  3,  1, 249,  0 },
    {  3,  1, 250,  0 },
    {  1,  3, 254,  0 },
    {  1,  3, 255,  0 },
    {  1,  1, 256,  0 },
    {  1,  3, 278,  0 },
    {  1,  3, 279,  0 },
    {  1,  1, 280,  0 },
    {  3,  1, 281,  0 },
    {  3,  1, 282,  0 },
    {  1,  3, 304,  0 },
    {  1,  3, 305,  0 },
    {  1,  1, 306,  0 },
    {  3,  1, 309,  0 },
    {  3,  1, 310,  0 },
    {  1,  3, 328,  0 },
    {  1,  3, 329,  0 },
    {  1,  1, 330,  0 },
    {  3,  1, 339,  0 },
    {  3,  1, 340,  0 },
    {  1,  3, 354,  0 },
    {  1,  3, 355,  0 },
    {  1,  1, 356,  0 },
    {  3,  1, 369,  0 },
    {  3,  1, 370,  0 },
    {  1,  3, 378,  0 },
    {  1,  3, 379,  0 },
    {  1,  1, 380,  0 },
    {  3,  1, 399,  0 },
    {  3,  1, 400,  0 },
    {  1,  3, 404,  0 },
    {  1,  3, 405,  0 },
    {  1,  1, 406,  0 },
    {  1,  3, 428,  0 },
    {  1,  3, 429,  0 },
    {  1,  1, 430,  0 },
    {  3,  1, 431,  0 },
    {  3,  1, 432,  0 },
    {  1,  2,  2,  1 },
    {  1,  2, 27,  0 },
    {  1,  2, 52,  1 },
    {  1,  2, 77,  0 },
    {  1,  2, 102,  1 },
    {  1,  2, 127,  0 },
    {  1,  2, 152,  1 },
    {  1,  2, 177,  0 },
    {  1,  2, 202,  1 },
    {  1,  2, 227,  0 },
    {  1,  2, 252,  1 },
    {  1,  2, 277,  0 },
    {  1,  2, 302,  1 },
    {  1,  2, 327,  0 },
    {  1,  2, 352,  1 },
    {  1,  2, 377,  0 },
    {  1,  2, 402,  1 },
    {  1,  2, 427,  0 },
  };
  hash_map_insert(&forward_to, 1, 1);
  hash_map_insert(&forward_to, 3, 1);
  const uint8_t cha_idx[] = {0, 84, };
  const uint8_t cha_idx_to[] = {1, 5, };
  add_links(add_link, 102, cha_idx, cha_idx_to, 2);
  destination.u8[NODE_ID_INDEX] = 1;
  tsch_queue_update_time_source(&destination);
  max_transmissions[0] = 3;
  max_transmissions[2] = 2;
  #if !TSCH_TTL_BASED_RETRANSMISSIONS
    sending_slots[0] = 9;
    sending_slots[1] = 10;
    num_sending_slots = 2;
  #endif /* !TSCH_TTL_BASED_RETRANSMISSIONS */
}
