#ifdef CONTIKI_TARGET_SKY
  if (node_id == 4) {
    etimer_set(&periodic_timer, (50 * CLOCK_SECOND) / 40);
  } else if (node_id == 5) {
    etimer_set(&periodic_timer, (25 * CLOCK_SECOND) / 40);
  }
#else
  if (node_id == 4) {
    etimer_set(&periodic_timer, (50 * CLOCK_SECOND) / 100);
  } else if (node_id == 5) {
    etimer_set(&periodic_timer, (25 * CLOCK_SECOND) / 100);
  }
#endif