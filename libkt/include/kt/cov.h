#ifndef KCOV_H
#define KCOV_H
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/ioctl.h> 

#define KCOV_INIT_TRACE _IOR('c', 1, unsigned long)
#define KCOV_ENABLE _IO('c', 100)
#define KCOV_DISABLE _IO('c', 101)
#define COVER_SIZE (64 << 10)

#define KCOV_TRACE_PC 0
#define KCOV_TRACE_CMP 1

#define AMD64_PREV_INSTR_SIZE 5

namespace cov
{
    class KCov
    {
    public:
        int fd;
        unsigned long *cover;
        uint32_t cover_size;
        uint32_t current_read_size;
        KCov() : KCov(COVER_SIZE) {}
        KCov(uint64_t cover_size) : cover_size(cover_size), current_read_size(0)
        {
            /* A single fd descriptor allows coverage collection on a single
             * thread.
             */
            fd = open("/sys/kernel/debug/kcov", O_RDWR);
            if (fd == -1)
                perror("open"), exit(1);
            /* Setup trace mode and trace size. */
            if (ioctl(fd, KCOV_INIT_TRACE, cover_size))
                perror("ioctl"), exit(1);
            /* Mmap buffer shared between kernel- and user-space. */
            cover = (unsigned long *)mmap(NULL, cover_size * sizeof(unsigned long),
                                          PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            if ((void *)cover == MAP_FAILED)
                perror("mmap"), exit(1);
            /* Enable coverage collection on the current thread. */
            if (ioctl(fd, KCOV_ENABLE, KCOV_TRACE_PC))
                perror("ioctl"), exit(1);

            reset_coverage();
        }
        ~KCov()
        {
            /* Disable coverage collection for the current thread. After this call
             * coverage can be enabled for a different thread.
             */
            if (ioctl(fd, KCOV_DISABLE, 0))
                perror("ioctl"), exit(1);
            /* Free resources. */
            if (munmap(cover, cover_size * sizeof(unsigned long)))
                perror("munmap"), exit(1);
            if (close(fd))
                perror("close"), exit(1);
        }

        void reset_coverage()
        {
            /* Reset coverage from the tail of the ioctl() call. */
            __atomic_store_n(&cover[0], 0, __ATOMIC_RELAXED);
            current_read_size = 0;
        }

        void read()
        {

            current_read_size = __atomic_load_n(&cover[0], __ATOMIC_RELAXED);
            std::cout << "There are " << current_read_size << " PCs collected." << std::endl;
        }

        void print()
        {
            for (uint64_t i = 0; i < current_read_size; i++)
            {
                std::cout << std::showbase << std::hex << cover[i + 1] - AMD64_PREV_INSTR_SIZE << std::endl;
            }
        }

        void save_kcov(std::string kcov_filename)
        {

            std::ofstream kcov_s(kcov_filename);
            for (uint64_t i = 0; i < current_read_size; i++)
            {
                /* Make sure address leads with 0x and substract 5 to get previous instruction address for syz-cover parser */
                kcov_s << std::showbase << std::hex << cover[i + 1] - AMD64_PREV_INSTR_SIZE << std::endl;
                // std::cout << std::showbase << std::hex << cover[i + 1] - AMD64_PREV_INSTR_SIZE << std::endl;
            }
        }
    };
} // namespace cov
#endif // KCOV_H
