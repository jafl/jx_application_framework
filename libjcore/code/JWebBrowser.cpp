/******************************************************************************
 JWebBrowser.cpp

	This class is designed to be used as a global object.  All requests to
	display URL's and files should be passed to this object.  It contacts
	the appropriate program (e.g. Firefox) to display the data.

	Copyright (C) 2000-10 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JWebBrowser.h>
#include <JSimpleProcess.h>
#include <jFileUtil.h>
#include <jGlobals.h>
#include <jAssert.h>

// If you change the variable names, update JXHelpManager::WriteSetupV3()
// to convert them back to u,f,a.
//
// "arrow --mailto \"$a\""

#ifdef _J_OSX
static const JCharacter* kDefShowURLCmd = "open $u";
#else
static const JCharacter* kDefShowURLCmd = "firefox $u";
#endif
static const JCharacter* kURLVarName    = "u";

static const JCharacter* kFileURLPrefix          = "file:";
const JSize kFileURLPrefixLength                 = strlen(kFileURLPrefix);
#ifdef _J_OSX
static const JCharacter* kDefShowFileContentCmd  = "open $f";
#else
static const JCharacter* kDefShowFileContentCmd  = "firefox file:'$f'";
#endif
static const JCharacter* kDefShowFileLocationCmd = "systemg --no-force-new";
static const JCharacter* kFileVarName            = "f";
static const JCharacter* kPathVarName            = "p";

static const JCharacter* kMailURLPrefix      = "mailto:";
const JSize kMailURLPrefixLength             = strlen(kMailURLPrefix);
#ifdef _J_OSX
static const JCharacter* kDefComposeMailCmd  = "open mailto:$a";
#else
static const JCharacter* kDefComposeMailCmd  = "firefox mailto:$a";
#endif
static const JCharacter* kMailAddressVarName = "a";

// setup information

const JFileVersion kCurrentPrefsVersion = 1;

	// version  1 includes itsShowFileLocationCmd

/******************************************************************************
 Constructor

 ******************************************************************************/

JWebBrowser::JWebBrowser()
	:
	itsShowURLCmd(kDefShowURLCmd),
	itsShowFileContentCmd(kDefShowFileContentCmd),
	itsShowFileLocationCmd(kDefShowFileLocationCmd),
	itsComposeMailCmd(kDefComposeMailCmd)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JWebBrowser::~JWebBrowser()
{
}

/******************************************************************************
 ShowURL

 ******************************************************************************/

void
JWebBrowser::ShowURL
	(
	const JCharacter* url
	)
{
	JString s = url;
	if (s.BeginsWith(kMailURLPrefix, kJFalse))
		{
		s.RemoveSubstring(1, kMailURLPrefixLength);
		ComposeMail(s);
		}
	else if (s.BeginsWith(kFileURLPrefix, kJFalse))
		{
		s.RemoveSubstring(1, kFileURLPrefixLength);
		ShowFileContent(s);
		}
	else
		{
		Exec(itsShowURLCmd, kURLVarName, url);
		}
}

/******************************************************************************
 ShowFileContent

 ******************************************************************************/

void
JWebBrowser::ShowFileContent
	(
	const JCharacter* fileName
	)
{
	Exec(itsShowFileContentCmd, kFileVarName, fileName);
}

/******************************************************************************
 ShowFileLocations

 ******************************************************************************/

void
JWebBrowser::ShowFileLocations
	(
	const JPtrArray<JString>& fileList
	)
{
	if (fileList.IsEmpty())
		{
		return;
		}

	const JSize count = fileList.GetElementCount();
	if (itsShowFileLocationCmd.Contains("$"))
		{
		for (JIndex i=1; i<=count; i++)
			{
			ShowFileLocation(*(fileList.NthElement(i)));
			}
		}
	else
		{
		JString s = itsShowFileLocationCmd;
		for (JIndex i=1; i<=count; i++)
			{
			s += " ";
			s += JPrepArgForExec(*(fileList.NthElement(i)));
			}

		JSimpleProcess::Create(s, kJTrue);
		}
}

/******************************************************************************
 ShowFileLocation

 ******************************************************************************/

