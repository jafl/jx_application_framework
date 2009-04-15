/******************************************************************************
 JXHistoryMenuBase.cpp

	Base class for menus that maintains a fixed-length history.  New items
	should be prepended to the menu so the oldest ones automatically fall off
	the bottom.

	The default icon is empty.  To change the icon for all items, call
	SetDefaultIcon().  Otherwise, you can create a derived class and
	override UpdateItemImage() to display different icons for different files.

	Since nothing on this menu is ever disabled, one only needs to listen
	for the ItemSelected message.

	We listen for NeedsUpdate and automatically call UpdateMenu(), so
	derived classes only need to override UpdateMenu().

	If SetFirstIndex() is called, this shifts the operations down to
	start at the given item index.  Items above this index will not
	be touched.

	BASE CLASS = JXTextMenu

	Copyright © 1998-05 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXHistoryMenuBase.h>
#include <JXImage.h>
#include <JXInputField.h>
#include <jAssert.h>

// setup information

const JFileVersion kCurrentSetupVersion = 10000;

	// version < 10000 => the first value is really the number of items (no shortcuts)

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXHistoryMenuBase::JXHistoryMenuBase
	(
	const JSize			historyLength,
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
	JXHistoryMenuBaseX(historyLength);
}

JXHistoryMenuBase::JXHistoryMenuBase
	(
	const JSize		historyLength,
	JXMenu*			owner,
	const JIndex	itemIndex,
	JXContainer*	enclosure
	)
	:
	JXTextMenu(owner, itemIndex, enclosure)
{
	JXHistoryMenuBaseX(historyLength);
}

// private

void
JXHistoryMenuBase::JXHistoryMenuBaseX
	(
	const JSize historyLength
	)
{
	assert( historyLength > 0 );

	itsFirstIndex    = 1;
	itsHistoryLength = historyLength;
	SetUpdateAction(kDisableNone);

	itsDefaultIcon     = NULL;
	itsOwnsDefIconFlag = kJFalse;

	ListenTo(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXHistoryMenuBase::~JXHistoryMenuBase()
{
	if (itsOwnsDefIconFlag)
		{
		delete itsDefaultIcon;
		}
}

/******************************************************************************
 SetHistoryLength

 ******************************************************************************/

void
JXHistoryMenuBase::SetHistoryLength
	(
	const JSize historyLength
	)
{
	assert( historyLength > 0 );

	itsHistoryLength = historyLength;
	AdjustLength();
}

/******************************************************************************
 AdjustLength (protected)

	Remove extra items from the bottom of the menu.

 ******************************************************************************/

void
JXHistoryMenuBase::AdjustLength()
{
	JSize itemCount = GetItemCount();
	while (itemCount > itsHistoryLength + itsFirstIndex - 1)
		{
		RemoveItem(itemCount);
		itemCount--;
		}
}

/******************************************************************************
 AddItem (protected)

	Prepend the given item to the menu and remove outdated entries
	at the bottom.

 ******************************************************************************/

void
JXHistoryMenuBase::AddItem
	(
	const JCharacter* text,
	const JCharacter* nmShortcut
	)
{
	const JSize itemCount = GetItemCount();
	JString itemNMShortcut;
	for (JIndex i=itsFirstIndex; i<=itemCount; i++)
		{
		GetItemNMShortcut(i, &itemNMShortcut);

		const JBoolean matches = JI2B(
			text == JXTextMenu::GetItemText(i) &&
			nmShortcut == itemNMShortcut );
		if (matches && i == itsFirstIndex)
			{
			return;		// already at top of list
			}
		else if (matches)
			{
			RemoveItem(i);
			break;
			}
		}

	InsertItem(itsFirstIndex, text, kJFalse, kJFalse, NULL, nmShortcut);
	AdjustLength();
}

/******************************************************************************
 ReadSetup

	Reads the setup information into the menu.

	We assert that the data is readable because there is no way to skip
	over it.

 ******************************************************************************/

void
JXHistoryMenuBase::ReadSetup
	(
	istream& input
	)
{
	ReadSetup(input, this, NULL, NULL);
}

/******************************************************************************
 ReadSetup (static)

	Reads the setup information into a list.

	We assert that the data is readable because there is no way to skip
	over it.

 ******************************************************************************/

void
JXHistoryMenuBase::ReadSetup
	(
	istream&			input,
	JPtrArray<JString>*	itemList,
	JPtrArray<JString>*	nmShortcutList
	)
{
	ReadSetup(input, NULL, itemList, nmShortcutList);
}

/******************************************************************************
 ReadSetup (static private)

	We assert that the data is readable because there is no way to skip
	over it.

 ******************************************************************************/

