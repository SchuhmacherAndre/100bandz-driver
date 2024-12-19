#ifndef UTILS_HH
#define UTILS_HH

#pragma once

class c_utils
{
public:
	VOID DisableWriteProtect(KIRQL& irql);
	VOID EnableWriteProtect(KIRQL& irql);
};
extern c_utils* utils;

#endif // UTILS_HH