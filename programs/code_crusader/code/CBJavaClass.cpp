/******************************************************************************
 CBJavaClass.cpp

	BASE CLASS = CBClass

	Copyright © 1999 John Lindal.

 ******************************************************************************/

#include "CBJavaClass.h"
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

CBJavaClass::CBJavaClass
	(
	const JString&		name,
	const DeclareType	declType,
	const JFAID_t		fileID,
	CBTree*				tree,
	const bool		isPublic,
	const bool		isFinal
	)
	:
	CBClass(name, declType, fileID, tree, kNamespaceOperator),
	itsIsPublicFlag(isPublic),
	itsIsFinalFlag(isFinal)
{
}

CBJavaClass::CBJavaClass
	(
	std::istream&		input,
	const JFileVersion	vers,
	CBTree*				tree
	)
	:
	CBClass(input, vers, tree, kNamespaceOperator),
	itsIsPublicFlag(true),
	itsIsFinalFlag(false)
{
	if (vers >= 52)
		{
		input >> JBoolFromString(itsIsPublicFlag)
			  >> JBoolFromString(itsIsFinalFlag);
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBJavaClass::~CBJavaClass()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
CBJavaClass::StreamOut
	(
	std::ostream& output
	)
	const
{
	CBClass::StreamOut(output);

	output << ' ' << JBoolToString(itsIsPublicFlag)
				  << JBoolToString(itsIsFinalFlag);
	output << ' ';
}

/******************************************************************************
 ViewSource (virtual)

 ******************************************************************************/

void
CBJavaClass::ViewSource()
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
				te->ScrollForDefinition(kCBJavaLang);
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
CBJavaClass::ViewHeader()
	const
{
}

/******************************************************************************
 NewGhost (virtual protected)

	Creates a new ghost CBJavaClass.

 ******************************************************************************/

CBClass*
CBJavaClass::NewGhost
	(
	const JString&	name,
	CBTree*			tree
	)
{
	auto* newClass = jnew CBJavaClass(name, kGhostType, JFAID::kInvalidID, tree,
											true, false);
	assert( newClass != nullptr );
	return newClass;
}

/******************************************************************************
 AdjustNameStyle (virtual protected)

 ******************************************************************************/

void
CBJavaClass::AdjustNameStyle
	(
	JFontStyle* style
	)
	const
{
	CBClass::AdjustNameStyle(style);

	if (GetDeclareType() != kGhostType)
		{
		if (!itsIsPublicFlag)
			{
			style->color = JColorManager::GetGrayColor(50);
			}

		if (itsIsFinalFlag)
			{
			style->bold = true;
			}
		}
}
