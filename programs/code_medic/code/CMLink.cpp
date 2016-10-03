/******************************************************************************
 CMLink.cpp

	Base class for debugger interfaces.

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 2007 by John Lindal.  All rights reserved.

 *****************************************************************************/

#include "CMLink.h"
#include "CMCommand.h"
#include "CMBreakpoint.h"
#include "cmGlobals.h"
#include <jFileUtil.h>
#include <jAssert.h>

// JBroadcaster message types

const JCharacter* CMLink::kUserOutput             = "UserOutput::CMLink";
const JCharacter* CMLink::kDebugOutput            = "DebugOutput::CMLink";

const JCharacter* CMLink::kDebuggerReadyForInput  = "DebuggerReadyForInput::CMLink";
const JCharacter* CMLink::kDebuggerBusy           = "DebuggerBusy::CMLink";
const JCharacter* CMLink::kDebuggerDefiningScript = "DebuggerDefiningScript::CMLink";

const JCharacter* CMLink::kDebuggerStarted        = "DebuggerStarted::CMLink";
const JCharacter* CMLink::kDebuggerRestarted      = "DebuggerRestarted::CMLink";
const JCharacter* CMLink::kPrepareToLoadSymbols   = "PrepareToLoadSymbols::CMLink";
const JCharacter* CMLink::kSymbolsLoaded          = "SymbolsLoaded::CMLink";
const JCharacter* CMLink::kSymbolsReloaded        = "SymbolsReloaded::CMLink";
const JCharacter* CMLink::kCoreLoaded             = "CoreLoaded::CMLink";
const JCharacter* CMLink::kCoreCleared            = "CoreCleared::CMLink";
const JCharacter* CMLink::kAttachedToProcess      = "AttachedToProcess::CMLink";
const JCharacter* CMLink::kDetachedFromProcess    = "DetachedFromProcess::CMLink";

const JCharacter* CMLink::kProgramRunning         = "ProgramRunning::CMLink";
const JCharacter* CMLink::kProgramFirstStop       = "ProgramFirstStop::CMLink";
const JCharacter* CMLink::kProgramStopped         = "ProgramStopped::CMLink";
const JCharacter* CMLink::kProgramStopped2        = "ProgramStopped2::CMLink";
const JCharacter* CMLink::kProgramFinished        = "ProgramFinished::CMLink";

const JCharacter* CMLink::kBreakpointsChanged     = "BreakpointsChanged::CMLink";
const JCharacter* CMLink::kFrameChanged           = "FrameChanged::CMLink";
const JCharacter* CMLink::kThreadChanged          = "ThreadChanged::CMLink";
const JCharacter* CMLink::kValueChanged           = "ValueChanged::CMLink";

const JCharacter* CMLink::kThreadListChanged      = "ThreadListChanged::CMLink";

const JCharacter* CMLink::kPlugInMessage          = "PlugInMessage::CMLink";

/******************************************************************************
 Constructor

 *****************************************************************************/

