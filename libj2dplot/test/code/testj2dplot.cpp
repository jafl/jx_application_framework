/******************************************************************************
 testj2dplot.cpp

	This provides a way to test the J2DPlot library and provides sample
	source code for everyone to study.

	Written by John Lindal.

 ******************************************************************************/

#include "Test2DPlotDirector.h"
#include "testj2dplotStringData.h"
#include <JXApplication.h>
#include <JX2DPlotWidget.h>
#include <J2DPlotData.h>
#include <J2DVectorData.h>
#include <J2DPlotLibVersion.h>
#include <JKLRand.h>
#include <jMath.h>
#include <string.h>
#include <stdlib.h>
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

	JXApplication* app = jnew JXApplication(&argc, argv, "testj2dplot", kDefaultStringData);
	assert( app != NULL );

	Test2DPlotDirector* dataDir = jnew Test2DPlotDirector(app);
	assert( dataDir != NULL );
	TestData(dataDir);

	Test2DPlotDirector* logDataDir = jnew Test2DPlotDirector(app);
	assert( logDataDir != NULL );
	TestLogData(logDataDir);

	Test2DPlotDirector* vectorDir = jnew Test2DPlotDirector(app);
	assert( vectorDir != NULL );
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
JIndex i;

	JX2DPlotWidget* plot = dir->GetPlotWidget();
	plot->ShowLegend();

	JArray<JFloat> x(100),y(100), xPErr(101),xMErr(101), yPErr(101), yMErr(101);
	JKLRand r;

	JFloat delta = kJPi/50.0;
	for (i=0; i<=100; i++)
		{
		x.AppendElement(i*delta);
		y.AppendElement(sin(i*delta));

		xPErr.AppendElement(r.UniformDouble(0.0, 0.1));
		xMErr.AppendElement(r.UniformDouble(0.0, 0.1));
		yPErr.AppendElement(r.UniformDouble(0.0, 0.1));
		yMErr.AppendElement(r.UniformDouble(0.0, 0.1));
		}

	J2DPlotData* data;
	JBoolean ok = J2DPlotData::Create(&data, x, y, kJFalse);
	assert( ok );
	ok = data->SetXErrors(xPErr, xMErr);
	assert( ok );
	ok = data->SetYErrors(yPErr, yMErr);
	assert( ok );

	plot->AddCurve(data, kJTrue, "sin(x)", kJTrue, kJTrue);

	x.RemoveAll();
	y.RemoveAll();

	delta = kJPi/5000.0;
	for (i=0; i<=10000; i++)
		{
		x.AppendElement(i*delta);
		y.AppendElement(cos(i*delta));
		}

	plot->AddCurve(x, y, kJFalse, "cos(x)", &i, kJTrue, kJFalse);

	plot->SetTitle("Error bars");
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
JIndex i;

	JX2DPlotWidget* plot = dir->GetPlotWidget();
	plot->ShowLegend();

	JArray<JFloat> x(1000),y(1000);

	const JFloat delta = 0.1;
	for (i=1; i<=100; i++)
		{
		x.AppendElement(i*delta);
		y.AppendElement(pow(2, i*delta));
		}

	plot->AddCurve(x, y, kJFalse, "log2(x)", &i, kJTrue, kJFalse);

	x.RemoveAll();
	y.RemoveAll();

	for (i=0; i<=100; i++)
		{
		x.AppendElement(i*delta);
		y.AppendElement((i*delta)*(i*delta));
		}

	plot->AddCurve(x, y, kJFalse, "x^2", &i, kJTrue, kJFalse);

	plot->SetYScale(0.1, 1000, 0, kJFalse);
	plot->SetTitle("Logarithmic scale");
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

	J2DVectorData* data = jnew J2DVectorData;
	assert( data != NULL );

	const JFloat delta = kJPi/4.0;
	for (JIndex i=0; i<8; i++)
		{
		const JFloat x = cos(i*delta);
		const JFloat y = sin(i*delta);
		data->AddElement(0.8+x, 5.0+y, x,y);
		}

	plot->AddCurve(data, kJTrue, "vector");

	plot->SetTitle("Vectors");
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
	cout << endl;
	cout << "This X application demonstrates the capabilities of the J2DPlot" << endl;
	cout << "class library." << endl;
	cout << endl;
	cout << "Usage:  <options>" << endl;
	cout << endl;
	cout << "-h  prints help" << endl;
	cout << "-v  prints version information" << endl;
	cout << endl;
}

/******************************************************************************
 PrintVersion

 ******************************************************************************/

void
PrintVersion()
{
	cout << endl;
	cout << "J2DPlot version " << kCurrentJ2DPlotLibVersionStr << endl;
	cout << endl;
}
