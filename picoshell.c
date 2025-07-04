/* Assignment name:    picoshell
Expected files:        picoshell.c
Allowed functions:    close, fork, wait, exit, execvp, dup2, pipe
___

Write the following function:

int    picoshell(char **cmds[]);

The goal of this function is to execute a pipeline. It must execute each
commands [sic] of cmds and connect the output of one to the input of the
next command (just like a shell).

Cmds contains a null-terminated list of valid commands. Each rows [sic]
of cmds are an argv array directly usable for a call to execvp. The first
arguments [sic] of each command is the command name or path and can be passed
directly as the first argument of execvp.

If any error occur [sic], The function must return 1 (you must of course
close all the open fds before). otherwise the function must wait all child
processes and return 0. You will find in this directory a file main.c which
contain [sic] something to help you test your function.


Examples: 
./picoshell /bin/ls "|" /usr/bin/grep picoshell
picoshell
./picoshell echo 'squalala' "|" cat "|" sed 's/a/b/g'
squblblb/
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void	close_fd(int *fd1, int *fd2)
{
	if (*fd1 >= 0)
	{
		close(*fd1);
		*fd1 = -1;
	}
	if (*fd2 >= 0)
	{
		close(*fd2);
		*fd2 = -1;
	}
}

int	picoshell(char ***cmds)
{
	int		fd[2];
	int		prev;
	pid_t	pid;
	int		i;

	fd[0] = -1;
	fd[1] = -1;
	prev = -1;
	i = -1;
	while (cmds[++i] != NULL)
	{
		if (cmds[i + 1] != NULL) // if NOT last command
			pipe(fd);
		pid = fork();
		if (pid < 0)
			return (close_fd(&fd[0], &fd[1]), 1);
		if (pid == 0)
		{
			if (prev != -1) // if there is prev command
			{
				dup2(prev, 0);
				close(prev);
			}
			if (cmds[i + 1] != NULL) // if NOT last command
			{
				dup2(fd[1], 1);
			}
			close_fd(&fd[0], &fd[1]);
			execvp(cmds[i][0], cmds[i]);
			exit(1);
		}
		if (prev != -1)
		{
			close(prev);
			prev = -1;
		}
		if (cmds[i + 1] != NULL) // if NOT last command
		{
			prev = fd[0];
		}
		close(fd[1]);
	}
	while (wait(NULL) > 0);
	if (prev >= 0)
	{
		close(prev);
		prev = -1;
	}
	return (0);
}

int	main()
{
	char	*cmd1[] = {"ls", "-la", NULL};
	char	*cmd2[] = {"grep", ".c", NULL};
	char	*cmd3[] = {"wc", "-l", NULL};
	char	*cmd4[] = {"echo", "Hello World!", NULL};
	char	*cmd5[] = {"nonexisting", NULL};
	char	**pipe1[] = {cmd1, cmd2, cmd3, NULL};
	char	**pipe2[] = {cmd4, NULL};
	char	**pipe3[] = {cmd5, NULL};
	char	**pipe4[] = {cmd1, NULL};
	printf("Running pipe: ls -la | grep .c | wc -l\n");
	printf("Pipe returned: %d\n\n", picoshell(pipe1));
	printf("Running pipe: echo \"Hello World!\"\n");
	printf("Pipe returned: %d\n\n", picoshell(pipe2));
	printf("Running pipe: nonexisting\n");
	printf("Pipe returned: %d\n\n", picoshell(pipe3));
	printf("Running pipe: ls -la\n");
	printf("Pipe returned: %d\n", picoshell(pipe4));
	return (0);
}

