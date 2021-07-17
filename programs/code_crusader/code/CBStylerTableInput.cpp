/******************************************************************************
 CBStylerTableInput.cpp

	Class to edit a string "in place" in a JXEditTable.

	BASE CLASS = JXInputField

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "CBStylerTableInput.h"
#include "CBStylerTable.h"
#include "cbmUtil.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBStylerTableInput::CBStylerTableInput
	(
	const CBTextFileType	fileType,
	CBStylerTable*			enclosure,
	const HSizingOption		hSizing,
	const VSizingOption		vSizing,
	const JCoordinate		x,
	const JCoordinate		y,
	const JCoordinate		w,
	const JCoordinate		h
	)
	:
	JXInputField(enclosure, hSizing, vSizing, x,y, w,h)
{
	itsStylerTable = enclosure;
	itsFileType    = fileType;

	CBMPrefsManager* prefsMgr = CBMGetPrefsManager();
	SetCaretColor(prefsMgr->GetColor(CBMPrefsManager::kCaretColorIndex));
	SetSelectionColor(prefsMgr->GetColor(CBMPrefsManager::kSelColorIndex));
	SetSelectionOutlineColor(prefsMgr->GetColor(CBMPrefsManager::kSelLineColorIndex));

	GetText()->SetCharacterInWordFunction([this] (const JUtf8Character& c)
	{
		return CBMIsCharacterInWord(c) || CBIsCharacterInWord(this->itsFileType, c);
	});
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBStylerTableInput::~CBStylerTableInput()
{
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
CBStylerTableInput::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if ((button == kJXLeftButton && modifiers.meta()) ||
		button == kJXRightButton)
		{
		JPoint cell;
		const bool ok = itsStylerTable->GetEditedCell(&cell);
		assert( ok );
		itsStylerTable->DisplayFontMenu(cell, this, pt, buttonStates, modifiers);
		}
	else
		{
		JXInputField::HandleMouseDown(pt, button, clickCount,
									  buttonStates, modifiers);
		}
}
