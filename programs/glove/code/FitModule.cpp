/*******************************************************************************
 FitModule.cpp

	FitModule class.
 
	Copyright @ 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/
 
#include "FitModule.h"
#include "PlotDir.h"
#include "JPlotDataBase.h"
#include "GloveModule.h"
#include "JPlotModuleFit.h"
#include "GlovePlotter.h"
#include <DeleteFitModTask.h>
#include "GVarList.h"

#include <JXApplication.h>
#include <jXGlobals.h>

#include <JOutPipeStream.h>
#include <JCreateProgressDisplay.h>
#include <JProcess.h>
#include <JFunction.h>

#include <jProcessUtil.h>
#include <jStreamUtil.h>
#include <jParseFunction.h>
#include <sstream>
#include <jAssert.h>

#undef new
#undef delete

const int kASCIIZero = 48;

/*********************************************************************************
 Create
 

 ********************************************************************************/

JBoolean
FitModule::Create
	(
	FitModule** 		module,
	PlotDir* 			dir, 
	JPlotDataBase* 		fitData,
	const JCharacter* 	sysCmd
	)
{
	int inFD;
	int outFD;
	JProcess* process;
	JError err = 
			JProcess::Create(&process, sysCmd,
							kJCreatePipe, &outFD,
							kJCreatePipe, &inFD,
							kJIgnoreConnection, NULL);						
	if (err.OK())
		{
		JOutPipeStream* op = new JOutPipeStream(outFD, kJTrue);
		assert( op != NULL );
		assert( op->good() );
		*module = new FitModule(dir, fitData, process, inFD, op);
		return kJTrue;
		}
		
	return kJFalse;
}

/*******************************************************************************
 Constructor (protected)
 

 ******************************************************************************/

FitModule::FitModule
	(
	PlotDir* 		dir, 
	JPlotDataBase* 	fitData,
	JProcess* 		process, 
	const int		fd,
	JOutPipeStream* output
	)
{
	itsDir = dir;
	itsData = fitData;
	itsProcess = process;
	ListenTo(itsProcess);
	
	itsLink = new ProcessLink(fd);
//	itsLink->set_hanle(input);
	assert(itsLink != NULL);
	ListenTo(itsLink);
//	itsProcessInput = new JIPCLine(input, kJTrue);
//	assert(itsProcessInput != NULL);
//	ListenTo(itsProcessInput);
	itsNames = new JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert(itsNames != NULL);
	itsValues = new JArray<JFloat>;
	assert(itsValues != NULL);
	itsFunction = new JString();
	assert (itsFunction != NULL);
	itsStatusRead = kJFalse;
	itsHeaderRead = kJFalse;
	itsFunctionRead = kJFalse;
	itsPG = NULL;
	
	JIndex type;
	if (itsData->HasXErrors())
		{
		if (itsData->HasYErrors())
			{
			type = kGloveXYdYdX;
			}
		else
			{
			type = kGloveXYdX;
			}		
		}
	else
		{
		if (itsData->HasYErrors())
			{
			type = kGloveXYdY;
			}
		else
			{
			type = kGloveXY;
			}		
		}
	*output << type << endl;
	J2DPlotWidget* plot = itsDir->GetPlot();
	JBoolean usingRange = plot->IsUsingRange();
	JFloat xmax, xmin, ymax, ymin;
	JSize validcount = 0;
	JArray<JBoolean> valid;
	JSize count = itsData->GetElementCount();
	JSize i;
	if (usingRange)
		{
		plot->GetRange(&xmin, &xmax, &ymin, &ymax);
		for (i = 1; i <= count; i++)
			{
			J2DDataPoint point;
			itsData->GetElement(i, &point);
			if ((point.x >= xmin) &&
				(point.x <= xmax) &&
				(point.y >= ymin) &&
				(point.y <= ymax))
				{
				valid.AppendElement(kJTrue);
				validcount++;
				}
			else
				{
				valid.AppendElement(kJFalse);
				}
			}
		}
	else
		{
		validcount = count;
		}
	*output << validcount << endl;
	for (i = 1; i <= count; i++)
		{
		J2DDataPoint point;
		itsData->GetElement(i, &point);
		JBoolean pointOk = kJTrue;
		if (usingRange && !valid.GetElement(i))
			{
			pointOk = kJFalse;
			}
		if (pointOk)
			{
			cout << point.x;
			cout << " " << point.y;
			if (itsData->HasYErrors())
				{
				cout << " " << point.yerr;
				}
			if (itsData->HasXErrors())
				{
				cout << " " << point.xerr;
				}
			cout << endl;
			}
		}
	delete output;
}

/*******************************************************************************
 Destructor
 

 ******************************************************************************/

FitModule::~FitModule()
{
	delete itsProcess;
	delete itsLink;
	if (itsPG != NULL)
		{
		itsPG->ProcessFinished();
		delete itsPG;
		}
}

