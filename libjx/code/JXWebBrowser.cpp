/******************************************************************************
 JXWebBrowser.cpp

	This class is designed to be used as a global object.  All requests to
	display URL's and files should be passed to this object.  It contacts
	the appropriate program (e.g. Netscape) to display the data.

	BASE CLASS = JXSharedPrefObject

	Copyright © 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXWebBrowser.h>
#include <JXEditWWWPrefsDialog.h>
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXSharedPrefsManager.h>
#include <jXGlobals.h>
#include <JSimpleProcess.h>
#include <jFileUtil.h>
#include <jAssert.h>

// If you change the variable names, update JXHelpManager::WriteSetupV3()
// to convert them back to u,f,a.
//
// "netscape -noraise -remote \"openBrowser($u)\""
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

static const JXSharedPrefObject::VersionInfo kVersList[] =
{
	JXSharedPrefObject::VersionInfo(0, JXSharedPrefsManager::kWebBrowserV0),
	JXSharedPrefObject::VersionInfo(1, JXSharedPrefsManager::kWebBrowserV1)
};

const JSize kVersCount = sizeof(kVersList) / sizeof(JXSharedPrefObject::VersionInfo);

/******************************************************************************
 Constructor

 ******************************************************************************/

JXWebBrowser::JXWebBrowser()
	:
	JXSharedPrefObject(kCurrentPrefsVersion,
					   JXSharedPrefsManager::kLatestWebBrowserVersionID,
					   kVersList, kVersCount),
	itsShowURLCmd(kDefShowURLCmd),
	itsShowFileContentCmd(kDefShowFileContentCmd),
	itsShowFileLocationCmd(kDefShowFileLocationCmd),
	itsComposeMailCmd(kDefComposeMailCmd),
	itsPrefsDialog(NULL)
{
	JXSharedPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXWebBrowser::~JXWebBrowser()
{
}

/******************************************************************************
 ShowURL

 ******************************************************************************/

void
JXWebBrowser::ShowURL
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
JXWebBrowser::ShowFileContent
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
JXWebBrowser::ShowFileLocations
	(
	const JPtrArray<JString>& fileList
	)
{
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
			s += " '";
			s += *(fileList.NthElement(i));
			s += "'";
			}

		JSimpleProcess::Create(s, kJTrue);
		}
}

/******************************************************************************
 ShowFileLocation

 ******************************************************************************/

void
JXWebBrowser::ShowFileLocation
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
JXWebBrowser::ComposeMail
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
JXWebBrowser::Exec
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
 Outsourced commands

 ******************************************************************************/

void
JXWebBrowser::SetShowURLCmd
	(
	const JCharacter* cmd
	)
{
	itsShowURLCmd = cmd;
	JXSharedPrefObject::WritePrefs();
}

void
JXWebBrowser::SetShowFileContentCmd
	(
	const JCharacter* cmd
	)
{
	itsShowFileContentCmd = cmd;
	JXSharedPrefObject::WritePrefs();
}

void
JXWebBrowser::SetShowFileLocationCmd
	(
	const JCharacter* cmd
	)
{
	itsShowFileLocationCmd = cmd;
	JXSharedPrefObject::WritePrefs();
}

void
JXWebBrowser::SetComposeMailCmd
	(
	const JCharacter* cmd
	)
{
	itsComposeMailCmd = cmd;
	JXSharedPrefObject::WritePrefs();
}

/******************************************************************************
 EditPrefs

 ******************************************************************************/

void
JXWebBrowser::EditPrefs()
{
	assert( itsPrefsDialog == NULL );

	itsPrefsDialog = new JXEditWWWPrefsDialog(JXGetApplication(), itsShowURLCmd,
											  itsShowFileContentCmd,
											  itsShowFileLocationCmd,
											  itsComposeMailCmd);
	assert( itsPrefsDialog != NULL );
	itsPrefsDialog->BeginDialog();
	ListenTo(itsPrefsDialog);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXWebBrowser::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsPrefsDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast(const JXDialogDirector::Deactivated*, &message);
		assert( info != NULL );
		if (info->Successful())
			{
			itsPrefsDialog->GetPrefs(&itsShowURLCmd, &itsShowFileContentCmd,
									 &itsShowFileLocationCmd, &itsComposeMailCmd);
			JXSharedPrefObject::WritePrefs();
			}
		itsPrefsDialog = NULL;
		}

	else
		{
		JXSharedPrefObject::Receive(sender, message);
		}
}

/******************************************************************************
 ReadPrefs (virtual)

	We assert that we can read the given data because there is no
	way to skip over it.

 ******************************************************************************/

void
JXWebBrowser::ReadPrefs
	(
	istream& input
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
 WritePrefs (virtual)

	This is for use by JXGlobalPrefsManager.  Nobody else should store
	our settings.

 ******************************************************************************/

void
JXWebBrowser::WritePrefs
	(
	ostream&			output,
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
JXWebBrowser::ConvertVarNames
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
