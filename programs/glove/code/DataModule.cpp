/*******************************************************************************
 DataModule.cpp

	DataModule class.
 
	Copyright @ 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/
 
#include "DataModule.h"
#include "GXRaggedFloatTable.h"
#include "GRaggedFloatTableData.h"
#include "GloveModule.h"
#include "GLUndoElementsInsert.h"
#include <DeleteBroadcasterTask.h>

#include <JXApplication.h>

#include <JOutPipeStream.h>
#include <JProcess.h>
#include <JUserNotification.h>
#include <JCreateProgressDisplay.h>
#include <JError.h>
#include <JString.h>

#include <jXGlobals.h>

#include <jProcessUtil.h>
#include <jStreamUtil.h>
#include <sstream>
#include <jAssert.h>

/*********************************************************************************
 Create

 ********************************************************************************/

JBoolean
DataModule::Create
	(
	DataModule** module,
	GXRaggedFloatTable* table, 
	GRaggedFloatTableData* data,
	const JCharacter* sysCmd
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
		JOutPipeStream* op = jnew JOutPipeStream(outFD, kJTrue);
		assert( op != NULL );
		assert( op->good() );
		*module = jnew DataModule(table, data, process, inFD, op);
		return kJTrue;
		}
		
	return kJFalse;
}

/*******************************************************************************
 Constructor (protected)

 ******************************************************************************/

DataModule::DataModule
	(
	GXRaggedFloatTable* 	table, 
	GRaggedFloatTableData*	data,
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

	itsLink = jnew ProcessLink(fd);
	assert(itsLink != NULL);
	ListenTo(itsLink);

	itsOutput = output;
//	ListenTo(output);
	itsHeaderRead = kJFalse;
	itsSentData = kJFalse;
	itsPG = NULL;
	itsCols = jnew JArray<JIndex>;
	assert(itsCols != NULL);
}

/*******************************************************************************
 Destructor

 ******************************************************************************/

DataModule::~DataModule()
{
	jdelete itsProcess;
	jdelete itsLink;
	if (itsPG != NULL)
		{
		itsPG->ProcessFinished();
		jdelete itsPG;
		}
}

/*******************************************************************************
 Receive

 ******************************************************************************/

void
DataModule::Receive
	(
	JBroadcaster* sender,
	const JBroadcaster::Message& message
	)
{
	if (message.Is(JProcess::kFinished))
		{
		JGetUserNotification()->DisplayMessage("Data module completed.");
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
		assert( info != NULL );
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
					JGetUserNotification()->ReportError("Module column removed.");
					DeleteBroadcasterTask* dbt = jnew DeleteBroadcasterTask(this);
					assert(dbt != NULL);
					dbt->Go();
					return;
					}
				}
			}
		}
		
	else if (sender == itsData && message.Is(JTableData::kColsInserted))
		{
		const JTableData::ColsInserted* info =
			dynamic_cast<const JTableData::ColsInserted*>(&message);
		assert( info != NULL );
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
DataModule::HandleInput
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
		std::string s(str);
		std::istringstream iss(s);
		int eval;
		if (!itsSentData)
			{
			iss >> eval;
			iss >> ws;
			if (eval == kGloveRequiresData)
				{
				int cols;
				iss >> cols;
				iss >> ws;
				JBoolean success = itsTable->WriteDataCols(*itsOutput, cols);
				if (!success)
					{
					JGetUserNotification()->ReportError("Module Error.");
					DeleteBroadcasterTask* dbt = jnew DeleteBroadcasterTask(this);
					assert(dbt != NULL);
					dbt->Go();
					return;
					}
				itsSentData = kJTrue;
				return;
				}
			}
		iss >> eval;
		iss >> ws;
		if (eval == kGloveDataDump)
			{
			itsDataIsDump = kJTrue;
			}
		else
			{
			itsDataIsDump = kJFalse;
			}
		iss >> eval;
		iss >> ws;
		HandlePrepareCols(eval);
		itsHeaderRead = kJTrue;
		}
}

/*******************************************************************************
 HandlePrepareCols

 ******************************************************************************/

void
DataModule::HandlePrepareCols
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
	assert(undo != NULL);
	itsTable->NewUndo(undo);
}

/*******************************************************************************
 HandleDataRead

 ******************************************************************************/

void
DataModule::HandleDataRead
	(
	const JString& str
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
	int eval;
	iss >> eval;
	if (eval == kGloveFinished)
		{
		DeleteBroadcasterTask* dbt = jnew DeleteBroadcasterTask(this);
		assert(dbt != NULL);
		dbt->Go();
		return;
		}
	else if (eval == kGloveFail || eval != kGloveOK)
		{
		JGetUserNotification()->ReportError("Module Error.");
		DeleteBroadcasterTask* dbt = jnew DeleteBroadcasterTask(this);
		assert(dbt != NULL);
		dbt->Go();
		return;
		}

	iss >> ws;
	JFloat value;
	if (itsDataIsDump)
		{
		for (JSize i = 0; i < itsColNum; i++)
			{
			if (iss.good())
				{
				iss >> value;
				iss >> ws;
				const JIndex cindex = itsCols->GetElement(i+1);
				assert(itsData->ColIndexValid(cindex));
				itsData->AppendElement(cindex, value);
				}
			else
				{
				JGetUserNotification()->ReportError("Module Error.");
				DeleteBroadcasterTask* dbt = jnew DeleteBroadcasterTask(this);
				assert(dbt != NULL);
				dbt->Go();
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
			iss >> ws;
			if (iss.good())
				{
				iss >> col;
				iss >> ws;
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
						JGetUserNotification()->ReportError("Module Error.");
						DeleteBroadcasterTask* dbt = jnew DeleteBroadcasterTask(this);
						assert(dbt != NULL);
						dbt->Go();
						return;	
						}
					}
				else
					{
					JGetUserNotification()->ReportError("Module Error.");
					DeleteBroadcasterTask* dbt = jnew DeleteBroadcasterTask(this);
					assert(dbt != NULL);
					dbt->Go();
					return;	
					}
				}
			else
				{
				JGetUserNotification()->ReportError("Module Error.");
				DeleteBroadcasterTask* dbt = jnew DeleteBroadcasterTask(this);
				assert(dbt != NULL);
				dbt->Go();
				return;	
				}
			}
		else
			{
			JGetUserNotification()->ReportError("Module Error.");
			DeleteBroadcasterTask* dbt = jnew DeleteBroadcasterTask(this);
			assert(dbt != NULL);
			dbt->Go();
			return;		
			}
		}
	const JBoolean keepGoing = itsPG->IncrementProgress();
	if (!keepGoing)
		{
		DeleteBroadcasterTask* dbt = jnew DeleteBroadcasterTask(this);
		assert(dbt != NULL);
		dbt->Go();
		return;
		}
	itsTable->Redraw();
}
