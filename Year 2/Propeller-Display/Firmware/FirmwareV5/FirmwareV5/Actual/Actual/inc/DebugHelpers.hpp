#pragma once


#ifdef NDEBUG
#define DEBUG_LOG(X) do { } while(0)

#else
#include <stdio.h>
#define DEBUG_LOG(...) printf("debug log: " __VA_ARGS__);
#endif
