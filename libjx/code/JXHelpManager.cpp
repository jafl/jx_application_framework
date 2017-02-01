/******************************************************************************
 JXHelpManager.cpp

	This class is designed to be used as a global object.  All requests by
	the user to display help information should be passed to this object.
	It creates and maintains JXHelpDirectors to display the text.

	Every help section needs to be registered by calling RegisterSection().
	The name is a unique string that identifies the section.  The title is
	used as the window title.  All three strings must be stored in static
	data because we do not want to have to copy them, especially the
	text itself.  The format of the text must be HTML.

	After being registered, all requests to display the section are done by
	name.  This allows JXHelpText to handle hypertext links by parsing URL's
	of the form <a href="jxhelp:name_of_section#name_of_subsection">.  All
	other URL's are passed to user specified commands (usually netscape).

	PrintAll() is not supported because hypertext does not work well on
	paper.  Instead, one should provide an equivalent PostScript file with
	a Table of Contents that lists page numbers.  This PostScript file can
	also be considerably spruced up with headers, footers, graphics, and
	an index.

	BASE CLASS = JXDirector, JXSharedPrefObject

	Copyright (C) 1997-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXHelpManager.h>
#include <JXHelpDirector.h>
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXPSPrinter.h>
#include <JXSharedPrefsManager.h>
#include <JXWebBrowser.h>
#include <JXStandAlonePG.h>
#include <jXGlobals.h>
#include <JStringIterator.h>
#include <jAssert.h>

static const JUtf8Byte* kJXHelpPrefix = "jxhelp:";
const JSize kJXHelpPrefixLength       = 7;

static const JUtf8Byte* kSubsectionMarker = "#";
const JSize kSubsectionMarkerLength       = 1;

// setup information

const JFileVersion kCurrentPrefsVersion = 4;

	// version 1 stores itsDefWindowGeom
	// version 2 converts variable marker from % to $
	// version 3 stores PS print setup
	// version 4 moved url and mail commands to JXWebBrowser

static const JXSharedPrefObject::VersionInfo kVersList[] =
{
	JXSharedPrefObject::VersionInfo(3, JXSharedPrefsManager::kHelpMgrV3),
	JXSharedPrefObject::VersionInfo(4, JXSharedPrefsManager::kHelpMgrV4)
};

const JSize kVersCount = sizeof(kVersList) / sizeof(JXSharedPrefObject::VersionInfo);

/******************************************************************************
 Constructor

 ******************************************************************************/

