/******************************************************************************
 CBCClass.cpp

	BASE CLASS = CBClass

	Copyright © 1996-99 John Lindal.

 ******************************************************************************/

#include "CBCClass.h"
#include "CBTree.h"
#include "CBTextDocument.h"
#include "CBTextEditor.h"
#include "cbGlobals.h"
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
#ifndef CODE_CRUSADER_UNIT_TEST

	CBDocumentManager* docMgr = CBGetDocumentManager();

	JString headerName;
	if (!GetFileName(&headerName))
		{
		JGetUserNotification()->ReportError(JGetString("NoGhostFile::CBTreeWidget"));
		}
	else
		{
		docMgr->OpenComplementFile(headerName, kCBCHeaderFT,
								   GetTree()->GetProjectDoc());
		}

#endif
}

/******************************************************************************
 ViewHeader (virtual)

 ******************************************************************************/

void
CBCClass::ViewHeader()
	const
{
#ifndef CODE_CRUSADER_UNIT_TEST

	JString headerName;
	if (GetFileName(&headerName))
		{
		CBGetDocumentManager()->OpenTextDocument(headerName);
		}
	else
		{
		JGetUserNotification()->ReportError(JGetString("NoGhostFile::CBTreeWidget"));
		}

#endif
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
	auto* newClass = jnew CBCClass(name, kGhostType, JFAID::kInvalidID, tree);
	assert( newClass != nullptr );
	return newClass;
}
