/*
 * Copyright (c) 2017, RISE SICS
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

#include "contiki.h"
#include "rpl.h"
#if UIP_CONF_IPV6_RPL_LITE == 0
#include "rpl-private.h"
#endif /* UIP_CONF_IPV6_RPL_LITE == 0 */

#include <stdio.h>
#include <string.h>

/*---------------------------------------------------------------------------*/
static const char *TOP = "<html><head><title>Contiki-NG</title></head><body>\n";
static const char *BOTTOM = "</body></html>\n";
#if BUF_USES_STACK
static char *bufptr, *bufend;
#define ADD(...) do {                                                   \
    bufptr += snprintf(bufptr, bufend - bufptr, __VA_ARGS__);      \
  } while(0)
#else
static char buf[256];
static int blen;
#define ADD(...) do {                                                   \
    blen += snprintf(&buf[blen], sizeof(buf) - blen, __VA_ARGS__);      \
  } while(0)
#endif

/* Use simple webserver with only one page for minimum footprint.
 * Multiple connections can result in interleaved tcp segments since
 * a single static buffer is used for all segments.
 */
#include "httpd-simple.h"
/* The internal webserver can provide additional information if
 * enough program flash is available.
 */
#define WEBSERVER_CONF_LOADTIME 0
#define WEBSERVER_CONF_FILESTATS 0
#define WEBSERVER_CONF_NEIGHBOR_STATUS 0
/* Adding links requires a larger RAM buffer. To avoid static allocation
 * the stack can be used for formatting; however tcp retransmissions
 * and multiple connections can result in garbled segments.
 * TODO:use PSOCk_GENERATOR_SEND and tcp state storage to fix this.
 */
#define WEBSERVER_CONF_ROUTE_LINKS 0
#if WEBSERVER_CONF_ROUTE_LINKS
#define BUF_USES_STACK 1
#endif

