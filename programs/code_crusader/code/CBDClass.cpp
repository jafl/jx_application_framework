/******************************************************************************
 CBDClass.cpp

	BASE CLASS = CBClass

	Copyright © 2021 John Lindal.

 ******************************************************************************/

#include "CBDClass.h"
#include "CBTree.h"
#include "CBTextDocument.h"
#include "CBTextEditor.h"
#include "cbGlobals.h"
#include <JRegex.h>
#include <jAssert.h>

static const JUtf8Byte* kNamespaceOperator = ".";

/******************************************************************************
 Constructor

 ******************************************************************************/

CBDClass::CBDClass
	(
	const JString&		name,
	const DeclareType	declType,
	const JFAID_t		fileID,
	CBTree*				tree,
	const bool			isFinal
	)
	:
	CBClass(name, declType, fileID, tree, kNamespaceOperator),
	itsIsFinalFlag(isFinal)
{
}

CBDClass::CBDClass
	(
	std::istream&		input,
	const JFileVersion	vers,
	CBTree*				tree
	)
	:
	CBClass(input, vers, tree, kNamespaceOperator),
	itsIsFinalFlag(false)
{
	if (vers >= 88)
		{
		input >> JBoolFromString(itsIsFinalFlag);
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBDClass::~CBDClass()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
CBDClass::StreamOut
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
CBDClass::ViewSource()
	const
{
#ifndef CODE_CRUSADER_UNIT_TEST

	JString fileName;
	if (GetFileName(&fileName))
		{
		CBDocumentManager* docMgr = CBGetDocumentManager();

		CBTextDocument* doc = nullptr;
		if (docMgr->OpenTextDocument(fileName, 0, &doc))
			{
			JString p("(class|interface)[ \t\n]*");
			p += GetName();
			p += "\\b";
			const JRegex r(p);

			CBTextEditor* te = doc->GetTextEditor();
			te->SetCaretLocation(1);

			const JStyledText::TextIndex start(1,1);
			bool wrapped;
			const JStringMatch m =
				te->GetText()->SearchForward(start, r, false, false, &wrapped);
			if (!m.IsEmpty())
				{
				te->SelectLine(te->GetLineForChar(m.GetCharacterRange().first));
				te->ScrollForDefinition(kCBDLang);
				}
			else
				{
				const JUtf8Byte* map[] =
				{
					"name", GetName().GetBytes()
				};
				const JString msg = JGetString("NoDefinition::CBClass", map, sizeof(map));
				JGetUserNotification()->ReportError(msg);
				}
			}
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
CBDClass::ViewHeader()
	const
{
}


/******************************************************************************
 NewGhost (virtual protected)

	Creates a new ghost CBDClass.

 ******************************************************************************/

CBClass*
CBDClass::NewGhost
	(
	const JString&	name,
	CBTree*			tree
	)
{
	auto* newClass = jnew CBDClass(name, kGhostType, JFAID::kInvalidID, tree, false);
	assert( newClass != nullptr );
	return newClass;
}

/******************************************************************************
 AdjustNameStyle (virtual protected)

 ******************************************************************************/

void
CBDClass::AdjustNameStyle
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
