// picoshell.c

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
int	picoshell(char	**cmds[])
{
	int		fd[2];
	int		i;
	int		prev;
	pid_t	pid;

	fd[0] = -1;
	fd[1] = -1;
	i = -1;
	prev = -1;
	while (cmds[++i])
	{
		if (cmds[i + 1] != NULL)
		{
			pipe(fd);
		}
		pid = fork();
		if (pid < 0)
		{
			close_fd(&fd[0], &fd[1]);
			return (1);
		}
		else if (pid == 0)
		{
			if (prev != -1)
			{
				dup2(prev, 0);
				close(prev);
			}
			if (cmds[i + 1] != NULL)
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
		if (cmds[i + 1] != NULL)
		{
			prev = fd[0];
		}
		close(fd[1]);
	}
	while (wait(NULL) > 0);
	if (prev <= 0)
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