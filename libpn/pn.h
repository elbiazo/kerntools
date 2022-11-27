#ifndef __PN_H__
#define __PN_H__
#include <stdarg.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
struct save_state
{
    uint64_t cs;
    uint64_t ss;
    uint64_t rsp;
    uint64_t rflags;
};

void err(const char *format, ...)
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

void save_state(struct save_state *state)
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

void restore_state(struct save_state *state, void (*f)())
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
                   "r"(state->cs), "r"(f));
}
#endif