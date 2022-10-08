#ifndef _INPUT_VALIDATION_H_
#define _INPUT_VALIDATION_H_

#include "includes.h"

#define MINIMUM_PASSWORD_LENGTH 12

_Success_(return)
_Must_inspect_result_
_No_competing_thread_
inline
BOOL
IsCharacterAllowed(
	_In_	CHAR Character
);

_Success_(return)
_Must_inspect_result_
_No_competing_thread_
BOOL
IsInputCleanUnsafe(
	_In_reads_z_(InputLength) PCHAR Input,
	_In_					  USHORT InputLength
);

_Success_(return)
_Must_inspect_result_
_No_competing_thread_
BOOL
IsInputCleanSafe(
	_In_z_  PCHAR   Input,
	_In_    PUSHORT InputLength
);

_Success_(return)
_Must_inspect_result_
_No_competing_thread_
BOOL
IsPasswordSafeEnough(
	_In_z_  PCHAR   Password,
	_In_    PUSHORT PasswordLength
);

typedef struct _StrongPassRestrictions {
	BOOL HasLowercaseChar : 1;
	BOOL HasUppercaseChar : 1;
	BOOL HasAtLeast12Chars : 1;
	BOOL HasNumberChar : 1;
	BOOL HasSpecialChar : 1;
	UINT Padding : 3;
} StrongPassRestrictions, * PStrongPassRestrictions;


#endif // _INPUT_VALIDATION_H_
