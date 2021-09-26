#include <jx-af/jcore/JKLRand.h>
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/JArray.h>
#include <jx-af/jcore/jFStreamUtil.h>
#include <jx-af/jcore/jCommandLine.h>
#include <stdio.h>

// Prototypes

void TestRndInt();
void TestRndFloat();

	// Create a generator
	JKLRand jRand;

int
main()
{
	long testType;
	std::cout << "Integer(0) or float(1): ";
	std::cin >> testType;
	JInputFinished();

	if (testType == 0)
	{
		TestRndInt();
	}
	else if (testType == 1)
	{
		TestRndFloat();
	}

	return 0;
}

void
TestRndInt()
{
	// get minimum, maximum, number of samples

	long min,max,sampleCount;

	std::cout << "Minimum: ";
	std::cin >> min;

	std::cout << "Maximum: ";
	std::cin >> max;

	std::cout << "# of samples: ";
	std::cin >> sampleCount;

	JInputFinished();

	// create histogram

	const long  binCount = max - min + 1;
	JArray<long> histogram;

	long i;
	for (i=1;i<=binCount;i++)
	{
		histogram.AppendElement(0);
	}

	// generate samples and build histogram

	for (i=1;i<=sampleCount;i++)
	{
		const long bin   = jRand.UniformLong(min,max) - min + 1;
		const long count = histogram.GetElement(bin) + 1;
		histogram.SetElement(bin, count);
	}

	// get name of histogram data file

	JString histogramFileName;
	std::cout << "Save histogram in: ";
	std::cin >> histogramFileName;
	JInputFinished();

	// write histogram as gnuplot file

	std::ofstream histogramFile(histogramFileName);

	for (i=1;i<=binCount;i++)
	{
		histogramFile << i << ' ' << histogram.GetElement(i) << std::endl;
	}
}

void
TestRndFloat()
{
	// get minimum, maximum, number of samples

	long   sampleCount;
	double min,max;

	std::cout << "Minimum: ";
	std::cin >> min;

	std::cout << "Maximum: ";
	std::cin >> max;

	std::cout << "# of samples: ";
	std::cin >> sampleCount;

	JInputFinished();

	// get name of data file

	JString dataFileName;
	std::cout << "Save values in: ";
	std::cin >> dataFileName;
	JInputFinished();

	// write values for gnuplot

	std::ofstream dataFile(dataFileName);

	for (long i=1;i<=sampleCount;i++)
	{
		dataFile << jRand.UniformDouble(min,max) << ' ' << 1 << std::endl;
	}
}