CMLink::CMLink
	(
	const JBoolean* features
	)
	:
	itsFeatures(features)
{
	// commands are often owned by other objects, who can delete them more reliably
	itsForegroundQ = new JPtrArray<CMCommand>(JPtrArrayT::kForgetAll);
	assert( itsForegroundQ != NULL );

	// commands are often owned by other objects, who can delete them more reliably
	itsBackgroundQ = new JPtrArray<CMCommand>(JPtrArrayT::kForgetAll);
	assert( itsBackgroundQ != NULL );

	itsRunningCommand = NULL;
	itsLastCommandID  = 0;

	itsFileNameMap = new JStringPtrMap<JString>(JPtrArrayT::kDeleteAll);
	assert( itsFileNameMap != NULL );
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CMLink::~CMLink()
{
	delete itsForegroundQ;	// must be last, after objects have deleted their commands
	delete itsBackgroundQ;

	delete itsFileNameMap;
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
		CMCommand* cmd = list->NthElement(i);
		if (cmd->IsOneShot())
			{
			delete cmd;		// automatically removed from list
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
				  JI2B(bp.GetAction() == CMBreakpoint::kRemoveBreakpoint));
}

void
CMLink::SetBreakpoint
	(
	const CMLocation&	loc,
	const JBoolean		temporary
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

JBoolean
CMLink::OKToSendMultipleCommands()
	const
{
	return kJTrue;
}

/******************************************************************************
 Send

	Sends the given command to the debugger.  If the command cannot be
	submitted, return kJFalse, and nothing is changed.

 *****************************************************************************/

JBoolean
CMLink::Send
	(
	CMCommand* command
	)
{
	if (command->GetState() != CMCommand::kUnassigned)
		{
		assert( command->GetTransactionID() != 0 );
		return kJFalse;
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

		return kJTrue;
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
		itsRunningCommand = NULL;
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
			CMCommand* command = itsForegroundQ->NthElement(i);
			if (command->GetState() != CMCommand::kExecuting)
				{
				SendMedicCommand(command);
				}
			}
		}
	else if (!itsForegroundQ->IsEmpty())
		{
		CMCommand* command = itsForegroundQ->FirstElement();
		if (command->GetState() != CMCommand::kExecuting && OKToSendCommands(kJFalse))
			{
			SendMedicCommand(command);
			}
		}
	else if (!itsBackgroundQ->IsEmpty() && OKToSendCommands(kJTrue))
		{
		CMCommand* command = itsBackgroundQ->FirstElement();
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
	assert( itsRunningCommand == NULL );

	const JSize fgCount = itsForegroundQ->GetElementCount();
	for (JIndex i=1; i<=fgCount; i++)
		{
		CMCommand* command = itsForegroundQ->NthElement(i);
		if (command->GetTransactionID() == cmdID)
			{
			itsRunningCommand = command;
			itsForegroundQ->RemoveElement(i);
			break;
			}
		}

	if (itsRunningCommand == NULL && !itsBackgroundQ->IsEmpty())
		{
		CMCommand* command = itsBackgroundQ->FirstElement();
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
	for (JIndex i=itsForegroundQ->GetElementCount(); i>=1; i--)
		{
		CMCommand* cmd = itsForegroundQ->NthElement(i);
		itsForegroundQ->RemoveElement(i);	// remove first, in case auto-delete
		cmd->Finished(kJFalse);

		if (itsRunningCommand == cmd)
			{
			itsRunningCommand = NULL;
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
	for (JIndex i=itsBackgroundQ->GetElementCount(); i>=1; i--)
		{
		CMCommand* cmd = itsBackgroundQ->NthElement(i);
		itsBackgroundQ->RemoveElement(i);	// remove first, in case auto-delete
		cmd->Finished(kJFalse);

		if (itsRunningCommand == cmd)
			{
			itsRunningCommand = NULL;
			}
		}
}

/******************************************************************************
 RememberFile

 ******************************************************************************/

void
CMLink::RememberFile
	(
	const JCharacter* fileName,
	const JCharacter* fullName
	)
{
	if (JStringEmpty(fullName))
		{
		itsFileNameMap->SetElement(fileName, NULL, JPtrArrayT::kDelete);
		}
	else
		{
		itsFileNameMap->SetElement(fileName, fullName, JPtrArrayT::kDelete);
		}
}

/******************************************************************************
 FindFile

 ******************************************************************************/

JBoolean
CMLink::FindFile
	(
	const JCharacter*	fileName,
	JBoolean*			exists,
	JString*			fullName
	)
	const
{
	const JString* s = NULL;
	if (JIsAbsolutePath(fileName) && JFileExists(fileName))
		{
		*exists   = kJTrue;
		*fullName = fileName;
		return kJTrue;
		}
	else if (itsFileNameMap->GetElement(fileName, &s))
		{
		if (s == NULL)
			{
			*exists = kJFalse;
			fullName->Clear();
			}
		else
			{
			*exists   = kJTrue;
			*fullName = *s;
			}
		return kJTrue;
		}
	else
		{
		*exists = kJFalse;
		fullName->Clear();
		return kJFalse;
		}

/*	All search paths are unreliable.  See CMGetFullPath.cc for more info.

	if (JIsRelativePath(fileName))
		{
		const JSize count = itsPathList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			*fullName = JCombinePathAndName(*(itsPathList->NthElement(i)), fileName);
			if (JFileExists(*fullName))
				{
				return kJTrue;
				}
			}
		}
	else if (JFileExists(fileName))
		{
		*fullName = fileName;
		return kJTrue;
		}

	fullName->Clear();
	return kJFalse; */
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
	const JCharacter*	msg,
	const JBoolean		error
	)
{
	(CMGetLink())->Broadcast(UserOutput(msg, error));
}

/******************************************************************************
 Log (static)

 *****************************************************************************/

void
CMLink::Log
	(
	const JCharacter* log
	)
{
	(CMGetLink())->Broadcast(DebugOutput(log, kLogType));
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
	const JCharacter*	origExpr,
	const JInteger		index
	)
{
	JString expr = origExpr;

	const JString indexStr(index, 0);	// must use floating point conversion
	if (expr.Contains("$i"))
		{
		const JCharacter* map[] =
			{
			"i", indexStr.GetCString()
			};
		(JGetStringManager())->Replace(&expr, map, sizeof(map));
		}
	else
		{
		if (expr.GetFirstCharacter() != '(' ||
			expr.GetLastCharacter()  != ')')
			{
			expr.PrependCharacter('(');
			expr.AppendCharacter(')');
			}

		expr.AppendCharacter('[');
		expr += indexStr;
		expr.AppendCharacter(']');
		}

	return expr;
}
/******************************************************************************
 Build2DArrayExpressionForCFamilyLanguage (protected)

 *****************************************************************************/

JString
CMLink::Build2DArrayExpressionForCFamilyLanguage
	(
	const JCharacter*	origExpr,
	const JInteger		rowIndex,
	const JInteger		colIndex
	)
{
	JString expr = origExpr;

	const JBoolean usesI = expr.Contains("$i");		// row
	const JBoolean usesJ = expr.Contains("$j");		// col

	const JString iStr(rowIndex, 0);	// must use floating point conversion
	const JString jStr(colIndex, 0);	// must use floating point conversion

	// We have to do both at the same time because otherwise we lose a $.

	if (usesI || usesJ)
		{
		const JCharacter* map[] =
			{
			"i", iStr.GetCString(),
			"j", jStr.GetCString()
			};
		(JGetStringManager())->Replace(&expr, map, sizeof(map));
		}

	if (!usesI || !usesJ)
		{
		if (expr.GetFirstCharacter() != '(' ||
			expr.GetLastCharacter()  != ')')
			{
			expr.PrependCharacter('(');
			expr.AppendCharacter(')');
			}

		if (!usesI)
			{
			expr.AppendCharacter('[');
			expr += iStr;
			expr.AppendCharacter(']');
			}
		if (!usesJ)
			{
			expr.AppendCharacter('[');
			expr += jStr;
			expr.AppendCharacter(']');
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
	const JCharacter* addr
	)
{
}

void
CMLink::SetExecutionPoint
	(
	const JCharacter* addr
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
