#include <jFStreamUtil.h>
#include <JString.h>
#include <JArray.h>

main
	(
	int argc,
	char *argv[]
	)
{
	if (argc = 1)
		{
		exit 0;
		}

	std::ifstream is(argv[1]);
	if (is.bad())
		{
		exit 0;
		}
		
	JCharacter line[255];
	is.getline(line,255); // Remove header comment
	
	is.getline(line,255);
	
	ostrstream iss(line); 	
	int colCount = 0;
	JArray<JFloat> values;
	JFloat value;
	
	while (iss)
		{
		value << iss;	
		values.AppendElement(value);
		colCount ++;
		}
		
	std::cout << colCount << std::endl;
	for (JSize i = 1; i <= values.GetElementCount(); i++)
		{
		std::cout << values.GetElement(i);
		}
	
	while (is)
		{
		value << is;
		std::cout << value;
		}
}	

