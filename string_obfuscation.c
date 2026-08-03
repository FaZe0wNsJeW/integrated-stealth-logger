#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// String obfuscation functions

// XOR obfuscation with dynamic key
VOID XorObfuscate(BYTE* pData, SIZE_T dwSize, BYTE bKey) {
    for (SIZE_T i = 0; i < dwSize; i++) {
        pData[i] ^= bKey;
    }
}

// ROT13 obfuscation
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

// Base64 encoding/decoding
static const char* base64_chars = 
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";

int Base64Encode(const unsigned char* src, size_t src_len, char* dst, size_t dst_len) {
    size_t i, j;
    unsigned char a3[3];
    unsigned char a4[4];

    if (dst_len < (src_len + 2) / 3 * 4 + 1) {
        return -1;
    }

    i = 0;
    j = 0;
    while (i < src_len) {
        a3[0] = src[i++];
        a3[1] = (i < src_len) ? src[i++] : 0;
        a3[2] = (i < src_len) ? src[i++] : 0;

        a4[0] = (a3[0] & 0xfc) >> 2;
        a4[1] = ((a3[0] & 0x03) << 4) + ((a3[1] & 0xf0) >> 4);
        a4[2] = ((a3[1] & 0x0f) << 2) + ((a3[2] & 0xc0) >> 6);
        a4[3] = a3[2] & 0x3f;

        dst[j++] = base64_chars[a4[0]];
        dst[j++] = base64_chars[a4[1]];
        dst[j++] = base64_chars[a4[2]];
        dst[j++] = base64_chars[a4[3]];
    }

    for (i = 0; i < (3 - src_len % 3) % 3; i++) {
        dst[j - 1 - i] = '=';
    }

    dst[j] = '\0';
    return j;
}

int Base64Decode(const char* src, size_t src_len, unsigned char* dst, size_t dst_len) {
    size_t i, j;
    unsigned char a3[3];
    unsigned char a4[4];
    int idx = 0;
    int* lookup = (int*)malloc(256 * sizeof(int));

    if (!lookup) {
        return -1;
    }

    for (i = 0; i < 256; i++) {
        lookup[i] = -1;
    }
    for (i = 0; i < 64; i++) {
        lookup[(unsigned char)base64_chars[i]] = i;
    }

    if (src_len % 4 != 0) {
        free(lookup);
        return -1;
    }

    i = 0;
    j = 0;
    while (i < src_len) {
        if (src[i] == '=') {
            break;
        }
        if (lookup[(unsigned char)src[i]] == -1) {
            free(lookup);
            return -1;
        }

        a4[0] = lookup[(unsigned char)src[i++]];
        a4[1] = lookup[(unsigned char)src[i++]];
        a4[2] = lookup[(unsigned char)src[i++]];
        a4[3] = lookup[(unsigned char)src[i++]];

        a3[0] = (a4[0] << 2) + ((a4[1] & 0x30) >> 4);
        a3[1] = ((a4[1] & 0xf) << 4) + ((a4[2] & 0x3c) >> 2);
        a3[2] = ((a4[2] & 0x3) << 6) + a4[3];

        dst[j++] = a3[0];
        if (a4[2] != 64) {
            dst[j++] = a3[1];
        }
        if (a4[3] != 64) {
            dst[j++] = a3[2];
        }
    }

    free(lookup);
    return j;
}

// AES encryption/decryption (simplified)
VOID AesEncrypt(BYTE* pData, SIZE_T dwSize, BYTE* pKey, SIZE_T dwKeySize) {
    // Simplified AES encryption for demonstration
    // In a real implementation, use a proper AES library
    for (SIZE_T i = 0; i < dwSize; i++) {
        pData[i] ^= pKey[i % dwKeySize];
    }
}

VOID AesDecrypt(BYTE* pData, SIZE_T dwSize, BYTE* pKey, SIZE_T dwKeySize) {
    // AES decryption is the same as encryption for XOR-based implementation
    AesEncrypt(pData, dwSize, pKey, dwKeySize);
}

// Combined obfuscation
VOID ObfuscateString(char* szString, BYTE bXorKey, BYTE* pAesKey, SIZE_T dwAesKeySize) {
    // First apply ROT13
    Rot13Obfuscate(szString);
    
    // Then apply XOR
    XorObfuscate((BYTE*)szString, strlen(szString), bXorKey);
    
    // Finally apply AES
    AesEncrypt((BYTE*)szString, strlen(szString), pAesKey, dwAesKeySize);
}

VOID DeobfuscateString(char* szString, BYTE bXorKey, BYTE* pAesKey, SIZE_T dwAesKeySize) {
    // Reverse order of obfuscation
    AesDecrypt((BYTE*)szString, strlen(szString), pAesKey, dwAesKeySize);
    
    XorObfuscate((BYTE*)szString, strlen(szString), bXorKey);
    
    Rot13Obfuscate(szString);
}
