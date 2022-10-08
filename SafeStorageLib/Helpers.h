#ifndef _HELPERS_H_
#define _HELPERS_H_

#include "includes.h"

_Success_(return)
_Must_inspect_result_
_No_competing_thread_
BOOL
DirectoryExists(
	_In_z_ PCHAR DirectoryPath
);


#endif // _HELPERS_H