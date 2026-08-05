CC = x86_64-w64-mingw32-gcc
CFLAGS = -Os -s -ffunction-sections -fdata-sections -Wl,--gc-sections -fno-exceptions -fno-rtti -fvisibility=hidden
TARGET = stealthlogger.exe

all: $(TARGET)

$(TARGET): main.c modular_payload.c modular_payload.h
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)