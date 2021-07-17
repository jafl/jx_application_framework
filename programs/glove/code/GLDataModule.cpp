/*******************************************************************************
 GLDataModule.cpp

	GLDataModule class.
 
	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/
 
#include "GLDataModule.h"
#include "GLRaggedFloatTable.h"
#include "GLRaggedFloatTableData.h"
#include "GloveModule.h"
#include "GLUndoElementsInsert.h"

#include <JXApplication.h>

#include <JOutPipeStream.h>
#include <JProcess.h>
#include <JUserNotification.h>
#include <JCreateProgressDisplay.h>

#include <JXDeleteObjectTask.h>
#include <jXGlobals.h>

#include <jProcessUtil.h>
#include <jStreamUtil.h>
#include <sstream>
#include <jAssert.h>

/*********************************************************************************
 Create

 ********************************************************************************/

bool
GLDataModule::Create
	(
	GLDataModule** module,
	GLRaggedFloatTable* table, 
	GLRaggedFloatTableData* data,
	const JString& sysCmd
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
		*module = jnew GLDataModule(table, data, process, inFD, op);
		return true;
		}
		
	return false;
}

/*******************************************************************************
 Constructor (protected)

 ******************************************************************************/

GLDataModule::GLDataModule
	(
	GLRaggedFloatTable* 	table, 
	GLRaggedFloatTableData*	data,
	JProcess* 				process, 
	const int				fd,
	JOutPipeStream* 		output
	)
{
	itsTable = table;
	itsData = data;
	ListenTo(itsData);
	itsProcess = process;
	ListenTo(itsProcess);

	itsLink = new ProcessLink(fd);
	assert(itsLink != nullptr);
	ListenTo(itsLink);

	itsOutput = output;
//	ListenTo(output);
	itsHeaderRead = false;
	itsSentData = false;
	itsPG = nullptr;
	itsCols = jnew JArray<JIndex>;
	assert(itsCols != nullptr);
}

/*******************************************************************************
 Destructor

 ******************************************************************************/

