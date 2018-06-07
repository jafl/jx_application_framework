/******************************************************************************
 CBTEScriptMenu.cpp

	Appends items to JXFSDirMenu.

	BASE CLASS = JXFSDirMenu

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#include "CBTEScriptMenu.h"
#include "CBTextEditor.h"
#include "cbGlobals.h"
#include <JXWebBrowser.h>
#include <JDirInfo.h>
#include <jDirUtil.h>
#include <jVCSUtil.h>
#include <jAssert.h>

static const JCharacter* kMenuSuffixStr =
	"  New script"
	"| Open directory";

enum	// in reverse order since from end of menu
{
	kOpenDirectoryCmdOffset = 0,
	kNewScriptCmdOffset
};

// string ID's

static const JCharacter* kSavePromptID = "SavePrompt::CBTEScriptMenu";

/******************************************************************************
 Constructor

 *****************************************************************************/

CBTEScriptMenu::CBTEScriptMenu
	(
	CBTextEditor*		te,
	const JCharacter*	path,
	const JCharacter*	title,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXFSDirMenu(path, title, enclosure, hSizing, vSizing, x, y, w, h),
	itsTE(te)
{
	CBTEScriptMenuX();
}

CBTEScriptMenu::CBTEScriptMenu
	(
	CBTextEditor*		te,
	const JCharacter*	path,
	JXMenu*				owner,
	const JIndex		itemIndex,
	JXContainer*		enclosure
	)
	:
	JXFSDirMenu(path, owner, itemIndex, enclosure),
	itsTE(te)
{
	CBTEScriptMenuX();
}

// private

void
CBTEScriptMenu::CBTEScriptMenuX()
{
	JDirInfo* info;
	if (GetDirInfo(&info))
		{
		info->SetWildcardFilter("*~ #*#", kJTrue);
		}
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CBTEScriptMenu::~CBTEScriptMenu()
{
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBTEScriptMenu::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == this && message.Is(JXMenu::kNeedsUpdate))
		{
		if (!UpdateSelf())
			{
			return;
			}
		}
	else if (sender == this && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* info =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert(info != nullptr);
		if (HandleSelection(info->GetIndex()))
			{
			return;
			}
		}

	JXFSDirMenu::Receive(sender, message);

	if (sender == this && message.Is(JXMenu::kNeedsUpdate))
		{
		ShowSeparatorAfter(GetItemCount());
		AppendMenuItems(kMenuSuffixStr);
		}
}

/******************************************************************************
 UpdateSelf (private)

 ******************************************************************************/

JBoolean
CBTEScriptMenu::UpdateSelf()
{
	JDirInfo* info;
	if (GetDirInfo(&info) &&
		(info->GoTo(itsTE->GetScriptPath())).OK())
		{
		info->SetDirEntryFilter(ShowExecutables);
		info->ForceUpdate();	// permissions may change
		return kJTrue;
		}
	else
		{
		ClearMenu();
		return kJFalse;
		}
}

/******************************************************************************
 ShowExecutables (static private)

 ******************************************************************************/

JBoolean
CBTEScriptMenu::ShowExecutables
	(
	const JDirEntry& entry
	)
{
	return JI2B( (entry.IsDirectory() && !JIsVCSDirectory(entry.GetName())) ||
				 (entry.IsFile() && entry.IsExecutable()) );
}

/******************************************************************************
 HandleSelection (private)

 ******************************************************************************/

JBoolean
CBTEScriptMenu::HandleSelection
	(
	const JIndex index
	)
{
	if (index == GetItemCount() - kNewScriptCmdOffset)
		{
		JDirInfo* info;
		if (GetDirInfo(&info))
			{
			JString origName = info->GetDirectory();
			origName         = JCombinePathAndName(origName, "script");

			JString fullName;
			if ((JGetChooseSaveFile())->SaveFile(JGetString(kSavePromptID), nullptr,
												 origName, &fullName))
				{
				std::ofstream output(fullName);
				output.close();
				JSetPermissions(fullName, 0744);
				(CBGetDocumentManager())->OpenTextDocument(fullName);
				}
			}
		return kJTrue;
		}
	else if (index == GetItemCount() - kOpenDirectoryCmdOffset)
		{
		JDirInfo* info;
		if (GetDirInfo(&info))
			{
			(JXGetWebBrowser())->ShowFileLocation(info->GetDirectory());
			}
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}
