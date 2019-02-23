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
#include <JFontManager.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CMTextDisplayBase::CMTextDisplayBase
	(
	const Type			type,
	const JBoolean		breakCROnly,
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
	JXTEBase(type, breakCROnly, kJFalse, scrollbarSet,
			 enclosure, hSizing, vSizing, x,y, w,h)
{
	WantInput(kJTrue, kJFalse);

	(scrollbarSet->GetVScrollbar())->SetScrollDelay(0);

	AppendEditMenu(menuBar);

	AdjustFont(this);

	ShouldAllowDragAndDrop(kJTrue);
	SetCharacterInWordFunction(CBMIsCharacterInWord);
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
	te->SetDefaultFont(
		JFontManager::GetFont(name, size, CMGetPrefsManager()->GetColor(CMPrefsManager::kTextColorIndex)));

	// colors

	te->SetFocusColor(CMGetPrefsManager()->GetColor(CMPrefsManager::kBackColorIndex));
	te->SetCaretColor(CMGetPrefsManager()->GetColor(CMPrefsManager::kCaretColorIndex));
	te->SetSelectionColor(CMGetPrefsManager()->GetColor(CMPrefsManager::kSelColorIndex));
	te->SetSelectionOutlineColor(CMGetPrefsManager()->GetColor(CMPrefsManager::kSelLineColorIndex));

	// tab width

	const JSize tabCharCount    = CMGetPrefsManager()->GetTabCharCount();
	const JCoordinate charWidth = te->GetDefaultFont().GetCharWidth(fontMgr, ' ');
	te->SetDefaultTabWidth(tabCharCount * charWidth);
	te->SetCRMTabCharCount(tabCharCount);
}
