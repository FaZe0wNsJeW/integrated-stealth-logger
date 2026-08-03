#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Logger functions

typedef enum {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR
} LOG_LEVEL;

HANDLE CreateLogger(const char* szLogFile, LOG_LEVEL nLogLevel) {
    FILE* pFile = fopen(szLogFile, "a");
    if (!pFile) {
        return NULL;
    }
    
    // Store log level and file handle in a struct
    // For simplicity, we'll just return the file handle directly
    return (HANDLE)pFile;
}

VOID LogMessage(HANDLE hLogger, LOG_LEVEL nLogLevel, const char* szFormat, ...) {
    if (!hLogger) {
        return;
    }
    
    FILE* pFile = (FILE*)hLogger;
    char szBuffer[1024];
    char szTime[64];
    
    // Get current time
    time_t t = time(NULL);
    struct tm* pTime = localtime(&t);
    strftime(szTime, sizeof(szTime), "%Y-%m-%d %H:%M:%S", pTime);
    
    // Format log level
    const char* szLevel;
    switch (nLogLevel) {
        case LOG_LEVEL_DEBUG: szLevel = "DEBUG"; break;
        case LOG_LEVEL_INFO: szLevel = "INFO"; break;
        case LOG_LEVEL_WARNING: szLevel = "WARNING"; break;
        case LOG_LEVEL_ERROR: szLevel = "ERROR"; break;
        default: szLevel = "UNKNOWN"; break;
    }
    
    // Format message
    va_list args;
    va_start(args, szFormat);
    vsnprintf(szBuffer, sizeof(szBuffer), szFormat, args);
    va_end(args);
    
    // Write to log file
    fprintf(pFile, "[%s] [%s] %s\n", szTime, szLevel, szBuffer);
    fflush(pFile);
}

VOID CloseLogger(HANDLE hLogger) {
    if (hLogger) {
        fclose((FILE*)hLogger);
    }
}
