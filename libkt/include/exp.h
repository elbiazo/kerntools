#ifndef __EXP_H__
#define __EXP_H__
void win()
{
    char *argv[] = {(char *)"/bin/sh", NULL};
    char *envp[] = {NULL};
    execve("/bin/sh", argv, envp);
}
#endif