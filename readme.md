Microshell: Command Execution and Pipe Handling

Overview

This project implements a simple shell-like program that can execute commands, handle pipes (|), and semicolon-separated (;) commands. Below is a detailed explanation of the runcmd function, which is the core of the program, and how it processes commands.

How execve Works

execve replaces the current process image with a new process image. This means:
If execve is successful, the current process stops executing the code after the execve call and starts running the new program specified by argv[0].
If execve fails (e.g., the command is not found or cannot be executed), it returns -1, and the program continues executing the code after the execve call. This is why the error message ("error: cannot execute") is printed immediately after the execve call.
How runcmd Works

Handling the cd Command
The first check in runcmd is: if (!strcmp(argv[0], "cd")) { cd(argc, argv); return; }
If the command is cd, it calls the cd function and immediately returns. This is because cd is a shell built-in command and cannot be executed as an external program using execve.
Handling Pipes
If the command involves a pipe (has_pipe is 1), the function creates a pipe using pipe(fd) and forks a child process using fork(): if ((has_pipe && pipe(fd) == -1) || (pid = fork()) == -1) { prerr("error: fatal\n"); exit(1); }
pipe(fd): Creates a pipe with two file descriptors:
fd[0]: The read end of the pipe.
fd[1]: The write end of the pipe.
fork(): Creates a child process. The parent process continues executing after the fork() call, while the child process starts executing from the same point.
Child Process Logic
In the child process (if (!pid)), the function sets up the pipe if has_pipe is 1: if (has_pipe && (dup2(fd[1], 1) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1)) { prerr("error: fatal\n"); exit(1); }

dup2(fd[1], 1): Redirects the standard output (stdout) to the write end of the pipe (fd[1]).
close(fd[0]) and close(fd[1]): Closes the unused ends of the pipe in the child process to avoid resource leaks.
After setting up the pipe, the child process calls execve to execute the command: execve(argv[0], argv, envp); prerr("error: cannot execute "); prerr(argv[0]); prerr("\n"); exit(1);

If execve succeeds, the child process is replaced by the new program and does not execute any further code.
If execve fails, an error message is printed, and the child process exits with an error code.
Parent Process Logic
The parent process waits for the child process to finish using waitpid: waitpid(pid, NULL, 0);

This ensures that the parent process does not proceed until the child process has completed.
If has_pipe is 1, the parent process sets up the pipe for the next command: if (has_pipe && (dup2(fd[0], 0) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1)) { prerr("error: fatal\n"); exit(1); }

dup2(fd[0], 0): Redirects the standard input (stdin) to the read end of the pipe (fd[0]).
close(fd[0]) and close(fd[1]): Closes the unused ends of the pipe in the parent process.
Flow for a Simple Command

The runcmd function is called with has_pipe = 0.
The function checks if the command is cd. If not, it forks a child process.
In the child process:
No pipe setup is needed.
The command is executed using execve.
In the parent process:
The parent waits for the child process to finish using waitpid.
Flow for a Piped Command

The runcmd function is called with has_pipe = 1.
The function creates a pipe and forks a child process.
In the child process:
The standard output is redirected to the write end of the pipe.
The command is executed using execve.
In the parent process:
The parent waits for the child process to finish using waitpid.
The standard input is redirected to the read end of the pipe for the next command.

Example Walkthrough

Input: ./microshell /bin/ls "|" /usr/bin/grep microshell ";" /bin/echo i love my microshell

Execution Steps:

First Command (/bin/ls |):

runcmd is called with argv = ["/bin/ls"] and has_pipe = 1.
A pipe is created, and a child process is forked.
In the child process:
stdout is redirected to the pipe.
/bin/ls is executed using execve.
In the parent process:
The parent waits for /bin/ls to finish.
stdin is redirected to the pipe for the next command.

Second Command (/usr/bin/grep microshell ;):

runcmd is called with argv = ["/usr/bin/grep", "microshell"] and has_pipe = 0.
A child process is forked.
In the child process:
stdin is already redirected to the pipe from the previous command.
/usr/bin/grep microshell is executed using execve.
In the parent process:
The parent waits for /usr/bin/grep to finish.

Third Command (/bin/echo i love my microshell):

runcmd is called with argv = ["/bin/echo", "i", "love", "my", "microshell"] and has_pipe = 0.
A child process is forked.
In the child process:
/bin/echo is executed using execve.
In the parent process:
The parent waits for /bin/echo to finish.
Let me know if you need further clarification!
