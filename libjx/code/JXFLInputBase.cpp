/******************************************************************************
 JXFLInputBase.cpp

	Base class for filter input fields used by JXFileListSet.

	BASE CLASS = JXInputField

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "JXFLInputBase.h"
#include "JXFileListTable.h"
#include "JXFileListSet.h"
#include "JXWindow.h"
#include "JXStringHistoryMenu.h"
#include "JXFontManager.h"
#include <jx-af/jcore/jASCIIConstants.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXFLInputBase::JXFLInputBase
	(
	JXFileListSet*			flSet,
	JXStringHistoryMenu*	historyMenu,
	JXContainer*			enclosure,
	const HSizingOption		hSizing,
	const VSizingOption		vSizing,
	const JCoordinate		x,
	const JCoordinate		y,
	const JCoordinate		w,
	const JCoordinate		h
	)
	:
	JXInputField(enclosure, hSizing, vSizing, x,y, w,h),
	itsFLSet(flSet),
	itsHistoryMenu(historyMenu)
{
	const JFont& font = JFontManager::GetDefaultMonospaceFont();
	SetFont(font);
	itsHistoryMenu->SetDefaultFont(font, true);
	ListenTo(itsHistoryMenu);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXFLInputBase::~JXFLInputBase()
{
}

/******************************************************************************
 Apply

 ******************************************************************************/

JError
JXFLInputBase::Apply()
	const
{
	const JString r = GetRegexString();  // put on stack since SetFilterRegex() takes char*
	return itsFLSet->GetTable()->SetFilterRegex(r);
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
JXFLInputBase::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	if (c == kJReturnKey)
	{
		(itsFLSet->GetTable())->Focus();
	}
	else
	{
		JXInputField::HandleKeyPress(c, keySym, modifiers);
	}
}

/******************************************************************************
 OKToUnfocus (virtual protected)

 ******************************************************************************/

bool
JXFLInputBase::OKToUnfocus()
{
	if (!JXInputField::OKToUnfocus())
	{
		return false;
	}

	const JError err = Apply();
	if (err.OK())
	{
		itsHistoryMenu->AddString(GetText()->GetText());
		return true;
	}
	else
	{
		err.ReportIfError();
		return false;
	}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXFLInputBase::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsHistoryMenu && message.Is(JXMenu::kItemSelected))
	{
		const JString origStr = GetText()->GetText();
		const JString newStr  = itsHistoryMenu->GetItemText(message);
		GetText()->SetText(newStr);

		const JError err = Apply();
		if (err.OK())
		{
			itsHistoryMenu->AddString(newStr);
		}
		else
		{
			GetText()->SetText(origStr);
			err.ReportIfError();
		}
	}

	else
	{
		JXInputField::Receive(sender, message);
	}
}
