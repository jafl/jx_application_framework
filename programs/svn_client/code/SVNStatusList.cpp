/******************************************************************************
 SVNStatusList.cpp

	BASE CLASS = SVNListBase

	Copyright @ 2008 by John Lindal.

 ******************************************************************************/

#include "SVNStatusList.h"
#include "svnMenus.h"
#include <JXTextMenu.h>
#include <JXTextSelection.h>
#include <JXColorManager.h>
#include <jXGlobals.h>
#include <JTableSelection.h>
#include <JSimpleProcess.h>
#include <JSubstitute.h>
#include <JStringIterator.h>
#include <jDirUtil.h>
#include <jAssert.h>

static const JString kIgnoreCmd("svn propedit svn:ignore $path", JString::kNoCopy);

/******************************************************************************
 Constructor

 ******************************************************************************/

SVNStatusList::SVNStatusList
	(
	SVNMainDirector*	director,
	JXTextMenu*			editMenu,
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
	SVNListBase(director, editMenu, JString("svn --non-interactive status", JString::kNoCopy),
				false, false, false, true,
				scrollbarSet, enclosure, hSizing, vSizing, x, y, w, h)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SVNStatusList::~SVNStatusList()
{
}

/******************************************************************************
 StyleLine (virtual protected)

 ******************************************************************************/

void
SVNStatusList::StyleLine
	(
	const JIndex		index,
	const JString&		line,
	const JFontStyle&	errorStyle,
	const JFontStyle&	addStyle,
	const JFontStyle&	removeStyle
	)
{
	JStringIterator iter(line);
	JUtf8Character c1, c2;
	iter.Next(&c1);
	iter.Next(&c2);
	iter.Invalidate();

	if (c1 == 'C' || c1 == '!' || c1 == '~' || c2 == 'C')
		{
		SetStyle(index, errorStyle);
		}
	else if (c1 == '?' || c1 == 'I')
		{
		SetStyle(index, JColorManager::GetGrayColor(50));
		}
	else if (c1 == 'A')
		{
		SetStyle(index, addStyle);
		}
	else if (c1 == 'D')
		{
		SetStyle(index, removeStyle);
		}
}

/******************************************************************************
 ExtractRelativePath (virtual protected)

 ******************************************************************************/

JString
SVNStatusList::ExtractRelativePath
	(
	const JString& line
	)
	const
{
	return SVNListBase::ExtractRelativePath(line, 8);
}

/******************************************************************************
 UpdateActionsMenu (virtual)

 ******************************************************************************/

void
SVNStatusList::UpdateActionsMenu
	(
	JXTextMenu* menu
	)
{
	SVNListBase::UpdateActionsMenu(menu);

	JTableSelection& s = GetTableSelection();
	JPoint cell;
	if (s.GetSingleSelectedCell(&cell) &&
		((GetStringList()).GetElement(cell.y))->GetFirstCharacter() == '?')
		{
		menu->EnableItem(kIgnoreSelectionCmd);
		}
}

/******************************************************************************
 UpdateContextMenu (virtual protected)

 ******************************************************************************/

void
SVNStatusList::UpdateContextMenu
	(
	JXTextMenu* menu
	)
{
	SVNListBase::UpdateContextMenu(menu);

	bool canIgnore = false;

	JTableSelection& s = GetTableSelection();
	JPoint cell;
	if (s.GetSingleSelectedCell(&cell) &&
		((GetStringList()).GetElement(cell.y))->GetFirstCharacter() == '?')
		{
		canIgnore = true;
		}

	menu->SetItemEnable(kIgnoreSelectionCtxCmd, canIgnore);
}

/******************************************************************************
 Ignore (virtual)

 ******************************************************************************/

bool
SVNStatusList::Ignore()
{
	JTableSelection& s = GetTableSelection();
	JPoint cell;
	if (s.GetSingleSelectedCell(&cell) &&
		((GetStringList()).GetElement(cell.y))->GetFirstCharacter() == '?')
		{
		JPtrArray<JString> list(JPtrArrayT::kDeleteAll);
		GetSelectedFiles(&list);

		JString path, name;
		JSplitPathAndName(*list.GetFirstElement(), &path, &name);

		auto* data = jnew JXTextSelection(GetDisplay(), name);
		assert( data != nullptr );

		GetSelectionManager()->SetData(kJXClipboardName, data);

		JString cmd = kIgnoreCmd;
		path        = JPrepArgForExec(path);

		JSubstitute subst;
		subst.DefineVariable("path", path);
		subst.Substitute(&cmd);

		JSimpleProcess::Create(cmd, true);
		}

	return true;
}
