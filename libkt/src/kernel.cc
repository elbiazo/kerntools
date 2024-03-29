#include "kt/kernel.h"

kernel::LinKern::LinKern()
{
    base = 0xffffffff81000000;
}

size_t kernel::LinKern::calc_base(std::string kallsym_path, std::string symbol, size_t sym_addr)
{
    size_t sym_offset = find_sym(kallsym_path, symbol) - find_sym(kallsym_path, "_text");
    size_t base = sym_addr - sym_offset;
    this->base = base;
    return this->base;
}
size_t kernel::LinKern::calc_sym(std::string kallsym_path, std::string symbol)
{
    size_t sym_offset = find_sym(kallsym_path, symbol) - find_sym(kallsym_path, "_text");
    return this->base = base + sym_offset;
}

// find the address of a symbol in the kallsyms file
size_t kernel::LinKern::find_sym(std::string kallsym_path, std::string symbol)
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

// print state struct
void kernel::print_state(userspace_state *state)
{
    printf("User state:\n");
    printf("\tcs: 0x%lx\n", state->cs);
    printf("\tss: 0x%lx\n", state->ss);
    printf("\trsp: 0x%lx\n", state->rsp);
    printf("\trflags: 0x%lx\n", state->rflags);
}
#if __x86_64__
void kernel::save_state(userspace_state *state)
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

void kernel::restore_state(userspace_state *state, void (*f)())
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
#endif // __x86_64__