void
JXHistoryMenuBase::ReadSetup
	(
	istream&			input,
	JXHistoryMenuBase*	menu,
	JPtrArray<JString>*	itemList,
	JPtrArray<JString>*	nmShortcutList
	)
{
	if (menu != NULL)
		{
		menu->ClearHistory();
		}
	if (itemList != NULL)
		{
		itemList->CleanOut();
		}
	if (nmShortcutList != NULL)
		{
		nmShortcutList->CleanOut();
		}

	JFileVersion vers;
	input >> vers;
	if (input.eof() || input.fail())
		{
		return;
		}
	assert( vers <= kCurrentSetupVersion );

	JSize count = vers;
	if (vers >= 10000)
		{
		input >> count;
		if (input.eof() || input.fail())
			{
			return;
			}
		}

	JString text, nmShortcut;
	for (JIndex i=1; i<=count; i++)
		{
		input >> text;
		if (vers >= 10000)
			{
			input >> nmShortcut;
			}
		if (input.fail())
			{
			break;
			}

		if (menu != NULL)
			{
			menu->AppendItem(text, kJFalse, kJFalse, NULL, nmShortcut);
			}
		if (itemList != NULL)
			{
			itemList->Append(text);
			}
		if (nmShortcutList != NULL)
			{
			nmShortcutList->Append(nmShortcut);
			}
		}

	// must always read them all to place file marker

	if (menu != NULL)
		{
		menu->AdjustLength();
		}
}

/******************************************************************************
 WriteSetup

 ******************************************************************************/

void
JXHistoryMenuBase::WriteSetup
	(
	ostream& output
	)
	const
{
	output << ' ' << kCurrentSetupVersion;

	const JSize count = GetItemCount();
	output << ' ' << count - itsFirstIndex + 1;

	JString nmShortcut;
	for (JIndex i=itsFirstIndex; i<=count; i++)
		{
		GetItemNMShortcut(i, &nmShortcut);
		output << ' ' << JXTextMenu::GetItemText(i) << ' ' << nmShortcut;
		}

	output << ' ';
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXHistoryMenuBase::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == this && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateMenu();
		}

	JXTextMenu::Receive(sender, message);
}

/******************************************************************************
 UpdateMenu (virtual protected)

 ******************************************************************************/

void
JXHistoryMenuBase::UpdateMenu()
{
	const JSize count = GetItemCount();
	for (JIndex i=itsFirstIndex; i<=count; i++)
		{
		UpdateItemImage(i);
		}
}

/******************************************************************************
 UpdateItemImage (virtual protected)

	Derived classes can override this to display a different icon for
	each item.

 ******************************************************************************/

void
JXHistoryMenuBase::UpdateItemImage
	(
	const JIndex index
	)
{
	const JXImage* image;
	if (!GetItemImage(index, &image))
		{
		SetItemImage(index, itsDefaultIcon, kJFalse);
		}
}

/******************************************************************************
 SetDefaultIcon

 ******************************************************************************/

void
JXHistoryMenuBase::SetDefaultIcon
	(
	JXImage*		icon,
	const JBoolean	menuOwnsIcon
	)
{
	if (itsOwnsDefIconFlag)
		{
		delete itsDefaultIcon;
		}

	itsDefaultIcon     = icon;
	itsOwnsDefIconFlag = menuOwnsIcon;

	// clear all icons so UpdateItemImage() will fix it
	// (We can't just use the default icon, since it can be overridden.)

	const JSize count = GetItemCount();
	for (JIndex i=itsFirstIndex; i<=count; i++)
		{
		ClearItemImage(i);
		}
}

/******************************************************************************
 GetItemText

	Convenience function to process ItemSelected message and return item text.

 ******************************************************************************/

const JString&
JXHistoryMenuBase::GetItemText
	(
	const Message& message
	)
	const
{
	assert( message.Is(JXMenu::kItemSelected) );

	const JXMenu::ItemSelected* selection =
		dynamic_cast(const JXMenu::ItemSelected*, &message);
	assert( selection != NULL );

	return JXTextMenu::GetItemText(selection->GetIndex());
}

/******************************************************************************
 UpdateInputField

	Convenience function to process ItemSelected message and place text in
	an input field.

	We can't use JXTEBase* because of the egcs thunks bug.  Why??  It isn't
	in a ctor!

 ******************************************************************************/

void
JXHistoryMenuBase::UpdateInputField
	(
	const Message&	message,
	JXInputField*	input
	)
	const
{
	// make local copy in case menu is changed
	const JString str = GetItemText(message);

	input->SetText(str);
}
