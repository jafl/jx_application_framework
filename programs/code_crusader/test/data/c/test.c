#include <stdio.h>

#if 0	// comment
abc
#endif

# if 1
// nothing
 #elif 0 /* comment */
abc
	# if abc
	123
	# elif defined xyz
	456
	# endif
# endif

int
main(int argc, char *argv[])
{
	printf("abcd\n");
	foo('\n', 'g', '\x1b');
	return 0;
}
