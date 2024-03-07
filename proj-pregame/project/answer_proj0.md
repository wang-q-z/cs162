1. What virtual address did the program try to access from userspace that caused it to crash?
Page fault at 0xc0000008: rights violation error reading page in user context.

2. What is the virtual address of the instruction that resulted in the crash?
Interrupt 0x0e (#PF Page-Fault Exception) at eip=0x80488ee

3.To investigate, disassemble the do-nothing binary using objdump (you used this tool in Homework 0). What is the name of the function the program was in when it crashed? Copy the disassembled code for that function onto Gradescope, and identify the instruction at which the program crashed.

objdump -d do-nothing

080488e8 <_start>:
 80488e8:	55                   	push   %ebp
 80488e9:	89 e5                	mov    %esp,%ebp
 80488eb:	83 ec 18             	sub    $0x18,%esp
 80488ee:	8b 45 0c             	mov    0xc(%ebp),%eax   //crashed
 80488f1:	89 44 24 04          	mov    %eax,0x4(%esp)
 80488f5:	8b 45 08             	mov    0x8(%ebp),%eax
 80488f8:	89 04 24             	mov    %eax,(%esp)
 80488fb:	e8 94 f7 ff ff       	call   8048094 <main>
 8048900:	89 04 24             	mov    %eax,(%esp)
 8048903:	e8 d3 21 00 00       	call   804aadb <exit>


4. Find the C code for the function you identified above (Hint: it was executed in userspace, so it’s either in do-nothing.c or one of the files in proj-pregame/src/lib or proj-pregame/src/lib/user), and copy it onto Gradescope. For each instruction in the disassembled function in #3, explain in a few words why it’s necessary and/or what it’s trying to do. 

in proj-pregame/src/lib/user/entry.c

#include <syscall.h>

int main(int, char* []);
void _start(int argc, char* argv[]);

void _start(int argc, char* argv[]) { exit(main(argc, argv)); }



5. Why did the instruction you identified in #3 try to access memory at the virtual address you identified in #1? Don’t explain this in terms of the values of registers; we’re looking for a higher-level explanation




1. Step into the process_execute function. What is the name and address of the thread running this function? What other threads are present in Pintos at this time? Copy their struct threads. (Hint: for the last part, dumplist &all_list thread allelem may be useful.)

thread : main   Don't konw the address

pintos-debug: dumplist #0: 0xc000e000 {tid = 1, status = THREAD_RUNNING, name = "main", '\000' <repeats 11 times>, stack = 0xc000edbc "\001", priority = 31, allelem = {prev = 0xc0039d98 <all_list>, next = 0xc0104020}, elem = {prev = 0xc0039d88<fifo_ready_list>, next = 0xc0039d90 <fifo_ready_list+8>}, pcb = 0xc010500c, magic = 3446325067}
pintos-debug: dumplist #1: 0xc0104000 {tid = 2, status = THREAD_BLOCKED, name = "idle", '\000' <repeats 11 times>, stack =0xc0104f14 "", priority = 0, allelem = {prev = 0xc000e020, next = 0xc0039da0 <all_list+8>}, elem = {prev = 0xc0039d88 <fifo_ready_list>, next = 0xc0039d90 <fifo_ready_list+8>}, pcb = 0x0, magic = 3446325067}


2. What is the backtrace for the current thread? Copy the backtrace from GDB as your answer and also copy down the line of C code corresponding to each function call.
#0  process_execute (file_name=0xc0007d50 "do-nothing") at ../../userprog/process.c:57
#1  0xc0020a19 in run_task (argv=0xc0039c8c <argv+12>) at ../../threads/init.c:315
#2  0xc0020b57 in run_actions (argv=0xc0039c8c <argv+12>) at ../../threads/init.c:388
#3  0xc00203d9 in main () at ../../threads/init.c:136

3. Set a breakpoint at start_process and continue to that point. What is the name and address of the thread running this function? What other threads are present in Pintos at this time? Copy their struct threads.

thread:do-nothing

pintos-debug: dumplist #0: 0xc000e000 {tid = 1, status = THREAD_BLOCKED, name = "main", '\000' <repeats 11 times>, stack =
 0xc000ee7c "", priority = 31, allelem = {prev = 0xc0039d98 <all_list>, next = 0xc0104020}, elem = {prev = 0xc003b7b8 <tem
porary+4>, next = 0xc003b7c0 <temporary+12>}, pcb = 0xc010500c, magic = 3446325067}
pintos-debug: dumplist #1: 0xc0104000 {tid = 2, status = THREAD_BLOCKED, name = "idle", '\000' <repeats 11 times>, stack =
 0xc0104f14 "", priority = 0, allelem = {prev = 0xc000e020, next = 0xc010b020}, elem = {prev = 0xc0039d88 <fifo_ready_list
>, next = 0xc0039d90 <fifo_ready_list+8>}, pcb = 0x0, magic = 3446325067}
pintos-debug: dumplist #2: 0xc010b000 {tid = 3, status = THREAD_RUNNING, name = "do-nothing\000\000\000\000\000", stack =
0xc010bfd4 "", priority = 31, allelem = {prev = 0xc0104020, next = 0xc0039da0 <all_list+8>}, elem = {prev = 0xc0039d88 <fi
fo_ready_list>, next = 0xc0039d90 <fifo_ready_list+8>}, pcb = 0x0, magic = 3446325067}

