/******************************************************************************
 CBPHPClass.cpp

	BASE CLASS = CBClass

	Copyright (C) 2014 John Lindal.

 ******************************************************************************/

#include "CBPHPClass.h"
#include "CBTree.h"
#include "CBTextDocument.h"
#include "CBTextEditor.h"
#include "cbGlobals.h"
#include <JXColorManager.h>
#include <JRegex.h>
#include <jAssert.h>

static const JUtf8Byte* kNamespaceOperator = "\\";

/******************************************************************************
 Constructor

 ******************************************************************************/

CBPHPClass::CBPHPClass
	(
	const JString&		name,
	const DeclareType	declType,
	const JFAID_t		fileID,
	CBTree*				tree,
	const JBoolean		isFinal
	)
	:
	CBClass(name, declType, fileID, tree, kNamespaceOperator),
	itsIsFinalFlag(isFinal)
{
}

CBPHPClass::CBPHPClass
	(
	std::istream&			input,
	const JFileVersion	vers,
	CBTree*				tree
	)
	:
	CBClass(input, vers, tree, kNamespaceOperator)
{
	input >> JBoolFromString(itsIsFinalFlag);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBPHPClass::~CBPHPClass()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
CBPHPClass::StreamOut
	(
	std::ostream& output
	)
	const
{
	CBClass::StreamOut(output);

	output << ' ' << JBoolToString(itsIsFinalFlag);
	output << ' ';
}

/******************************************************************************
 ViewSource (virtual)

 ******************************************************************************/

void
CBPHPClass::ViewSource()
	const
{
	JString fileName;
	if (GetFileName(&fileName))
		{
		(CBGetDocumentManager())->OpenTextDocument(fileName);
		}
	else
		{
		JGetUserNotification()->ReportError(JGetString("NoGhostFile::CBClass"));
		}
}

/******************************************************************************
 ViewHeader (virtual)

 ******************************************************************************/

void
CBPHPClass::ViewHeader()
	const
{
}

/******************************************************************************
 NewGhost (virtual protected)

	Creates a new ghost CBPHPClass.

 ******************************************************************************/

CBClass*
CBPHPClass::NewGhost
	(
	const JString&	name,
	CBTree*			tree
	)
{
	CBPHPClass* newClass = jnew CBPHPClass(name, kGhostType, JFAID::kInvalidID, tree,
										  kJFalse);
	assert( newClass != nullptr );
	return newClass;
}

/******************************************************************************
 IsInherited (virtual)

	Returns kJTrue if the specified function is inherited by derived classes.
	Constructors, destructors, and private functions are not inherited.

	If it is inherited, *access contains the access level adjusted according
	to the inheritance access.

 ******************************************************************************/

JBoolean
CBPHPClass::IsInherited
	(
	const JIndex		index,
	const InheritType	inherit,
	FnAccessLevel*		access
	)
	const
{
	const JString& fnName = GetFunctionName(index);
	*access               = GetFnAccessLevel(index);

	return JI2B(*access != kPrivateAccess &&	// private
				fnName != GetName()       &&	// ctor
				fnName != "__construct"   &&	// ctor
				fnName != "__destruct");		// dtor
}

/******************************************************************************
 AdjustNameStyle (virtual protected)

 ******************************************************************************/

void
CBPHPClass::AdjustNameStyle
	(
	JFontStyle* style
	)
	const
{
	CBClass::AdjustNameStyle(style);

	if (GetDeclareType() != kGhostType)
		{
		if (itsIsFinalFlag)
			{
			style->bold = kJTrue;
			}
		}
}
