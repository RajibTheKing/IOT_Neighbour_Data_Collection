CONTIKI_PROJECT = strength-vector.c neighbour-data-collection.c
all: $(CONTIKI_PROJECT)

CONTIKI = ../..

TARGET = sky

#use this to enable TSCH: MAKE_MAC = MAKE_MAC_TSCH
MAKE_MAC = MAKE_MAC_CSMA
MAKE_NET = MAKE_NET_NULLNET
include $(CONTIKI)/Makefile.include
