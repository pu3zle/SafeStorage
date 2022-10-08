#include "Globals.h"

GLOBAL_DATA gGlobalData;


_Use_decl_annotations_
NTSTATUS
GlobalDataInitialize()
{
    NTSTATUS status = STATUS_UNSUCCESSFUL;
    DWORD lengthNeeded = GetCurrentDirectoryA(0, NULL);
    if (lengthNeeded == 0)
    {
        return NTSTATUS_FROM_WIN32(GetLastError());
    }

    lengthNeeded += 1; // add space for the null terminating character
    PVOID currentDir = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, lengthNeeded * sizeof(CHAR));
    if (!currentDir)
    {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    DWORD charsWritten = GetCurrentDirectoryA(lengthNeeded, (LPSTR)currentDir);
    if (charsWritten == 0)
    {
        status = STATUS_UNSUCCESSFUL;
        goto cleanup;
    }

    gGlobalData.ApplicationDirectory = (PSTR)currentDir;

cleanup:
    if (!NT_SUCCESS(status))
    {
        if (currentDir)
        {
            HeapFree(GetProcessHeap(), 0, currentDir);
        }
    }

    return status;
}

_Use_decl_annotations_
VOID
GlobalDataUninitialize()
{
    if (gGlobalData.ApplicationDirectory)
    {
        HeapFree(GetProcessHeap(), 0, gGlobalData.ApplicationDirectory);
    }
    return;
}