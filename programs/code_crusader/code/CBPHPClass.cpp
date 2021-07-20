/******************************************************************************
 CBPHPClass.cpp

	BASE CLASS = CBClass

	Copyright © 2014 John Lindal.

 ******************************************************************************/

#include "CBPHPClass.h"
#include "CBTree.h"
#include "CBTextDocument.h"
#include "CBTextEditor.h"
#include "cbGlobals.h"
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
	const bool		isFinal
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
#ifndef CODE_CRUSADER_UNIT_TEST

	JString fileName;
	if (GetFileName(&fileName))
		{
		CBGetDocumentManager()->OpenTextDocument(fileName);
		}
	else
		{
		JGetUserNotification()->ReportError(JGetString("NoGhostFile::CBTreeWidget"));
		}

#endif
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
	auto* newClass = jnew CBPHPClass(name, kGhostType, JFAID::kInvalidID, tree,
										  false);
	assert( newClass != nullptr );
	return newClass;
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
			style->bold = true;
			}
		}
}
