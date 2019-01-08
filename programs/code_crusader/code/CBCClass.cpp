/******************************************************************************
 CBCClass.cpp

	BASE CLASS = CBClass

	Copyright (C) 1996-99 John Lindal.

 ******************************************************************************/

#include "CBCClass.h"
#include "CBTree.h"
#include "CBTextDocument.h"
#include "CBTextEditor.h"
#include "cbGlobals.h"
#include <JXColorManager.h>
#include <jAssert.h>

static const JUtf8Byte* kNamespaceOperator = "::";

/******************************************************************************
 Constructor

 ******************************************************************************/

CBCClass::CBCClass
	(
	const JString&		name,
	const DeclareType	declType,
	const JFAID_t		fileID,
	CBTree*				tree
	)
	:
	CBClass(name, declType, fileID, tree, kNamespaceOperator)
{
}

CBCClass::CBCClass
	(
	std::istream&			input,
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

CBCClass::~CBCClass()
{
}

/******************************************************************************
 ViewSource (virtual)

 ******************************************************************************/

void
CBCClass::ViewSource()
	const
{
	CBDocumentManager* docMgr = CBGetDocumentManager();

	JString headerName;
	if (!GetFileName(&headerName))
		{
		JGetUserNotification()->ReportError(JGetString("NoGhostFile::CBClass"));
		}
	else if (GetDeclareType() == kEnumType)
		{
		docMgr->OpenTextDocument(headerName);
		}
	else
		{
		docMgr->OpenComplementFile(headerName, kCBCHeaderFT,
								   GetTree()->GetProjectDoc());
		}
}

/******************************************************************************
 ViewHeader (virtual)

 ******************************************************************************/

void
CBCClass::ViewHeader()
	const
{
	JString headerName;
	if (GetFileName(&headerName))
		{
		(CBGetDocumentManager())->OpenTextDocument(headerName);
		}
	else
		{
		JGetUserNotification()->ReportError(JGetString("NoGhostFile::CBClass"));
		}
}

/******************************************************************************
 NewGhost (virtual protected)

	Creates a new ghost CBCClass.

 ******************************************************************************/

CBClass*
CBCClass::NewGhost
	(
	const JString&	name,
	CBTree*			tree
	)
{
	CBCClass* newClass = jnew CBCClass(name, kGhostType, JFAID::kInvalidID, tree);
	assert( newClass != nullptr );
	return newClass;
}

/******************************************************************************
 AdjustNameStyle (virtual protected)

 ******************************************************************************/

void
CBCClass::AdjustNameStyle
	(
	JFontStyle* style
	)
	const
{
	CBClass::AdjustNameStyle(style);

	const DeclareType type = GetDeclareType();
	if (type == kStructType || type == kEnumType)
		{
		style->color = JColorManager::GetGrayColor(50);
		}
}
