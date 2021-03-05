#ifdef CONTIKI_TARGET_SKY
  if (node_id == 3) {
    etimer_set(&periodic_timer, (40 * CLOCK_SECOND) / 40);
  }
#else
  if (node_id == 3) {
    etimer_set(&periodic_timer, (40 * CLOCK_SECOND) / 100);
  }
#endif