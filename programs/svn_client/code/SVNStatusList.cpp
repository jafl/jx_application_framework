/******************************************************************************
 SVNStatusList.cc

	BASE CLASS = SVNListBase

	Copyright @ 2008 by John Lindal.  All rights reserved.

 ******************************************************************************/

#include "SVNStatusList.h"
#include "svnMenus.h"
#include <JXTextMenu.h>
#include <JXTextSelection.h>
#include <JXColormap.h>
#include <jXGlobals.h>
#include <JTableSelection.h>
#include <JSimpleProcess.h>
#include <JSubstitute.h>
#include <jDirUtil.h>
#include <jAssert.h>

static const JCharacter* kIgnoreCmd = "svn propedit svn:ignore $path";

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
	SVNListBase(director, editMenu, "svn --non-interactive status",
				kJFalse, kJFalse, kJFalse, kJTrue,
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
	const JCharacter c1 = line.GetCharacter(1);
	const JCharacter c2 = line.GetCharacter(2);
	if (c1 == 'C' || c1 == '!' || c1 == '~' || c2 == 'C')
		{
		SetStyle(index, errorStyle);
		}
	else if (c1 == '?' || c1 == 'I')
		{
		SetStyle(index, GetColormap()->GetGrayColor(50));
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
	JString s = line.GetSubstring(8, line.GetLength());
	s.TrimWhitespace();
	return s;
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
		((GetStringList()).NthElement(cell.y))->GetFirstCharacter() == '?')
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

	JBoolean canIgnore = kJFalse;

	JTableSelection& s = GetTableSelection();
	JPoint cell;
	if (s.GetSingleSelectedCell(&cell) &&
		((GetStringList()).NthElement(cell.y))->GetFirstCharacter() == '?')
		{
		canIgnore = kJTrue;
		}

	menu->SetItemEnable(kIgnoreSelectionCtxCmd, canIgnore);
}

/******************************************************************************
 Ignore (virtual)

 ******************************************************************************/

JBoolean
SVNStatusList::Ignore()
{
	JTableSelection& s = GetTableSelection();
	JPoint cell;
	if (s.GetSingleSelectedCell(&cell) &&
		((GetStringList()).NthElement(cell.y))->GetFirstCharacter() == '?')
		{
		JPtrArray<JString> list(JPtrArrayT::kDeleteAll);
		GetSelectedFiles(&list);

		JString path, name;
		JSplitPathAndName(*(list.FirstElement()), &path, &name);

		JXTextSelection* data = jnew JXTextSelection(GetDisplay(), name);
		assert( data != NULL );

		GetSelectionManager()->SetData(kJXClipboardName, data);

		JString cmd = kIgnoreCmd;
		path        = JPrepArgForExec(path);

		JSubstitute subst;
		subst.DefineVariable("path", path);
		subst.Substitute(&cmd);

		JSimpleProcess::Create(cmd, kJTrue);
		}

	return kJTrue;
}