4. Where is the thread running start_process created? Copy down this line of code

 tid = thread_create(file_name, PRI_DEFAULT, start_process, fn_copy);

 5. Step through the start_process function until you have stepped over the call to load. Note that load sets the eip and esp fields in the if_ structure. Print out the value of the if_ structure, displaying the values in hex (hint: print/x if_).

 before:
 $5 = {edi = 0x0, esi = 0x0, ebp = 0x0, esp_dummy = 0x0, ebx = 0x0, edx = 0x0, ecx = 0x0, eax = 0x0, gs = 0x23, fs = 0x23,
es = 0x23, ds = 0x23, vec_no = 0x0, error_code = 0x0, frame_pointer = 0x0, eip = 0x0, cs = 0x1b, eflags = 0x202, esp = 0x0
, ss = 0x23}

after:
$7 = {edi = 0x0, esi = 0x0, ebp = 0x0, esp_dummy = 0x0, ebx = 0x0, edx = 0x0, ecx = 0x0, eax = 0x0, gs = 0x23, fs = 0x23,
es = 0x23, ds = 0x23, vec_no = 0x0, error_code = 0x0, frame_pointer = 0x0, eip = 0x80488e8, cs = 0x1b, eflags = 0x202, esp
 = 0xc0000000, ss = 0x23}

 6.
先恢复寄存器，然后通过iret指令返回到中断前程序的执行位置继续执行。
before:
eax            0x0      0
ecx            0x0      0
edx            0x0      0
ebx            0x0      0
esp            0xc010bf8c       0xc010bf8c
ebp            0x0      0x0
esi            0x0      0
edi            0x0      0
eip            0xc00223af       0xc00223af <intr_exit+10>
eflags         0x282    [ SF IF ]
cs             0x8      8
ss             0x10     16
ds             0x23     35
es             0x23     35
fs             0x23     35
gs             0x23     35

 after:
 eax            0x0      0
ecx            0x0      0
edx            0x0      0
ebx            0x0      0
esp            0xc0000000       0xc0000000
ebp            0x0      0x0
esi            0x0      0
edi            0x0      0
eip            0x80488e8        0x80488e8
eflags         0x202    [ IF ]
cs             0x1b     27
ss             0x23     35
ds             0x23     35
es             0x23     35
fs             0x23     35
gs             0x23     35


8.
#0  _start (argc=-268370093, argv=0xf000ff53) at ../../lib/user/entry.c:6
#1  0xf000ff53 in ?? ()



debug
1. 用户地址空间开始为0xc0000000，修改之前esp=ebp=0xbffffffc,所以ebp+0xc取参数会越界，将esp-0xc为参数留出空间

2. call main之前 esp = 0xbfffffd8
call main 之后 esp = 0xbfffffd4 为ret留下空间 之后 要求esp % 16 = 12
所以esp - 0x8


3. 0xbfffff98:     0x00000001      0x000000a2

4. args[0]: 1
args[1]: 162
int $0x30 push these arguments from user code into kernel code