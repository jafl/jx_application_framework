/******************************************************************************
 JTEKeyHandler.cpp

	Base class to support processing key presses sent to JTextEditor.

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#include "jx-af/jcore/JTEKeyHandler.h"
#include "jx-af/jcore/jAssert.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JTEKeyHandler::JTEKeyHandler()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTEKeyHandler::~JTEKeyHandler()
{
}

/******************************************************************************
 Initialize (virtual)

	This is necessary because the old key handler is deleted in
	JTextEditor::SetKeyHandler(), *after* the jnew one is constructed.

 ******************************************************************************/

void
JTEKeyHandler::Initialize
	(
	JTextEditor* te
	)
{
	itsTE = te;
}

/******************************************************************************
 InsertKeyPress (protected)

 ******************************************************************************/

void
JTEKeyHandler::InsertKeyPress
	(
	const JUtf8Character& c
	)
{
	itsTE->InsertCharacter(c);
}

/******************************************************************************
 BackwardDelete (protected)

 ******************************************************************************/

void
JTEKeyHandler::BackwardDelete
	(
	const bool	deleteToTabStop,
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
	const bool	deleteToTabStop,
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
