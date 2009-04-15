/******************************************************************************
 JXFontSizeMenu.cpp

	Since we have to update our index before anybody can request its new
	value, the correct way to use this object is to listen for the
	SizeChanged message, not the ItemSelected message.  In addition, since
	we are responsible for marking the menu item, clients should listen
	for SizeNeedsUpdate instead of NeedsUpdate.

	BASE CLASS = JXTextMenu

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXFontSizeMenu.h>
#include <JXFontNameMenu.h>
#include <JXChooseFontSizeDialog.h>
#include <JXWindow.h>
#include <jXConstants.h>
#include <jXActionDefs.h>
#include <JString.h>
#include <JFontManager.h>
#include <jMath.h>
#include <jAssert.h>

static const JCharacter* kVarSizeStr = "Other...";

// JBroadcaster message types

const JCharacter* JXFontSizeMenu::kSizeNeedsUpdate = "SizeNeedsUpdate::JXFontSizeMenu";
const JCharacter* JXFontSizeMenu::kSizeChanged     = "SizeChanged::JXFontSizeMenu";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXFontSizeMenu::JXFontSizeMenu
	(
	const JCharacter*	fontName,
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
	JXFontSizeMenuX(NULL);
	BuildMenu(fontName);
	SetFontSize(kJXDefaultFontSize);
}

JXFontSizeMenu::JXFontSizeMenu
	(
	const JCharacter*	fontName,
	JXMenu*				owner,
	const JIndex		itemIndex,
	JXContainer*		enclosure
	)
	:
	JXTextMenu(owner, itemIndex, enclosure)
{
	JXFontSizeMenuX(NULL);
	BuildMenu(fontName);
	SetFontSize(kJXDefaultFontSize);
}

JXFontSizeMenu::JXFontSizeMenu
	(
	JXFontNameMenu*		fontMenu,
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
	JXFontSizeMenuX(fontMenu);
	const JString fontName = itsFontNameMenu->GetFontName();
	BuildMenu(fontName);
	SetFontSize(kJXDefaultFontSize);
}

JXFontSizeMenu::JXFontSizeMenu
	(
	JXFontNameMenu*		fontMenu,
	JXMenu*				owner,
	const JIndex		itemIndex,
	JXContainer*		enclosure
	)
	:
	JXTextMenu(owner, itemIndex, enclosure)
{
	JXFontSizeMenuX(fontMenu);
	const JString fontName = itsFontNameMenu->GetFontName();
	BuildMenu(fontName);
	SetFontSize(kJXDefaultFontSize);
}

// private

void
JXFontSizeMenu::JXFontSizeMenuX
	(
	JXFontNameMenu* fontMenu
	)
{
	itsFontNameMenu = fontMenu;
	if (itsFontNameMenu != NULL)
		{
		ListenTo(itsFontNameMenu);
		}

	itsBroadcastChangeFlag = kJTrue;
	itsChooseSizeDialog    = NULL;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXFontSizeMenu::~JXFontSizeMenu()
{
}

/******************************************************************************
 SetFontSize

 ******************************************************************************/

void
JXFontSizeMenu::SetFontSize
	(
	const JSize size
	)
{
	assert( size > 0 );

	const JString itemText(size, 0, JString::kForceNoExponent);

	JIndex newIndex = 0;
	const JIndex count = GetItemCount();
	for (JIndex i=1; i<=count; i++)
		{
		if (GetItemText(i) == itemText)
			{
			newIndex = i;
			break;
			}
		}

	if (newIndex > 0)
		{
		itsCurrIndex = newIndex;
		itsFontSize  = size;
		AdjustVarSizeItem(0);
		}
	else if (itsVarSizeIndex > 0)
		{
		itsCurrIndex = itsVarSizeIndex;
		itsFontSize  = size;
		AdjustVarSizeItem(itsFontSize);
		}

	SetPopupChoice(itsCurrIndex);
	if (itsBroadcastChangeFlag)
		{
		Broadcast(SizeChanged(itsFontSize));
		}
}

/******************************************************************************
 BuildMenu (private)

	Caller should call SetFontSize() afterwards.

 ******************************************************************************/

