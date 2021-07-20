/******************************************************************************
 CBGoClass.cpp

	BASE CLASS = CBClass

	Copyright © 2021 John Lindal.

 ******************************************************************************/

#include "CBGoClass.h"
#include "CBTree.h"
#include "CBTextDocument.h"
#include "CBTextEditor.h"
#include "cbGlobals.h"
#include <JXColorManager.h>
#include <JRegex.h>
#include <jAssert.h>

static const JUtf8Byte* kNamespaceOperator = ".";

/******************************************************************************
 Constructor

 ******************************************************************************/

CBGoClass::CBGoClass
	(
	const JString&		name,
	const DeclareType	declType,
	const JFAID_t		fileID,
	CBTree*				tree,
	const bool		isPublic
	)
	:
	CBClass(name, declType, fileID, tree, kNamespaceOperator),
	itsIsPublicFlag(isPublic)
{
}

CBGoClass::CBGoClass
	(
	std::istream&		input,
	const JFileVersion	vers,
	CBTree*				tree
	)
	:
	CBClass(input, vers, tree, kNamespaceOperator),
	itsIsPublicFlag(true)
{
	if (vers >= 88)
		{
		input >> JBoolFromString(itsIsPublicFlag);
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBGoClass::~CBGoClass()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
CBGoClass::StreamOut
	(
	std::ostream& output
	)
	const
{
	CBClass::StreamOut(output);

	output << ' ' << JBoolToString(itsIsPublicFlag);
	output << ' ';
}

/******************************************************************************
 ViewSource (virtual)

 ******************************************************************************/

void
CBGoClass::ViewSource()
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
			JString p("struct[ \t\n]*");
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
				te->ScrollForDefinition(kCBGoLang);
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
CBGoClass::ViewHeader()
	const
{
}

/******************************************************************************
 NewGhost (virtual protected)

	Creates a new ghost CBGoClass.

 ******************************************************************************/

CBClass*
CBGoClass::NewGhost
	(
	const JString&	name,
	CBTree*			tree
	)
{
	auto* newClass = jnew CBGoClass(name, kGhostType, JFAID::kInvalidID, tree,
										 true);
	assert( newClass != nullptr );
	return newClass;
}

/******************************************************************************
 AdjustNameStyle (virtual protected)

 ******************************************************************************/

void
CBGoClass::AdjustNameStyle
	(
	JFontStyle* style
	)
	const
{
	CBClass::AdjustNameStyle(style);

	if (GetDeclareType() != kGhostType && !itsIsPublicFlag)
		{
		style->color = JColorManager::GetGrayColor(50);
		}
}
