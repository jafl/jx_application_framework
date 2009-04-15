#include <stdio.h>
#include <sys/file.h>

int
main
	(
	int argc,
	char** argv
	)
{
char c;

	FILE* file1 = fopen("test.flock.c", "r");
	int fd1 = fileno(file1);
	int result1 = flock(fd1, LOCK_EX | LOCK_NB);
	int result2 = flock(fd1, LOCK_EX | LOCK_NB);

	printf("flock1 = %d\n", result1);
	printf("flock2 = %d\n", result2);

	FILE* file2 = fopen("test.flock.c", "r");
	int fd2 = fileno(file2);
	int result3 = flock(fd2, LOCK_EX | LOCK_NB);

	printf("flock3 = %d\n", result3);

	printf("Press return...");
	scanf("%c", &c);
}
