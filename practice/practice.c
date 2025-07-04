//popen

#include <stdlib.h>
#include <unistd.h>

int    ft_popen(const char *file, char *const argv[], char type)
{
	int		fd[2];
	pid_t	pid;

	if (!file || !argv)
		return (-1);
	if (pipe(fd) < 0)
		return (-1);
	if (type == 'r')
	{
		pid = fork();
		if (pid < 0)
			return (-1);
		if (pid == 0)
		{
			dup2(fd[1], 1);
			close(fd[1]);
			close(fd[0]);
			execvp(file, argv);
			exit(-1);
		}
		close(fd[0]);
		return (fd[1]);
	}
	else if (type == 'w')
	{
		pid = fork();
		if (pid < 0)
			return (-1);
		if (pid == 0)
		{
			dup2(fd[0], 0);
			close(fd[1]);
			close(fd[0]);
			execvp(file, argv);
			exit(-1);
		}
		close(fd[1]);
		return (fd[0]);
	}
	close(fd[0]);
	close(fd[1]);
	return (-1);
}

int main() {
    int fd = ft_popen("ls", (char *const[]){"ls", NULL}, 'r');

    // charline;
    // while(line = get_next_line(fd))
    //     ft_putstr(line);
}