#include <stdio.h>
#include <pthread.h>

void*
f(void* unused)
{
	int x = 2;
	int y = 3;
	printf("abc\n");
	return NULL;
}

int
main()
{
	pthread_t t1, t2, t3;
	pthread_create(&t1, NULL, f, NULL);
	pthread_create(&t2, NULL, f, NULL);
	pthread_create(&t3, NULL, f, NULL);

	sleep(2);

	printf("xyz\n");
	return 0;
}
