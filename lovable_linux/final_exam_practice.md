# Angrave's 2018 Acme-CS241-Exam Prep		
## AKA Preparing for the Final Exam & Beyond CS241... 

Some of the questions require research (wikibook; websearch; wikipedia). 
It is ok to work together & discuss answers! 
Be ready to discuss and clear confusions & misconceptions in your last discussion section.
The final will also include pthreads, fork-exec-wait questions and virtual memory address translation. 
Be awesome. Angrave.

## 1. C 


1.	What are the differences between a library call and a system call? Include an example of each.
Library calls: provided by library, may call system call, e.g. printf()
System calls: provided by kernel. Costly. E.g. write()

2.	What is the `*` operator in C? What is the `&` operator? Give an example of each.
* dereference or multiplication
& get the address

3.	When is `strlen(s)` != `1+strlen(s+1)` ?
*s = ‘\0’ && *(s + 1) != ‘\0’	

4.	How are C strings represented in memory? What is the wrong with `malloc(strlen(s))` when copying strings?
A char array with NULL terminated. 
malloc(strlen(s)) doesn’t count the ‘\0’ when copying strings.

5.	Implement a truncation function `void trunc(char*s,size_t max)` to ensure strings are not too long with the following edge cases.
```
if (length < max)
    strcmp(trunc(s, max), s) == 0
else if (s is NULL)
    trunc(s, max) == NULL
else
    strlen(trunc(s, max)) <= max
    // i.e. char s[]="abcdefgh; trunc(s,3); s == "abc". 
```
void trunc(char*s,size_t max){
if(s && strlen(s) > max)  s[max] = '\0';
}


6.	Complete the following function to create a deep-copy on the heap of the argv array. Set the result pointer to point to your array. The only library calls you may use are malloc and memcpy. You may not use strdup.

    `void duplicate(char **argv, char ***result);` 
void duplicate(char**argv, char***result) {
	int count = 0;
	char** temp = argv;
	while(temp){
	   count++;
	   temp++;
	}
*result= malloc(sizeof(char*)*(count+1));
	for(int i = 0; i < count; i++) {
   		(*result)[i] = malloc(strlen(argv[i])+1); 
            	 memcpy((*result)[i], argv[i], strlen(argv[i])+1);
	}
	(*result)[i] = NULL;
}
7.	Write a program that reads a series of lines from `stdin` and prints them to `stdout` using `fgets` or `getline`. Your program should stop if a read error or end of file occurs. The last text line may not have a newline char.

int main(){
	 char* buffer = NULL;
	 size_t size;
	 while(getline(&buffer,&size, stdin) != -1){
		printf(“%s”,buffer);
	}
	free(buffer);

}
## 2. Memory 

1.	Explain how a virtual address is converted into a physical address using a multi-level page table. You may use a concrete example e.g. a 64bit machine with 4KB pages. 
First search TLB(translation lookaside buffer), every time a virtual address needs to be translated into a physical memory location, the TLB is queried in parallel to the page table. For most memory accesses of most programs, there is a significant chance that the TLB has cached the results. However if a program does not have good cache coherence (for example is reading from random memory locations of many different pages) then the TLB will not have the result cache and now the MMU must use the much slower page table to determine the physical frame
The MMU does the search in multilevel page table. For example, the following is a three level page table. The offset is 12bit to hold 4KB address in each block. Use the VPN1 search through the page table 1 then, use the VPN2 search through the result got from precious search, finally go to the lowest level page table and search through vpn3. Then you arrived the frame (which is the physical address block) then use the offset to get the specific physical address.
|--------VPN1--------|--------VPN2--------|--------VPN3--------|--------Offset--------|
> top_level = top_levels[process]
> level1 = top level[VPN1]
> level2 = level1[VPN2]
> level3 = level2[VPN3]
> address = level3 + Offset
2.	Explain Knuth's and the Buddy allocation scheme. Discuss internal & external Fragmentation.
Knuth’s memory allocation scheme makes use of boundary tags. Boundary tags hold the size of the user allocated memory of the block. This allows blocks to be merged forwards and backwards.
Buddy Allocator: Split allocation into blocks of size 2^n. If there are no free blocks of size 2^n, go to the next level and steal that block and split it into two. If two neighboring blocks of the same size become unallocated, they can be coalesced back together into a single large block of twice the size.
Internal Fragmentation: allocations are rounded up to the nearest block size, the leftover space within a block of memory that isn’t being utilized.
External Fragmentation: free memory is separated into small blocks and is interspersed by allocated memory.

