#ifndef SENTINEL_DLL_H
#define SENTINEL_DLL_H

#ifdef __cplusplus
extern "C" {
#endif

__declspec(dllexport) void StartSentinel();
__declspec(dllexport) void StopSentinel();

#ifdef __cplusplus
}
#endif

#endif