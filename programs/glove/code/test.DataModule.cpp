#include <glStdInc.h>
#include <unistd.h>
#include <jFStreamUtil.h>
#include "GloveModule.h"

main
	(
	int argc,
	char *argv[]
	)
{
	cout << kGloveRequiresNoData << " " << kGloveDataDump << " " ;
	cout << 2 << " ";
	cout << flush;
	int i;
	for (i=0;i<=10;i++)
		{
		cout << kGloveOK << " " << i << " " << i*2 << " ";
		cout << flush;
		usleep(25000000);
		}
	cout << kGloveFinished;
}
