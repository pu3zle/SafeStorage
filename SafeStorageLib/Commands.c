#include "Commands.h"
#include "Helpers.h"
#include "InputValidation.h"

extern GLOBAL_DATA gGlobalData;

_Use_decl_annotations_
NTSTATUS
SafeStorageHandleRegister(
    _In_reads_opt_(UsernameLength) PCHAR Username,
    _In_ USHORT UsernameLength,
    _In_reads_opt_(PasswordLength) PCHAR Password,
    _In_ USHORT PasswordLength
)
{
    // [1] Basic input validation
    if (!Username)
    {
        return STATUS_INVALID_PARAMETER_1;
    }
    if (!Password)
    {
        return STATUS_INVALID_PARAMETER_3;
    }
    if (gGlobalData.IsUserLoggedIn)
    {
        return STATUS_INVALID_SESSION;
    }

    // [2] Input sanitization
    BOOL isUsernameClean = IsInputCleanSafe(Username, UsernameLength);
    if (!isUsernameClean)
    {
        return STATUS_INVALID_PARAMETER_1;
    }

    BOOL isPasswordClean = IsInputCleanSafe(Password, PasswordLength);
    if (!isPasswordClean)
    {
        return STATUS_INVALID_PARAMETER_3;
    }
    BOOL isPasswordSafeEnough = IsPasswordSafeEnough(Password, PasswordLength);
    if (!isPasswordSafeEnough)
    {
        return STATUS_PASSWORD_RESTRICTION;
    }

    // [3] Find corresponding directory

    return STATUS_SUCCESS;
}

_Use_decl_annotations_
NTSTATUS
SafeStorageHandleLogin(
    _In_reads_bytes_opt_(UsernameLength) PCHAR Username,
    _In_ USHORT UsernameLength,
    _In_reads_bytes_opt_(PasswordLength) PCHAR Password,
    _In_ USHORT PasswordLength
)
{
    UNREFERENCED_PARAMETER(Username);
    UNREFERENCED_PARAMETER(UsernameLength);
    UNREFERENCED_PARAMETER(Password);
    UNREFERENCED_PARAMETER(PasswordLength);

    return STATUS_NOT_IMPLEMENTED;
}

_Use_decl_annotations_
NTSTATUS
SafeStorageHandleLogout()
{
    return STATUS_NOT_IMPLEMENTED;
}

_Use_decl_annotations_
NTSTATUS
SafeStorageHandleStore(
    _In_reads_bytes_opt_(SubmissionNameLength) PCHAR SubmissionName,
    _In_ USHORT SubmissionNameLength,
    _In_reads_bytes_opt_(SourceFileLength) PCHAR SourceFile,
    _In_ USHORT SourceFileLength
)
{
    UNREFERENCED_PARAMETER(SubmissionName);
    UNREFERENCED_PARAMETER(SubmissionNameLength);
    UNREFERENCED_PARAMETER(SourceFile);
    UNREFERENCED_PARAMETER(SourceFileLength);

    return STATUS_NOT_IMPLEMENTED;
}

_Use_decl_annotations_
NTSTATUS
SafeStorageHandleRetrieve(
    _In_reads_bytes_opt_(SubmissionNameLength) PCHAR SubmissionName,
    _In_ USHORT SubmissionNameLength,
    _In_reads_bytes_opt_(DestinationFileLength) PCHAR DestinationFile,
    _In_ USHORT DestinationFileLength
)
{
    UNREFERENCED_PARAMETER(SubmissionName);
    UNREFERENCED_PARAMETER(SubmissionNameLength);
    UNREFERENCED_PARAMETER(DestinationFile);
    UNREFERENCED_PARAMETER(DestinationFileLength);

    return STATUS_NOT_IMPLEMENTED;
}