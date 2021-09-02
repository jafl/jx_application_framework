/******************************************************************************
 CBCharActionManager.cpp

	Stores a list of characters and associated actions.  The action is
	performed immediately after the character is typed.

	BASE CLASS = none

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "CBCharActionManager.h"
#include "CBMacroManager.h"
#include <jAssert.h>

// setup information

const JFileVersion kCurrentSetupVersion   = 1;
const unsigned char kEndOfSetupListMarker = '\1';

	// version 1 no longer stores dialog geometry or column width

/******************************************************************************
 Constructor

 ******************************************************************************/

CBCharActionManager::CBCharActionManager()
{
	itsActionMap = jnew JStringPtrMap<JString>(JPtrArrayT::kDeleteAll);
	assert( itsActionMap != nullptr );
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

CBCharActionManager::CBCharActionManager
	(
	const CBCharActionManager& source
	)
{
	itsActionMap = jnew JStringPtrMap<JString>(JPtrArrayT::kDeleteAll);
	assert( itsActionMap != nullptr );

	JStringMapCursor cursor(source.itsActionMap);
	while (cursor.Next())
		{
		itsActionMap->SetNewElement(
			cursor.GetKey().GetFirstCharacter(),
			*cursor.GetValue());
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBCharActionManager::~CBCharActionManager()
{
	jdelete itsActionMap;
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
CBCharActionManager::Perform
	(
	const JUtf8Character&	c,
	CBTextDocument*			doc
	)
{
	JString* script;
	if (itsActionMap->GetElement(JString(c), &script))
		{
		CBMacroManager::Perform(*script, doc);
		}
}

/******************************************************************************
 SetAction

 ******************************************************************************/

void
CBCharActionManager::SetAction
	(
	const JUtf8Character&	c,
	const JString&			script
	)
{
	if (script.IsEmpty())
		{
		ClearAction(c);
		}
	else
		{
		itsActionMap->SetElement(JString(c), script, JPtrArrayT::kDelete);
		}
}

/******************************************************************************
 ClearAction

 ******************************************************************************/

void
CBCharActionManager::ClearAction
	(
	const JUtf8Character& c
	)
{
	itsActionMap->DeleteElement(JString(c));
}

/******************************************************************************
 ClearAllActions

 ******************************************************************************/

void
CBCharActionManager::ClearAllActions()
{
	itsActionMap->CleanOut();
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

	JUtf8Character c;
	JString script;
	while (true)
		{
		input >> c;
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

	JStringMapCursor cursor(itsActionMap);
	while (cursor.Next())
		{
		output << cursor.GetKey().GetFirstCharacter();
		output << ' ' << *cursor.GetValue();
		}

	output << kEndOfSetupListMarker;
}
