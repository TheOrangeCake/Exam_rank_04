/* Allowed functions: pipe, fork, dup2, execvp, close, exit

write the following function:

    int    ft_popen(const char *file, char *const argv[], char type)

The function must launch the executable file with the arguments argv (using execvp).
If the type is 'r' the function must return a file descriptor connected to the output of the command.
If the type is 'w' the function must return a file descriptor connected to the input of the command.
In case of error or invalid parameter the function must return -1.

example:

int main() {
    int fd = ft_popen("ls", (char *const[]){"ls", NULL}, 'r');

    charline;
    while(line = get_next_line(fd))
        ft_putstr(line);
}

Hint: Do not leak file descriptors! */

#include <unistd.h>
#include <stdlib.h>

char	*get_next_line(int fd);
void	ft_putstr_fd(char *s, int fd);

int	ft_popen(const char *file, char *const argv[], char type)
{
	pid_t	pid;
	int		fd[2];

	if (!file || !argv)
		return (-1);
	if (pipe(fd) < 0)
		return(-1);
	if (type == 'r')
	{
		pid = fork();
		if (pid < 0)
			return (close(fd[0]), close(fd[1]),-1);
		else if (pid == 0)
		{
			if (dup2(fd[1], 1) < 0)
			{
				close(fd[0]);
				close(fd[1]);
				exit(-1);
			}
			close(fd[0]);
			close(fd[1]);
			execvp(file, argv);
			exit(-1);
		}
		close(fd[1]);
		return (fd[0]);
	}
	else if (type == 'w')
	{
		pid = fork();
		if (pid < 0)
			return (close(fd[0]), close(fd[1]),-1);
		else if (pid == 0)
		{
			if (dup2(fd[0], 0) < 0)
			{
				close(fd[0]);
				close(fd[1]);
				exit(-1);
			}
			close(fd[0]);
			close(fd[1]);
			execvp(file, argv);
			exit(-1);
		}
		close(fd[0]);
		return (fd[1]);
	}
	return (close(fd[0]), close(fd[1]),-1);
}

#include <stdio.h>
int main() {
	char *const av[] = {"ls", NULL};
    int fd = ft_popen("ls", av, 'r');

    char	*line;
    while(line = get_next_line(fd))
        ft_putstr_fd(line, 1);
}

// start gnl
# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 1
# endif
# include <fcntl.h>
# include <unistd.h>
# include <stdlib.h>

void	*safe_free(char **s1)
{
	free(*s1);
	*s1 = NULL;
	return (NULL);
}

size_t	ft_strlen_line(const char *s, int *signal)
{
	size_t	i;

	i = 0;
	while (s[i])
	{
		if (s[i] == '\n')
		{
			i++;
			*signal = 1;
			return (i);
		}
		i++;
	}
	return (i);
}

char	*ft_strjoin_line(char **s1, char *s2, int *signal, size_t *buffer_mover)
{
	size_t	i;
	size_t	j;
	char	*ptr;

	ptr = malloc(ft_strlen_line(*s1, signal) + ft_strlen_line(s2, signal) + 1);
	if (ptr == NULL)
		return (safe_free(s1));
	i = 0;
	j = 0;
	while ((*s1)[i])
	{
		ptr[i] = (*s1)[i];
		i++;
	}
	while (s2[j] && s2[j] != '\n')
	{
		ptr[i++] = s2[j++];
	}
	if (s2[j] == '\n')
		ptr[i++] = s2[j++];
	ptr[i] = '\0';
	*buffer_mover = j;
	safe_free(s1);
	return (ptr);
}

char	*start(char **line, int fd, char **buffer)
{
	size_t	i;
	int		signal;
	size_t	buffer_mover;

	signal = 0;
	buffer_mover = 0;
	i = read(fd, *buffer, BUFFER_SIZE);
	if ((*buffer)[0] == '\0' && (*line)[0] == '\0' && i == 0)
	{
		safe_free(buffer);
		return (safe_free(line));
	}
	(*buffer)[i] = '\0';
	*line = ft_strjoin_line(line, *buffer, &signal, &buffer_mover);
	if (*line == NULL)
		return (safe_free(buffer));
	if (signal == 1 || i < BUFFER_SIZE)
	{
		i = 0;
		while ((*buffer)[buffer_mover])
			(*buffer)[i++] = (*buffer)[buffer_mover++];
		(*buffer)[i] = '\0';
		return (*line);
	}
	return (start(line, fd, buffer));
}

char	*end(char **line, int fd, char **buffer)
{
	int		signal;
	size_t	buffer_mover;
	int		i;

	signal = 0;
	buffer_mover = 0;
	*line = ft_strjoin_line(line, *buffer, &signal, &buffer_mover);
	if (*line == NULL)
		return (safe_free(buffer));
	if (signal == 1)
	{
		i = 0;
		while ((*buffer)[buffer_mover])
			(*buffer)[i++] = (*buffer)[buffer_mover++];
		(*buffer)[i] = '\0';
		return (*line);
	}
	else
	{
		(*buffer)[0] = '\0';
		*line = start(line, fd, buffer);
	}
	return (*line);
}

char	*get_next_line(int fd)
{
	char		*line;
	static char	*buffer[1024];

	if (fd < 0 || BUFFER_SIZE <= 0 || read(fd, 0, 0) < 0)
		return (safe_free(&(buffer[fd])));
	line = malloc(sizeof(char) * 1);
	if (line == NULL)
	{
		safe_free(&line);
		return (safe_free(&(buffer[fd])));
	}
	line[0] = '\0';
	if (buffer[fd] == NULL)
	{
		buffer[fd] = malloc(sizeof(char) * BUFFER_SIZE + 1);
		if (buffer[fd] == NULL)
			return (safe_free(&line));
		buffer[fd][0] = '\0';
		line = start(&line, fd, &(buffer[fd]));
	}
	else
		line = end(&line, fd, &(buffer[fd]));
	return (line);
}

void	ft_putstr_fd(char *s, int fd)
{
	while (*s)
	{
		write(fd, s, 1);
		s++;
	}
}
// end gnl
