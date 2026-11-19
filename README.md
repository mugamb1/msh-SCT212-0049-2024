Name: CHRISTIAN MWENDA MUGAMBI
REG NO: SCT212-0049/2024






QUESTION 1

How to build and run
Compile the shell using `make` and launch the executable:
make
./msh

Features Implemented


Task 1 — Prompt & EOF Handling: Complete (getline() reads arbitrary-length lines and Ctrl-D frees memory and exits cleanly)

Task 2 — Argument Tokenization: Complete (strtok() splits on spaces and tabs, ignores blank lines, and NULL-terminates argv)

Task 3 — External Command Execution: Complete (fork(), execvp(), and waitpid() run external binaries with perror() error reporting and status 127 for unknown commands)

Task 4 — Built-in Commands: Complete (cd [dir] defaulting to $HOME, help, and exit)

Task 5 — Exit Status in Prompt: Complete (displays previous exit status in msh[<status>]> , handling normal exits and 128 + signal terminations)

Task 6 — Background Jobs & Zombie Reaping: Complete (trailing & runs jobs asynchronously, prints child PID, and reaps finished children with WNOHANG)

Task 7 — Output Redirection: Complete (> creates or truncates target files with mode 0644 inside the child process using open(), dup2(), and close())

Task 8 — Signal Handling: Complete (SIGINT is ignored in the shell and restored to SIG_DFL in foreground child processes so Ctrl-C interrupts only the child)

Design Explanation


To execute an external command, msh uses the standard Unix fork(), execvp(), and waitpid() lifecycle. After reading an input line with getline() and splitting it into a NULL-terminated argv array via strtok(), the parent shell calls fork() to duplicate itself into a child process. Inside the child (pid == 0), any output redirection (>) is configured using open(), dup2(), and close(), and default SIGINT handling is restored for foreground jobs before execvp(argv[0], argv) replaces the child's memory image with the requested binary. If execvp() fails—for example, when a command does not exist—the child reports the error with perror() and terminates immediately using _exit(127). Meanwhile, for foreground commands, the parent shell blocks on waitpid(pid, &status, 0) until the child finishes, then extracts the exit code or signal termination status (128 + signal) to display in the next prompt.

Commands like cd and exit must be implemented as shell built-ins in the parent process rather than external executables. When fork() creates a child process, the child inherits a separate copy of the parent's attributes, including its current working directory. If cd ran as an external program inside a child process, calling chdir() would alter only the child's directory and disappear as soon as that child exited, leaving the parent shell's directory unchanged. Similarly, an external exit binary would terminate only its own child process rather than ending the main shell loop and freeing allocated line buffers.

Finally, when a command ends with &, msh runs it in the background by skipping the blocking waitpid() call and immediately returning the prompt. Because terminated child processes remain in the kernel process table as zombies until their parent collects their exit status, msh prevents zombie accumulation by polling waitpid(-1, &bg_status, WNOHANG) in a loop before printing every prompt, reaping any finished background children without blocking interactive input.


Known Limitations


Single Output Redirection Only: Supports truncating stdout redirection (>), but does not support input redirection (<), append redirection (>>), or stderr redirection (2>).

No Pipelines or Command Chaining: Does not support pipes (|) or sequential/conditional operators (;, &&, ||).

No Quote Parsing: Because strtok() splits strictly on spaces and tabs, arguments enclosed in quotes (such as echo "hello world") are split into separate tokens rather than preserved as one argument.

No Variable or Wildcard Expansion: Does not expand environment variables (such as $HOME) or filename globs (*.c) passed as arguments.


Sources and AI-Tool Declaration
Linux Programmer's Manual (man pages): Consulted man 2 fork, man 2 execvp, man 2 waitpid, man 2 open, man 2 dup2, man 2 chdir, man 3 getline, man 3 strtok, and man 7 signal for POSIX system call signatures, flags, and return values.

AI-Tool Declaration: Gemini was used for finding bugs and solving compiler and runtime errors, as well as finding information and explanations about the assignment questions and POSIX system calls.
EOF