JXHelpManager::JXHelpManager()
	:
	JXDirector(NULL),
	JXSharedPrefObject(kCurrentPrefsVersion,
					   JXSharedPrefsManager::kLatestHelpMgrVersionID,
					   kVersList, kVersCount),
	itsTOCSectionName(NULL),
	itsComposeHelpDir(NULL)
{
	itsSections = jnew JArray<SectionInfo>;
	assert( itsSections != NULL );
	itsSections->SetCompareFunction(CompareSections);

	JXDisplay* display = (JXGetApplication())->GetCurrentDisplay();
	itsPrinter = jnew JXPSPrinter(display);
	assert( itsPrinter != NULL );

	JXSharedPrefObject::ReadPrefs();

	ListenTo(itsPrinter);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXHelpManager::~JXHelpManager()
{
	jdelete itsSections;		// JXHelpDirectors deleted by JXDirector
	jdelete itsPrinter;
}

/******************************************************************************
 RegisterSection

 ******************************************************************************/

void
JXHelpManager::RegisterSection
	(
	const JUtf8Byte* name
	)
{
	SectionInfo info(name);

	JBoolean isDuplicate;
	const JIndex index = itsSections->GetInsertionSortIndex(info, &isDuplicate);
	if (!isDuplicate)
		{
		itsSections->InsertElementAtIndex(index, info);
		}
}

/******************************************************************************
 ShowTOC

	The first registered section is assumed to be the Table of Contents.

 ******************************************************************************/

void
JXHelpManager::ShowTOC()
{
	if (itsTOCSectionName != NULL)
		{
		ShowSection(itsTOCSectionName);
		}
	else
		{
		(JGetUserNotification())->ReportError(JGetString("MissingTOC::JXHelpManager"));
		}
}

/******************************************************************************
 ShowSection

	The JXHelpDirector* can be NULL.  If it isn't, and the section name is
	"#name_of_subsection", the director is searched for the subsection.

 ******************************************************************************/

void
JXHelpManager::ShowSection
	(
	const JUtf8Byte*	origName,
	JXHelpDirector*		helpDir
	)
{
	// check for subsection name

	JPtrArray<JString> list(JPtrArrayT::kDeleteAll);
	JString(origName, 0, kJFalse).Split(kSubsectionMarker, &list, 2);

	const JString name = *(list.GetElement(1));

	JString subsectionName;
	if (list.GetElementCount() > 1)
		{
		subsectionName = *(list.GetElement(2));
		}

	// find section name

	SectionInfo target;
	target.name = name.GetBytes();

	if (name.IsEmpty())
		{
		if (helpDir == NULL)
			{
			return;
			}
		target.dir = helpDir;
		}
	else
		{
		JBoolean found = kJFalse;

		JIndex i;
		if (itsSections->SearchSorted(target, JListT::kAnyMatch, &i))
			{
			target = itsSections->GetElement(i);
			JString* text;
			if (target.dir != NULL)
				{
				found = kJTrue;
				}
			else if ((JGetStringManager())->GetElement(
						JString(target.name, 0, kJFalse), &text))
				{
				target.dir = CreateHelpDirector(*text);
				itsSections->SetElement(i, target);
				found = kJTrue;
				}
			}

		if (!found)
			{
			const JUtf8Byte* map[] =
				{
				"name", name.GetBytes()
				};
			const JString msg = JGetString("MissingSection::JXHelpManager", map, sizeof(map));
			(JGetUserNotification())->ReportError(msg);
			return;
			}
		}

	(target.dir)->ShowSubsection(subsectionName.GetBytes());
	(target.dir)->Activate();
}

/******************************************************************************
 SearchAllSections

 ******************************************************************************/

void
JXHelpManager::SearchAllSections()
{
	const JSize count = itsSections->GetElementCount();

	JXStandAlonePG pg;
	pg.RaiseWhenUpdate();
	pg.FixedLengthProcessBeginning(count, JGetString("SearchingLabel::JXHelpManager"), kJTrue, kJFalse);

	for (JIndex i=1; i<=count; i++)
		{
		SectionInfo info = itsSections->GetElement(i);
		JBoolean hadDir  = kJTrue;

		JString* text;
		if (info.dir == NULL &&
			(JGetStringManager())->GetElement(
				JString(info.name, 0, kJFalse), &text))
			{
			hadDir   = kJFalse;
			info.dir = CreateHelpDirector(*text);
			}

		if ((info.dir)->Search())
			{
			(info.dir)->Activate();
			itsSections->SetElement(i, info);
			}
		else if (!hadDir)
			{
			(info.dir)->Close();
			}

		if (!pg.IncrementProgress())
			{
			break;
			}
		}

	pg.ProcessFinished();
}

/******************************************************************************
 CreateHelpDirector (private)

 ******************************************************************************/

JXHelpDirector*
JXHelpManager::CreateHelpDirector
	(
	const JString& text
	)
{
	JXHelpDirector* dir = jnew JXHelpDirector(text, itsPrinter);
	assert( dir != NULL );

	if (!itsDefWindowGeom.IsEmpty())
		{
		(dir->GetWindow())->ReadGeometry(itsDefWindowGeom);
		}
	else
		{
		(dir->GetWindow())->PlaceAsDialogWindow();
		}

	return dir;
}

/******************************************************************************
 ShowURL

	helpDir can be NULL.

 ******************************************************************************/

void
JXHelpManager::ShowURL
	(
	const JString&	url,
	JXHelpDirector*	helpDir
	)
{
	// check for jxhelp

	JString s = url;
	if (IsLocalURL(s))
		{
		JStringIterator iter(&s);
		iter.Next(kJXHelpPrefix);	// already know it starts with this prefix
		iter.RemoveAllPrev();
		ShowSection(s.GetBytes(), helpDir);
		}

	// outsource all other URLs

	else
		{
		(JXGetWebBrowser())->ShowURL(url);
		}
}

/******************************************************************************
 IsLocalURL (static)

 ******************************************************************************/

JBoolean
JXHelpManager::IsLocalURL
	(
	const JString& url
	)
{
	return url.BeginsWith(kJXHelpPrefix, kJFalse);
}

/*****************************************************************************
 CloseAll

	Close all open help windows.

 ******************************************************************************/

void
JXHelpManager::CloseAll()
{
	const JSize count = itsSections->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		SectionInfo info = itsSections->GetElement(i);
		if (info.dir != NULL)
			{
			(info.dir)->Close();
			// DirectorClosed() clears pointer
			}
		}

	if (itsComposeHelpDir != NULL)
		{
		itsComposeHelpDir->Close();
		itsComposeHelpDir = NULL;
		}
}

