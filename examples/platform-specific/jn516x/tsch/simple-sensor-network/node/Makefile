CONTIKI_PROJECT = node

TARGET ?= jn516x
JN516x_WITH_DONGLE = 1

CONTIKI=../../../../../..

MODULES_REL += .. ../../tools
MAKE_MAC = MAKE_MAC_TSCH
MODULES += os/net/app-layer/coap
MODULES += os/services/orchestra os/lib/json

all: $(CONTIKI_PROJECT)

include $(CONTIKI)/Makefile.include
