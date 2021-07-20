/******************************************************************************
 SVNInfoLog.cpp

	BASE CLASS = SVNTextBase

	Copyright @ 2008 by John Lindal.

 ******************************************************************************/

#include "SVNInfoLog.h"
#include "SVNMainDirector.h"
#include "svnMenus.h"
#include <JXTextMenu.h>
#include <JXColorManager.h>
#include <jXGlobals.h>
#include <JProcess.h>
#include <JRegex.h>
#include <JStringIterator.h>
#include <JStringMatch.h>
#include <jStreamUtil.h>
#include <jAssert.h>

static const JRegex revisionPattern = "^r([0-9]+)$";

// Context menu

static const JUtf8Byte* kContextMenuStr =
	"    Compare with edited"
	"  | Compare with current"
	"  | Compare with previous"
	"%l| Details"
	"%l| Browse this revision";

enum
{
	kDiffEditedSelectedFilesCtxCmd = 1,
	kDiffCurrentSelectedFilesCtxCmd,
	kDiffPrevSelectedFilesCtxCmd,
	kCommitDetailsCtxCmd,
	kBrowseRepoRevisionCtxCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

SVNInfoLog::SVNInfoLog
	(
	SVNMainDirector*	director,
	JXTextMenu*			editMenu,
	const JString&		fullName,
	const JString&		rev,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption hSizing,
	const VSizingOption vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	SVNTextBase(director, editMenu, scrollbarSet, enclosure, hSizing, vSizing, x, y, w, h),
	itsFullName(fullName),
	itsRevision(rev),
	itsContextMenu(nullptr)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SVNInfoLog::~SVNInfoLog()
{
}

/******************************************************************************
 StartProcess (virtual protected)

 ******************************************************************************/

JError
SVNInfoLog::StartProcess
	(
	JProcess**	p,
	int*		outFD
	)
{
	JString cmd = "svn --non-interactive info " + JPrepArgForExec(itsFullName);
	Execute(cmd);

	cmd = "svn --non-interactive log ";
	if (!itsRevision.IsEmpty())
		{
		cmd += "-v -r ";
		cmd += JPrepArgForExec(itsRevision);
		cmd += " ";
		}
	cmd += JPrepArgForExec(itsFullName);

	return JProcess::Create(p, cmd,
							kJIgnoreConnection, nullptr,
							kJForceNonblockingPipe, outFD,
							kJAttachToFromFD);
}

/******************************************************************************
 Execute (private)

 ******************************************************************************/

void
SVNInfoLog::Execute
	(
	const JString& cmd
	)
{
	JXGetApplication()->DisplayBusyCursor();

	pid_t pid;
	int outFD, errFD;
	JError err = JExecute(cmd, &pid,
						  kJIgnoreConnection, nullptr,
						  kJCreatePipe, &outFD,
						  kJCreatePipe, &errFD);
	if (!err.OK())
		{
		err.ReportIfError();
		return;
		}

	const JFontStyle red(true, false, 0, false, JColorManager::GetRedColor());

	JString text;
	JReadAll(errFD, &text);
	SetCurrentFontStyle(red);
	Paste(text);

	JReadAll(outFD, &text);
	SetCurrentFontStyle(JFontStyle());
	Paste(text);
}

/******************************************************************************
 GetSelectedFiles (virtual)

 ******************************************************************************/

void
SVNInfoLog::GetSelectedFiles
	(
	JPtrArray<JString>*	fullNameList,
	const bool		includeDeleted
	)
{
	fullNameList->CleanOut();
	fullNameList->SetCleanUpAction(JPtrArrayT::kDeleteAll);

	JString s;
	if (GetSelection(&s) && revisionPattern.Match(s))
		{
		fullNameList->Append(itsFullName);
		}
}

/******************************************************************************
 GetBaseRevision (virtual)

 ******************************************************************************/

bool
SVNInfoLog::GetBaseRevision
	(
	JString* rev
	)
{
	JString s;
	if (GetSelection(&s))
		{
		JStringIterator iter(s);
		if (iter.Next(revisionPattern))
			{
			*rev = iter.GetLastMatch().GetSubstring(1);
			return true;
			}
		}

	rev->Clear();
	return false;
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
SVNInfoLog::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (button == kJXRightButton && clickCount == 1)
		{
		JXKeyModifiers emptyMod(GetDisplay());
		SVNTextBase::HandleMouseDown(pt, kJXLeftButton, 2, buttonStates, emptyMod);

		JString rev;
		if (GetBaseRevision(&rev))
			{
			CreateContextMenu();
			itsContextMenu->PopUp(this, pt, buttonStates, modifiers);
			}
		}
	else
		{
		SVNTextBase::HandleMouseDown(pt, button, clickCount, buttonStates, modifiers);
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
SVNInfoLog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsContextMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateContextMenu();
		}
	else if (sender == itsContextMenu && message.Is(JXMenu::kItemSelected))
		{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleContextMenu(selection->GetIndex());
		}

	else
		{
		SVNTextBase::Receive(sender, message);
		}
}

/******************************************************************************
 CreateContextMenu (private)

 ******************************************************************************/

void
SVNInfoLog::CreateContextMenu()
{
	if (itsContextMenu == nullptr)
		{
		itsContextMenu = jnew JXTextMenu(JString::empty, this, kFixedLeft, kFixedTop, 0,0, 10,10);
		assert( itsContextMenu != nullptr );
		itsContextMenu->SetMenuItems(kContextMenuStr, "SVNInfoLog");
		itsContextMenu->SetUpdateAction(JXMenu::kDisableNone);
		itsContextMenu->SetToHiddenPopupMenu();
		ListenTo(itsContextMenu);
		}
}

/******************************************************************************
 UpdateContextMenu (private)

 ******************************************************************************/

void
SVNInfoLog::UpdateContextMenu()
{
	JString rev;
	if (GetBaseRevision(&rev))
		{
		itsContextMenu->EnableItem(kDiffEditedSelectedFilesCtxCmd);
		itsContextMenu->EnableItem(kDiffCurrentSelectedFilesCtxCmd);
		itsContextMenu->EnableItem(kDiffPrevSelectedFilesCtxCmd);
		itsContextMenu->EnableItem(kCommitDetailsCtxCmd);
		itsContextMenu->EnableItem(kBrowseRepoRevisionCtxCmd);
		}
}

/******************************************************************************
 HandleContextMenu (private)

 ******************************************************************************/

void
SVNInfoLog::HandleContextMenu
	(
	const JIndex index
	)
{
	JString rev;
	if (!GetBaseRevision(&rev))
		{
		return;
		}

	if (index == kDiffEditedSelectedFilesCtxCmd)
		{
		CompareEdited(rev);
		}
	else if (index == kDiffCurrentSelectedFilesCtxCmd)
		{
		CompareCurrent(rev);
		}
	else if (index == kDiffPrevSelectedFilesCtxCmd)
		{
		ComparePrev(rev);
		}

	else if (index == kCommitDetailsCtxCmd)
		{
		GetDirector()->ShowInfoLog(itsFullName, rev);
		}

	else if (index == kBrowseRepoRevisionCtxCmd)
		{
		GetDirector()->BrowseRepo(rev);
		}
}

/******************************************************************************
 UpdateInfoMenu (virtual)

 ******************************************************************************/

void
SVNInfoLog::UpdateInfoMenu
	(
	JXTextMenu* menu
	)
{
	JString rev;
	if (GetBaseRevision(&rev))
		{
		menu->EnableItem(kDiffEditedSelectedFilesCmd);
		menu->EnableItem(kDiffCurrentSelectedFilesCmd);
		menu->EnableItem(kDiffPrevSelectedFilesCmd);
		menu->EnableItem(kCommitDetailsCmd);
		menu->EnableItem(kBrowseSelectedRepoRevisionCmd);
		}
}
