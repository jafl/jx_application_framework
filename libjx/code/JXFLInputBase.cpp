/******************************************************************************
 JXFLInputBase.cpp

	Base class for filter input fields used by JXFileListSet.

	BASE CLASS = JXInputField

	Copyright © 1998 by John Lindal.  All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXFLInputBase.h>
#include <JXFileListTable.h>
#include <JXFileListSet.h>
#include <JXWindow.h>
#include <JXStringHistoryMenu.h>
#include <JString.h>
#include <jASCIIConstants.h>
#include <jGlobals.h>
#include <jAssert.h>

static const JCharacter* kFontName = "6x13";

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
	SetFontName(kFontName);
	itsHistoryMenu->SetDefaultFontName(kFontName, kJTrue);
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
	return (itsFLSet->GetTable())->SetFilterRegex(r);
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
JXFLInputBase::HandleKeyPress
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	if (key == kJReturnKey)
		{
		(itsFLSet->GetTable())->Focus();
		}
	else
		{
		JXInputField::HandleKeyPress(key, modifiers);
		}
}

/******************************************************************************
 OKToUnfocus (virtual protected)

 ******************************************************************************/

JBoolean
JXFLInputBase::OKToUnfocus()
{
	if (!JXInputField::OKToUnfocus())
		{
		return kJFalse;
		}

	const JError err = Apply();
	if (err.OK())
		{
		itsHistoryMenu->AddString(GetText());
		return kJTrue;
		}
	else
		{
		err.ReportIfError();
		return kJFalse;
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
		const JString origStr = GetText();
		const JString newStr  = itsHistoryMenu->GetItemText(message);
		SetText(newStr);

		const JError err = Apply();
		if (err.OK())
			{
			itsHistoryMenu->AddString(newStr);
			}
		else
			{
			SetText(origStr);
			err.ReportIfError();
			}
		}

	else
		{
		JXInputField::Receive(sender, message);
		}
}
