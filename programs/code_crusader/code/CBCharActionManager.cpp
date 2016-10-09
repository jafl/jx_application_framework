/******************************************************************************
 CBCharActionManager.cpp

	Stores a list of characters and associated actions.  The action is
	performed immediately after the character is typed.

	BASE CLASS = none

	Copyright (C) 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBCharActionManager.h"
#include "CBMacroManager.h"
#include <jAssert.h>

const JSize kActionCount = 256;

// setup information

const JFileVersion kCurrentSetupVersion   = 1;
const unsigned char kEndOfSetupListMarker = '\1';

	// version 1 no longer stores dialog geometry or column width

/******************************************************************************
 Constructor

 ******************************************************************************/

CBCharActionManager::CBCharActionManager()
{
	itsActionList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll, kActionCount);
	assert( itsActionList != NULL );

	ClearAllActions();
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

CBCharActionManager::CBCharActionManager
	(
	const CBCharActionManager& source
	)
{
	itsActionList = jnew JDCCPtrArray<JString>(*(source.itsActionList), JPtrArrayT::kDeleteAll);
	assert( itsActionList != NULL );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBCharActionManager::~CBCharActionManager()
{
	jdelete itsActionList;
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
CBCharActionManager::Perform
	(
	const unsigned char	c,
	CBTextDocument*		doc
	)
{
	const JString* script = itsActionList->NthElement(c+1);
	if (script != NULL)
		{
		CBMacroManager::Perform(*script, doc);
		}
}

/******************************************************************************
 GetAction

 ******************************************************************************/

JBoolean
CBCharActionManager::GetAction
	(
	const unsigned char	c,
	JString*			script
	)
	const
{
	const JString* s = itsActionList->NthElement(c+1);
	if (s != NULL)
		{
		*script = *s;
		return kJTrue;
		}
	else
		{
		script->Clear();
		return kJFalse;
		}
}

/******************************************************************************
 SetAction

 ******************************************************************************/

void
CBCharActionManager::SetAction
	(
	const unsigned char	c,
	const JCharacter*	script
	)
{
	if (JStringEmpty(script))
		{
		ClearAction(c);
		}
	else
		{
		itsActionList->SetElement(c+1, script, JPtrArrayT::kDelete);
		}
}

/******************************************************************************
 ClearAction

 ******************************************************************************/

void
CBCharActionManager::ClearAction
	(
	const unsigned char c
	)
{
	itsActionList->SetToNull(c+1, JPtrArrayT::kDelete);
}

/******************************************************************************
 ClearAllActions

 ******************************************************************************/

void
CBCharActionManager::ClearAllActions()
{
	itsActionList->DeleteAll();
	for (JIndex i=1; i<=kActionCount; i++)
		{
		itsActionList->Append(NULL);
		}
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
CBCharActionManager::ReadSetup
	(
	istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentSetupVersion)
		{
		return;
		}

	ClearAllActions();

	input.ignore(1);

	char c;
	JString script;
	while (1)
		{
		input.get(c);
		if (c == kEndOfSetupListMarker)
			{
			break;
			}
		input >> script;
		SetAction(c, script);
		}

	if (vers == 0)
		{
		JString geom;
		JCoordinate width;
		input >> geom >> width;
		}
}

/******************************************************************************
 WriteSetup

 ******************************************************************************/

void
CBCharActionManager::WriteSetup
	(
	ostream& output
	)
	const
{
	output << ' ' << kCurrentSetupVersion << ' ';

	for (JIndex i=1; i<=kActionCount; i++)
		{
		const JString* script = itsActionList->NthElement(i);
		if (script != NULL)
			{
			output.put((unsigned char) i-1);
			output << *script;
			}
		}

	output << kEndOfSetupListMarker;
}
