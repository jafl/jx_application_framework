#include <JKLRand.h>
#include <JString.h>
#include <JArray.h>
#include <jFStreamUtil.h>
#include <jCommandLine.h>
#include <stdio.h>

// Prototypes

void TestRndInt();
void TestRndFloat();

	// Create a generator
	JKLRand jRand;

int
main()
{
	#ifdef __SC__
		_ftype = 'TEXT';		// request text file -- Think C specific
	#endif

	long testType;
	cout << "Integer(0) or float(1): ";
	cin >> testType;
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

	cout << "Minimum: ";
	cin >> min;

	cout << "Maximum: ";
	cin >> max;

	cout << "# of samples: ";
	cin >> sampleCount;

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
	cout << "Save histogram in: ";
	cin >> histogramFileName;
	JInputFinished();

	// write histogram as gnuplot file

	ofstream histogramFile(histogramFileName);

	for (i=1;i<=binCount;i++)
		{
		histogramFile << i << ' ' << histogram.GetElement(i) << endl;
		}
}

void
TestRndFloat()
{
	// get minimum, maximum, number of samples

	long   sampleCount;
	double min,max;

	cout << "Minimum: ";
	cin >> min;

	cout << "Maximum: ";
	cin >> max;

	cout << "# of samples: ";
	cin >> sampleCount;

	JInputFinished();

	// get name of data file

	JString dataFileName;
	cout << "Save values in: ";
	cin >> dataFileName;
	JInputFinished();

	// write values for gnuplot

	ofstream dataFile(dataFileName);

	for (long i=1;i<=sampleCount;i++)
		{
		dataFile << jRand.UniformDouble(min,max) << ' ' << 1 << endl;
		}
}
