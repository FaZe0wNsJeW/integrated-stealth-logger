CC = gcc
CFLAGS = -Wall -Wextra -O2

all: sentinel_test sentinel_upgrade

sentinel_test: sentinel_test.c
	$(CC) $(CFLAGS) -o sentinel_test sentinel_test.c

sentinel_upgrade: sentinel_upgrade.c sentinel_upgrade.h
	$(CC) $(CFLAGS) -c sentinel_upgrade.c

clean:
	rm -f sentinel_test sentinel_upgrade.o

.PHONY: all clean