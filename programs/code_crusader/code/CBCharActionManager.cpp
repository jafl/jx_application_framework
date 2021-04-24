/******************************************************************************
 CBCharActionManager.cpp

	Stores a list of characters and associated actions.  The action is
	performed immediately after the character is typed.

	BASE CLASS = none

	Copyright (C) 1998 by John Lindal.

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
	assert( itsActionList != nullptr );

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
	assert( itsActionList != nullptr );
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
	const JString* script = itsActionList->GetElement(c+1);
	if (script != nullptr)
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
	const JString* s = itsActionList->GetElement(c+1);
	if (s != nullptr)
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
	const JString&		script
	)
{
	if (script.IsEmpty())
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
		itsActionList->Append(nullptr);
		}
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
CBCharActionManager::ReadSetup
	(
	std::istream& input
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
	std::ostream& output
	)
	const
{
	output << ' ' << kCurrentSetupVersion << ' ';

	for (JIndex i=1; i<=kActionCount; i++)
		{
		const JString* script = itsActionList->GetElement(i);
		if (script != nullptr)
			{
			output.put((unsigned char) i-1);
			output << *script;
			}
		}

	output << kEndOfSetupListMarker;
}
