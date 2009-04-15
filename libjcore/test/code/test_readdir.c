#include <stdio.h>
#include <dirent.h>

int
main
	(
	int argc,
	char** argv
	)
{
struct dirent* direntry;

	DIR* dir = opendir(argv[1]);
	if (dir == NULL)
		{
		printf("opendir() failed\n");
		return 1;
		}

	while ((direntry = readdir(dir)) != NULL)
		{
		printf("%s\n", direntry->d_name);
		}

	return 0;
}
