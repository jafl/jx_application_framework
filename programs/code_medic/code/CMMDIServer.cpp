/******************************************************************************
 CMMDIServer.cpp

	BASE CLASS = public JXMDIServer

	Copyright © 1997 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include <cmStdInc.h>
#include "CMMDIServer.h"
#include "CMCommandDirector.h"
#include "cmGlobals.h"
#include <JTextEditor.h>
#include <jDirUtil.h>
#include <jStreamUtil.h>
#include <jAssert.h>

enum
{
	kWaitingForProgram,
	kWaitingForCore,
	kWaitingForFile,
	kWaitingForBreakpoint,
	kWaitingForName
};

/******************************************************************************
 Constructor

 *****************************************************************************/

CMMDIServer::CMMDIServer()
	:
	JXMDIServer(),
	itsFirstTimeFlag(kJTrue)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CMMDIServer::~CMMDIServer()
{
}

/******************************************************************************
 HandleMDIRequest (virtual protected)

 *****************************************************************************/

void
CMMDIServer::HandleMDIRequest
	(
	const JCharacter*			dir,
	const JPtrArray<JString>&	argList
	)
{
	const JString origDir = JGetCurrentDirectory();
	const JError err      = JChangeDirectory(dir);
	if (!err.OK())
		{
		err.ReportIfError();
		return;
		}

	CMCommandDirector* cmdDir = CMGetCommandDirector();

	JIndex context = kWaitingForProgram;
	JString program, core;
	JBoolean forcedType = kJFalse;
	JPtrArray<JString> fileList(JPtrArrayT::kDeleteAll);
	JArray<JIndex> lineIndexList;
	JArray<JBoolean> breakList;

	const JSize count = argList.GetElementCount();
	for (JIndex i=2; i<=count; i++)
		{
		const JString* arg = argList.NthElement(i);

		if (*arg == "--gdb")
			{
			forcedType = kJTrue;
			(CMGetPrefsManager())->SetDebuggerType(CMPrefsManager::kGDBType);
			continue;
			}
		else if (*arg == "--xdebug")
			{
			forcedType = kJTrue;
			(CMGetPrefsManager())->SetDebuggerType(CMPrefsManager::kXdebugType);
			continue;
			}
		else if (*arg == "--java")
			{
			forcedType = kJTrue;
			(CMGetPrefsManager())->SetDebuggerType(CMPrefsManager::kJavaType);
			continue;
			}
		else if (*arg == "-f")
			{
			context = kWaitingForFile;
			continue;
			}
		else if (*arg == "-c")
			{
			(CMGetPrefsManager())->SetDebuggerType(CMPrefsManager::kGDBType);
			context = kWaitingForCore;
			continue;
			}
		else if (*arg == "-b")
			{
			context = kWaitingForBreakpoint;
			continue;
			}

		if (context == kWaitingForProgram)
			{
			if (program.IsEmpty())
				{
				program = *arg;
				}
			context = kWaitingForCore;
			}
		else if (context == kWaitingForCore)
			{
			if (core.IsEmpty())
				{
				core = *arg;
				}
			context = kWaitingForProgram;
			}
		else if (context == kWaitingForFile ||
				 context == kWaitingForBreakpoint)
			{
			// must do before changing context
			breakList.AppendElement(JI2B(context == kWaitingForBreakpoint));

			const JString* arg = argList.NthElement(i);
			if (arg->GetFirstCharacter() == '+')
				{
				JString temp = *arg;
				JInteger value = 0;
				temp.ConvertToInteger(&value);
				lineIndexList.AppendElement(value);
				context = kWaitingForName;
				}
			else
				{
				JString* str = new JString(*arg);
				assert(str != NULL);
				JIndex findex;
				JInteger value = 0;
				if (str->LocateSubstring(":", &findex))
					{
					if (findex < str->GetLength())
						{
						JString line = str->GetSubstring(findex+1, str->GetLength());
						line.ConvertToInteger(&value);
						}
					str->RemoveSubstring(findex, str->GetLength());
					}
				lineIndexList.AppendElement(value);
				fileList.Append(str);
				context = kWaitingForProgram;
				}
			}
		else if (context == kWaitingForName)
			{
			fileList.Append(*arg);
			context = kWaitingForProgram;
			}
		}

	const JBoolean wasFirstTime = itsFirstTimeFlag;
	if (itsFirstTimeFlag)
		{
		if (!program.IsEmpty())
			{
			if (!forcedType)
				{
				UpdateDebuggerType(program);
				}

			(CMGetLink())->SetProgram(program);
			}

		itsFirstTimeFlag = kJFalse;
		}

	if (!core.IsEmpty())
		{
		(CMGetLink())->SetCore(core);
		}

	const JSize fileCount = fileList.GetElementCount();
	for (JIndex i=1; i<=fileCount; i++)
		{
		const JString* fileName = fileList.NthElement(i);
		const JIndex lineIndex  = lineIndexList.GetElement(i);
		if (breakList.GetElement(i) && lineIndex > 0)
			{
			if (wasFirstTime)
				{
				cerr << "-b option only works when Code Medic is already running" << endl;
				}
			else
				{
				(CMGetLink())->SetBreakpoint(*fileName, lineIndex);
				}
			}
		else
			{
			cmdDir->OpenSourceFile(*fileName, lineIndex, kJFalse);
			}
		}

	JChangeDirectory(origDir);
}

/******************************************************************************
 UpdateDebuggerType (static)

 *****************************************************************************/

void
CMMDIServer::UpdateDebuggerType
	(
	const JCharacter* program
	)
{
	JString language;
	if (IsBinary(program))
		{
		(CMGetPrefsManager())->SetDebuggerType(CMPrefsManager::kGDBType);
		}
	else if (!GetLanguage(program, &language))
		{
		// accept current debugging mode
		}
	else if (language == "java")
		{
		(CMGetPrefsManager())->SetDebuggerType(CMPrefsManager::kJavaType);
		}
	else if (language == "php")
		{
		(CMGetPrefsManager())->SetDebuggerType(CMPrefsManager::kXdebugType);
		}
}

/******************************************************************************
 IsBinary (static)

 *****************************************************************************/

JBoolean
CMMDIServer::IsBinary
	(
	const JCharacter* fileName
	)
{
	char buffer[1000];
	memset(buffer, ' ', 1000);

	ifstream input(fileName);
	input.read(buffer, 1000);
	return JTextEditor::ContainsIllegalChars(buffer, 1000);
}

/******************************************************************************
 GetLanguage (static)

 *****************************************************************************/

JBoolean
CMMDIServer::GetLanguage
	(
	const JCharacter*	fileName,
	JString*			language
	)
{
	language->Clear();

	ifstream input(fileName);
	JString line = JReadLine(input);
	line.TrimWhitespace();
	if (line.BeginsWith("code-medic:"))
		{
		line.RemoveSubstring(1, 11);
		line.TrimWhitespace();
		*language = line;
		}

	return !language->IsEmpty();
}
