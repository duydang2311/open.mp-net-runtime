#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__CYGWIN__)
#define OS_WIN
#else
#define OS_UNIX
#endif
