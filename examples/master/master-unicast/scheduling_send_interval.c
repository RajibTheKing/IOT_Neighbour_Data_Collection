#ifdef CONTIKI_TARGET_SKY
  if (node_id == 2) {
    etimer_set(&periodic_timer, (45 * CLOCK_SECOND) / 40);
  }
#else
  if (node_id == 2) {
    etimer_set(&periodic_timer, (45 * CLOCK_SECOND) / 100);
  }
#endif