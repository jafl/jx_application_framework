/******************************************************************************
 CBVIKeyHandler.cpp

	Class to implement vi keybindings.

	BASE CLASS = JXVIKeyHandler

	Copyright © 2010 by John Lindal.

 ******************************************************************************/

#include "CBVIKeyHandler.h"
#include "CBTextDocument.h"
#include "CBTextEditor.h"
#include "CBTELineIndexInput.h"
#include <jASCIIConstants.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBVIKeyHandler::CBVIKeyHandler
	(
	CBTextEditor* te
	)
	:
	itsCBTE(te)
{
	Initialize(te);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBVIKeyHandler::~CBVIKeyHandler()
{
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

bool
CBVIKeyHandler::HandleKeyPress
	(
	const JUtf8Character&			key,
	const bool					selectText,
	const JTextEditor::CaretMotion	motion,
	const bool					deleteToTabStop
	)
{
	bool result;
	if (PrehandleKeyPress(key, &result))
		{
		return result;
		}

	if (key == ':')
		{
		SetMode(kCommandLineMode);
		return true;
		}
	else if (GetMode() == kCommandLineMode && GetCommandLine().IsEmpty() &&
			 key.IsDigit() && key != '0')
		{
		CBTELineIndexInput* field = itsCBTE->GetLineInput();
		field->Focus();

		field->GetText()->SetText(JString(key));
		field->GoToEndOfLine();

		SetMode(kCommandMode);
		return true;
		}
	else if (GetMode() == kCommandLineMode && key == '\n')
		{
		CBTextDocument* doc = itsCBTE->GetDocument();
		const JString& buf  = GetCommandLine();
		if (buf == "0")
			{
			itsCBTE->SetCaretLocation(1);
			}
		else if (buf == "$")
			{
			itsCBTE->SetCaretLocation(itsCBTE->GetText()->GetText().GetCharacterCount()+1);
			}
		else if (buf == "w" || buf == "w!")
			{
			doc->SaveInCurrentFile();
			}
		else if (buf == "q")
			{
			doc->Close();
			return true;
			}
		else if (buf == "q!")
			{
			doc->DataReverted(true);
			doc->Close();
			return true;
			}
		else if (buf == "wq")
			{
			doc->SaveInCurrentFile();
			doc->Close();
			return true;
			}

		SetMode(kCommandMode);
		return true;
		}
	else if (GetMode() == kCommandLineMode)
		{
		AppendToCommandLine(key);
		return true;
		}

	else
		{
		return JXVIKeyHandler::HandleKeyPress(key, selectText, motion, deleteToTabStop);
		}
}