void
JXFontSizeMenu::BuildMenu
	(
	const JCharacter* fontName
	)
{
	RemoveAllItems();

	JSize minSize, maxSize;
	JArray<JSize> sizeList;
	(GetFontManager())->GetFontSizes(fontName, &minSize, &maxSize, &sizeList);

	const JSize count = sizeList.GetElementCount();
	JString id;
	if (count > 0)
		{
		for (JIndex i=1; i<=count; i++)
			{
			const JString itemText(sizeList.GetElement(i), 0, JString::kForceNoExponent);
			id = itemText + "::JX";
			AppendItem(itemText, kJTrue, kJTrue, NULL, NULL, id);
			}
		itsVarSizeIndex = 0;
		}
	else
		{
		for (JSize size=minSize; size<=maxSize; size+=2)
			{
			const JString itemText(size, 0, JString::kForceNoExponent);
			id = itemText + "::JX";
			AppendItem(itemText, kJTrue, kJTrue, NULL, NULL, id);
			}
		ShowSeparatorAfter(GetItemCount());
		AppendItem(kVarSizeStr, kJTrue, kJTrue, NULL, NULL, kJXOtherFontSizeAction);
		itsVarSizeIndex = GetItemCount();
		}
	SetUpdateAction(kDisableNone);

	itsCurrIndex = 1;
	itsFontSize  = minSize;

	ListenTo(this);
}

/******************************************************************************
 AdjustVarSizeItem (private)

 ******************************************************************************/

void
JXFontSizeMenu::AdjustVarSizeItem
	(
	const JSize fontSize
	)
{
	if (itsVarSizeIndex > 0 && fontSize > 0)
		{
		JString str = kVarSizeStr;
		str += " (";
		str += JString(fontSize, 0, JString::kForceNoExponent);
		str += ")";
		SetItemText(itsVarSizeIndex, str);
		}
	else if (itsVarSizeIndex > 0)
		{
		SetItemText(itsVarSizeIndex, kVarSizeStr);
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXFontSizeMenu::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsFontNameMenu && message.Is(JXFontNameMenu::kNameChanged))
		{
		itsBroadcastChangeFlag = kJFalse;
		const JSize currSize   = GetFontSize();
		const JString fontName = itsFontNameMenu->GetFontName();
		BuildMenu(fontName);
		SetFontSize(currSize);
		itsBroadcastChangeFlag = kJTrue;
		}

	else if (sender == this && message.Is(JXMenu::kNeedsUpdate))
		{
		itsBroadcastChangeFlag = kJFalse;
		Broadcast(SizeNeedsUpdate());
		CheckItem(itsCurrIndex);
		itsBroadcastChangeFlag = kJTrue;
		}
	else if (sender == this && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		ChooseFontSize(selection->GetIndex());
		}

	else if (sender == itsChooseSizeDialog &&
			 message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast(const JXDialogDirector::Deactivated*, &message);
		assert( info != NULL );
		if (info->Successful())
			{
			SetFontSize(itsChooseSizeDialog->GetFontSize());
			}
		else
			{
			SetPopupChoice(itsCurrIndex);	// revert displayed string
			}
		itsChooseSizeDialog = NULL;
		}

	else
		{
		JXTextMenu::Receive(sender, message);
		}
}

/******************************************************************************
 ChooseFontSize (private)

 ******************************************************************************/

void
JXFontSizeMenu::ChooseFontSize
	(
	const JIndex sizeIndex
	)
{
	if (sizeIndex != itsVarSizeIndex)
		{
		itsCurrIndex = sizeIndex;
		const JBoolean ok = (GetItemText(sizeIndex)).ConvertToUInt(&itsFontSize);
		assert( ok );
		AdjustVarSizeItem(0);
		Broadcast(SizeChanged(itsFontSize));
		}
	else
		{
		assert( itsChooseSizeDialog == NULL );
		JXWindowDirector* supervisor = (GetWindow())->GetDirector();
		itsChooseSizeDialog = new JXChooseFontSizeDialog(supervisor, itsFontSize);
		assert( itsChooseSizeDialog != NULL );
		ListenTo(itsChooseSizeDialog);
		itsChooseSizeDialog->BeginDialog();
		}
}

/******************************************************************************
 SetToPopupChoice

 ******************************************************************************/

void
JXFontSizeMenu::SetToPopupChoice
	(
	const JBoolean isPopup
	)
{
	JXTextMenu::SetToPopupChoice(isPopup, itsCurrIndex);
}
