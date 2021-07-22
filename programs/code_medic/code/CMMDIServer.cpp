/******************************************************************************
 CMMDIServer.cpp

	BASE CLASS = public JXMDIServer

	Copyright (C) 1997 by Glenn W. Bach.

 *****************************************************************************/

#include "CMMDIServer.h"
#include "CMCommandDirector.h"
#include "cmGlobals.h"
#include <JStyledText.h>
#include <JStringIterator.h>
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
	itsFirstTimeFlag(true)
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
	const JString&				dir,
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
	bool forcedType = false;
	JPtrArray<JString> fileList(JPtrArrayT::kDeleteAll);
	JArray<JIndex> lineIndexList;
	JArray<bool> breakList;

	const JSize count = argList.GetElementCount();
	for (JIndex i=2; i<=count; i++)
		{
		const JString* arg = argList.GetElement(i);

		if (*arg == "--gdb")
			{
			forcedType = true;
			CMGetPrefsManager()->SetDebuggerType(CMPrefsManager::kGDBType);
			continue;
			}
		else if (*arg == "--lldb")
			{
			forcedType = true;
			CMGetPrefsManager()->SetDebuggerType(CMPrefsManager::kLLDBType);
			continue;
			}
		else if (*arg == "--xdebug")
			{
			forcedType = true;
			CMGetPrefsManager()->SetDebuggerType(CMPrefsManager::kXdebugType);
			continue;
			}
		else if (*arg == "--java")
			{
			forcedType = true;
			CMGetPrefsManager()->SetDebuggerType(CMPrefsManager::kJavaType);
			continue;
			}
		else if (*arg == "-f")
			{
			context = kWaitingForFile;
			continue;
			}
		else if (*arg == "-c")
			{
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
			breakList.AppendElement(context == kWaitingForBreakpoint);

			const JString* arg = argList.GetElement(i);
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
				JPtrArray<JString> s(JPtrArrayT::kDeleteAll);
				arg->Split(":", &s, 2);

				fileList.Append(s.GetFirstElement());
				s.RemoveElement(1);

				JInteger value = 0;
				if (!s.IsEmpty())
					{
					s.GetFirstElement()->ConvertToInteger(&value);
					}
				lineIndexList.AppendElement(value);

				context = kWaitingForProgram;
				}
			}
		else if (context == kWaitingForName)
			{
			fileList.Append(*arg);
			context = kWaitingForProgram;
			}
		}

	const bool wasFirstTime = itsFirstTimeFlag;
	if (itsFirstTimeFlag)
		{
		if (!program.IsEmpty())
			{
			if (!forcedType)
				{
				UpdateDebuggerType(program);
				}

			CMGetLink()->SetProgram(program);
			}

		itsFirstTimeFlag = false;
		}

	if (!core.IsEmpty())
		{
		CMGetLink()->SetCore(core);
		}

	const JSize fileCount = fileList.GetElementCount();
	for (JIndex i=1; i<=fileCount; i++)
		{
		const JString* fileName = fileList.GetElement(i);
		const JIndex lineIndex  = lineIndexList.GetElement(i);
		if (breakList.GetElement(i) && lineIndex > 0)
			{
			if (wasFirstTime)
				{
				std::cerr << "-b option only works when Code Medic is already running" << std::endl;
				}
			else
				{
				CMGetLink()->SetBreakpoint(*fileName, lineIndex);
				CMLink::NotifyUser("Breakpoint set in " + *fileName + " at line " + JString((JUInt64) lineIndex) + "\n\n", false);
				}
			}
		else
			{
			cmdDir->OpenSourceFile(*fileName, lineIndex, false);
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
	const JString& program
	)
{
	JString language;
	if (IsBinary(program))
		{
		const CMPrefsManager::DebuggerType type = CMGetPrefsManager()->GetDebuggerType();
		if (type != CMPrefsManager::kGDBType &&
			type != CMPrefsManager::kLLDBType)
			{
			CMGetPrefsManager()->SetDebuggerType(
#ifdef _J_OSX
				CMPrefsManager::kLLDBType
#else
				CMPrefsManager::kGDBType
#endif
			);
			}
		}
	else if (!GetLanguage(program, &language))
		{
		// accept current debugging mode
		}
	else if (language == "java")
		{
		CMGetPrefsManager()->SetDebuggerType(CMPrefsManager::kJavaType);
		}
	else if (language == "php")
		{
		CMGetPrefsManager()->SetDebuggerType(CMPrefsManager::kXdebugType);
		}
}

/******************************************************************************
 IsBinary (static)

 *****************************************************************************/

bool
CMMDIServer::IsBinary
	(
	const JString& fileName
	)
{
	JUtf8Byte buffer[1000];
	memset(buffer, ' ', 1000);

	std::ifstream input(fileName.GetBytes());
	input.read(buffer, 1000);

	const bool ignore = JUtf8Character::IgnoreBadUtf8();
	JUtf8Character::SetIgnoreBadUtf8(true);

	JSize byteCount;
	for (JIndex i = 0; i < 995; )
		{
		if (!JUtf8Character::GetCharacterByteCount(buffer + i, &byteCount))
			{
			JUtf8Character::SetIgnoreBadUtf8(ignore);
			return false;
			}
		i += byteCount;
		}

	JUtf8Character::SetIgnoreBadUtf8(ignore);
	return true;
}

/******************************************************************************
 GetLanguage (static)

 *****************************************************************************/

bool
CMMDIServer::GetLanguage
	(
	const JString&	fileName,
	JString*		language
	)
{
	language->Clear();

	std::ifstream input(fileName.GetBytes());

	const bool ignore = JUtf8Character::IgnoreBadUtf8();
	JUtf8Character::SetIgnoreBadUtf8(true);
	JString line = JReadLine(input);
	JUtf8Character::SetIgnoreBadUtf8(ignore);

	line.TrimWhitespace();
	if (line.BeginsWith("code-medic:"))
		{
		JStringIterator iter(&line);
		iter.RemoveNext(11);
		line.TrimWhitespace();
		*language = line;
		}

	return !language->IsEmpty();
}
