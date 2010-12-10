/******************************************************************************
 JTEKeyHandler.cpp

	Base class to support overriding key presses sent to JTextEditor.

	Copyright © 2010 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JTEKeyHandler.h>
#include <jAssert.h>

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
 InsertKeyPress (protected; delegation)

 ******************************************************************************/

void
JTEKeyHandler::InsertKeyPress
	(
	const JCharacter key
	)
{
	itsTE->InsertKeyPress(key);
}

/******************************************************************************
 BackwardDelete (protected; delegation)

 ******************************************************************************/

void
JTEKeyHandler::BackwardDelete
	(
	const JBoolean deleteToTabStop
	)
{
	if (itsTE->HasSelection())
		{
		itsTE->DeleteSelection();
		}
	else
		{
		itsTE->BackwardDelete(deleteToTabStop);
		}
}

/******************************************************************************
 ForwardDelete (protected; delegation)

 ******************************************************************************/

void
JTEKeyHandler::ForwardDelete
	(
	const JBoolean deleteToTabStop
	)
{
	if (itsTE->HasSelection())
		{
		itsTE->DeleteSelection();
		}
	else
		{
		itsTE->ForwardDelete(deleteToTabStop);
		}
}

/******************************************************************************
 MoveCaretVert (protected; delegation)

 ******************************************************************************/

void
JTEKeyHandler::MoveCaretVert
	(
	const JInteger deltaLines
	)
{
	itsTE->MoveCaretVert(deltaLines);
}
