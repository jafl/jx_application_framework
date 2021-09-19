/******************************************************************************
 CMLink.cpp

	Base class for debugger interfaces.

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 2007 by John Lindal.

 *****************************************************************************/

#include "CMLink.h"
#include "CMCommand.h"
#include "CMBreakpoint.h"
#include "cmGlobals.h"
#include <jFileUtil.h>
#include <jAssert.h>

// JBroadcaster message types

const JUtf8Byte* CMLink::kUserOutput             = "UserOutput::CMLink";
const JUtf8Byte* CMLink::kDebugOutput            = "DebugOutput::CMLink";

const JUtf8Byte* CMLink::kDebuggerReadyForInput  = "DebuggerReadyForInput::CMLink";
const JUtf8Byte* CMLink::kDebuggerBusy           = "DebuggerBusy::CMLink";
const JUtf8Byte* CMLink::kDebuggerDefiningScript = "DebuggerDefiningScript::CMLink";

const JUtf8Byte* CMLink::kDebuggerStarted        = "DebuggerStarted::CMLink";
const JUtf8Byte* CMLink::kDebuggerRestarted      = "DebuggerRestarted::CMLink";
const JUtf8Byte* CMLink::kPrepareToLoadSymbols   = "PrepareToLoadSymbols::CMLink";
const JUtf8Byte* CMLink::kSymbolsLoaded          = "SymbolsLoaded::CMLink";
const JUtf8Byte* CMLink::kSymbolsReloaded        = "SymbolsReloaded::CMLink";
const JUtf8Byte* CMLink::kCoreLoaded             = "CoreLoaded::CMLink";
const JUtf8Byte* CMLink::kCoreCleared            = "CoreCleared::CMLink";
const JUtf8Byte* CMLink::kAttachedToProcess      = "AttachedToProcess::CMLink";
const JUtf8Byte* CMLink::kDetachedFromProcess    = "DetachedFromProcess::CMLink";

const JUtf8Byte* CMLink::kProgramRunning         = "ProgramRunning::CMLink";
const JUtf8Byte* CMLink::kProgramFirstStop       = "ProgramFirstStop::CMLink";
const JUtf8Byte* CMLink::kProgramStopped         = "ProgramStopped::CMLink";
const JUtf8Byte* CMLink::kProgramStopped2        = "ProgramStopped2::CMLink";
const JUtf8Byte* CMLink::kProgramFinished        = "ProgramFinished::CMLink";

const JUtf8Byte* CMLink::kBreakpointsChanged     = "BreakpointsChanged::CMLink";
const JUtf8Byte* CMLink::kFrameChanged           = "FrameChanged::CMLink";
const JUtf8Byte* CMLink::kThreadChanged          = "ThreadChanged::CMLink";
const JUtf8Byte* CMLink::kValueChanged           = "ValueChanged::CMLink";

const JUtf8Byte* CMLink::kThreadListChanged      = "ThreadListChanged::CMLink";

const JUtf8Byte* CMLink::kPlugInMessage          = "PlugInMessage::CMLink";

/******************************************************************************
 Constructor

 *****************************************************************************/

