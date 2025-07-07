#ifdef _WIN32
#ifdef ARCH64BITS
#include "include/win64config.h"
#else
#include "include/win32config.h"
#endif
#else
#ifdef MAC_OS
#include "config-mac.h"
#else
#include "config.h.unix"
#endif
#endif
