/******************************************************************************
 JTEKeyHandler.cpp

	Base class to support processing key presses sent to JTextEditor.

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#include "JTEKeyHandler.h"
#include "jAssert.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JTEKeyHandler::JTEKeyHandler
	(
	JTextEditor* te
	)
	:
	itsTE(te)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTEKeyHandler::~JTEKeyHandler()
{
}

/******************************************************************************
 Initialize (virtual protected)

	This is necessary because the old key handler is deleted in
	JTextEditor::SetKeyHandler(), *after* the jnew one is constructed.

 ******************************************************************************/

void
JTEKeyHandler::Initialize()
{
}

/******************************************************************************
 InsertKeyPress (protected)

 ******************************************************************************/

void
JTEKeyHandler::InsertKeyPress
	(
	const JUtf8Character& key
	)
{
	itsTE->InsertKeyPress(key);
}

/******************************************************************************
 BackwardDelete (protected)

 ******************************************************************************/

void
JTEKeyHandler::BackwardDelete
	(
	const JBoolean	deleteToTabStop,
	JString*		text	// can be nullptr
	)
{
	if (itsTE->HasSelection())
		{
		if (text != nullptr)
			{
			itsTE->GetSelection(text);
			}
		itsTE->DeleteSelection();
		}
	else
		{
		itsTE->BackwardDelete(deleteToTabStop, text);
		}
}

/******************************************************************************
 ForwardDelete (protected)

 ******************************************************************************/

void
JTEKeyHandler::ForwardDelete
	(
	const JBoolean	deleteToTabStop,
	JString*		text	// can be nullptr
	)
{
	if (itsTE->HasSelection())
		{
		if (text != nullptr)
			{
			itsTE->GetSelection(text);
			}
		itsTE->DeleteSelection();
		}
	else
		{
		itsTE->ForwardDelete(deleteToTabStop, text);
		}
}

/******************************************************************************
 MoveCaretVert (protected)

 ******************************************************************************/

void
JTEKeyHandler::MoveCaretVert
	(
	const JInteger deltaLines
	)
{
	itsTE->MoveCaretVert(deltaLines);
}
