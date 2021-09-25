/******************************************************************************
 testj2dplot.cpp

	This provides a way to test the J2DPlot library and provides sample
	source code for everyone to study.

	Written by John Lindal.

 ******************************************************************************/

#include "Test2DPlotDirector.h"
#include "testj2dplotStringData.h"
#include <JXApplication.h>
#include "JX2DPlotWidget.h"
#include "J2DPlotData.h"
#include "J2DVectorData.h"
#include "J2DPlotLibVersion.h"
#include <JKLRand.h>
#include <jMath.h>
#include <jGlobals.h>
#include <jAssert.h>

// Prototypes

void TestData(Test2DPlotDirector* dir);
void TestLogData(Test2DPlotDirector* dir);
void TestVector(Test2DPlotDirector* dir);

void ParseTextOptions(int* argc, char* argv[]);
void PrintHelp();
void PrintVersion();

/******************************************************************************
 main

 ******************************************************************************/

int
main
	(
	int		argc,
	char*	argv[]
	)
{
	ParseTextOptions(&argc, argv);

	auto* app = jnew JXApplication(&argc, argv, "testj2dplot", kDefaultStringData);
	assert( app != nullptr );

	auto* dataDir = jnew Test2DPlotDirector(app);
	assert( dataDir != nullptr );
	TestData(dataDir);

	auto* logDataDir = jnew Test2DPlotDirector(app);
	assert( logDataDir != nullptr );
	TestLogData(logDataDir);

	auto* vectorDir = jnew Test2DPlotDirector(app);
	assert( vectorDir != nullptr );
	TestVector(vectorDir);

	logDataDir->Activate();
	dataDir->Activate();
	vectorDir->Activate();
	app->Run();
	return 0;
}

/******************************************************************************
 TestData

	Tests data plotting in J2DPlotWidget.

 ******************************************************************************/

void
TestData
	(
	Test2DPlotDirector* dir
	)
{
	JX2DPlotWidget* plot = dir->GetPlotWidget();
	plot->ShowLegend();

	JArray<JFloat> x(100),y(100), xPErr(101),xMErr(101), yPErr(101), yMErr(101);
	JKLRand r;

	JFloat delta = kJPi/50.0;
	for (JUInt32 i=0; i<=100; i++)
	{
		x.AppendElement(i*delta);
		y.AppendElement(sin(i*delta));

		xPErr.AppendElement(r.UniformDouble(0.0, 0.1));
		xMErr.AppendElement(r.UniformDouble(0.0, 0.1));
		yPErr.AppendElement(r.UniformDouble(0.0, 0.1));
		yMErr.AppendElement(r.UniformDouble(0.0, 0.1));
	}

	J2DPlotData* data;
	bool ok = J2DPlotData::Create(&data, x, y, false);
	assert( ok );
	ok = data->SetXErrors(xPErr, xMErr);
	assert( ok );
	ok = data->SetYErrors(yPErr, yMErr);
	assert( ok );

	plot->AddCurve(data, true, JString("sin(x)", JString::kNoCopy), true, true);

	x.RemoveAll();
	y.RemoveAll();

	delta = kJPi/5000.0;
	for (JUInt32 i=0; i<=10000; i++)
	{
		x.AppendElement(i*delta);
		y.AppendElement(cos(i*delta));
	}

	JIndex i;
	plot->AddCurve(x, y, false, JString("cos(x)", JString::kNoCopy), &i, true, false);

	plot->SetTitle(JGetString("DataTitle::main"));
}

/******************************************************************************
 TestLogData

	Tests logarithmic data plotting in J2DPlotWidget.

 ******************************************************************************/

void
TestLogData
	(
	Test2DPlotDirector* dir
	)
{
	JX2DPlotWidget* plot = dir->GetPlotWidget();
	plot->ShowLegend();

	JArray<JFloat> x(1000),y(1000);

	const JFloat delta = 0.1;
	for (JUInt32 i=1; i<=100; i++)
	{
		x.AppendElement(i*delta);
		y.AppendElement(pow(2, i*delta));
	}

	JIndex i;
	plot->AddCurve(x, y, false, JString("log2(x)", JString::kNoCopy), &i, true, false);

	x.RemoveAll();
	y.RemoveAll();

	for (JUInt32 i=0; i<=100; i++)
	{
		x.AppendElement(i*delta);
		y.AppendElement((i*delta)*(i*delta));
	}

	plot->AddCurve(x, y, false, JString("x^2", JString::kNoCopy), &i, true, false);

	plot->SetYScale(0.1, 1000, 0, false);
	plot->SetTitle(JGetString("LogDataTitle::main"));
}

/******************************************************************************
 TestVector

	Tests vector plotting in J2DPlotWidget.

 ******************************************************************************/

void
TestVector
	(
	Test2DPlotDirector* dir
	)
{
	JX2DPlotWidget* plot = dir->GetPlotWidget();
	plot->ShowLegend();

	auto* data = jnew J2DVectorData;
	assert( data != nullptr );

	const JFloat delta = kJPi/4.0;
	for (JUnsignedOffset i=0; i<8; i++)
	{
		const JFloat x = cos(i*delta);
		const JFloat y = sin(i*delta);
		data->AddElement(0.8+x, 5.0+y, x,y);
	}

	plot->AddCurve(data, true, JString("vector", JString::kNoCopy));

	plot->SetTitle(JGetString("VectorTitle::main"));
}

/******************************************************************************
 ParseTextOptions

	Handle the command line options that don't require opening an X display.

	Options that don't call exit() should use JXApplication::RemoveCmdLineOption()
	so ParseXOptions() won't gag.

 ******************************************************************************/

void
ParseTextOptions
	(
	int*	argc,
	char*	argv[]
	)
{
	long index = 1;
	while (index < *argc)
	{
		if (strcmp(argv[index], "-h") == 0 ||
			strcmp(argv[index], "--help") == 0)
		{
			PrintHelp();
			exit(0);
		}
		else if (strcmp(argv[index], "-v") == 0 ||
				 strcmp(argv[index], "--version") == 0)
		{
			PrintVersion();
			exit(0);
		}
		index++;
	}
}

/******************************************************************************
 PrintHelp

 ******************************************************************************/

void
PrintHelp()
{
	std::cout << std::endl;
	std::cout << "This X application demonstrates the capabilities of the J2DPlot" << std::endl;
	std::cout << "class library." << std::endl;
	std::cout << std::endl;
	std::cout << "Usage:  <options>" << std::endl;
	std::cout << std::endl;
	std::cout << "-h  prints help" << std::endl;
	std::cout << "-v  prints version information" << std::endl;
	std::cout << std::endl;
}

/******************************************************************************
 PrintVersion

 ******************************************************************************/

void
PrintVersion()
{
	std::cout << std::endl;
	std::cout << "J2DPlot version " << kCurrentJ2DPlotLibVersionStr << std::endl;
	std::cout << std::endl;
}
