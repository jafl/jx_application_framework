#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

/* returns 0 if the first argument is a directory and the two directories
 * are different
 */

int
main
	(
	int		argc,
	char**	argv
	)
{
	struct stat stbuf1, stbuf2;

	if (argc == 3 &&
		stat(argv[1], &stbuf1) == 0 &&
		stat(argv[2], &stbuf2) == 0 &&
		S_ISDIR(stbuf1.st_mode) &&
		(stbuf1.st_dev != stbuf2.st_dev ||
		 stbuf1.st_ino != stbuf2.st_ino))
		{
		return 0;
		}
	else
		{
		return 1;
		}
}
