#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// String obfuscation functions

VOID ObfuscateString(char* szString, BYTE bKey) {
    while (*szString) {
        *szString ^= bKey;
        szString++;
    }
}

VOID DeobfuscateString(char* szString, BYTE bKey) {
    // Same as obfuscation since XOR is reversible
    ObfuscateString(szString, bKey);
}
