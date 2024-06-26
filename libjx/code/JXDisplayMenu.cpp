/******************************************************************************
 JXDisplayMenu.cpp

	Instant menu of all open X Displays.

	Since we have to update our index before anybody can request its new
	value, the correct way to use this object is to listen for the
	DisplayChanged message, not the ItemSelected message.  In addition, since
	we are responsible for marking the menu item, clients should listen
	for DisplayNeedsUpdate instead of NeedsUpdate.

	BASE CLASS = JXTextMenu

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXDisplayMenu.h"
#include "JXOpenDisplayDialog.h"
#include "JXDisplay.h"
#include "JXWindow.h"
#include "jXGlobals.h"
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/jAssert.h>

// JBroadcaster message types

const JUtf8Byte* JXDisplayMenu::kDisplayNeedsUpdate = "DisplayNeedsUpdate::JXDisplayMenu";
const JUtf8Byte* JXDisplayMenu::kDisplayChanged     = "DisplayChanged::JXDisplayMenu";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXDisplayMenu::JXDisplayMenu
	(
	const JString&		title, 
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXTextMenu(title, enclosure, hSizing, vSizing, x,y, w,h)
{
	BuildMenu();
}

JXDisplayMenu::JXDisplayMenu
	(
	JXMenu*			owner,
	const JIndex	itemIndex,
	JXContainer*	enclosure
	)
	:
	JXTextMenu(owner, itemIndex, enclosure)
{
	BuildMenu();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXDisplayMenu::~JXDisplayMenu()
{
}

/******************************************************************************
 SelectCurrentDisplay

 ******************************************************************************/

void
JXDisplayMenu::SelectCurrentDisplay()
	const
{
	JXGetApplication()->SetCurrentDisplay(itsDisplayIndex);
}

/******************************************************************************
 GetSelectedDisplay

 ******************************************************************************/

JXDisplay*
JXDisplayMenu::GetSelectedDisplay()
	const
{
	return JXGetApplication()->GetDisplay(itsDisplayIndex);
}

/******************************************************************************
 BuildMenu (private)

 ******************************************************************************/

void
JXDisplayMenu::BuildMenu()
{
	RemoveAllItems();

	JXApplication* app = JXGetApplication();
	const JSize count = app->GetDisplayCount();
	for (JIndex i=1; i<=count; i++)
	{
		JXDisplay* display = app->GetDisplay(i);
		AppendItem(display->GetName(), kRadioType);
	}

	ShowSeparatorAfter(count);
	AppendItem(JGetString("NewDisplay::JXDisplayMenu"));
	itsNewDisplayIndex = count+1;

	SetUpdateAction(kDisableNone);

	const bool found =
		JXGetApplication()->GetDisplayIndex(GetDisplay(), &itsDisplayIndex);
	assert( found );

	ListenTo(this);
	ListenTo( JXGetApplication()->GetDisplayList() );
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
JXDisplayMenu::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	const JPtrArray<JXDisplay>* displayList = JXGetApplication()->GetDisplayList();

	if (sender == this && message.Is(JXMenu::kNeedsUpdate))
	{
		Broadcast(DisplayNeedsUpdate());
		CheckItem(itsDisplayIndex);
	}
	else if (sender == this && message.Is(JXMenu::kItemSelected))
	{
		auto& selection = dynamic_cast<const JXMenu::ItemSelected&>(message);
		ChooseDisplay(selection.GetIndex());
	}

	else if (sender == const_cast<JPtrArray<JXDisplay>*>(displayList))
	{
		// we do this no matter what change occurs

		BuildMenu();
	}

	else
	{
		JXTextMenu::Receive(sender, message);
	}
}

/******************************************************************************
 ChooseDisplay (private)

 ******************************************************************************/

void
JXDisplayMenu::ChooseDisplay
	(
	const JIndex index
	)
{
	if (index < itsNewDisplayIndex)
	{
		itsDisplayIndex = index;
		Broadcast(DisplayChanged(itsDisplayIndex));
	}
	else
	{
		auto* dlog = jnew JXOpenDisplayDialog();
		if (dlog->DoDialog())
		{
			itsDisplayIndex = dlog->GetDisplayIndex();
			Broadcast(DisplayChanged(itsDisplayIndex));
		}
		SetPopupChoice(itsDisplayIndex);
	}
}

/******************************************************************************
 SetToPopupChoice

 ******************************************************************************/

void
JXDisplayMenu::SetToPopupChoice
	(
	const bool isPopup
	)
{
	JXTextMenu::SetToPopupChoice(isPopup, itsDisplayIndex);
}
