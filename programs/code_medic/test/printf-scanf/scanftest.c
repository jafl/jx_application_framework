#include <stdio.h>

int
main()
{
	char monthname[256];
	printf("enter a string:");
	fflush(stdout);
	scanf("%s", &monthname);
	printf("%s\n",monthname);
	fflush(stdout);
/*	printf("abc\n"); */
	return 0;
}
