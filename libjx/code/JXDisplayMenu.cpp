/******************************************************************************
 JXDisplayMenu.cpp

	Instant menu of all open X Displays.

	Since we have to update our index before anybody can request its new
	value, the correct way to use this object is to listen for the
	DisplayChanged message, not the ItemSelected message.  In addition, since
	we are responsible for marking the menu item, clients should listen
	for DisplayNeedsUpdate instead of NeedsUpdate.

	BASE CLASS = JXTextMenu

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXDisplayMenu.h>
#include <JXOpenDisplayDialog.h>
#include <JXDisplay.h>
#include <JXWindow.h>
#include <jXGlobals.h>
#include <JString.h>
#include <jAssert.h>

static const JCharacter* kNewDisplayStr = "Open new display...";

// JBroadcaster message types

const JCharacter* JXDisplayMenu::kDisplayNeedsUpdate = "DisplayNeedsUpdate::JXDisplayMenu";
const JCharacter* JXDisplayMenu::kDisplayChanged     = "DisplayChanged::JXDisplayMenu";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXDisplayMenu::JXDisplayMenu
	(
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
	JXTextMenu(title, enclosure, hSizing, vSizing, x,y, w,h)
{
	itsNewDisplayDialog = NULL;
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
	itsNewDisplayDialog = NULL;
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
	(JXGetApplication())->SetCurrentDisplay(itsDisplayIndex);
}

/******************************************************************************
 GetSelectedDisplay

 ******************************************************************************/

JXDisplay*
JXDisplayMenu::GetSelectedDisplay()
	const
{
	return (JXGetApplication())->GetDisplay(itsDisplayIndex);
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
	AppendItem(kNewDisplayStr);
	itsNewDisplayIndex = count+1;

	SetUpdateAction(kDisableNone);

	const JBoolean found =
		(JXGetApplication())->GetDisplayIndex(GetDisplay(), &itsDisplayIndex);
	assert( found );

	ListenTo(this);
	ListenTo( (JXGetApplication())->GetDisplayList() );
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
	const JPtrArray<JXDisplay>* displayList = (JXGetApplication())->GetDisplayList();

	if (sender == this && message.Is(JXMenu::kNeedsUpdate))
		{
		Broadcast(DisplayNeedsUpdate());
		CheckItem(itsDisplayIndex);
		}
	else if (sender == this && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		ChooseDisplay(selection->GetIndex());
		}

	else if (sender == const_cast<JPtrArray<JXDisplay>*>(displayList))
		{
		// we do this no matter what change occurs

		BuildMenu();
		}

	else if (sender == itsNewDisplayDialog &&
			 message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			itsDisplayIndex = itsNewDisplayDialog->GetDisplayIndex();
			Broadcast(DisplayChanged(itsDisplayIndex));
			}
		SetPopupChoice(itsDisplayIndex);
		itsNewDisplayDialog = NULL;
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
		assert( itsNewDisplayDialog == NULL );
		JXWindowDirector* supervisor = GetWindow()->GetDirector();
		itsNewDisplayDialog = jnew JXOpenDisplayDialog(supervisor);
		assert( itsNewDisplayDialog != NULL );
		ListenTo(itsNewDisplayDialog);
		itsNewDisplayDialog->BeginDialog();
		}
}

/******************************************************************************
 SetToPopupChoice

 ******************************************************************************/

void
JXDisplayMenu::SetToPopupChoice
	(
	const JBoolean isPopup
	)
{
	JXTextMenu::SetToPopupChoice(isPopup, itsDisplayIndex);
}
