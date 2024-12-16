#ifndef UTILS_HH
#define UTILS_HH

#pragma once

namespace utils {
	VOID DisableWriteProtect(KIRQL& irql);
	VOID EnableWriteProtect(KIRQL& irql);
}

#endif // UTILS_HH