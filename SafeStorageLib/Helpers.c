#include "Helpers.h"

_Use_decl_annotations_
BOOL
DirectoryExists(
	_In_z_ PCHAR DirectoryPath
)
{
	// [1] The Directory Path is build by the application, it will not be user input, no further checks needed
	DWORD fileAttr = GetFileAttributesA(DirectoryPath);
	if (fileAttr == INVALID_FILE_ATTRIBUTES)
		return FALSE;  // Invalid Path

	return (fileAttr & FILE_ATTRIBUTE_DIRECTORY);
}

