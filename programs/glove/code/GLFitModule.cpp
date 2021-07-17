/*******************************************************************************
 GLFitModule.cpp

	GLFitModule class.
 
	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/
 
#include "GLFitModule.h"
#include "GLPlotDir.h"
#include "JPlotDataBase.h"
#include "GloveModule.h"
#include "GLPlotModuleFit.h"
#include "GLPlotter.h"
#include "GLVarList.h"

#include <JXApplication.h>
#include <JXDeleteObjectTask.h>
#include <jXGlobals.h>

#include <JOutPipeStream.h>
#include <JCreateProgressDisplay.h>
#include <JProcess.h>
#include <JExprParser.h>
#include <JStringIterator.h>
#include <jProcessUtil.h>
#include <jStreamUtil.h>
#include <jAssert.h>

const int kASCIIZero = 48;

/*********************************************************************************
 Create
 

 ********************************************************************************/

bool
GLFitModule::Create
	(
	GLFitModule** 	module,
	GLPlotDir* 		dir,
	JPlotDataBase* 	fitData,
	const JString& 	sysCmd
	)
{
	int inFD;
	int outFD;
	JProcess* process;
	JError err = 
			JProcess::Create(&process, sysCmd,
							kJCreatePipe, &outFD,
							kJCreatePipe, &inFD,
							kJIgnoreConnection, nullptr);						
	if (err.OK())
		{
		JOutPipeStream* op = jnew JOutPipeStream(outFD, true);
		assert( op != nullptr );
		assert( op->good() );
		*module = jnew GLFitModule(dir, fitData, process, inFD, op);
		return true;
		}
		
	return false;
}

/*******************************************************************************
 Constructor (protected)
 

 ******************************************************************************/

GLFitModule::GLFitModule
	(
	GLPlotDir* 		dir, 
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
	assert(itsLink != nullptr);
//	itsLink->set_hanle(input);
	ListenTo(itsLink);
//	itsProcessInput = jnew JIPCLine(input, true);
//	assert(itsProcessInput != nullptr);
//	ListenTo(itsProcessInput);
	itsNames = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert(itsNames != nullptr);
	itsValues = jnew JArray<JFloat>;
	assert(itsValues != nullptr);
	itsStatusRead = false;
	itsHeaderRead = false;
	itsFunctionRead = false;
	itsPG = nullptr;
	
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
	*output << type << std::endl;
	J2DPlotWidget* plot = itsDir->GetPlot();
	bool usingRange = plot->IsUsingRange();
	JFloat xmax, xmin, ymax, ymin;
	JSize validcount = 0;
	JArray<bool> valid;
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
				valid.AppendElement(true);
				validcount++;
				}
			else
				{
				valid.AppendElement(false);
				}
			}
		}
	else
		{
		validcount = count;
		}
	*output << validcount << std::endl;
	for (i = 1; i <= count; i++)
		{
		J2DDataPoint point;
		itsData->GetElement(i, &point);
		bool pointOk = true;
		if (usingRange && !valid.GetElement(i))
			{
			pointOk = false;
			}
		if (pointOk)
			{
			std::cout << point.x << ' ' << point.y;
			if (itsData->HasYErrors())
				{
				std::cout << " " << point.yerr;
				}
			if (itsData->HasXErrors())
				{
				std::cout << " " << point.xerr;
				}
			std::cout << std::endl;
			}
		}
	jdelete output;
}

/*******************************************************************************
 Destructor
 

 ******************************************************************************/

GLFitModule::~GLFitModule()
{
	jdelete itsProcess;
	delete itsLink;
	if (itsPG != nullptr)
		{
		itsPG->ProcessFinished();
		jdelete itsPG;
		}
}

/*******************************************************************************
 Receive
 

 ******************************************************************************/

void
GLFitModule::Receive
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
GLFitModule::HandleInput
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
		itsFunction = istr;
		itsFunction.TrimWhitespace();
		itsFunctionRead = true;
		}
	else if (itsStatusRead)
		{
		std::string s(str.GetRawBytes(), str.GetByteCount());
		std::istringstream iss(s);
		iss >> itsParmsCount;
		iss >> JBoolFromString(itsHasErrors);
		iss >> JBoolFromString(itsHasGOF);
		itsHeaderRead = true;
		}
	else
		{
		JUtf8Byte c = str.GetRawBytes()[0];
		int val = c - kASCIIZero;
		if (val == kGloveFail)
			{
			JStringIterator iter(&str);
			iter.SkipNext(2);
			iter.RemoveAllPrev();
			iter.Invalidate();

			str.Prepend(JGetString("Error::GLDataModule"));
			JGetUserNotification()->ReportError(str);
			JXDeleteObjectTask<GLFitModule>::Delete(this);
			}
		else if (val == kGloveOK)
			{
			itsStatusRead = true;
			}
		else
			{
			JGetUserNotification()->ReportError(JGetString("UnknownError::GLFitModule"));
			JXDeleteObjectTask<GLFitModule>::Delete(this);
			}
		}
}

/*******************************************************************************
 HandleDataRead
 

 ******************************************************************************/

void
GLFitModule::HandleDataRead
	(
	JString& str
	)
{
	if (itsPG == nullptr)
		{
		itsPG = JGetCreatePG()->New();
		itsPG->VariableLengthProcessBeginning(
			JGetString("Loading::DateModule"), true, true);
		}
	std::string s(str.GetRawBytes(), str.GetByteCount());
	std::istringstream iss(s);
	JString* instr = jnew JString();
	iss >> *instr;
	JFloat value;
	iss >> value;
	itsValues->AppendElement(value);
	itsNames->Append(instr);
	const bool keepGoing = itsPG->IncrementProgress();
	if (!keepGoing)
		{
		JXDeleteObjectTask<GLFitModule>::Delete(this);
		return;
		}
}

/*******************************************************************************
 HandleFit
 

 ******************************************************************************/

void
GLFitModule::HandleFit()
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
		JGetUserNotification()->ReportError(JGetString("UnknownError::GLFitModule"));
		JXDeleteObjectTask<GLFitModule>::Delete(this);
		return;;
		}
	else
		{
		GLVarList* list = jnew GLVarList;
		list->AddVariable(JGetString("DefaultVarName::GLGlobal"), 0);
		for (JSize i = 1; i <= itsParmsCount; i++)
			{
			JSize index = i;
			if (itsHasErrors)
				{
				index = i * 2 - 1;
				}
			JString parm(*(itsNames->GetElement(index)));
			JFloat value = itsValues->GetElement(index);
			list->AddVariable(parm, value);
			}

		JExprParser p(list);

		JFunction* f;
		if (p.Parse(itsFunction, &f))
			{
			JFloat xmax, xmin, ymax, ymin;
			itsDir->GetPlot()->GetRange(&xmin, &xmax, &ymin, &ymax);
			GLPlotModuleFit* fit = 
				jnew GLPlotModuleFit(itsDir->GetPlot(), itsData, xmin, xmax, 
					itsNames, itsValues, f, list, itsParmsCount, 
					itsHasErrors, itsHasGOF);
			assert(fit != nullptr);
			if (!(itsDir->AddFitModule(fit, itsData)))
				{
				jdelete fit;
				JGetUserNotification()->ReportError(JGetString("CannotAddFit::GLFitModule"));
				}
			}
		else
			{
			JGetUserNotification()->ReportError(JGetString("ParseFailed::GLFitModule"));
			}
		}
	JXDeleteObjectTask<GLFitModule>::Delete(this);
	return;
}
