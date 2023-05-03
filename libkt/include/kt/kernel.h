#ifndef __KT_KERNEL_H__
#define __KT_KERNEL_H__
#include <vector>
#include <fstream>
#include <sstream>
#include <map>

namespace kernel
{
    // Linux Kernel
    class LinKern
    {

    public:
        size_t base;
        // custom sym_map
        std::map<std::string, size_t> syms;

        LinKern();

        size_t calc_base(std::string kallsym_path, std::string symbol, size_t sym_addr);
        size_t calc_sym(std::string kallsym_path, std::string symbol);

        // find the address of a symbol in the kallsyms file
        size_t find_sym(std::string kallsym_path, std::string symbol);
    };

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

    void print_state(userspace_state *state);

#if __x86_64__
    void save_state(userspace_state *state);
    void restore_state(userspace_state *state, void (*f)());
#endif // __x86_64__

}
#endif // __KT_KERNEL_H__