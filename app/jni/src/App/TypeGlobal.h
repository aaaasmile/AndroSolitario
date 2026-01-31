#ifndef _WINTYPEGLOBAL__H_
#define _WINTYPEGLOBAL__H_

#include <string>
#include <vector>

#ifndef BYTE
typedef unsigned char BYTE;
#endif

#ifndef STRING
typedef std::string STRING;
#endif

#ifndef LPCSTR
typedef const char *LPCSTR, *PCSTR;
#endif

#ifndef VCT_STRING
typedef std::vector<std::string> VCT_STRING;
#endif

#if USE_EMPTY_DATA_PREFIX
#define DATA_PREFIX
#endif

#endif