CMLink::CMLink
	(
	const bool* features
	)
	:
	itsFeatures(features)
{
	// commands are often owned by other objects, who can delete them more reliably
	itsForegroundQ = jnew JPtrArray<CMCommand>(JPtrArrayT::kForgetAll);
	assert( itsForegroundQ != nullptr );

	// commands are often owned by other objects, who can delete them more reliably
	itsBackgroundQ = jnew JPtrArray<CMCommand>(JPtrArrayT::kForgetAll);
	assert( itsBackgroundQ != nullptr );

	itsRunningCommand = nullptr;
	itsLastCommandID  = 0;

	itsFileNameMap = jnew JStringPtrMap<JString>(JPtrArrayT::kDeleteAll);
	assert( itsFileNameMap != nullptr );
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CMLink::~CMLink()
{
	jdelete itsForegroundQ;	// must be last, after objects have deleted their commands
	jdelete itsBackgroundQ;

	jdelete itsFileNameMap;
}

/******************************************************************************
 DeleteOneShotCommands (private)

	*** Only for use by dtor.

 *****************************************************************************/

void
CMLink::DeleteOneShotCommands()
{
	DeleteOneShotCommands(itsForegroundQ);
	DeleteOneShotCommands(itsBackgroundQ);
}

void
CMLink::DeleteOneShotCommands
	(
	JPtrArray<CMCommand>* list
	)
{
	const JSize count = list->GetElementCount();
	for (JIndex i=count; i>=1; i--)
		{
		CMCommand* cmd = list->GetElement(i);
		if (cmd->IsOneShot())
			{
			jdelete cmd;		// automatically removed from list
			}
		}
}

/******************************************************************************
 SetBreakpoint

 *****************************************************************************/

void
CMLink::SetBreakpoint
	(
	const CMBreakpoint& bp
	)
{
	SetBreakpoint(bp.GetFileName(), bp.GetLineNumber(),
				  bp.GetAction() == CMBreakpoint::kRemoveBreakpoint);
}

void
CMLink::SetBreakpoint
	(
	const CMLocation&	loc,
	const bool		temporary
	)
{
	SetBreakpoint(loc.GetFileName(), loc.GetLineNumber(), temporary);
}

/******************************************************************************
 RemoveBreakpoint

 *****************************************************************************/

void
CMLink::RemoveBreakpoint
	(
	const CMBreakpoint& bp
	)
{
	RemoveBreakpoint(bp.GetDebuggerIndex());
}

/******************************************************************************
 OKToSendMultipleCommands (virtual)

 *****************************************************************************/

bool
CMLink::OKToSendMultipleCommands()
	const
{
	return true;
}

/******************************************************************************
 Send

	Sends the given command to the debugger.  If the command cannot be
	submitted, return false, and nothing is changed.

 *****************************************************************************/

bool
CMLink::Send
	(
	CMCommand* command
	)
{
	if (command->GetState() != CMCommand::kUnassigned)
		{
		assert( command->GetTransactionID() != 0 );
		return false;
		}
	else
		{
		command->SetTransactionID(GetNextTransactionID());

		if (command->IsBackground())
			{
			itsBackgroundQ->Append(command);
			}
		else
			{
			itsForegroundQ->Append(command);
			}
		RunNextCommand();

		return true;
		}
}

/******************************************************************************
 GetNextTransactionID (protected)

 *****************************************************************************/

JIndex
CMLink::GetNextTransactionID()
{
	itsLastCommandID++;
	if (itsLastCommandID == 0) // Wrap, even though it isn't likely to ever happen
		{
		itsLastCommandID = 1;
		}
	return itsLastCommandID;
}

/******************************************************************************
 Cancel

	Do not call directly.

 *****************************************************************************/

void
CMLink::Cancel
	(
	CMCommand* cmd
	)
{
	if (itsRunningCommand == cmd)
		{
		itsRunningCommand = nullptr;
		}
	itsBackgroundQ->Remove(cmd);
	itsForegroundQ->Remove(cmd);
}

/******************************************************************************
 RunNextCommand (protected)

 *****************************************************************************/

void
CMLink::RunNextCommand()
{
	if (!itsForegroundQ->IsEmpty() && OKToSendMultipleCommands())
		{
		const JSize count = itsForegroundQ->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			CMCommand* command = itsForegroundQ->GetElement(i);
			if (command->GetState() != CMCommand::kExecuting)
				{
				SendMedicCommand(command);
				}
			}
		}
	else if (!itsForegroundQ->IsEmpty())
		{
		CMCommand* command = itsForegroundQ->GetFirstElement();
		if (command->GetState() != CMCommand::kExecuting && OKToSendCommands(false))
			{
			SendMedicCommand(command);
			}
		}
	else if (!itsBackgroundQ->IsEmpty() && OKToSendCommands(true))
		{
		CMCommand* command = itsBackgroundQ->GetFirstElement();
		if (command->GetState() != CMCommand::kExecuting)
			{
			SendMedicCommand(command);
			}
		}
}