/******************************************************************************
 SaveWindowPrefs

	The window will never be iconified because this is triggered by a menu
	item without a shortcut.

 ******************************************************************************/

void
JXHelpManager::SaveWindowPrefs
	(
	JXWindow* window
	)
{
	window->WriteGeometry(&itsDefWindowGeom);
	JXSharedPrefObject::WritePrefs();
}

/******************************************************************************
 ReadPrefs (virtual)

	We assert that we can read the given data because there is no
	way to skip over it.

 ******************************************************************************/

void
JXHelpManager::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	assert( vers <= kCurrentPrefsVersion );

	if (vers < 4)
		{
		JString showURLCmd, composeMailCmd;
		input >> showURLCmd >> composeMailCmd;
		if (vers < 2)
			{
			JXWebBrowser::ConvertVarNames(&showURLCmd,     "u");
			JXWebBrowser::ConvertVarNames(&composeMailCmd, "a");
			}
		(JXGetWebBrowser())->SetShowURLCmd(showURLCmd);
		(JXGetWebBrowser())->SetComposeMailCmd(composeMailCmd);
		}

	if (vers >= 1)
		{
		input >> itsDefWindowGeom;
		}

	if (vers >= 3)
		{
		itsPrinter->ReadXPSSetup(input);
		}
}

/******************************************************************************
 WritePrefs (virtual)

	This is for use by JXGlobalPrefsManager.  Nobody else should store
	our settings.

 ******************************************************************************/

void
JXHelpManager::WritePrefs
	(
	std::ostream&			output,
	const JFileVersion	vers
	)
	const
{
	if (vers == 3)
		{
		output << ' ' << 3;		// version

		output << ' ' << (JXGetWebBrowser())->GetShowURLCmd();
		output << ' ' << (JXGetWebBrowser())->GetComposeMailCmd();
		output << ' ' << itsDefWindowGeom;

		output << ' ';
		itsPrinter->WriteXPSSetup(output);
		}
	else
		{
		output << ' ' << 4;		// version
		output << ' ' << itsDefWindowGeom;

		output << ' ';
		itsPrinter->WriteXPSSetup(output);
		}

	output << ' ';
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXHelpManager::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsPrinter &&
		(message.Is(JPrinter::kPageSetupFinished) ||
		 message.Is(JPrinter::kPrintSetupFinished)))
		{
		JXSharedPrefObject::WritePrefs();
		}
	else
		{
		JXDirector::Receive(sender, message);
		JXSharedPrefObject::Receive(sender, message);
		}
}

/*****************************************************************************
 DirectorClosed (virtual protected)

	Listen for help directors that are closed.

 ******************************************************************************/

void
JXHelpManager::DirectorClosed
	(
	JXDirector* theDirector
	)
{
	const JSize count = itsSections->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		SectionInfo info = itsSections->GetElement(i);
		if (theDirector == info.dir)
			{
			info.dir = NULL;
			itsSections->SetElement(i, info);
			break;
			}
		}

	if (theDirector == itsComposeHelpDir)
		{
		itsComposeHelpDir = NULL;
		}

	JXDirector::DirectorClosed(theDirector);
}

/******************************************************************************
 CompareSections (static private)

 ******************************************************************************/

JListT::CompareResult
JXHelpManager::CompareSections
	(
	const SectionInfo& s1,
	const SectionInfo& s2
	)
{
	const int r = JString::Compare(s1.name, s2.name, kJFalse);

	if (r > 0)
		{
		return JListT::kFirstGreaterSecond;
		}
	else if (r < 0)
		{
		return JListT::kFirstLessSecond;
		}
	else
		{
		return JListT::kFirstEqualSecond;
		}
}
