/******************************************************************************
 CBGoClass.cpp

	BASE CLASS = CBClass

	Copyright (C) 2021 John Lindal.

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
	const JBoolean		isPublic,
	const JBoolean		isFinal
	)
	:
	CBClass(name, declType, fileID, tree, kNamespaceOperator)
{
}

CBGoClass::CBGoClass
	(
	std::istream&		input,
	const JFileVersion	vers,
	CBTree*				tree
	)
	:
	CBClass(input, vers, tree, kNamespaceOperator)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBGoClass::~CBGoClass()
{
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
			JBoolean wrapped;
			const JStringMatch m =
				te->GetText()->SearchForward(start, r, kJFalse, kJFalse, &wrapped);
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
				const JString msg = JGetString("NoDefinition::CBGoClass", map, sizeof(map));
				JGetUserNotification()->ReportError(msg);
				}
			}
		}
	else
		{
		JGetUserNotification()->ReportError(JGetString("NoGhostFile::CBClass"));
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
	CBGoClass* newClass = jnew CBGoClass(name, kGhostType, JFAID::kInvalidID, tree,
										 kJTrue, kJFalse);
	assert( newClass != nullptr );
	return newClass;
}
