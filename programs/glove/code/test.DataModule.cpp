#include <unistd.h>
#include <jFStreamUtil.h>
#include "GloveModule.h"

main
	(
	int argc,
	char *argv[]
	)
{
	std::cout << kGloveRequiresNoData << " " << kGloveDataDump << " " ;
	std::cout << 2 << " ";
	std::cout << flush;
	int i;
	for (i=0;i<=10;i++)
		{
		std::cout << kGloveOK << " " << i << " " << i*2 << " ";
		std::cout << flush;
		usleep(25000000);
		}
	std::cout << kGloveFinished;
}
