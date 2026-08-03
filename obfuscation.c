#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Obfuscation functions

VOID XorObfuscate(BYTE* pData, SIZE_T dwSize, BYTE bKey) {
    for (SIZE_T i = 0; i < dwSize; i++) {
        pData[i] ^= bKey;
    }
}

VOID Rot13Obfuscate(char* szData) {
    while (*szData) {
        if ((*szData >= 'a' && *szData <= 'm') || (*szData >= 'A' && *szData <= 'M')) {
            *szData += 13;
        } else if ((*szData >= 'n' && *szData <= 'z') || (*szData >= 'N' && *szData <= 'Z')) {
            *szData -= 13;
        }
        szData++;
    }
}
