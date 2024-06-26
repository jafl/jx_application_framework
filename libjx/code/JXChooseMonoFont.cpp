/******************************************************************************
 JXChooseMonoFont.cpp

	Widget set that lets the user choose a monospace font and size.

	BASE CLASS = JXWidgetSet

	Copyright (C) 1997-2004 by John Lindal.

 ******************************************************************************/

#include "JXChooseMonoFont.h"
#include "JXXFontMenu.h"
#include "JXFontSizeMenu.h"
#include "JXDisplay.h"
#include "jXConstants.h"
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/JListUtil.h>
#include <jx-af/jcore/jGlobals.h>
#include <stdlib.h>
#include <jx-af/jcore/jAssert.h>

static const JRegex fontRegex("^[1-9][0-9]*x[1-9][0-9]*$");

// JBroadcaster message types

const JUtf8Byte* JXChooseMonoFont::kFontChanged = "FontChanged::JXChooseMonoFont";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXChooseMonoFont::JXChooseMonoFont
	(
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXWidgetSet(enclosure, hSizing, vSizing, x,y, w,h)
{
	const bool ok =
		JXXFontMenu::Create(fontRegex, CompareFontNames, JGetString("FontMenuTitle::JXChooseMonoFont"), this,
							kFixedLeft, kFixedTop, 0,0, w,25, &itsFontMenu);
	assert( ok );
	PrependOtherMonospaceFonts(itsFontMenu);
	itsFontMenu->SetFontName(JFontManager::GetDefaultMonospaceFontName());
	itsFontMenu->SetToPopupChoice();
	ListenTo(itsFontMenu);

	itsSizeMenu = jnew JXFontSizeMenu(JFontManager::GetDefaultMonospaceFontName(), JGetString("SizeMenuTitle::JXChooseMonoFont"), this,
									 kFixedLeft, kFixedTop, 0,30, w,25);
	itsSizeMenu->SetFontSize(JFontManager::GetDefaultMonospaceFontSize());
	itsSizeMenu->SetToPopupChoice();
	ListenTo(itsSizeMenu);

	UpdateMenus(true);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXChooseMonoFont::~JXChooseMonoFont()
{
}

/******************************************************************************
 GetFont

 ******************************************************************************/

void
JXChooseMonoFont::GetFont
	(
	JString*	name,
	JSize*		size
	)
	const
{
	*name = itsFontMenu->GetFontName();
	*size = itsSizeMenu->GetFontSize();
}

/******************************************************************************
 SetFont

 ******************************************************************************/

void
JXChooseMonoFont::SetFont
	(
	const JString&	name,
	const JSize		size
	)
{
	itsFontMenu->SetFontName(name);
	itsSizeMenu->SetFontSize(size);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXChooseMonoFont::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsFontMenu && message.Is(JXFontNameMenu::kNameChanged))
	{
		StopListening(itsSizeMenu);
		UpdateMenus(true);
		ListenTo(itsSizeMenu);
		Broadcast(FontChanged());
	}
	else if (sender == itsSizeMenu && message.Is(JXFontSizeMenu::kSizeChanged))
	{
		UpdateMenus(false);
		Broadcast(FontChanged());
	}
	else
	{
		JXWidgetSet::Receive(sender, message);
	}
}

/******************************************************************************
 UpdateMenus (private)

 ******************************************************************************/

void
JXChooseMonoFont::UpdateMenus
	(
	const bool updateSize
	)
{
	const JString& fontName = itsFontMenu->GetFontName();
	if (fontRegex.Match(fontName))
	{
		itsSizeMenu->Deactivate();
	}
	else
	{
		itsSizeMenu->Activate();
		if (updateSize)
		{
			itsSizeMenu->SetFontName(fontName);
		}
	}
}

/******************************************************************************
 PrependOtherMonospaceFonts (private)

 ******************************************************************************/

void
JXChooseMonoFont::PrependOtherMonospaceFonts
	(
	JXXFontMenu* menu
	)
{
	JXFontManager* fontManager = GetDisplay()->GetXFontManager();

	JPtrArray<JString> fontNames(JPtrArrayT::kDeleteAll);
	fontManager->GetMonospaceFontNames(&fontNames);

	const JSize count = fontNames.GetItemCount();
	if (count > 0)
	{
		for (JIndex i=count; i>=1; i--)
		{
			const JString* fontName = fontNames.GetItem(i);
			menu->PrependItem(*fontName, JXMenu::kRadioType);
			menu->SetItemFontName(1, *fontName);

			fontManager->Preload(menu->GetItemFont(1).GetID());
		}

		menu->ShowSeparatorAfter(count);
	}
}

/******************************************************************************
 CompareFontNames (static private)

	We sort by the first number, then by the second number: #x##

 ******************************************************************************/

std::weak_ordering
JXChooseMonoFont::CompareFontNames
	(
	JString* const & s1,
	JString* const & s2
	)
{
	// compare the value before the 'x'

	char *endPtr1, *endPtr2;
	JUInt v1 = strtoul(s1->GetBytes(), &endPtr1, 10);
	JUInt v2 = strtoul(s2->GetBytes(), &endPtr2, 10);

	auto result = v1 <=> v2;
	if (result == std::weak_ordering::equivalent)
	{
		// compare the value after the 'x'

		v1     = strtoul(endPtr1+1, &endPtr1, 10);
		v2     = strtoul(endPtr2+1, &endPtr2, 10);
		result = v1 <=> v2;
	}
	return result;
}
