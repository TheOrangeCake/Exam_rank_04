/*
Assignment name        :    sandbox
Expected files        :    sandbox.c
Allowed functions    :    fork, waitpid, exit, alarm, sigaction, kill,
                        printf, strsignal, errno
===============================================================================

Write the following function:

#include <stdbool.h>
int    sandbox(void (*f)(void), unsigned int timeout, bool verbose)

This function must test if the function f is a nice function or a bad function,
you will return 1 if f is nice , 0 if f is bad or -1 in case of an error in
your function.

A function is considered bad if it is terminated or stopped by a signal
(segfault, abort...), if it exit with any other exit code than 0 or if it
times out.

If verbose is true, you must write the appropriate message among the following:

"Nice function!\n"
"Bad function: exited with code <exit_code>\n"
"Bad function: <signal description>\n"
"Bad function: timed out after <timeout> seconds\n"

You must not leak processes (even in zombie state, this will be checked using
wait).

We will test your code with very bad functions.
*/
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

pid_t	g_pid;
volatile sig_atomic_t g_timeout;

void	reset_signals(struct sigaction *sa_default)
{
	sigaction(SIGTERM, sa_default, NULL);
	sigaction(SIGQUIT, sa_default, NULL);
	sigaction(SIGINT, sa_default, NULL);
	sigaction(SIGALRM, sa_default, NULL);
}

void	alarm_handler(int signal)
{
	if (signal == SIGALRM)
	{
		g_timeout = 1;
		if (g_pid > 0)
			kill(g_pid, SIGKILL);
	}
}

int    sandbox(void (*f)(void), unsigned int timeout, bool verbose)
{
	struct sigaction sa_default;
	struct sigaction sa_ignore;
	struct sigaction sa_alarm;
	int code;
	int status;

	sigemptyset(&sa_default.sa_mask);
	sa_default.sa_flags = SA_RESTART;
	sa_default.sa_handler = SIG_DFL;
	sigemptyset(&sa_ignore.sa_mask);
	sa_ignore.sa_flags = SA_RESTART;
	sa_ignore.sa_handler = SIG_IGN;
	sigemptyset(&sa_alarm.sa_mask);
	sa_alarm.sa_flags = SA_RESTART;
	sa_alarm.sa_handler = alarm_handler;
	if (sigaction(SIGTERM, &sa_ignore, NULL) < 0 
	|| sigaction(SIGQUIT, &sa_ignore, NULL) < 0
	|| sigaction(SIGINT, &sa_ignore, NULL) < 0)
		return (reset_signals(&sa_default), -1);
	g_pid = fork();
	if (g_pid < 0)
		return (reset_signals(&sa_default), -1);
	else if (g_pid == 0)
	{
		if (sigaction(SIGTERM, &sa_default, NULL) < 0
		|| sigaction(SIGQUIT, &sa_default, NULL) < 0
		|| sigaction(SIGINT, &sa_default, NULL) < 0)
			exit(1);
		(*f)();
		exit(0);
	}
	g_timeout = 0;
	if (sigaction(SIGALRM, &sa_alarm, NULL) < 0)
		return (reset_signals(&sa_default), -1);
	alarm(timeout);
	if (waitpid(g_pid, &status, 0) < 0)
		return (reset_signals(&sa_default), -1);
	alarm(0);
	if (g_timeout == 1)
	{
		if (verbose)
			printf("Bad function: timed out after %d seconds\n", timeout);
		return (reset_signals(&sa_default), 0);
	}
	if (WIFEXITED(status))
	{
		code = WEXITSTATUS(status);
		if (!code)
		{
			if (verbose)
				printf("Nice function!\n");
			return (reset_signals(&sa_default), 1);
		}
		else
		{
			if (verbose)
				printf("Bad function: exited with code %d\n", code);
			return (reset_signals(&sa_default), 0);
		}
	}
	if (WIFSIGNALED(status))
	{
		if (verbose)
			printf("Bad function: %s\n", strsignal(WTERMSIG(status)));
		return (reset_signals(&sa_default), 0);
	}
	return (reset_signals(&sa_default), -1);
}

static void	nice_exit()
{
	exit(0);
}

static void	bad_exit()
{
	exit (3);
}

static void	timeout_test()
{
	while (1)
	{
		printf("timeout_test function running...\n");
		sleep(1);
	}
}

static void	sigsegv_test()
{
	char	*str = NULL;
	str[0] = '0';
}

static void	sigabrt_test()
{
	abort();
}

int	main()
{
	sandbox(&nice_exit, 5, 1);
	sandbox(&bad_exit, 5, 1);
	sandbox(&timeout_test, 5, 1);
	sandbox(&sigsegv_test, 5, 1);
	sandbox(&sigabrt_test, 5, 1);
	return (0);
}