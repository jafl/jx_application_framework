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
		std::cerr << "Usage: testfstat file_name" << std::endl;
		return 1;
		}

	const int fd = open(argv[1], O_RDWR | O_CREAT);

	std::cout << argv[1] << " created" << std::endl;

	struct stat stbuf;
	while (1)
		{
		std::cout << std::endl << "Press return to continue...";
		std::cin.clear();
		while (std::cin.get() != '\n') { };

		if (fstat(fd, &stbuf) == -1)
			{
			std::cout << "Lost connection to file" << std::endl;
			return 0;
			}
		else
			{
			FILE* f = fdopen(fd, "r");
			if (fseek(f, 0, SEEK_SET) == 0)
				{
				std::cout << std::endl;
				std::cout << "File contents:" << std::endl;
				std::cout << std::endl;

				int c;
				while ((c = fgetc(f)) != EOF)
					{
					std::cout << (char) c;
					}

				std::cout << std::endl;
				}
			else
				{
				std::cerr << "fseek() failed" << std::endl;
				return 0;
				}
			}
		}
}
