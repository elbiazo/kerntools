#ifndef __LOGGER_H__
#define __LOGGER_H__
#include "Hexdump.hpp"
#include <iostream>
#include <stdarg.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
//
// debug print function
//
namespace logger
{
    void fatal(const char *msg)
    {
        perror(msg);
        exit(-1);
    }
    void error(const char *format, ...)
    {
        if (!format)
        {
            exit(-1);
        }

        fprintf(stderr, "%s", "[!] ");
        va_list args;
        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
        fprintf(stderr, "%s", "\n");
        exit(-1);
    }
    void info(const char *format, ...)
    {
        if (!format)
        {
            exit(-1);
        }

        fprintf(stdout, "%s", "[+] ");
        va_list args;
        va_start(args, format);
        vfprintf(stdout, format, args);
        va_end(args);
        fprintf(stdout, "%s", "\n");
    }

    void hex_dump(const char *data, size_t size)
    {
        std::cout << Hexdump(data, size) << std::endl;
    }
    void c_hex_dump(const void *data, size_t size)
    {
        info("Dumping hex of size: 0x%lx", size);

        char ascii[17];
        size_t i, j;
        ascii[16] = '\0';
        for (i = 0; i < size; ++i)
        {
            if (i % 0x10 == 0)
            {
                printf("0x%0lx: \n\t", i);
            }
            printf("%02X ", ((unsigned char *)data)[i]);
            if (((unsigned char *)data)[i] >= ' ' && ((unsigned char *)data)[i] <= '~')
            {
                ascii[i % 16] = ((unsigned char *)data)[i];
            }
            else
            {
                ascii[i % 16] = '.';
            }
            if ((i + 1) % 8 == 0 || i + 1 == size)
            {
                printf(" ");
                if ((i + 1) % 16 == 0)
                {
                    printf("|  %s \n", ascii);
                }
                else if (i + 1 == size)
                {
                    ascii[(i + 1) % 16] = '\0';
                    if ((i + 1) % 16 <= 8)
                    {
                        printf(" ");
                    }
                    for (j = (i + 1) % 16; j < 16; ++j)
                    {
                        printf("   ");
                    }
                    printf("|  %s \n", ascii);
                }
            }
        }
    }

}
// end of debug print functions
#endif