/******************************************************************************
 HandleCommandRunning (protected)

 *****************************************************************************/

void
CMLink::HandleCommandRunning
	(
	const JIndex cmdID
	)
{
	assert( itsRunningCommand == nullptr );

	const JSize fgCount = itsForegroundQ->GetElementCount();
	for (JIndex i=1; i<=fgCount; i++)
		{
		CMCommand* command = itsForegroundQ->GetElement(i);
		if (command->GetTransactionID() == cmdID)
			{
			itsRunningCommand = command;
			itsForegroundQ->RemoveElement(i);
			break;
			}
		}

	if (itsRunningCommand == nullptr && !itsBackgroundQ->IsEmpty())
		{
		CMCommand* command = itsBackgroundQ->GetFirstElement();
		if (command->GetTransactionID() == cmdID)
			{
			itsRunningCommand = command;
			itsBackgroundQ->RemoveElement(1);
			}
		}
}

/******************************************************************************
 CancelAllCommands (protected)

 *****************************************************************************/

void
CMLink::CancelAllCommands()
{
	if (CMIsShuttingDown())		// command owners already deleted
		{
		return;
		}

	for (JIndex i=itsForegroundQ->GetElementCount(); i>=1; i--)
		{
		CMCommand* cmd = itsForegroundQ->GetElement(i);
		itsForegroundQ->RemoveElement(i);	// remove first, in case auto-delete
		cmd->Finished(false);

		if (itsRunningCommand == cmd)
			{
			itsRunningCommand = nullptr;
			}
		}

	CancelBackgroundCommands();
}

/******************************************************************************
 CancelBackgroundCommands (protected)

 *****************************************************************************/

void
CMLink::CancelBackgroundCommands()
{
	if (CMIsShuttingDown())		// command owners already deleted
		{
		return;
		}

	for (JIndex i=itsBackgroundQ->GetElementCount(); i>=1; i--)
		{
		CMCommand* cmd = itsBackgroundQ->GetElement(i);
		itsBackgroundQ->RemoveElement(i);	// remove first, in case auto-delete
		cmd->Finished(false);

		if (itsRunningCommand == cmd)
			{
			itsRunningCommand = nullptr;
			}
		}
}

/******************************************************************************
 RememberFile

 ******************************************************************************/

void
CMLink::RememberFile
	(
	const JString& fileName,
	const JString& fullName
	)
{
	if (fullName.IsEmpty())
		{
		itsFileNameMap->SetElement(fileName, nullptr, JPtrArrayT::kDelete);
		}
	else
		{
		itsFileNameMap->SetElement(fileName, fullName, JPtrArrayT::kDelete);
		}
}

/******************************************************************************
 FindFile

 ******************************************************************************/

bool
CMLink::FindFile
	(
	const JString&	fileName,
	bool*		exists,
	JString*		fullName
	)
	const
{
	const JString* s = nullptr;
	if (JIsAbsolutePath(fileName) && JFileExists(fileName))
		{
		*exists   = true;
		*fullName = fileName;
		return true;
		}
	else if (itsFileNameMap->GetElement(fileName, &s))
		{
		if (s == nullptr)
			{
			*exists = false;
			fullName->Clear();
			}
		else
			{
			*exists   = true;
			*fullName = *s;
			}
		return true;
		}
	else
		{
		*exists = false;
		fullName->Clear();
		return false;
		}

/*	All search paths are unreliable.  See CMGetFullPath.cc for more info.

	if (JIsRelativePath(fileName))
		{
		const JSize count = itsPathList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			*fullName = JCombinePathAndName(*(itsPathList->GetElement(i)), fileName);
			if (JFileExists(*fullName))
				{
				return true;
				}
			}
		}
	else if (JFileExists(fileName))
		{
		*fullName = fileName;
		return true;
		}

	fullName->Clear();
	return false; */
}

