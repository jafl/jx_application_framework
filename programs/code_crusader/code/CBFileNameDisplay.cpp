/******************************************************************************
 CBFileNameDisplay.cpp

	BASE CLASS = JXFileInput

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#include "CBFileNameDisplay.h"
#include "CBFileDragSource.h"
#include "CBTextDocument.h"
#include "cbUtil.h"
#include <JXMenu.h>
#include <JXColorManager.h>
#include <jFileUtil.h>
#include <jVCSUtil.h>
#include <jASCIIConstants.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBFileNameDisplay::CBFileNameDisplay
	(
	CBTextDocument*		doc,
	CBFileDragSource*	dragSource,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXFileInput(jnew StyledText(this, enclosure->GetFontManager()),
				enclosure, hSizing, vSizing, x,y, w,h)
{
	assert( dragSource != nullptr );	// force ordering of JXLayout code

	itsTE             = nullptr;
	itsUnfocusAction  = kCancel;
	itsDoc            = doc;
	itsDragSource     = dragSource;
	itsDiskModFlag    = kJFalse;
	itsCBHasFocusFlag = kJFalse;
	SetIsRequired(kJFalse);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBFileNameDisplay::~CBFileNameDisplay()
{
}

/******************************************************************************
 SetTE

	We are created before the text editor so we can't get the pointer
	in our constructor.

 ******************************************************************************/

void
CBFileNameDisplay::SetTE
	(
	JXTEBase* te
	)
{
	itsTE = te;

	// this re-orders the list so text editor gets focus

	WantInput(kJFalse);
	WantInput(kJTrue, WantsTab(), kJTrue);
}

/******************************************************************************
 DiskCopyIsModified

 ******************************************************************************/

void
CBFileNameDisplay::DiskCopyIsModified
	(
	const JBoolean mod
	)
{
	itsDiskModFlag = mod;
	UpdateDisplay(HasFocus());
}

/******************************************************************************
 UpdateDisplay (private)

 ******************************************************************************/

void
CBFileNameDisplay::UpdateDisplay
	(
	const JBoolean hasFocus
	)
{
	itsCBHasFocusFlag = hasFocus;
	if (!hasFocus && itsDiskModFlag)
		{
		SetFontStyle(JColorManager::GetDarkRedColor());
		}
	else if (!hasFocus)
		{
		SetFontStyle(JColorManager::GetBlackColor());
		}
}

/******************************************************************************
 HandleFocusEvent (virtual protected)

 ******************************************************************************/

void
CBFileNameDisplay::HandleFocusEvent()
{
	UpdateDisplay(kJTrue);					// allow JXFileInput to control text style
	itsDragSource->ProvideDirectSave(this);

	itsOrigFile = GetText()->GetText();
	GetText()->SetText(itsOrigFile);		// force recalc of styles

	JXFileInput::HandleFocusEvent();
}

/******************************************************************************
 HandleUnfocusEvent (virtual protected)

 ******************************************************************************/

void
CBFileNameDisplay::HandleUnfocusEvent()
{
	JXFileInput::HandleUnfocusEvent();

	JBoolean saved = kJFalse;

	JString fullName;
	if (itsUnfocusAction != kCancel &&
		JExpandHomeDirShortcut(GetText()->GetText(), &fullName))
		{
		if (JIsRelativePath(fullName) &&
			!JGetChooseSaveFile()->SaveFile(JGetString("SaveAsPrompt::CBFileNameDisplay"), JString::empty, fullName, &fullName))
			{
			fullName.Clear();
			}

		if (!fullName.IsEmpty() &&
			!JSameDirEntry(fullName, itsOrigFile) &&
			itsDoc->SaveInNewFile(fullName))
			{
			saved = kJTrue;

			JBoolean onDisk;
			fullName = itsDoc->GetFullName(&onDisk);
			GetText()->SetText(fullName);

			if (itsUnfocusAction == kRename)
				{
				CBCleanUpAfterRename(itsOrigFile, fullName);
				}
			}
		}

	if (!saved)
		{
		GetText()->SetText(itsOrigFile);
		}

	UpdateDisplay(kJFalse);			// take control of text style
	itsDragSource->ProvideDirectSave(nullptr);
}

