/******************************************************************************
 CBCClass.cpp

	BASE CLASS = CBClass

	Copyright (C) 1996-99 John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBCClass.h"
#include "CBTree.h"
#include "CBTextDocument.h"
#include "CBTextEditor.h"
#include "cbGlobals.h"
#include <JColormap.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBCClass::CBCClass
	(
	const JCharacter*	name,
	const DeclareType	declType,
	const JFAID_t		fileID,
	CBTree*				tree
	)
	:
	CBClass(name, declType, fileID, tree, RemoveNamespace)
{
}

CBCClass::CBCClass
	(
	istream&			input,
	const JFileVersion	vers,
	CBTree*				tree
	)
	:
	CBClass(input, vers, tree, RemoveNamespace)
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
		(JGetUserNotification())->ReportError("Ghost classes cannot be opened.");
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
		(JGetUserNotification())->ReportError("Ghost classes cannot be opened.");
		}
}

/******************************************************************************
 ViewDefinition (virtual)

	Returns kJTrue if the function was found.

 ******************************************************************************/

JBoolean
CBCClass::ViewDefinition
	(
	const JCharacter*	fnName,
	const JBoolean		caseSensitive,
	const JBoolean		reportNotFound
	)
	const
{
	JBoolean found = kJFalse;

	JString headerName;
	if (IsEnum())
		{
		found = ViewDeclaration(fnName, caseSensitive, reportNotFound);
		}
	else if (!Implements(fnName, caseSensitive))
		{
		found = ViewInheritedDefinition(fnName, caseSensitive, reportNotFound);
		if (!found && reportNotFound)
			{
			JString msg = "Unable to find any definition for \"";
			msg += fnName;
			msg += "\".";
			(JGetUserNotification())->ReportError(msg);
			}
		}
	else if (GetFileName(&headerName))
		{
		// search for class::fn

		const JCharacter* nsOp = "[[:space:]]*::[[:space:]]*";

		JString searchStr = GetFullName();

		JIndex i=1;
		while (searchStr.LocateNextSubstring("::", &i))
			{
			searchStr.ReplaceSubstring(i,i+1, nsOp);
			i += strlen(nsOp);
			}

		searchStr += nsOp;
		searchStr += fnName;
		found = FindDefinition(headerName, searchStr, caseSensitive);

		if (!found)
			{
			// "::" insures that we find the source instead of a call to the function.
			// We can't use "class::" because this doesn't work for templates.

			searchStr = "::[[:space:]]*";
			searchStr += fnName;
			found = FindDefinition(headerName, searchStr, caseSensitive);
			}

		if (!found)
			{
			CBDocumentManager* docMgr = CBGetDocumentManager();

			// look for it in the header file (pure virtual or inline in class defn)

			JIndex lineIndex;
			if (docMgr->SearchFile(headerName, fnName, caseSensitive, &lineIndex))
				{
				docMgr->OpenTextDocument(headerName, lineIndex);
				found = kJTrue;
				}

			// we couldn't find it anywhere

			else if (reportNotFound)
				{
				JString msg = "Unable to find the definition for \"";
				msg += fnName;
				msg += "\".";
				(JGetUserNotification())->ReportError(msg);
				}
			}
		}
	else if (reportNotFound)
		{
		JString msg = GetFullName();
		msg.PrependCharacter('"');
		msg += "\" is a ghost class, so no information is available for it.";
		(JGetUserNotification())->ReportError(msg);
		}

	return found;
}

/******************************************************************************
 FindDefinition (private)

	Returns kJTrue if the function was found.

 ******************************************************************************/