/******************************************************************************
 ClearFileNameMap

 ******************************************************************************/

void
CMLink::ClearFileNameMap()
{
	itsFileNameMap->DeleteAll();
}

/******************************************************************************
 NotifyUser (static)

 *****************************************************************************/

void
CMLink::NotifyUser
	(
	const JString&	msg,
	const bool	error
	)
{
	CMGetLink()->Broadcast(UserOutput(msg, error));
}

/******************************************************************************
 Log (static)

 *****************************************************************************/

void
CMLink::Log
	(
	const JUtf8Byte* log
	)
{
	CMGetLink()->Broadcast(DebugOutput(JString(log, JString::kNoCopy), kLogType));
}

/******************************************************************************
 Log (static)

 *****************************************************************************/

void
CMLink::Log
	(
	std::ostringstream& log
	)
{
	std::string logData = log.str();
	Log(logData.data());
}

/******************************************************************************
 Build1DArrayExpressionForCFamilyLanguage (protected)

 *****************************************************************************/

JString
CMLink::Build1DArrayExpressionForCFamilyLanguage
	(
	const JString&	origExpr,
	const JInteger	index
	)
{
	JString expr = origExpr;

	const JString indexStr(index, 0);	// must use floating point conversion
	if (expr.Contains("$i"))
		{
		const JUtf8Byte* map[] =
			{
			"i", indexStr.GetBytes()
			};
		JGetStringManager()->Replace(&expr, map, sizeof(map));
		}
	else
		{
		if (expr.GetFirstCharacter() != '(' ||
			expr.GetLastCharacter()  != ')')
			{
			expr.Prepend("(");
			expr.Append(")");
			}

		expr += "[";
		expr += indexStr;
		expr += "]";
		}

	return expr;
}
/******************************************************************************
 Build2DArrayExpressionForCFamilyLanguage (protected)

 *****************************************************************************/

JString
CMLink::Build2DArrayExpressionForCFamilyLanguage
	(
	const JString&	origExpr,
	const JInteger	rowIndex,
	const JInteger	colIndex
	)
{
	JString expr = origExpr;

	const bool usesI = expr.Contains("$i");		// row
	const bool usesJ = expr.Contains("$j");		// col

	const JString iStr(rowIndex, 0);	// must use floating point conversion
	const JString jStr(colIndex, 0);	// must use floating point conversion

	// We have to do both at the same time because otherwise we lose a $.

	if (usesI || usesJ)
		{
		const JUtf8Byte* map[] =
			{
			"i", iStr.GetBytes(),
			"j", jStr.GetBytes()
			};
		JGetStringManager()->Replace(&expr, map, sizeof(map));
		}

	if (!usesI || !usesJ)
		{
		if (expr.GetFirstCharacter() != '(' ||
			expr.GetLastCharacter()  != ')')
			{
			expr.Prepend("(");
			expr.Append(")");
			}

		if (!usesI)
			{
			expr += "[";
			expr += iStr;
			expr += "]";
			}
		if (!usesJ)
			{
			expr += "[";
			expr += jStr;
			expr += "]";
			}
		}

	return expr;
}

/******************************************************************************
 Uncommon functionality, so not pure virtual

 *****************************************************************************/

void
CMLink::StepOverAssembly()
{
}

void
CMLink::StepIntoAssembly()
{
}

void
CMLink::RunUntil
	(
	const JString& addr
	)
{
}

void
CMLink::SetExecutionPoint
	(
	const JString& addr
	)
{
}

void
CMLink::BackupOver()
{
}

void
CMLink::BackupInto()
{
}

void
CMLink::BackupOut()
{
}

void
CMLink::BackupContinue()
{
}
