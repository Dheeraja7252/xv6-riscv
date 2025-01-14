# Modified xv6

## `trace` implementation

`trace` is a system call used to trace specific system calls made by a program. It takes 2 arguments - pid of the process whose syscalls are to be traced and a bitmask containing the syscall numbers of the syscalls that should be traced.

usage: `strace mask command [args]`

The systemcall is implemented by saving the mask in `struck proc` in the variable `trace`. The trace of the parent is copied to the child when a process is forked ( `trace` of init is of course set to 0).

Now, since every system call is routed through `syscall()`. So after each system call returns, we modify the function to check if the last syscall was to be traced and if necessary, display pid of the calling process, syscall name, arguments and return value pf the syscall. Since the same register is used to pass the first argument and the return value, the first argument to the syscall is saved by `syscall()` before the appropriate function is called. 

The syscall name and number of arguments are stored in `sys_call_info` for easy access. 

To enable the syscall, a syscall number was added to `syscall.h`, prototype for `trace` was added to `user.h`, and a stub to `usys.pl`.

FInally, create a user program `strace.c` in the user directory that uses the trace syscall to set the mask of the process and then executes the appropriate command (using `exec` ). `$U/_strace` is added to the makefile to compile `strace.c`

## Scheduling

xv6 uses a round robin based scheduler by default. The modified version supports the use of 2 other scheduling policies. The desired scheduling algorithm can be set at compile time using the macro `SCHEDULER`.

Using `SCHEDULER=x` while compiling uses the following alternate schedulers:

- x= `FCFS` : first come first serve
- x= `PBS` : priority based scheduling

### First come first serve (FCFS)

FCFS is a non-preemptive scheduler that selects the process with the earliest creation time. 

It is implemented by adding a variable `ctime` to `struct proc` that saves the time when a process was created (set `ctime = ticks` in `allocproc()` ). In the scheduler, we iterate over all process and compare `ctime` of all runnable process, while holding the lock on the process with the oldest `ctime`. When a process with older creation time is found, we release the lock on the previous process and hold the lock on this one. After iterating over all process, the CPU is assigned to the process whose lock is being held.

Additionally, disable timer interrupts in `trap.c` to make it non-preemptive.

### Priority based schedling (PBS)

PBS is a non-preemptive scheduler that assigns priorities to each process and selects the one with the highest priority.

This is implemented by adding the following variables to `struct proc` :

- `sp` : static priority
- `niceness` : niceness of a process is used to calculate its dynamic priority
- `nsched` :  number of times the process has been scheduled to run on cpu
- `rtime` : How long the process ran for
- `rltime`: How long the process ran for last time it was scheduled on run on the cpu
- `stime` : How long the process was sleeping since the last time it was scheduled on run on the cpu

The static priority of a process is 60 by default and can be changed using `set_priority()`. Niceness is calculated as `niceness = (ticks spent in sleeping state) * 10 /(ticks spent in (running +
sleeping) state)` (the number of ticks are counted from the last time the process was on the cpu).

 Dynamic priority is then calculated as `DP = max(0, min(SP - niceness + 5, 100))`. Lower the value of `DP`, higher the priority. 

`rtime`, `rltime`and `stime` are incremented in `update_time()` with each clock tick. 

In the scheduler, we calculate the values of niceness and dp and select the highest-priority process like we did in FCFS.

Everytime a process is scheduled on the cpu, reset `rltime` and `stime` to 0 and increment `nsched`. 

In addition to this, we also implement a syscall `set_priority` that sets the static priority of a process to the specified value and resets its niceness to 5. The steps to add a syscall are similar to those mentioned in the implementation of `trace()`. Similar to `strace`, we add a user program `setpriority` that sets the priority of a process.

usage: `setpriority priority pid`

## procdump

The procdump function is modified to display priority, rtime, wtime and nrun when the scheduling algorithm used is PBS.

## Benchmark

The 3 scheduling policies were used to run a series of programs and their average run and wait times are recorded in the below table.

| Scheduling policy | Average run time | Average wait time |
|-------------------|------------------|-------------------|
| RR                | 31               | 184               |
| FCFS              | 54               | 206               |
| PBS               | 29               | 163               |

FCFS has the highest average run and wait times by quite a large margin. Note that FCFS was tested on only cpu bound processes, which justifies the higher wait times.

We also observe that PBS performs better than RR and FCFS in terms of both runtime and waittime, which is to be expected.