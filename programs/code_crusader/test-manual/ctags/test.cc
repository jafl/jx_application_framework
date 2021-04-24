#include <iostream.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char* testStr = "";

int
main
	(
	int		argc,
	char**	argv
	)
{
	const char* arg[] =
		{
		"ctags",
		"--filter=yes",
		"--filter-terminator=\f",
		"--fields=kzafims",
		"--format=1",
		"--excmd=number",
		"--sort=no",
		"--extra=q",
//		"--c-types=f",
		nullptr
		};

	int toFD[2], fromFD[2];
	pipe(toFD);
	pipe(fromFD);

	const pid_t pid = fork();
	if (pid == 0)	// child
		{
		dup2(toFD[0], fileno(stdin));
		close(toFD[0]);
		close(toFD[1]);

		dup2(fromFD[1], fileno(stdout));
		close(fromFD[0]);
		close(fromFD[1]);

		execvp(arg[0], const_cast<char* const*>(arg));
		}

	else			// parent
		{
		const char* langCmd = "--language-force=c++\n";
		write(toFD[1], langCmd, strlen(langCmd));

		const char* fileName = "test.cc\n";
		write(toFD[1], fileName, strlen(fileName));

		char c;
		while (1)
			{
			if (read(fromFD[0], &c, 1) == 1)
				{
				if (c == '\f')
					{
					break;
					}
				std::cout << c;
				}
			}

		close(toFD[1]);
		close(fromFD[0]);
		}

	return 0;
}

void
foo()
{
	printf(testStr);
}