/*---------------------------------------------------------------------------*/
static void
ipaddr_add(const uip_ipaddr_t *addr)
{
  uint16_t a;
  int i, f;
  for(i = 0, f = 0; i < sizeof(uip_ipaddr_t); i += 2) {
    a = (addr->u8[i] << 8) + addr->u8[i + 1];
    if(a == 0 && f >= 0) {
      if(f++ == 0) {
        ADD("::");
      }
    } else {
      if(f > 0) {
        f = -1;
      } else if(i > 0) {
        ADD(":");
      }
      ADD("%x", a);
    }
  }
}
/*---------------------------------------------------------------------------*/
static
PT_THREAD(generate_routes(struct httpd_state *s))
{
  static uip_ds6_route_t *r;
#if RPL_WITH_NON_STORING
  static rpl_ns_node_t *link;
#endif /* RPL_WITH_NON_STORING */
  static uip_ds6_nbr_t *nbr;
#if BUF_USES_STACK
  char buf[256];
#endif
#if WEBSERVER_CONF_LOADTIME
  static clock_time_t numticks;
  numticks = clock_time();
#endif

  PSOCK_BEGIN(&s->sout);

  SEND_STRING(&s->sout, TOP);
#if BUF_USES_STACK
  bufptr = buf;
  bufend = bufptr + sizeof(buf);
#else
  blen = 0;
#endif
  ADD("Neighbors<pre>");

  for(nbr = nbr_table_head(ds6_neighbors);
      nbr != NULL;
      nbr = nbr_table_next(ds6_neighbors, nbr)) {

#if WEBSERVER_CONF_NEIGHBOR_STATUS
#if BUF_USES_STACK
    { char *j = bufptr + 25;
      ipaddr_add(&nbr->ipaddr);
      while(bufptr < j) ADD(" ");
      switch(nbr->state) {
      case NBR_INCOMPLETE: ADD(" INCOMPLETE");
        break;
      case NBR_REACHABLE: ADD(" REACHABLE");
        break;
      case NBR_STALE: ADD(" STALE");
        break;
      case NBR_DELAY: ADD(" DELAY");
        break;
      case NBR_PROBE: ADD(" NBR_PROBE");
        break;
      }
    }
#else
    { uint8_t j = blen + 25;
      ipaddr_add(&nbr->ipaddr);
      while(blen < j) ADD(" ");
      switch(nbr->state) {
      case NBR_INCOMPLETE: ADD(" INCOMPLETE");
        break;
      case NBR_REACHABLE: ADD(" REACHABLE");
        break;
      case NBR_STALE: ADD(" STALE");
        break;
      case NBR_DELAY: ADD(" DELAY");
        break;
      case NBR_PROBE: ADD(" NBR_PROBE");
        break;
      }
    }
#endif
#else
    ipaddr_add(&nbr->ipaddr);
#endif

    ADD("\n");
#if BUF_USES_STACK
    if(bufptr > bufend - 45) {
      SEND_STRING(&s->sout, buf);
      bufptr = buf;
      bufend = bufptr + sizeof(buf);
    }
#else
    if(blen > sizeof(buf) - 45) {
      SEND_STRING(&s->sout, buf);
      blen = 0;
    }
#endif
  }
  ADD("</pre>Routes<pre>\n");
  SEND_STRING(&s->sout, buf);
#if BUF_USES_STACK
  bufptr = buf;
  bufend = bufptr + sizeof(buf);
#else
  blen = 0;
#endif

  for(r = uip_ds6_route_head(); r != NULL; r = uip_ds6_route_next(r)) {

#if BUF_USES_STACK
#if WEBSERVER_CONF_ROUTE_LINKS
    ADD("<a href=http://[");
    ipaddr_add(&r->ipaddr);
    ADD("]/status.shtml>");
    ipaddr_add(&r->ipaddr);
    ADD("</a>");
#else
    ipaddr_add(&r->ipaddr);
#endif
#else
#if WEBSERVER_CONF_ROUTE_LINKS
    ADD("<a href=http://[");
    ipaddr_add(&r->ipaddr);
    ADD("]/status.shtml>");
    SEND_STRING(&s->sout, buf);
    blen = 0;
    ipaddr_add(&r->ipaddr);
    ADD("</a>");
#else
    ipaddr_add(&r->ipaddr);
#endif
#endif
    ADD("/%u (via ", r->length);
    ipaddr_add(uip_ds6_route_nexthop(r));
    if(1 || (r->state.lifetime < 600)) {
      ADD(") %lus\n", (unsigned long)r->state.lifetime);
    } else {
      ADD(")\n");
    }
    SEND_STRING(&s->sout, buf);
#if BUF_USES_STACK
    bufptr = buf;
    bufend = bufptr + sizeof(buf);
#else
    blen = 0;
#endif
  }
  ADD("</pre>");

#if RPL_WITH_NON_STORING
  ADD("Links<pre>\n");
  SEND_STRING(&s->sout, buf);
#if BUF_USES_STACK
  bufptr = buf;
  bufend = bufptr + sizeof(buf);
#else
  blen = 0;
#endif
  for(link = rpl_ns_node_head(); link != NULL; link = rpl_ns_node_next(link)) {
    if(link->parent != NULL) {
      uip_ipaddr_t child_ipaddr;
      uip_ipaddr_t parent_ipaddr;

      rpl_ns_get_node_global_addr(&child_ipaddr, link);
      rpl_ns_get_node_global_addr(&parent_ipaddr, link->parent);

#if BUF_USES_STACK
#if WEBSERVER_CONF_ROUTE_LINKS
      ADD("<a href=http://[");
      ipaddr_add(&child_ipaddr);
      ADD("]/status.shtml>");
      ipaddr_add(&child_ipaddr);
      ADD("</a>");
#else
      ipaddr_add(&child_ipaddr);
#endif
#else
#if WEBSERVER_CONF_ROUTE_LINKS
      ADD("<a href=http://[");
      ipaddr_add(&child_ipaddr);
      ADD("]/status.shtml>");
      SEND_STRING(&s->sout, buf);
      blen = 0;
      ipaddr_add(&child_ipaddr);
      ADD("</a>");
#else
      ipaddr_add(&child_ipaddr);
#endif
#endif

      ADD(" (parent: ");
      ipaddr_add(&parent_ipaddr);
      if(1 || (link->lifetime < 600)) {
        ADD(") %us\n", (unsigned int)link->lifetime);
      } else {
        ADD(")\n");
      }
      SEND_STRING(&s->sout, buf);
#if BUF_USES_STACK
      bufptr = buf;
      bufend = bufptr + sizeof(buf);
#else
      blen = 0;
#endif
    }
  }
  ADD("</pre>");
#endif /* RPL_WITH_NON_STORING */

#if WEBSERVER_CONF_FILESTATS
  static uint16_t numtimes;
  ADD("<br><i>This page sent %u times</i>", ++numtimes);
#endif

#if WEBSERVER_CONF_LOADTIME
  numticks = clock_time() - numticks + 1;
  ADD(" <i>(%u.%02u sec)</i>", numticks / CLOCK_SECOND, (100 * (numticks % CLOCK_SECOND)) / CLOCK_SECOND));
#endif

  SEND_STRING(&s->sout, buf);
  SEND_STRING(&s->sout, BOTTOM);

  PSOCK_END(&s->sout);
}
/*---------------------------------------------------------------------------*/
PROCESS(webserver_nogui_process, "Web server");
PROCESS_THREAD(webserver_nogui_process, ev, data)
{
  PROCESS_BEGIN();

  httpd_init();

  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == tcpip_event);
    httpd_appcall(data);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
httpd_simple_script_t
httpd_simple_get_script(const char *name)
{
  return generate_routes;
}
/*---------------------------------------------------------------------------*/
