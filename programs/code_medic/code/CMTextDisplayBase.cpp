/******************************************************************************
 CMTextDisplayBase.cpp

	BASE CLASS = JXTEBase

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "CMTextDisplayBase.h"
#include "cmGlobals.h"
#include "cbmUtil.h"
#include <JXScrollbarSet.h>
#include <JXScrollbar.h>
#include <JXStyledText.h>
#include <JFontManager.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CMTextDisplayBase::CMTextDisplayBase
	(
	const Type			type,
	const bool		breakCROnly,
	JXMenuBar*			menuBar,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXTEBase(type,
			 jnew JXStyledText(false, false, enclosure->GetFontManager()),
			 true, breakCROnly, scrollbarSet,
			 enclosure, hSizing, vSizing, x,y, w,h)
{
	CMTextDisplayBaseX(menuBar, scrollbarSet);
}

CMTextDisplayBase::CMTextDisplayBase
	(
	JXStyledText*		text,
	const Type			type,
	const bool		breakCROnly,
	JXMenuBar*			menuBar,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXTEBase(type, text, true, breakCROnly, scrollbarSet,
			 enclosure, hSizing, vSizing, x,y, w,h)
{
	CMTextDisplayBaseX(menuBar, scrollbarSet);
}

// private

void
CMTextDisplayBase::CMTextDisplayBaseX
	(
	JXMenuBar*		menuBar,
	JXScrollbarSet*	scrollbarSet
	)
{
	WantInput(true, false);

	scrollbarSet->GetVScrollbar()->SetScrollDelay(0);

	AppendEditMenu(menuBar);

	AdjustFont(this);

	ShouldAllowDragAndDrop(true);
	GetText()->SetCharacterInWordFunction(CBMIsCharacterInWord);
	SetPTPrinter(CMGetPTPrinter());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMTextDisplayBase::~CMTextDisplayBase()
{
}

/******************************************************************************
 AdjustFont (static)

	Shared with CMCommandInput.

 ******************************************************************************/

void
CMTextDisplayBase::AdjustFont
	(
	JXTEBase* te
	)
{
	JFontManager* fontMgr = te->TEGetFontManager();

	JString name;
	JSize size;
	CMGetPrefsManager()->GetDefaultFont(&name, &size);
	te->GetText()->SetDefaultFont(
		JFontManager::GetFont(name, size, CMGetPrefsManager()->GetColor(CMPrefsManager::kTextColorIndex)));

	// colors

	te->SetFocusColor(CMGetPrefsManager()->GetColor(CMPrefsManager::kBackColorIndex));
	te->SetCaretColor(CMGetPrefsManager()->GetColor(CMPrefsManager::kCaretColorIndex));
	te->SetSelectionColor(CMGetPrefsManager()->GetColor(CMPrefsManager::kSelColorIndex));
	te->SetSelectionOutlineColor(CMGetPrefsManager()->GetColor(CMPrefsManager::kSelLineColorIndex));

	// tab width

	const JSize tabCharCount    = CMGetPrefsManager()->GetTabCharCount();
	const JCoordinate charWidth = te->GetText()->GetDefaultFont().GetCharWidth(fontMgr, JUtf8Character(' '));
	te->SetDefaultTabWidth(tabCharCount * charWidth);
	te->GetText()->SetCRMTabCharCount(tabCharCount);
}
