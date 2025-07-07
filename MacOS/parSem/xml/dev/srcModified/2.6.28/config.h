#ifdef _WIN32
#ifdef ARCH64BITS
#include "include/win64config.h"
#else
#include "include/win32config.h"
#endif
#else
#include "config.h.unix"
#endif
