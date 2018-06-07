/******************************************************************************
 SVNWDManager.cpp

	BASE CLASS = JXWDManager

	Copyright (C) 1997-2008 by John Lindal.

 ******************************************************************************/

#include "SVNWDManager.h"
#include "SVNMainDirector.h"
#include <JXChooseSaveFile.h>
#include <jXGlobals.h>
#include <jDirUtil.h>
#include <jWebUtil.h>
#include <jAssert.h>

// state information

const JFileVersion kCurrentStateVersion = 1;

	// version  1 restores open nodes in repo view

// string ID's

static const JCharacter* kNewBrowserInstrID = "NewBrowserInstr::SVNWDManager";

/******************************************************************************
 Constructor

 ******************************************************************************/

SVNWDManager::SVNWDManager
	(
	JXDisplay*		display,
	const JBoolean	wantShortcuts
	)
	:
	JXWDManager(display, wantShortcuts)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SVNWDManager::~SVNWDManager()
{
}

/******************************************************************************
 NewBrowser

 *****************************************************************************/

JBoolean
SVNWDManager::NewBrowser
	(
	SVNMainDirector** dir
	)
{
	JString path;
	if ((JXGetChooseSaveFile())->ChooseRWPath("", JGetString(kNewBrowserInstrID),
											  nullptr, &path))
		{
		*dir = OpenDirectory(path);
		return kJTrue;
		}
	else
		{
		*dir = nullptr;
		return kJFalse;
		}
}

/******************************************************************************
 GetBrowser

 *****************************************************************************/

JBoolean
SVNWDManager::GetBrowser
	(
	const JCharacter*	path,
	SVNMainDirector**	dir
	)
{
	*dir = nullptr;

	JString p1;
	JBoolean isURL = JIsURL(path);
	if (isURL)
		{
		p1 = path;
		JAppendDirSeparator(&p1);
		}
	else if (!JGetTrueName(path, &p1))
		{
		return kJFalse;
		}

	JPtrArray<JXWindowDirector> windowList(JPtrArrayT::kForgetAll);
	GetDirectors(&windowList);

	const JSize windowCount = windowList.GetElementCount();
	JString p2, p3;
	for (JIndex i=1; i<=windowCount; i++)
		{
		SVNMainDirector* d = dynamic_cast<SVNMainDirector*>(windowList.GetElement(i));
		if (d == nullptr)
			{
			continue;
			}

		if (isURL && d->GetRepoPath(&p2))
			{
			JAppendDirSeparator(&p2);
			if (p1.BeginsWith(p2))
				{
				*dir = d;
				break;
				}
			}
		else if (!isURL && d->GetPath(&p3) &&
				 (JSameDirEntry(p1, p3) ||
				  (JGetTrueName(p3, &p2) && p1.BeginsWith(p2))))
			{
			*dir = d;
			break;
			}
		}

	return JI2B( *dir != nullptr );
}

/******************************************************************************
 GetBrowserForExactURL

 *****************************************************************************/

JBoolean
SVNWDManager::GetBrowserForExactURL
	(
	const JCharacter*	url,
	SVNMainDirector**	dir
	)
{
	*dir = nullptr;

	JString p1 = url;
	JStripTrailingDirSeparator(&p1);

	JPtrArray<JXWindowDirector> windowList(JPtrArrayT::kForgetAll);
	GetDirectors(&windowList);

	const JSize windowCount = windowList.GetElementCount();
	JString p2;
	for (JIndex i=1; i<=windowCount; i++)
		{
		SVNMainDirector* d = dynamic_cast<SVNMainDirector*>(windowList.GetElement(i));
		if (d != nullptr && d->GetRepoPath(&p2) && p1 == p2)
			{
			*dir = d;
			break;
			}
		}

	return JI2B( *dir != nullptr );
}

/******************************************************************************
 OpenDirectory

 *****************************************************************************/

SVNMainDirector*
SVNWDManager::OpenDirectory
	(
	const JCharacter*	path,
	JBoolean*			wasOpen		// can be nullptr
	)
{
	SVNMainDirector* dir;
	const JBoolean open = GetBrowser(path, &dir);
	if (wasOpen != nullptr)
		{
		*wasOpen = open;
		}

	if (!open)
		{
		dir = jnew SVNMainDirector(JXGetApplication(), path);
		assert( dir != nullptr );
		}

	dir->Activate();
	return dir;
}

/******************************************************************************
 RestoreState

	Reopens files that were open when we quit the last time.  Returns kJFalse
	if nothing is opened.

 ******************************************************************************/

JBoolean
SVNWDManager::RestoreState
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentStateVersion)
		{
		return kJFalse;
		}

	JSize windowCount;
	input >> windowCount;

	JString fileName;
	for (JIndex i=1; i<=windowCount; i++)
		{
		SVNMainDirector* dir = jnew SVNMainDirector(JXGetApplication(), input, vers);
		assert( dir != nullptr );
		dir->Activate();
		}

	return JI2B( windowCount > 0 );
}

/******************************************************************************
 SaveState

	Saves files that are currently open so they can be reopened next time.
	Returns kJFalse if there is nothing to save.

	Always calls WriteForProject() because project documents might be
	write protected, e.g., CVS.

 ******************************************************************************/

JBoolean
SVNWDManager::SaveState
	(
	std::ostream& output
	)
	const
{
	JPtrArray<JXWindowDirector> windowList(JPtrArrayT::kForgetAll);
	GetDirectors(&windowList);
	if (windowList.IsEmpty())
		{
		return kJFalse;
		}

	output << kCurrentStateVersion;

	const JSize windowCount = windowList.GetElementCount();
	output << ' ' << windowCount;

	for (JIndex i=1; i<=windowCount; i++)
		{
		SVNMainDirector* dir = dynamic_cast<SVNMainDirector*>(windowList.GetElement(i));

		output << ' ';
		dir->StreamOut(output);
		}

	return kJTrue;
}
