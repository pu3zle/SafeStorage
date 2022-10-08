#include "InputValidation.h"

_Use_decl_annotations_
inline
BOOL
IsCharacterAllowed(
	_In_ CHAR Character
)
{
	return (Character >= 'A' && Character <= 'Z') ||
		(Character >= 'a' && Character <= 'z') ||
		(Character >= '0' && Character <= '9') ||
		(Character >= '#' && Character <= ')') ||
		(Character == '-') || (Character == '.') ||
		(Character == '!') || (Character == "_") ||
		(Character == '@');
}

_Use_decl_annotations_
BOOL
IsInputCleanUnsafe(
	_In_reads_z_(InputLength) PCHAR  Input,
	_In_				      USHORT InputLength
)
{
	for (USHORT i = 0; i < InputLength; i++)
	{
		if (!IsCharacterAllowed(Input[i]))
		{
			return FALSE;
		}
	}
	return TRUE;
}

_Use_decl_annotations_
BOOL
IsInputCleanSafe(
	_In_z_		PCHAR   Input,
	_In_        PUSHORT InputLength
)
{
	USHORT i = 0;
	while (TRUE)
	{
		if (Input[i] == '\0')
		{
			return i == InputLength;
		}
		if (!IsCharacterAllowed(Input[i]))
		{
			return FALSE;
		}
		if (i == USHRT_MAX)
		{
			return FALSE;
		}
		i++;
	}
}

_Use_decl_annotations_
BOOL
IsPasswordSafeEnough
(
	_In_reads_z_(PasswordLength) PCHAR   Password,
	_In_                         PUSHORT PasswordLength
)
{
	StrongPassRestrictions passRestriction = { 0 };
	if (PasswordLength >= MINIMUM_PASSWORD_LENGTH)
	{
		passRestriction.HasAtLeast12Chars |= 1;
	}

	for (USHORT i = 0; i < PasswordLength; i++)
	{
		if (Password[i] >= 'a' && Password[i] <= 'z')
		{
			passRestriction.HasLowercaseChar |= 1;
		}
		else if (Password[i] >= 'A' && Password[i] <= 'Z')
		{
			passRestriction.HasUppercaseChar |= 1;
		}
		else if (Password[i] >= '0' && Password[i] <= '9')
		{
			passRestriction.HasNumberChar |= 1;
		}
		else
		{
			passRestriction.HasSpecialChar |= 1;
		}
	}
	return (*(PUINT)&passRestriction == 0b11111);
}
