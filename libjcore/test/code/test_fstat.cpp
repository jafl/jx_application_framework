#include <fcntl.h>
#include <stdio.h>
#include <iostream.h>
#include <sys/stat.h>

int main
	(
	int		argc,
	char**	argv
	)
{
	if (argc != 2)
		{
		cerr << "Usage: testfstat file_name" << endl;
		return 1;
		}

	const int fd = open(argv[1], O_RDWR | O_CREAT);

	cout << argv[1] << " created" << endl;

	struct stat stbuf;
	while (1)
		{
		cout << endl << "Press return to continue...";
		cin.clear();
		while (cin.get() != '\n') { };

		if (fstat(fd, &stbuf) == -1)
			{
			cout << "Lost connection to file" << endl;
			return 0;
			}
		else
			{
			FILE* f = fdopen(fd, "r");
			if (fseek(f, 0, SEEK_SET) == 0)
				{
				cout << endl;
				cout << "File contents:" << endl;
				cout << endl;

				int c;
				while ((c = fgetc(f)) != EOF)
					{
					cout << (char) c;
					}

				cout << endl;
				}
			else
				{
				cerr << "fseek() failed" << endl;
				return 0;
				}
			}
		}
}
