/******************************************************************************
 JXChooseMonoFont.cpp

	Widget set that lets the user choose a monospace font and size.

	BASE CLASS = JXWidgetSet

	Copyright © 1997-2004 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXChooseMonoFont.h>
#include <JXXFontMenu.h>
#include <JXFontSizeMenu.h>
#include <JXFontCharSetMenu.h>
#include <jXConstants.h>
#include <JString.h>
#include <JRegex.h>
#include <stdlib.h>
#include <jGlobals.h>
#include <jAssert.h>

static const JRegex fontRegex = "^[1-9][0-9]*x[1-9][0-9]*$";

static const JCharacter* kFontMenuTitleStr = "Font:";
static const JCharacter* kDefaultFontName  = "6x13";

static const JCharacter* kSizeMenuTitleStr = "Size:";

static const JCharacter* kCharSetMenuTitleStr = "Character set:";

// JBroadcaster message types

const JCharacter* JXChooseMonoFont::kFontChanged = "FontChanged::JXChooseMonoFont";

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
	const JBoolean ok =
		JXXFontMenu::Create(fontRegex, CompareFontNames, kFontMenuTitleStr, this,
							kFixedLeft, kFixedTop, 0,0, w,25, &itsFontMenu);
	assert( ok );
	AppendOtherMonospaceFonts(itsFontMenu);
	itsFontMenu->SetFontName(kDefaultFontName);
	itsFontMenu->SetToPopupChoice();
	ListenTo(itsFontMenu);

	itsSizeMenu = new JXFontSizeMenu(JGetMonospaceFontName(), kSizeMenuTitleStr, this,
									 kFixedLeft, kFixedTop, 0,30, w,25);
	assert( itsSizeMenu != NULL );
	itsSizeMenu->SetFontSize(kJXDefaultFontSize);
	itsSizeMenu->SetToPopupChoice();
	ListenTo(itsSizeMenu);

	itsCharSetMenu = new JXFontCharSetMenu(JGetMonospaceFontName(), itsSizeMenu->GetFontSize(),
										   kCharSetMenuTitleStr, this,
										   kFixedLeft, kFixedTop, 0,60, w,25);
	assert( itsCharSetMenu != NULL );
	itsCharSetMenu->SetToPopupChoice();
	ListenTo(itsCharSetMenu);

	UpdateMenus(kJTrue);
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

	if (!fontRegex.Match(*name))
		{
		*name = JFontManager::CombineNameAndCharacterSet(*name, itsCharSetMenu->GetCharSet());
		}
}

/******************************************************************************
 SetFont

 ******************************************************************************/

void
JXChooseMonoFont::SetFont
	(
	const JCharacter*	name,
	const JSize			size
	)
{
	JString n, s;
	JFontManager::ExtractCharacterSet(name, &n, &s);

	itsFontMenu->SetFontName(n);
	itsSizeMenu->SetFontSize(size);
	itsCharSetMenu->SetCharSet(s);
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
	if (sender == itsFontMenu && message.Is(JXXFontMenu::kFontChanged))
		{
		StopListening(itsSizeMenu);
		StopListening(itsCharSetMenu);
		UpdateMenus(kJTrue);
		ListenTo(itsSizeMenu);
		ListenTo(itsCharSetMenu);
		Broadcast(FontChanged());
		}
	else if (sender == itsSizeMenu && message.Is(JXFontSizeMenu::kSizeChanged))
		{
		StopListening(itsCharSetMenu);
		UpdateMenus(kJFalse);
		ListenTo(itsCharSetMenu);
		Broadcast(FontChanged());
		}
	else if (sender == itsCharSetMenu && message.Is(JXFontCharSetMenu::kCharSetChanged))
		{
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
	const JBoolean updateSize
	)
{
	const JString& fontName = itsFontMenu->GetFontName();
	if (fontRegex.Match(fontName))
		{
		itsSizeMenu->Deactivate();
		itsCharSetMenu->Deactivate();
		}
	else
		{
		itsSizeMenu->Activate();
		itsCharSetMenu->Activate();
		if (updateSize)
			{
			itsSizeMenu->SetFontName(fontName);
			}
		itsCharSetMenu->SetFont(fontName, itsSizeMenu->GetFontSize());
		}
}

/******************************************************************************
 AppendOtherMonospaceFonts (private)

 ******************************************************************************/

void
JXChooseMonoFont::AppendOtherMonospaceFonts
	(
	JXXFontMenu* menu
	)
{
	JPtrArray<JString> fontNames(JPtrArrayT::kDeleteAll);
	(GetFontManager())->GetMonospaceFontNames(&fontNames);

	const JSize count = fontNames.GetElementCount();
	if (count > 0)
		{
		menu->ShowSeparatorAfter(menu->GetItemCount());		// JXXFontMenu guarantees non-empty

		for (JIndex i=1; i<=count; i++)
			{
			const JString* fontName = fontNames.NthElement(i);
			menu->AppendFontItem(*fontName, kJFalse);
			}
		}
}

/******************************************************************************
 CompareFontNames (static private)

	We sort by the first number, then by the second number: #x##

 ******************************************************************************/

JOrderedSetT::CompareResult
JXChooseMonoFont::CompareFontNames
	(
	JString* const & s1,
	JString* const & s2
	)
{
	// compare the value before the 'x'

	char *endPtr1, *endPtr2;
	JUInt v1 = strtoul(*s1, &endPtr1, 10);
	JUInt v2 = strtoul(*s2, &endPtr2, 10);

	if (v1 > v2)
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
	else if (v1 < v2)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else
		{
		// compare the value after the 'x'

		v1 = strtoul(endPtr1+1, &endPtr1, 10);
		v2 = strtoul(endPtr2+1, &endPtr2, 10);

		if (v1 > v2)
			{
			return JOrderedSetT::kFirstGreaterSecond;
			}
		else if (v1 < v2)
			{
			return JOrderedSetT::kFirstLessSecond;
			}
		else
			{
			return JOrderedSetT::kFirstEqualSecond;
			}
		}
}