GLDataModule::~GLDataModule()
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
GLDataModule::Receive
	(
	JBroadcaster* sender,
	const JBroadcaster::Message& message
	)
{
	if (message.Is(JProcess::kFinished))
		{
		JGetUserNotification()->DisplayMessage(JGetString("Finished::GLDataModule"));
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
	else if (sender == itsData && message.Is(JTableData::kColsRemoved))
		{
		const JTableData::ColsRemoved* info =
			dynamic_cast<const JTableData::ColsRemoved*>(&message);
		assert( info != nullptr );
		JIndex fIndex	= info->GetFirstIndex();
		JIndex lIndex	= info->GetLastIndex();
		for (JIndex index = fIndex; index <= lIndex; index++)
			{
			for (JIndex i = 1; i <= itsCols->GetElementCount(); i++)
				{
				JIndex cindex = itsCols->GetElement(i);
				if (index < cindex)
					{
					itsCols->SetElement(i, cindex-1);
					}
				else if (index == cindex)
					{
					JGetUserNotification()->ReportError(JGetString("ColumnRemoved::GLDataModule"));
					JXDeleteObjectTask<GLDataModule>::Delete(this);
					return;
					}
				}
			}
		}
		
	else if (sender == itsData && message.Is(JTableData::kColsInserted))
		{
		const JTableData::ColsInserted* info =
			dynamic_cast<const JTableData::ColsInserted*>(&message);
		assert( info != nullptr );
		JIndex fIndex	= info->GetFirstIndex();
		JIndex lIndex	= info->GetLastIndex();
		for (JIndex index = fIndex; index <= lIndex; index++)
			{
			for (JIndex i = 1; i <= itsCols->GetElementCount(); i++)
				{
				JIndex cindex = itsCols->GetElement(i);
				if (index <= cindex)
					{
					itsCols->SetElement(i, cindex+1);
					}
				}
			}
		}
}

/*******************************************************************************
 HandleInput

 ******************************************************************************/

void
GLDataModule::HandleInput
	(
	const JString& str
	)
{
	if (itsHeaderRead)
		{
		HandleDataRead(str);
		}
	else
		{
		std::string s(str.GetRawBytes(), str.GetByteCount());
		std::istringstream iss(s);
		int eval;
		if (!itsSentData)
			{
			iss >> eval;
			iss >> std::ws;
			if (eval == kGloveRequiresData)
				{
				int cols;
				iss >> cols;
				iss >> std::ws;
				bool success = itsTable->WriteDataCols(*itsOutput, cols);
				if (!success)
					{
					JGetUserNotification()->ReportError(JGetString("Error::GLDataModule"));
					JXDeleteObjectTask<GLDataModule>::Delete(this);
					return;
					}
				itsSentData = true;
				return;
				}
			}
		iss >> eval;
		iss >> std::ws;
		if (eval == kGloveDataDump)
			{
			itsDataIsDump = true;
			}
		else
			{
			itsDataIsDump = false;
			}
		iss >> eval;
		iss >> std::ws;
		HandlePrepareCols(eval);
		itsHeaderRead = true;
		}
}

/*******************************************************************************
 HandlePrepareCols

 ******************************************************************************/

void
GLDataModule::HandlePrepareCols
	(
	const JSize cols
	)
{
	itsColNum = cols;
	itsColStart = itsData->GetDataColCount() + 1;
	for (JSize i = 1; i <= cols; i++)
		{
		itsData->AppendCol();
		itsCols->AppendElement(itsColStart - 1 + i);
		}
	GLUndoElementsInsert* undo =
		jnew GLUndoElementsInsert(itsTable, JPoint(itsColStart, 1), 
								 JPoint(itsColStart + itsColNum - 1, itsTable->GetRowCount()),
								 GLUndoElementsBase::kCols);
	assert(undo != nullptr);
	itsTable->NewUndo(undo);
}

/*******************************************************************************
 HandleDataRead

 ******************************************************************************/

void
GLDataModule::HandleDataRead
	(
	const JString& str
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
	int eval;
	iss >> eval;
	if (eval == kGloveFinished)
		{
		JXDeleteObjectTask<GLDataModule>::Delete(this);
		return;
		}
	else if (eval == kGloveFail || eval != kGloveOK)
		{
		JGetUserNotification()->ReportError(JGetString("Error::GLDataModule"));
		JXDeleteObjectTask<GLDataModule>::Delete(this);
		return;
		}

	iss >> std::ws;
	JFloat value;
	if (itsDataIsDump)
		{
		for (JSize i = 0; i < itsColNum; i++)
			{
			if (iss.good())
				{
				iss >> value;
				iss >> std::ws;
				const JIndex cindex = itsCols->GetElement(i+1);
				assert(itsData->ColIndexValid(cindex));
				itsData->AppendElement(cindex, value);
				}
			else
				{
				JGetUserNotification()->ReportError(JGetString("Error::GLDataModule"));
				JXDeleteObjectTask<GLDataModule>::Delete(this);
				return;		
				}
			}
		}
	else
		{
		JSize row, col;
		if (iss.good())
			{
			iss >> row;
			iss >> std::ws;
			if (iss.good())
				{
				iss >> col;
				iss >> std::ws;
				if (iss.good())
					{
					iss >> value;
					if (col <= itsColNum)
						{
						JIndex cindex = itsCols->GetElement(col);
						assert(itsData->CellValid(row,cindex));
						itsData->SetElement(row, cindex, value);
						}
					else
						{
						JGetUserNotification()->ReportError(JGetString("Error::GLDataModule"));
						JXDeleteObjectTask<GLDataModule>::Delete(this);
						return;	
						}
					}
				else
					{
					JGetUserNotification()->ReportError(JGetString("Error::GLDataModule"));
					JXDeleteObjectTask<GLDataModule>::Delete(this);
					return;	
					}
				}
			else
				{
				JGetUserNotification()->ReportError(JGetString("Error::GLDataModule"));
				JXDeleteObjectTask<GLDataModule>::Delete(this);
				return;	
				}
			}
		else
			{
			JGetUserNotification()->ReportError(JGetString("Error::GLDataModule"));
			JXDeleteObjectTask<GLDataModule>::Delete(this);
			return;		
			}
		}
	const bool keepGoing = itsPG->IncrementProgress();
	if (!keepGoing)
		{
		JXDeleteObjectTask<GLDataModule>::Delete(this);
		return;
		}
	itsTable->Redraw();
}
