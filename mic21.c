#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

void prerr(char *str) {
	while(*str)
		write(2, str++, 1);
}

void cd(int argc, char **argv) {
	if(argc != 2)
		return(prerr("error: cd: bad arguments\n"));
	if(chdir(argv[1]) < 0)
		prerr("error: cd: cannot change to "), prerr(argv[1]), prerr("\n"), exit(1);
}

void runcmd(int argc, char **argv, char **envp, int has_pipe) {
	int fd[2];
	int pid;
	if(!strcmp(argv[0], "cd"))
		return(cd(argc, argv));
	if((has_pipe && pipe(fd) == -1) || (pid = fork()) == -1)
		prerr("error: fatal\n"), exit(1);
	if(!pid) {
		if(has_pipe && (dup2(fd[1], 1) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1))
			prerr("error: fatal\n"), exit(1);
		execve(argv[0], argv, envp);
		prerr("error: cannot execute "), prerr(argv[0]), prerr("\n"), exit(1);
	}
	waitpid(pid, NULL, 0);
	if(has_pipe && (dup2(fd[0], 0) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1))
		prerr("error: fatal\n"), exit(1);	
}

int main(int argc, char **argv, char **envp) {
	for(int i = 1; i < argc; i++) {
		int cmd = i, has_pipe = 0;
		while((argv[i] && strcmp(argv[i], ";")) && strcmp(argv[i], "|"))
			i++;
		has_pipe = (argv[i] && !strcmp(argv[i], "|"));
		argv[i] = 0;
		runcmd((i -cmd), &argv[cmd], envp, has_pipe);
	}
	return (0);
}