3.	What is the difference between the MMU and TLB? What is the purpose of each?
TLB(Translation lookaside buffer) is a memory cache that stores recent translations of virtual memory to physical addresses for faster retrieval.
MMU(Memory Management Unit) part of the CPU. Converts from virtual to physical address. Handles memory protection, cache control, bus arbitration, and bank switching.

4.	Assuming 4KB page tables what is the page number and offset for virtual address 0x12345678  ?
Offset = 0x678
Page number = 0x12345
5.	What is a page fault? When is it an error? When is it not an error?
A page fault is when a running program tries to access some virtual memory in its address space that is not mapped to physical memory.
Major - The page is on disk and needs to be loaded into memory
Minor - The page is not allocated in the multi-level page table so the kernel needs to map.
Invalid - If the memory that is trying to be accessed is not part of the memory mapping (virtual address space) then this kind of error is generated. The operating system usually segfaults
When it’s an Error - Invalid
When it’s not an Error - Major or Minor
6.	What is Spatial and Temporal Locality? Swapping? Swap file? Demand Paging?
Spatial locality refers to an address nearby in memory. [AND that it will be likely be used in the
near future. If this was not true, then working with VM pages would not have good performance]
Temporal locality refers to a recently accessed address. [AND that the same memory item will likely be used in the near future. Hence memory cache gives you improved performance]
Swapping is exchanging memory pages between RAM and Secondary storage (disk)
Swap file is a system file that holds swapped memory pages.
Demand paging is when pages are only copied from the disk to RAM [ONLY] when they are needed.
## 3. Processes and Threads 

1.	What resources are shared between threads in the same process?
	- Everything except stack memory.

2.	Explain the operating system actions required to perform a process context switch
	- Storing the state of a process, so that it can be restored and execution resumed from the same point later. 

3.	Explain the actions required to perform a thread context switch to a thread in the same process
	- Storing the state of a thread, so that it can be restored and execution resumed from the same point later. 

4.	How can a process be orphaned? What does the process do about it?
	- Its parent process is terminated while the child process keep running.	

5.	How do you create a process zombie?
	- a zombie process or defunct process is a process that has completed execution but still has an entry in the process table: it is a process in the "		Terminated state".

6.	Under what conditions will a multi-threaded process exit? (List at least 4)
	- join, main process exits,  
## 4. Scheduling 
1.	Define arrival time, pre-emption, turnaround time, waiting time and response time in the context of scheduling algorithms. What is starvation?  Which scheduling policies have the possibility of resulting in starvation?
	- The time at which a process first arrives at the ready queue, and is ready to start executing. 
	- With preemption, the existing processes may be removed immediately if a more preferable process is added to the ready queue. 
	- turnaround time = end time - arrival time // response time = start time - arrival time // wait time = (end time - arrival time) - run time 

2.	Which scheduling algorithm results the smallest average wait time?
	- SJF

3.	What scheduling algorithm has the longest average response time? shortest total wait time?
	- Round robin has the shortest average response time 

4.	Describe Round-Robin scheduling and its performance advantages and disadvantages.
	- Pros
		Simple to implement // no starvation or convoy effect
	- cons
		Does not support real-time applicationsNo priority(Some CPU-bound process may need to execute more to get jobs done)Requires a lot of context			-switching overhead

5.	Describe the First Come First Serve (FCFS) scheduling algorithm. Explain how it leads to the convoy effect. 
	-  it executes the first process on the queue and never switches it out until it’s done. Then it simply loads up the next process. Processes are added to the queue in the order that they arrive.It has the longest average response time The convoy effect occurs when a huge process that takes a long time is holding up the queue, with many processes stuck behind it. FCFS naturally leads to the convoy effect since no process is allowed to get in front of the blocker process.

6.	Describe the Pre-emptive and Non-preemptive SJF scheduling algorithms. 
	- Shortest Job First scheduling algorithm as the name hints , selects the process for execution which has the smallest amount of time remaining until completion . It can be categorized into two parts : Non-preemptive : Once selected for execution , a process continues to run until the end of its CPU burst .It is also known as Shortest Job First (SJF) . Preemptive : The process which is currently in execution , runs until it complete or a new process is added in the cpu Scheduler that requires smaller amount of time for execution. It is also known as shortest remaining time first(SRTF).This has the shortest wait time.

7.	How does the length of the time quantum affect Round-Robin scheduling? What is the problem if the quantum is too small? In the limit of large time slices Round Robin is identical to _____?
	- (FCFS)
	- Round-Robin has a shorter response time when the time quantum is shorter, but a longer total waiting time. It does better for many short processes. When the time 
quantum is longer, jobs have a shorter total waiting time, but the scheduler becomes vulnerable to the convoy effect.

