CC = x86_64-w64-mingw32-gcc
CFLAGS = -O2 -Wall -Wextra -D_CRT_SECURE_NO_WARNINGS -D_WIN32_WINNT=0x0601
LDFLAGS = -lkernel32 -luser32 -ladvapi32 -lws2_32 -lcrypt32 -lshell32

all: stealth-logger.exe

stealth-logger.exe: main.cpp
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

clean:
	rm -f stealth-logger.exe *.o

.PHONY: all clean
