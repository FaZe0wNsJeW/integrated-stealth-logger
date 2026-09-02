#ifndef COM_HIJACK_H
#define COM_HIJACK_H

int HijackCOMCLSID(const char* clsid, const char* payloadPath);
int RestoreCOMCLSID(const char* clsid, const char* originalPath);

#endif