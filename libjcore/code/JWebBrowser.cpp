/******************************************************************************
 JWebBrowser.cpp

	This class is designed to be used as a global object.  All requests to
	display URL's and files should be passed to this object.  It contacts
	the appropriate program (e.g. Firefox) to display the data.

	Copyright (C) 2000-10 by John Lindal.

 ******************************************************************************/

#include "JWebBrowser.h"
#include "JSimpleProcess.h"
#include "JStringIterator.h"
#include "jFileUtil.h"
#include "jGlobals.h"
#include "jAssert.h"

// If you change the variable names, update JXHelpManager::WriteSetupV3()
// to convert them back to u,f,a.
//
// "arrow --mailto \"$a\""

#ifdef _J_MACOS
static const JUtf8Byte* kDefShowURLCmd = "open $u";
#else
static const JUtf8Byte* kDefShowURLCmd = "firefox $u";
#endif
static const JUtf8Byte* kURLVarName    = "u";

static const JUtf8Byte* kFileURLPrefix          = "file:";
#ifdef _J_MACOS
static const JUtf8Byte* kDefShowFileContentCmd  = "open $f";
#else
static const JUtf8Byte* kDefShowFileContentCmd  = "firefox file:'$f'";
#endif
static const JUtf8Byte* kDefShowFileLocationCmd = "systemg --no-force-new";
static const JUtf8Byte* kFileVarName            = "f";
static const JUtf8Byte* kPathVarName            = "p";

static const JUtf8Byte* kMailURLPrefix      = "mailto:";
#ifdef _J_MACOS
static const JUtf8Byte* kDefComposeMailCmd  = "open mailto:$a";
#else
static const JUtf8Byte* kDefComposeMailCmd  = "firefox mailto:$a";
#endif
static const JUtf8Byte* kMailAddressVarName = "a";

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
	const JString& url
	)
{
	if (url.BeginsWith(kMailURLPrefix))
	{
		ComposeMail(JString(url,
			JCharacterRange(strlen(kMailURLPrefix)+1, url.GetCharacterCount()),
			JString::kNoCopy));
		return;
	}

	if (url.BeginsWith(kFileURLPrefix))
	{
		ShowFileContent(JString(url,
			JCharacterRange(strlen(kFileURLPrefix)+1, url.GetCharacterCount()),
			JString::kNoCopy));
		return;
	}

	Exec(itsShowURLCmd, kURLVarName, url);
}

/******************************************************************************
 ShowFileContent

 ******************************************************************************/

void
JWebBrowser::ShowFileContent
	(
	const JString& fileName
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

	if (itsShowFileLocationCmd.Contains("$"))
	{
		for (const auto* f : fileList)
		{
			ShowFileLocation(*f);
		}
	}
	else
	{
		JString s = itsShowFileLocationCmd;
		for (const auto* f : fileList)
		{
			s += " ";
			s += JPrepArgForExec(*f);
		}

		JSimpleProcess::Create(s, true);
	}
}

/******************************************************************************
 ShowFileLocation

 ******************************************************************************/

void
JWebBrowser::ShowFileLocation
	(
	const JString& fileName
	)
{
	if (!itsShowFileLocationCmd.IsEmpty())
	{
		JString fullName = fileName;
		JStripTrailingDirSeparator(&fullName);

		JString path, name;
		JSplitPathAndName(fullName, &path, &name);

		const JUtf8Byte* map[] =
		{
			kFileVarName, fullName.GetBytes(),
			kPathVarName, path.GetBytes()
		};

		JString s = itsShowFileLocationCmd;
		if (!s.Contains("$"))
		{
			s += " '$";
			s += kFileVarName;
			s += "'";
		}
		JGetStringManager()->Replace(&s, map, sizeof(map));
		JSimpleProcess::Create(s, true);
	}
}

/******************************************************************************
 ComposeMail

 ******************************************************************************/

void
JWebBrowser::ComposeMail
	(
	const JString& address
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
	const JString&		cmd,
	const JUtf8Byte*	varName,
	const JString&		value
	)
	const
{
	if (!cmd.IsEmpty())
	{
		const JUtf8Byte* map[] =
		{
			varName, value.GetBytes()
		};

		JString s = cmd;
		JGetStringManager()->Replace(&s, map, sizeof(map));
		JSimpleProcess::Create(s, true);
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
	std::ostream&		output,
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
	const JUtf8Byte*	varNameList
	)
{
	// escape existing backslashes

	JStringIterator iter(s);
	while (iter.Next("\\"))
	{
		iter.ReplaceLastMatch("\\\\");
	}

	// escape existing dollars

	iter.MoveTo(kJIteratorStartAtBeginning, 0);
	while (iter.Next("$"))
	{
		iter.ReplaceLastMatch("\\$");
	}

	// convert % to $ if followed by a variable name

	iter.MoveTo(kJIteratorStartAtBeginning, 0);
	JUtf8Character c;
	while (iter.Next("%") && iter.Next(&c))
	{
		if (strstr(varNameList, c.GetBytes()) != nullptr)
		{
			iter.ReplaceLastMatch("$");
		}
	}
}
