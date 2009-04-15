#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

int
main
	(
	int argc,
	char** argv
	)
{
	struct stat info;
	int result = stat("code/test_stat.c", &info);
	printf("%d %o\n", result, info.st_mode);
	result = stat("/usr/include/limits.h", &info);
	printf("%d %o\n", result, info.st_mode);
	result = stat("test_stat", &info);
	printf("%d %o\n", result, info.st_mode);
	result = stat("../Make.header", &info);
	printf("%d %o\n", result, info.st_mode);
	return 0;
}
