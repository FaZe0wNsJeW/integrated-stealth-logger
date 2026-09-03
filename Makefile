# Integrated Stealth Logger Makefile
CC = gcc
CFLAGS = -Wall -Wextra -O2 -fPIC -DPIC -std=c99
LDFLAGS = -shared -ldl

# Targets
all: payload.so

payload.so: main.o payload.o evasion.o c2_communication_fixed.o
	$(CC) $(LDFLAGS) -o $@ $^

main.o: main.c payload.h evasion.h c2_communication_fixed.h config.h
	$(CC) $(CFLAGS) -c -o $@ $<

payload.o: payload.c payload.h payload_config.h evasion.h config.h
	$(CC) $(CFLAGS) -c -o $@ $<

evasion.o: evasion.c evasion.h config.h
	$(CC) $(CFLAGS) -c -o $@ $<

c2_communication_fixed.o: c2_communication_fixed.c c2_communication_fixed.h config.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o payload.so

install: payload.so
	cp payload.so /usr/local/lib/
	chmod 755 /usr/local/lib/payload.so

.PHONY: all clean install