/******************************************************************************
 JXFontSizeMenu.cpp

	Since we have to update our index before anybody can request its new
	value, the correct way to use this object is to listen for the
	SizeChanged message, not the ItemSelected message.  In addition, since
	we are responsible for marking the menu item, clients should listen
	for SizeNeedsUpdate instead of NeedsUpdate.

	BASE CLASS = JXTextMenu

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXFontSizeMenu.h"
#include "JXFontNameMenu.h"
#include "JXChooseFontSizeDialog.h"
#include "JXWindow.h"
#include "jXConstants.h"
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/jMath.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

// JBroadcaster message types

const JUtf8Byte* JXFontSizeMenu::kSizeNeedsUpdate = "SizeNeedsUpdate::JXFontSizeMenu";
const JUtf8Byte* JXFontSizeMenu::kSizeChanged     = "SizeChanged::JXFontSizeMenu";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXFontSizeMenu::JXFontSizeMenu
	(
	const JString&		fontName,
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
	JXFontSizeMenuX(nullptr);
	BuildMenu(fontName);
	SetFontSize(JFontManager::GetDefaultFontSize());
}

JXFontSizeMenu::JXFontSizeMenu
	(
	const JString&	fontName,
	JXMenu*			owner,
	const JIndex	itemIndex,
	JXContainer*	enclosure
	)
	:
	JXTextMenu(owner, itemIndex, enclosure)
{
	JXFontSizeMenuX(nullptr);
	BuildMenu(fontName);
	SetFontSize(JFontManager::GetDefaultFontSize());
}

JXFontSizeMenu::JXFontSizeMenu
	(
	JXFontNameMenu*		fontMenu,
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
	JXFontSizeMenuX(fontMenu);
	const JString fontName = itsFontNameMenu->GetFontName();
	BuildMenu(fontName);
	SetFontSize(JFontManager::GetDefaultFontSize());
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
	SetFontSize(JFontManager::GetDefaultFontSize());
}

// private

void
JXFontSizeMenu::JXFontSizeMenuX
	(
	JXFontNameMenu* fontMenu
	)
{
	itsFontNameMenu = fontMenu;
	if (itsFontNameMenu != nullptr)
	{
		ListenTo(itsFontNameMenu);
	}

	itsBroadcastChangeFlag = true;
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
	const JString& fontName
	)
{
	RemoveAllItems();

	JSize minSize, maxSize;
	JArray<JSize> sizeList;
	GetFontManager()->GetFontSizes(fontName, &minSize, &maxSize, &sizeList);

	JString id;
	if (!sizeList.IsEmpty())
	{
		for (const auto size : sizeList)
		{
			const JString itemText(size);
			id = itemText + "::JX";
			AppendItem(itemText, kRadioType, JString::empty, JString::empty, id);
		}
		itsVarSizeIndex = 0;
	}
	else
	{
		if (minSize < 10)
		{
			for (JSize size=minSize; size<10; size++)
			{
				const JString itemText(size);
				id = itemText + "::JX";
				AppendItem(itemText, kRadioType, JString::empty, JString::empty, id);
			}
		}

		for (JSize size=JMax((JSize) 10, minSize); size<=maxSize; size+=2)
		{
			const JString itemText(size);
			id = itemText + "::JX";
			AppendItem(itemText, kRadioType, JString::empty, JString::empty, id);
		}
		ShowSeparatorAfter(GetItemCount());
		AppendItem(JGetString("VariableSizeLabel::JXFontSizeMenu"),
				   kRadioType, JString::empty, JString::empty,
				   "OtherFontSize::JX");
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
		JString str = JGetString("VariableSizeLabel::JXFontSizeMenu");
		str += " (";
		str += JString(fontSize, 0, JString::kForceNoExponent);
		str += ")";
		SetItemText(itsVarSizeIndex, str);
	}
	else if (itsVarSizeIndex > 0)
	{
		SetItemText(itsVarSizeIndex, JGetString("VariableSizeLabel::JXFontSizeMenu"));
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
		itsBroadcastChangeFlag = false;
		const JSize currSize   = GetFontSize();
		const JString fontName = itsFontNameMenu->GetFontName();
		BuildMenu(fontName);
		SetFontSize(currSize);
		itsBroadcastChangeFlag = true;
	}

	else if (sender == this && message.Is(JXMenu::kNeedsUpdate))
	{
		itsBroadcastChangeFlag = false;
		Broadcast(SizeNeedsUpdate());
		CheckItem(itsCurrIndex);
		itsBroadcastChangeFlag = true;
	}
	else if (sender == this && message.Is(JXMenu::kItemSelected))
	{
		auto& selection = dynamic_cast<const JXMenu::ItemSelected&>(message);
		ChooseFontSize(selection.GetIndex());
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
		const bool ok = GetItemText(sizeIndex).ConvertToUInt(&itsFontSize);
		assert( ok );
		AdjustVarSizeItem(0);
		Broadcast(SizeChanged(itsFontSize));
	}
	else
	{
		auto* dlog = jnew JXChooseFontSizeDialog(itsFontSize);

		if (dlog->DoDialog())
		{
			SetFontSize(dlog->GetFontSize());
		}
		else
		{
			SetPopupChoice(itsCurrIndex);	// revert displayed string
		}
	}
}

/******************************************************************************
 SetToPopupChoice

 ******************************************************************************/

void
JXFontSizeMenu::SetToPopupChoice
	(
	const bool isPopup
	)
{
	JXTextMenu::SetToPopupChoice(isPopup, itsCurrIndex);
}
