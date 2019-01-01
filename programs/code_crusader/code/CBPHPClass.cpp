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

/******************************************************************************
 Constructor

 ******************************************************************************/

CBPHPClass::CBPHPClass
	(
	const JCharacter*	name,
	const DeclareType	declType,
	const JFAID_t		fileID,
	CBTree*				tree,
	const JBoolean		isFinal
	)
	:
	CBClass(name, declType, fileID, tree, RemoveNamespace),
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
	CBClass(input, vers, tree, RemoveNamespace)
{
	input >> itsIsFinalFlag;
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

	output << ' ' << itsIsFinalFlag;
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
		JGetUserNotification()->ReportError("Ghost classes cannot be opened.");
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
 ViewDefinition (virtual)

	Returns kJTrue if the function was found.

 ******************************************************************************/

JBoolean
CBPHPClass::ViewDefinition
	(
	const JCharacter*	fnName,
	const JBoolean		caseSensitive,
	const JBoolean		reportNotFound
	)
	const
{
	JBoolean found = kJFalse;

	JString fileName;
	if (!Implements(fnName, caseSensitive))
		{
		found = ViewInheritedDefinition(fnName, caseSensitive, reportNotFound);
		if (!found && reportNotFound)
			{
			JString msg = "Unable to find any definition for \"";
			msg += fnName;
			msg += "\".";
			JGetUserNotification()->ReportError(msg);
			}
		}
	else if (GetFileName(&fileName))
		{
		CBDocumentManager* docMgr = CBGetDocumentManager();

		// We need to use a multi-line regex to find the constructor
		// instead of the class name.

		CBTextDocument* doc = nullptr;
		if (docMgr->OpenTextDocument(fileName, 0, &doc))
			{
			JString p = "\\b";
			p        += fnName;
			p        += "[ \t\n]*\\(";
			const JRegex r(p);

			CBTextEditor* te = doc->GetTextEditor();
			te->SetCaretLocation(1);

			JArray<JIndexRange> matchList;
			JBoolean wrapped;
			if (te->JTextEditor::SearchForward(r, kJFalse, kJFalse, &wrapped, &matchList))
				{
				JIndexRange range = matchList.GetElement(1);
				te->SelectLine(te->GetLineForChar(range.first));
				te->ScrollForDefinition(kCBPHPLang);
				}
			else if (reportNotFound)
				{
				JString msg = "Unable to find the definition of \"";
				msg += fnName;
				msg += "\".";
				JGetUserNotification()->ReportError(msg);
				}
			}
		}
	else if (reportNotFound)
		{
		JString msg = GetFullName();
		msg.PrependCharacter('"');
		msg += "\" is a ghost class, so no information is available for it.";
		JGetUserNotification()->ReportError(msg);
		}

	return found;
}

/******************************************************************************
 ViewDeclaration (virtual)

	Returns kJTrue if the function was found.

 ******************************************************************************/

JBoolean
CBPHPClass::ViewDeclaration
	(
	const JCharacter*	fnName,
	const JBoolean		caseSensitive,
	const JBoolean		reportNotFound
	)
	const
{
	return ViewDefinition(fnName, caseSensitive, reportNotFound);
}

/******************************************************************************
 NewGhost (virtual protected)

	Creates a new ghost CBPHPClass.

 ******************************************************************************/

CBClass*
CBPHPClass::NewGhost
	(
	const JCharacter*	name,
	CBTree*				tree
	)
{
	CBPHPClass* newClass = jnew CBPHPClass(name, kGhostType, JFAID::kInvalidID, tree,
										  kJFalse);
	assert( newClass != nullptr );
	return newClass;
}

/******************************************************************************
 GetNamespaceOperator (virtual protected)

	Returns the languages's namespace operator.

 ******************************************************************************/

const JCharacter*
CBPHPClass::GetNamespaceOperator()
	const
{
	return "\\";
}

/******************************************************************************
 RemoveNamespace (static)

	Extracts the name of the class without the namespace prefix.

	This can't use GetNamespaceOperator() because it is called from a
	constructor.

 ******************************************************************************/

JString
CBPHPClass::RemoveNamespace
	(
	const JCharacter* fullName
	)
{
	JString name = fullName;

	JIndex dotIndex;
	if (name.LocateLastSubstring("\\", &dotIndex))
		{
		name.RemoveSubstring(1, dotIndex);
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

	return JI2B(!IsPrivate(*access)     &&		// private
				fnName != GetName()     &&		// ctor
				fnName != "__construct" &&		// ctor
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
	CBClass::AdjustNameStyle(colormap, style);

	if (GetDeclareType() != kGhostType)
		{
		if (itsIsFinalFlag)
			{
			style->bold = kJTrue;
			}
		}
}
