#ifndef __KERNEL_H__
#define __KERNEL_H__
#include <vector>
#include <fstream>
namespace kernel
{
    // Linux Kernel
    class LinKern
    {

    public:
        size_t base;

        LinKern()
        {
            base = 0;
        }

        size_t calc_base(std::string kallsym_path, std::string symbol, size_t sym_addr)
        {
            size_t sym_offset = find_sym(kallsym_path, symbol) - find_sym(kallsym_path, "_text");
            size_t base = sym_addr - sym_offset;
            this->base = base;
            return this->base;
        }

        size_t find_sym(std::string kallsym_path, std::string symbol)
        {
            std::string line;
            std::ifstream ksym_file{kallsym_path};
            if (ksym_file.is_open())
            {
                while (std::getline(ksym_file, line))
                {
                    std::stringstream line_tok(line);
                    std::string item;
                    std::vector<std::string> syms;

                    while (getline(line_tok, item, ' '))
                    {
                        syms.push_back(item);
                    }

                    // check if line has three item which are <symbol address> <symbol type> <symbol name>
                    if (syms.size() != 3)
                    {
                        return 0;
                    }

                    std::string sym_name = syms[2];
                    if (sym_name == symbol)
                    {
                        std::stringstream str_sym_addr;
                        str_sym_addr << std::hex << syms[0];
                        size_t sym_addr;
                        str_sym_addr >> sym_addr;

                        return sym_addr;
                    }
                }
            }

            return 0;
        }
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

    // print state struct
    void print_state(userspace_state *state)
    {
        printf("User state:\n");
        printf("\tcs: 0x%lx\n", state->cs);
        printf("\tss: 0x%lx\n", state->ss);
        printf("\trsp: 0x%lx\n", state->rsp);
        printf("\trflags: 0x%lx\n", state->rflags);
    }

    void save_state(userspace_state *state)
    {
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
}
#endif