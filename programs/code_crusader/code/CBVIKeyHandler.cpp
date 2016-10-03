/******************************************************************************
 CBVIKeyHandler.cpp

	Class to implement vi keybindings.

	BASE CLASS = JXVIKeyHandler

	Copyright (C) 2010 by John Lindal. All rights reserved.

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
	JXVIKeyHandler(te),
	itsCBTE(te)
{
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

JBoolean
CBVIKeyHandler::HandleKeyPress
	(
	const JCharacter				key,
	const JBoolean					selectText,
	const JTextEditor::CaretMotion	motion,
	const JBoolean					deleteToTabStop
	)
{
	JBoolean result;
	if (PrehandleKeyPress(key, &result))
		{
		return result;
		}

	if (key == ':')
		{
		SetMode(kCommandLineMode);
		return kJTrue;
		}
	else if (GetMode() == kCommandLineMode && GetCommandLine().IsEmpty() &&
			 isdigit(key) && key != '0')
		{
		CBTELineIndexInput* field = itsCBTE->GetLineInput();
		field->Focus();

		const JCharacter s[2] = { key, '\0' };
		field->SetText(s);
		field->GoToEndOfLine();

		SetMode(kCommandMode);
		return kJTrue;
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
			itsCBTE->SetCaretLocation(itsCBTE->GetTextLength()+1);
			}
		else if (buf == "w" || buf == "w!")
			{
			doc->SaveInCurrentFile();
			}
		else if (buf == "q")
			{
			doc->Close();
			return kJTrue;
			}
		else if (buf == "q!")
			{
			doc->DataReverted(kJTrue);
			doc->Close();
			return kJTrue;
			}
		else if (buf == "wq")
			{
			doc->SaveInCurrentFile();
			doc->Close();
			return kJTrue;
			}

		SetMode(kCommandMode);
		return kJTrue;
		}
	else if (GetMode() == kCommandLineMode)
		{
		AppendToCommandLine(key);
		return kJTrue;
		}

	else
		{
		return JXVIKeyHandler::HandleKeyPress(key, selectText, motion, deleteToTabStop);
		}
}