8.	What reasons might cause a scheduler switch a process from the running to the ready state?
	- when its quantum expires // when it waits for I/O resources // when it is preempted

## 5. Synchronization and Deadlock

1.	Define circular wait, mutual exclusion, hold and wait, and no-preemption. How are these related to deadlock?

2.	What problem does the Banker's Algorithm solve?

3.	What is the difference between Deadlock Prevention, Deadlock Detection and Deadlock Avoidance?

4.	Sketch how to use condition-variable based barrier to ensure your main game loop does not start until the audio and graphic threads have initialized the hardware and are ready.

5.	Implement a producer-consumer fixed sized array using condition variables and mutex lock.

6.	Create an incorrect solution to the CSP for 2 processes that breaks: i) Mutual exclusion. ii) Bounded wait.

7.	Create a reader-writer implementation that suffers from a subtle problem. Explain your subtle bug.

## 6. IPC and signals

1.	Write brief code to redirect future standard output to a file.

2.	Write a brief code example that uses dup2 and fork to redirect a child process output to a pipe

3.	Give an example of kernel generated signal. List 2 calls that can a process can use to generate a SIGUSR1.

4.	What signals can be caught or ignored?

5.	What signals cannot be caught? What is signal disposition?

6.	Write code that uses sigaction and a signal set to create a SIGALRM handler.

7.	Why is it unsafe to call printf, and malloc inside a signal handler?

## 7. Networking 

1.	Explain the purpose of `socket`, `bind`, `listen`, and `accept` functions

2.	Write brief (single-threaded) code using `getaddrinfo` to create a UDP IPv4 server. Your server should print the contents of the packet or stream to standard out until an exclamation point "!" is read.

3.	Write brief (single-threaded) code using `getaddrinfo` to create a TCP IPv4 server. Your server should print the contents of the packet or stream to standard out until an exclamation point "!" is read.

4.	Explain the main differences between using `select` and `epoll`. What are edge- and level-triggered epoll modes?

5.	Describe the services provided by TCP but not UDP. 

6.	How does TCP connection establishment work? And how does it affect latency in HTTP1.0 vs HTTP1.1?

7.	Wrap a version of read in a loop to read up to 16KB into a buffer from a pipe or socket. Handle restarts (`EINTR`), and socket-closed events (return 0).

8.	How is Domain Name System (DNS) related to IP and UDP? When does host resolution not cause traffic?

9.	What is NAT and where and why is it used? 

## 8. Files 

1.	Write code that uses `fseek`, `ftell`, `read` and `write` to copy the second half of the contents of a file to a `pipe`.

2.	Write code that uses `open`, `fstat`, `mmap` to print in reverse the contents of a file to `stderr`.

3.	Write brief code to create a symbolic link and hard link to the file /etc/password

4.	"Creating a symlink in my home directory to the file /secret.txt succeeds but creating a hard link fails" Why? 

5.	Briefly explain permission bits (including sticky and setuid bits) for files and directories.

6.	Write brief code to create a function that returns true (1) only if a path is a directory.

7.	Write brief code to recursive search user's home directory and sub-directories (use `getenv`) for a file named "xkcd-functional.png' If the file is found, print the full path to stdout.

8.	The file 'installmeplz' can't be run (it's owned by root and is not executable). Explain how to use sudo, chown and chmod shell commands, to change the ownership to you and ensure that it is executable.

## 9. File system 
Assume 10 direct blocks, a pointer to an indirect block, double-indirect, and triple indirect block, and block size 4KB.

1.	A file uses 10 direct blocks, a completely full indirect block and one double-indirect block. The latter has just one entry to a half-full indirect block. How many disk blocks does the file use, including its content, and all indirect, double-indirect blocks, but not the inode itself? A sketch would be useful.

2.	How many i-node reads are required to fetch the file access time at /var/log/dmesg ? Assume the inode of (/) is cached in memory. Would your answer change if the file was created as a symbolic link? Hard link?

3.	What information is stored in an i-node?  What file system information is not? 

4.	Using a version of stat, write code to determine a file's size and return -1 if the file does not exist, return -2 if the file is a directory or -3 if it is a symbolic link.

5.	If an i-node based file uses 10 direct and n single-indirect blocks (1 <= n <= 1024), what is the smallest and largest that the file contents can be in bytes? You can leave your answer as an expression.

6.	When would `fstat(open(path,O_RDONLY),&s)` return different information in s than `lstat(path,&s)`?

## 10. "I know the answer to one exam question because I helped write it"

Create a hard but fair 'spot the lie/mistake' multiple choice or short-answer question. Ideally, 50% can get it correct.