JBoolean
CBCClass::FindDefinition
	(
	const JString&		headerName,
	const JCharacter*	searchStr,
	const JBoolean		caseSensitive
	)
	const
{
	CBDocumentManager* docMgr = CBGetDocumentManager();
	CBTextDocument* doc       = NULL;

	// look for it in the source file

	JString sourceName;
	JIndex lineIndex;
	if (docMgr->GetComplementFile(headerName, kCBCHeaderFT, &sourceName,
								  GetTree()->GetProjectDoc()) &&
		docMgr->SearchFile(sourceName, searchStr, caseSensitive, &lineIndex))
		{
		docMgr->OpenTextDocument(sourceName, lineIndex, &doc);
		}

	// look for it in the header file (inline)

	else if (docMgr->SearchFile(headerName, searchStr, caseSensitive, &lineIndex))
		{
		docMgr->OpenTextDocument(headerName, lineIndex, &doc);
		}

	if (doc != NULL)
		{
		(doc->GetTextEditor())->ScrollForDefinition(kCBCLang);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 ViewDeclaration (virtual)

	Returns kJTrue if the function was found.

 ******************************************************************************/

JBoolean
CBCClass::ViewDeclaration
	(
	const JCharacter*	fnName,
	const JBoolean		caseSensitive,
	const JBoolean		reportNotFound
	)
	const
{
	JBoolean found = kJFalse;

	JString headerName;
	if (!Implements(fnName, caseSensitive))
		{
		found = ViewInheritedDeclaration(fnName, caseSensitive, reportNotFound);
		if (!found && reportNotFound)
			{
			JString msg = "Unable to find any declaration for \"";
			msg += fnName;
			msg += "\".";
			(JGetUserNotification())->ReportError(msg);
			}
		}
	else if (GetFileName(&headerName))
		{
		CBDocumentManager* docMgr = CBGetDocumentManager();

		JIndex lineIndex;
		if (docMgr->SearchFile(headerName, fnName, caseSensitive, &lineIndex))
			{
			docMgr->OpenTextDocument(headerName, lineIndex);
			found = kJTrue;
			}
		else if (reportNotFound)
			{
			JString msg = "Unable to find the declaration of \"";
			msg += fnName;
			msg += "\".";
			(JGetUserNotification())->ReportError(msg);
			}
		}
	else if (reportNotFound)
		{
		JString msg = GetFullName();
		msg.PrependCharacter('"');
		msg += "\" is a ghost class, so no information is available for it.";
		(JGetUserNotification())->ReportError(msg);
		}

	return found;
}

/******************************************************************************
 NewGhost (virtual protected)

	Creates a new ghost CBCClass.

 ******************************************************************************/

CBClass*
CBCClass::NewGhost
	(
	const JCharacter*	name,
	CBTree*				tree
	)
{
	CBCClass* newClass = new CBCClass(name, kGhostType, JFAID::kInvalidID, tree);
	assert( newClass != NULL );
	return newClass;
}

/******************************************************************************
 GetNamespaceOperator (virtual protected)

	Returns the languages's namespace operator.

 ******************************************************************************/

const JCharacter*
CBCClass::GetNamespaceOperator()
	const
{
	return "::";
}

/******************************************************************************
 RemoveNamespace (static)

	Extracts the name of the class without the namespace prefix.

	This can't use GetNamespaceOperator() because it is called from a
	constructor.

 ******************************************************************************/

JString
CBCClass::RemoveNamespace
	(
	const JCharacter* fullName
	)
{
	JString name = fullName;

	JIndex colonIndex;
	if (name.LocateLastSubstring("::", &colonIndex))
		{
		name.RemoveSubstring(1, colonIndex+1);
		}

	return name;
}

/******************************************************************************
 IsInherited (virtual)

	Returns kJTrue if the specified function is inherited by derived classes.
	Constructors, destructors, and private functions are not inherited.

	If it is inherited, *access contains the access level adjusted according
	to the inheritance access.

 ******************************************************************************/

JBoolean
CBCClass::IsInherited
	(
	const JIndex		index,
	const InheritType	inherit,
	FnAccessLevel*		access
	)
	const
{
	const JString& fnName = GetFunctionName(index);
	*access               = GetFnAccessLevel(index);

	if (!IsPrivate(*access) &&					// private
		fnName.GetFirstCharacter() != '~' &&	// dtor
		fnName != GetName())					// ctor
		{
		if (inherit == kInheritPrivate)
			{
			if (*access == kPublicAccess ||
				*access == kProtectedAccess)
				{
				*access = kPrivateAccess;
				}
			else if (*access == kQtPublicSlotAccess ||
					 *access == kQtProtectedSlotAccess)
				{
				*access = kQtPrivateSlotAccess;
				}
			}
		else if (inherit == kInheritProtected &&
				 *access == kPublicAccess)
			{
			*access = kProtectedAccess;
			}
		else if (inherit == kInheritProtected &&
				 *access == kQtPublicSlotAccess)
			{
			*access = kQtProtectedSlotAccess;
			}
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 AdjustNameStyle (virtual protected)

 ******************************************************************************/

void
CBCClass::AdjustNameStyle
	(
	const JColormap*	colormap,
	JFontStyle*			style
	)
	const
{
	CBClass::AdjustNameStyle(colormap, style);

	const DeclareType type = GetDeclareType();
	if (type == kStructType || type == kEnumType)
		{
		style->color = colormap->GetGrayColor(50);
		}
}