/******************************************************************************
 InputValid (virtual)

 ******************************************************************************/

JBoolean
CBFileNameDisplay::InputValid()
{
	const JString& text = GetText()->GetText();
	if (itsUnfocusAction == kCancel)
		{
		return kJTrue;
		}
	else if (text.IsEmpty())
		{
		JGetUserNotification()->ReportError(JGetString("EmptyError::CBFileNameDisplay"));
		return kJFalse;
		}
	else if (text.EndsWith(ACE_DIRECTORY_SEPARATOR_STR))
		{
		JGetUserNotification()->ReportError(JGetString("NoFileName::CBFileNameDisplay"));
		return kJFalse;
		}
	else if (JIsRelativePath(text))
		{
		return kJTrue;	// open Choose File dialog
		}

	JString path, fullPath, name;
	JSplitPathAndName(text, &path, &name);
	if (!JExpandHomeDirShortcut(path, &fullPath))
		{
		return kJFalse;
		}

	const JString fullName = JCombinePathAndName(fullPath, name);
	JBoolean onDisk;
	const JString origFullName = itsDoc->GetFullName(&onDisk);
	if (onDisk && JSameDirEntry(origFullName, fullName))
		{
		itsUnfocusAction = kCancel;
		return kJTrue;
		}

	if (JFileExists(fullName))
		{
		const JUtf8Byte* map[] =
			{
			"f", text.GetBytes()
			};
		const JString msg = JGetString("OKToReplace::CBFileNameDisplay", map, sizeof(map));
		if (!JGetUserNotification()->AskUserNo(msg))
			{
			return kJFalse;
			}
		}

	if (!JDirectoryExists(fullPath))
		{
		const JError err = JCreateDirectory(fullPath);
		if (!err.OK())
			{
			err.ReportIfError();
			return kJFalse;
			}
		}
	else if (!JDirectoryWritable(fullPath))
		{
		const JError err = JAccessDenied(fullPath);
		err.ReportIfError();
		return kJFalse;
		}

	if (itsUnfocusAction == kRename)
		{
		const JError err = JRenameVCS(origFullName, fullName);
		err.ReportIfError();
		return err.OK();
		}
	else
		{
		return kJTrue;
		}
}

/******************************************************************************
 HandleKeyPress (virtual)

	Escape cancels the changes.
	Return and tab switch focus to the text editor.
	Ctrl-tab is handled by the text editor.

 ******************************************************************************/

void
CBFileNameDisplay::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	if (c == kJEscapeKey)
		{
		itsTE->Focus();
		}
	else if (c == '\t' &&
			 !modifiers.GetState(kJXMetaKeyIndex)   &&
			 modifiers.GetState(kJXControlKeyIndex) &&
			 !modifiers.shift())
		{
		itsTE->Focus();
		itsTE->HandleKeyPress(c, keySym, modifiers);
		}
	else if ((c == '\r' || c == '\n') &&
			 !modifiers.GetState(JXMenu::AdjustNMShortcutModifier(kJXMetaKeyIndex)) &&
			 !modifiers.shift())
		{
		itsUnfocusAction =
			modifiers.GetState(JXMenu::AdjustNMShortcutModifier(kJXControlKeyIndex)) ?
			kRename : kSaveAs;
		itsTE->Focus();				// trigger HandleUnfocusEvent()
		itsUnfocusAction = kCancel;
		}
	else
		{
		JXFileInput::HandleKeyPress(c, keySym, modifiers);
		}
}

/******************************************************************************
 AdjustStylesBeforeBroadcast (virtual protected)

	Don't let JXFileInput adjust the styles unless we have focus.

 ******************************************************************************/

void
CBFileNameDisplay::StyledText::AdjustStylesBeforeBroadcast
	(
	const JString&			text,
	JRunArray<JFont>*		styles,
	JStyledText::TextRange*	recalcRange,
	JStyledText::TextRange*	redrawRange,
	const JBoolean			deletion
	)
{
	if (itsCBField->itsCBHasFocusFlag)
		{
		JXFileInput::StyledText::AdjustStylesBeforeBroadcast(
			text, styles, recalcRange, redrawRange, deletion);
		}
}