void
JWebBrowser::ShowFileLocation
	(
	const JCharacter* fileName
	)
{
	if (!JStringEmpty(itsShowFileLocationCmd))
		{
		JString fullName = fileName;
		JStripTrailingDirSeparator(&fullName);

		JString path, name;
		JSplitPathAndName(fullName, &path, &name);

		const JCharacter* map[] =
			{
			kFileVarName, fullName,
			kPathVarName, path
			};

		JString s = itsShowFileLocationCmd;
		if (!s.Contains("$"))
			{
			s += " '$";
			s += kFileVarName;
			s += "'";
			}
		(JGetStringManager())->Replace(&s, map, sizeof(map));
		JSimpleProcess::Create(s, kJTrue);
		}
}

/******************************************************************************
 ComposeMail

 ******************************************************************************/

void
JWebBrowser::ComposeMail
	(
	const JCharacter* address
	)
{
	Exec(itsComposeMailCmd, kMailAddressVarName, address);
}

/******************************************************************************
 Exec (private)

 ******************************************************************************/

void
JWebBrowser::Exec
	(
	const JCharacter* cmd,
	const JCharacter* varName,
	const JCharacter* value
	)
	const
{
	if (!JStringEmpty(cmd))
		{
		const JCharacter* map[] =
			{
			varName, value
			};

		JString s = cmd;
		(JGetStringManager())->Replace(&s, map, sizeof(map));
		JSimpleProcess::Create(s, kJTrue);
		}
}

/******************************************************************************
 SaveCommands (virtual protected)

	Derived classes can override to save state.

 ******************************************************************************/

void
JWebBrowser::SaveCommands()
{
}

/******************************************************************************
 GetCurrentConfigVersion (static)

 ******************************************************************************/

JFileVersion
JWebBrowser::GetCurrentConfigVersion()
{
	return kCurrentPrefsVersion;
}

/******************************************************************************
 ReadConfig

	We assert that we can read the given data because there is no
	way to skip over it.

 ******************************************************************************/

void
JWebBrowser::ReadConfig
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	assert( vers <= kCurrentPrefsVersion );

	input >> itsShowURLCmd >> itsShowFileContentCmd >> itsComposeMailCmd;

	if (vers >= 1)
		{
		input >> itsShowFileLocationCmd;
		}
}

/******************************************************************************
 WriteConfig

	This is for use by JXGlobalPrefsManager.  Nobody else should store
	our settings.

 ******************************************************************************/

void
JWebBrowser::WriteConfig
	(
	std::ostream&			output,
	const JFileVersion	vers
	)
	const
{
	if (vers == 0)
		{
		output << ' ' << 0;		// version
		output << ' ' << itsShowURLCmd;
		output << ' ' << itsShowFileContentCmd;
		output << ' ' << itsComposeMailCmd;
		}
	else
		{
		output << ' ' << 1;		// version
		output << ' ' << itsShowURLCmd;
		output << ' ' << itsShowFileContentCmd;
		output << ' ' << itsComposeMailCmd;
		output << ' ' << itsShowFileLocationCmd;
		}

	output << ' ';
}

/******************************************************************************
 ConvertVarNames (static)

	Convert % to $ when followed by any character in varNameList.
	Backslashes and dollars are also backslashed, as required by JSubstitute.

 ******************************************************************************/

void
JWebBrowser::ConvertVarNames
	(
	JString*			s,
	const JCharacter*	varNameList
	)
{
	// escape existing backslashes

	JIndex i = 1;
	while (s->LocateNextSubstring("\\", &i))
		{
		s->InsertSubstring("\\", i);
		i += 2;		// move past both backslashes
		}

	// escape existing dollars

	i = 1;
	while (s->LocateNextSubstring("$", &i))
		{
		s->InsertSubstring("\\", i);
		i += 2;		// move past $
		}

	// convert % to $ if followed by a variable name

	i = 1;
	while (s->LocateNextSubstring("%", &i) && i < s->GetLength())
		{
		const JCharacter c = s->GetCharacter(i+1);
		if (strchr(varNameList, c) != NULL)
			{
			s->SetCharacter(i, '$');
			}
		i += 2;		// move past variable name
		}
}