/*******************************************************************************
 Receive
 

 ******************************************************************************/

void
FitModule::Receive
	(
	JBroadcaster* sender,
	const JBroadcaster::Message& message
	)
{
	if (message.Is(JProcess::kFinished))
		{
		HandleFit();
		}

	else if (sender == itsLink && message.Is(JMessageProtocolT::kMessageReady))
		{
		if (itsLink->HasMessages())
			{
			JSize count = itsLink->GetMessageCount();
			for (JSize i = 1; i <= count; i++)
				{
				JString str;
				if (itsLink->GetNextMessage(&str))
					{
					HandleInput(str);
					}
				}
			}
		}

}

/*******************************************************************************
 HandleInput
 

 ******************************************************************************/

void
FitModule::HandleInput
	(
	JString& istr
	)
{
	JString str(istr);
	if (itsFunctionRead)
		{
		HandleDataRead(str);
		}
	else if (itsHeaderRead)
		{
		*itsFunction = istr;
		itsFunction->TrimWhitespace();
		itsFunctionRead = kJTrue;
		}
	else if (itsStatusRead)
		{
		std::string s(str);
		std::istringstream iss(s);
		iss >> itsParmsCount;
		iss >> itsHasErrors;
		iss >> itsHasGOF;
		itsHeaderRead = kJTrue;
		}
	else
		{
		JCharacter c = str.GetCharacter(1);
		int val = c - kASCIIZero;
		if (val == kGloveFail)
			{
			str.RemoveSubstring(1,2);
			str.Prepend("Module error: ");
			JGetUserNotification()->ReportError(str);
			DeleteFitModTask* dft = new DeleteFitModTask(this);
			assert(dft != NULL);
			dft->Go();
			}
		else if (val == kGloveOK)
			{
			itsStatusRead = kJTrue;
			}
		else
			{
			JGetUserNotification()->ReportError("Unknown module error");
			DeleteFitModTask* dft = new DeleteFitModTask(this);
			assert(dft != NULL);
			dft->Go();
			}
		}
}

/*******************************************************************************
 HandleDataRead
 

 ******************************************************************************/

void
FitModule::HandleDataRead
	(
	JString& str
	)
{
	if (itsPG == NULL)
		{
		itsPG = JGetCreatePG()->New();
		itsPG->VariableLengthProcessBeginning(
			"Loading data...", kJTrue, kJTrue);
		}
	std::string s(str);
	std::istringstream iss(s);
	JString* instr = new JString();
	iss >> *instr;
	JFloat value;
	iss >> value;
	itsValues->AppendElement(value);
	itsNames->Append(instr);
	const JBoolean keepGoing = itsPG->IncrementProgress();
	if (!keepGoing)
		{
		DeleteFitModTask* dft = new DeleteFitModTask(this);
		assert(dft != NULL);
		dft->Go();
		return;
		}
}

/*******************************************************************************
 HandleFit
 

 ******************************************************************************/

void
FitModule::HandleFit()
{
	JSize count = itsParmsCount;
	if (itsHasErrors)
		{
		count = count * 2;
		}
	if (itsHasGOF)
		{
		count = count + 1;
		}
	JSize realcount = itsNames->GetElementCount();
	if (realcount != count)
		{
		JGetUserNotification()->ReportError("Unknown module error.");
		DeleteFitModTask* dft = new DeleteFitModTask(this);
		assert(dft != NULL);
		dft->Go();
		return;;
		}
	else
		{
		GVarList* list = new GVarList;
		list->AddVariable("x", 0);
		for (JSize i = 1; i <= itsParmsCount; i++)
			{
			JSize index = i;
			if (itsHasErrors)
				{
				index = i * 2 - 1;
				}
			JString parm(*(itsNames->NthElement(index)));
			JFloat value = itsValues->GetElement(index);
			JBoolean added = list->AddVariable(parm, value);
			}
		JString function(*itsFunction);
		JFunction* f;
		if (JParseFunction(function, list, &f))
			{
			JFloat xmax, xmin, ymax, ymin;
			itsDir->GetPlot()->GetRange(&xmin, &xmax, &ymin, &ymax);
			JPlotModuleFit* fit = 
				new JPlotModuleFit(itsDir->GetPlot(), itsData, xmin, xmax, 
					itsNames, itsValues, f, list, itsParmsCount, 
					itsHasErrors, itsHasGOF);
			assert(fit != NULL);
			if (!(itsDir->AddFitModule(fit, itsData)))
				{
				delete fit;
				JGetUserNotification()->ReportError("Fit could not be added.");
				}
			}
		else
			{
			JGetUserNotification()->ReportError("Function could not be parsed.");
			}
		}
	DeleteFitModTask* dft = new DeleteFitModTask(this);
	assert(dft != NULL);
	dft->Go();
	return;
}
