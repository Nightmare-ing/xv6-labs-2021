## Error during Compilation

When compiling the xv6, encountered two compilation errors, I compile on
platform macOS with HomeBrew, install QEMU stable version 10.1.2,
riscv/riscv/riscv-tools stable version 0.2.
So I changed two places in the source code pulled from
git://g.csail.mit.edu/xv6-labs-2021, one is infinite recursion error in
`/user/sh.c`, the `runcmd` function, another one is incompatible pointer in
`user/usertests.c`.

I fixed the first error by change signature of `runcmd` to be
`__attribute__((noreturn)) void runcmd`, according to
[this link](https://github.com/mit-pdos/xv6-riscv/pull/126).

I fixed the second error by change the signature of `rwsbrk` to be `void
rwsbrk(char *s)`, according to [this link](https://github.com/mit-pdos/xv6-riscv/commit/a3f3a04f913535868f5030d9845f7e4b90edb379).

Now I can compile successfully.

## Lab util

### Sleep

`kernel/sysproc.c` implements the `sleep` system call, which is the code run
when entering kernel space. The `sleep` callable from a user program in
`user/user.h` is actually implemented in `user/usys.S`, which loads the address
for the `sleep` system call in `kernel/sysproc.c` and then run `ecall`.

### Pingpong

Some notes for this part

- For a pipe `p`, the read end is `p[0]`, the write end is `p[1]`, can't be
  exchanged.
- No need to wait for child to return, because `read` will pause until all file
  descriptors referring to the write end to be closed
- Should handle all the errors for library functions
- Also checked that whether the read contents match the written contents

### Find

Some notes for this part

- When trying to get the type of the items in the directory, we can use `stat`,
  with which we don't need to open the file to get a file descriptor, and
  don't need to close it after retrieving status. Otherwise, if we forget to
  close the file descriptor, it's very likely to run out of file descriptors
- Can print the error message to `stderr` using `fprintf` with `2` as the file
  descriptor, but if the error happens when trying to get the status of the
  items in the directory, can just use `printf`
- Follows the convention in `ls.c`, the maximum length of the file name is
  defined as `DIRSIZ`, and no matter no many characters the actual file name
  has, we always use a fixed length buffer (which is `DIRSIZ`) to store it
- `dirent` stands for directory entry. xv6 book mentioned that, each `inode`
  can have multiple names, called *links*, and each link consists of an **entry
  in a directory**, and that's actually `dirent`. It also mentioned that, the
  entry contains a file name and a reference to an inode, which is `inum` and
  `name` in `struct dirent`.

### Xargs

Use `malloc` to allocate memory when storing the command args into a new `argv`
array, which later will be fed into `exec` in the child process.
Remember to free up this part of memory to avoid memroy leak.

## System Calls

### System Call Tracing

In `sysproc.c`, `sys_trace` reads calling arguments to variable `mask`, and
store this `mask` to field `traced_syscall` of `proc`.  
Then in `syscall.c`, when we call system calls, we check whether there's a mask
bit in `proc.traced_syscall`, if it is, then we print out tracing information

### Sysinfo

In `sysproc.c`, `sys_sysinfo` reads the calling arguments to variable `info_t`,
which is the address that the user wants to store `sysinfo`.

To get the free memory size, according to the contents in `book-riscv-rev4`
Chapter 3.5, in `kalloc.c` we use a struct `kmem` to store all free pages in a list
`keme.freelist`. So we just need to iterate through this list, find how many
pages are free, and then multiply with the page size.

To get the number of processes, see `allocproc` function in `proc.c` for hint.
We have to iterate through all processes stored in `proc[]`, check the status
of each process, and count the number of processes whose state is not `UNUSED`.

Finally we use `copyout` to copy the struct `sinfo` back to user page table. As
is mentioned in `book-riscv-rev4` Chapter 4.4, `copyout` will use `walk` to
find the PA of VA `info_t`, and then store `sinfo` into that address

