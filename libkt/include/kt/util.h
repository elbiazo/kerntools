#ifndef __KT_UTIL_H__
#define __KT_UTIL_H__
#include "Hexdump.hpp"
#include <iostream>
#include <stdarg.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <spdlog/spdlog.h>

#ifdef __ANDROID__
#include <sys/system_properties.h>
#endif
//
// debug print function
//
namespace util
{
    void hex_dump(const char *data, size_t size);

    void c_hex_dump(const void *data, size_t size);

#ifdef __ANDROID__
#include <sys/system_properties.h>
    bool check_version(std::string version);
    std::string get_version();
#endif

}
// end of debug print functions
#endif