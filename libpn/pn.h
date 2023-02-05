#ifndef __PN_H__
#define __PN_H__
#include <stdarg.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

//
// debug print function
//
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

void hex_dump(const void *data, size_t size)
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

// end of debug print functions

//
// privilege escalation functions
//

// strucutre that is use to store off the user state.
// when you are trying to go from kernel to user mode
// code is form pawnyable.cafe
// https://pawnyable.cafe/linux-kernel/LK01/stack_overflow.html
typedef struct userspace_state
{
    uint64_t cs;
    uint64_t ss;
    uint64_t rsp;
    uint64_t rflags;
} userspace_state;

// structure that holds every kernel info
typedef struct kernel
{
    size_t kbase;
    userspace_state us_state;
} kernel;

void save_state(userspace_state *state)
{
    info("Saving state");
    asm(
        "movq %%cs, %0\n"
        "movq %%ss, %1\n"
        "movq %%rsp, %2\n"
        "pushfq\n"
        "popq %3\n"
        : "=r"(state->cs), "=r"(state->ss), "=r"(state->rsp), "=r"(state->rflags)
        :
        : "memory");
}

static void win()
{
    char *argv[] = {"/bin/sh", NULL};
    char *envp[] = {NULL};
    info("You win!");
    execve("/bin/sh", argv, envp);
}

void restore_state(userspace_state *state, void (*f)())
{
    asm volatile("swapgs ;"
                 "movq %0, 0x20(%%rsp)\t\n"
                 "movq %1, 0x18(%%rsp)\t\n"
                 "movq %2, 0x10(%%rsp)\t\n"
                 "movq %3, 0x08(%%rsp)\t\n"
                 "movq %4, 0x00(%%rsp)\t\n"
                 "iretq"
                 :
                 : "r"(state->ss),
                   "r"(state->rsp),
                   "r"(state->rflags),
                   "r"(state->cs),
                   "r"(f));
}

// print kernel struct
void print_kernel(kernel *k)
{
    printf("Kernel base: 0x%lx\n", k->kbase);
    printf("User state:\n");
    printf("\tcs: 0x%lx\n", k->us_state.cs);
    printf("\tss: 0x%lx\n", k->us_state.ss);
    printf("\trsp: 0x%lx\n", k->us_state.rsp);
    printf("\trflags: 0x%lx\n", k->us_state.rflags);
}
// end of privilege escalation functions

#endif