/******************************************************************************
 CBClass.cpp

	This class encapsulates everything the class browser needs to know about
	each class.

	Note that only CBTree can delete CBClass.  Since extra generality is
	pointless in this case, it is too much trouble to allow others to
	delete us.  (Among other actions, one would have to call
	CBTree::UpdatePlacement(), which is -very- expensive.)

	Derived classes must implement:

		ViewSource
			Open the appropriate file to display the implementation for
			this class.

		ViewHeader
			Open the appropriate file to display the declaration for this
			class.

		ViewDefinition
			Open the appropriate file to display the definition of the
			specified function.

		ViewDeclaration
			Open the appropriate file to display the declaration of the
			specified function.

		NewGhost
			Create a ghost of the appropriate derived type.

		GetNamespaceOperator
			Returns the languages's namespace operator.

		IsInherited
			Returns kJTrue if the specified function will be inherited by
			derived classes.

	Derived classes can override:

		StreamOut
			Write out extra data after calling CBClass::StreamOut().

		AdjustNameStyle
			Adjust the font style used to draw the class name.

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 1995-97 John Lindal.

 ******************************************************************************/

#include "CBClass.h"
#include "CBTree.h"
#include "CBProjectDocument.h"
#include "CBFileListTable.h"
#include "cbGlobals.h"
#include <JPainter.h>
#include <JXColorManager.h>
#include <JFontManager.h>
#include <JString.h>
#include <JMinMax.h>
#include <jGlobals.h>
#include <jAssert.h>

// class data

JColorID CBClass::itsGhostNameColor;

static const JCharacter* kGhostFileName = "Unknown";

static const JCharacter* kInheritPublicStr    = "public";
static const JCharacter* kInheritProtectedStr = "protected";
static const JCharacter* kInheritPrivateStr   = "private";

static const JCharacter* kInheritStr[] =
	{ kInheritPublicStr, kInheritProtectedStr, kInheritPrivateStr };

static const JCharacter* kTemplateNamePrefix = "template ";
static const JCharacter* kStructNamePrefix   = "struct ";
static const JCharacter* kEnumNamePrefix     = "enum ";

static JFontStyle kConcreteLabelStyle;
static JFontStyle kAbstractLabelStyle(kJFalse, kJTrue, 0, kJFalse, 0);

const JCoordinate kHMarginWidth        = 3;
const JCoordinate kVMarginWidth        = 2;
const JSize       kMinFrameWidth       = 50;
const JCoordinate kLinkWidth           = 15;
const JCoordinate kCollapsedLinkLength = 14;

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBClass::CBClass
	(
	const JCharacter*		fullName,
	const DeclareType		declType,
	const JFAID_t			fileID,
	CBTree*					tree,
	CBClassRemoveNamespace*	removeNamespaceFn
	)
	:
	itsFullName(fullName),
	itsName(removeNamespaceFn(fullName)),
	itsFileID(fileID)
{
	CBClassX(tree);

	itsDeclType       = declType;
	itsIsAbstractFlag = kJFalse;
	itsIsTemplateFlag = kJFalse;

	SetCoords(0,0);
	itsTree->AddClass(this);
}

CBClass::CBClass
	(
	std::istream&				input,
	const JFileVersion		vers,
	CBTree*					tree,
	CBClassRemoveNamespace* removeNamespaceFn
	)
{
JIndex i;

	CBClassX(tree);

	if (vers < 30)
		{
		input >> i;
		assert( i == 1 );
		}

	input >> itsFullName;
	if (vers >= 6)
		{
		input >> itsName;
		}
	else
		{
		itsName = removeNamespaceFn(itsFullName);
		}

	if (vers >= 1)
		{
		input >> itsDeclType;
		}
	else
		{
		itsDeclType = kClassType;
		}

	if (vers < 40)
		{
		JString fileName;
		input >> fileName;
		itsFileID = JFAID::kInvalidID;	// ignore ID since we will be tossed
		}
	else
		{
		input >> itsFileID;
		}

	input >> itsHCoord >> itsVCoord;

	if (vers >= 6)
		{
		input >> itsFrame;
		}
	else
		{
		CalcFrame();
		}

	if (vers == 0)
		{
		JIndex firstFoundParent;
		input >> firstFoundParent;
		}

	input >> itsIsAbstractFlag;

	itsIsTemplateFlag = kJFalse;
	if (vers >= 49)
		{
		input >> itsIsTemplateFlag;
		}

	if (vers >= 10)
		{
		input >> itsVisibleFlag >> itsCollapsedFlag >> itsIsSelectedFlag;
		if (vers == 10)
			{
			itsCollapsedFlag = kJFalse;	// itsWasVisibleFlag was kJTrue
			}
		}

	if (vers >= 11)
		{
		input >> itsHasPrimaryChildrenFlag >> itsHasSecondaryChildrenFlag;
		}

	// parents

	JSize parentCount;
	input >> parentCount;

	for (i=1; i<=parentCount; i++)
		{
		ParentInfo pInfo;
		pInfo.name = jnew JString;
		assert( pInfo.name != nullptr );

		input >> *(pInfo.name) >> pInfo.indexFromFile >> pInfo.inheritance;
		itsParentInfo->AppendElement(pInfo);
		}

	// functions

	JSize fnCount;
	input >> fnCount;

	for (i=1; i<=fnCount; i++)
		{
		FunctionInfo fInfo;
		fInfo.name = jnew JString;
		assert( fInfo.name != nullptr );
		JBoolean pureVirtual;
		input >> *(fInfo.name) >> fInfo.access >> pureVirtual;
		fInfo.implemented = !pureVirtual;
		itsFunctionInfo->AppendElement(fInfo);
		}
}

// search target

CBClass::CBClass
	(
	const JCharacter* name
	)
	:
	itsFullName(name),
	itsName(name),
	itsTree(nullptr),
	itsParentInfo(nullptr),
	itsFunctionInfo(nullptr)
{
}

// private

void
CBClass::CBClassX
	(
	CBTree* tree
	)
{
	itsTree = tree;

	itsParentInfo = jnew JArray<ParentInfo>;
	assert( itsParentInfo != nullptr );

	itsHasPrimaryChildrenFlag   = kJFalse;
	itsHasSecondaryChildrenFlag = kJFalse;

	itsFunctionInfo = jnew JArray<FunctionInfo>;
	assert( itsFunctionInfo != nullptr );
	itsFunctionInfo->SetCompareFunction(CompareFunctionNames);
	itsFunctionInfo->SetSortOrder(JListT::kSortAscending);

	itsVisibleFlag    = kJTrue;
	itsCollapsedFlag  = kJFalse;
	itsIsSelectedFlag = kJFalse;

	kConcreteLabelStyle       = JFontStyle();
	kAbstractLabelStyle.color = kConcreteLabelStyle.color;
}

/******************************************************************************
 Destructor

	Any or all can be nullptr if we were constructed as a search target.

 ******************************************************************************/

CBClass::~CBClass()
{
	if (itsParentInfo != nullptr)
		{
		const JSize pCount = itsParentInfo->GetElementCount();
		for (JIndex i=1; i<=pCount; i++)
			{
			ParentInfo pInfo = itsParentInfo->GetElement(i);
			jdelete pInfo.name;
			}
		jdelete itsParentInfo;
		}

	if (itsFunctionInfo != nullptr)
		{
		const JSize fCount = itsFunctionInfo->GetElementCount();
		for (JIndex i=1; i<=fCount; i++)
			{
			FunctionInfo fInfo = itsFunctionInfo->GetElement(i);
			jdelete fInfo.name;
			}
		jdelete itsFunctionInfo;
		}
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
CBClass::StreamOut
	(
	std::ostream& output
	)
	const
{
JIndex i;

	output << ' ' << itsFullName << ' ' << itsName;
	output << ' ' << itsDeclType << ' ' << itsFileID;
	output << ' ' << itsHCoord << ' ' << itsVCoord << ' ' << itsFrame;
	output << ' ' << itsIsAbstractFlag;
	output << ' ' << itsIsTemplateFlag;
	output << ' ' << itsVisibleFlag;
	output << ' ' << itsCollapsedFlag;
	output << ' ' << itsIsSelectedFlag;
	output << ' ' << itsHasPrimaryChildrenFlag;
	output << ' ' << itsHasSecondaryChildrenFlag;

	// parents

	const JSize parentCount = GetParentCount();
	output << ' ' << parentCount;

	for (i=1; i<=parentCount; i++)
		{
		const ParentInfo pInfo = itsParentInfo->GetElement(i);
		output << ' ' << *(pInfo.name);
		output << ' ' << itsTree->ClassToIndexForWrite(pInfo.parent);
		output << ' ' << pInfo.inheritance;
		}

	// functions

	const JSize fnCount = GetFunctionCount();
	output << ' ' << fnCount;

	for (i=1; i<=fnCount; i++)
		{
		const FunctionInfo fInfo = itsFunctionInfo->GetElement(i);
		output << ' ' << *(fInfo.name) << ' ' << fInfo.access;
		output << ' ' << !fInfo.implemented;
		}

	output << ' ';
}

/******************************************************************************
 SetSelected

	Sets its state and tells itsTree to broadcast.

 ******************************************************************************/

void
CBClass::SetSelected
	(
	const JBoolean selected
	)
{
	if (itsIsSelectedFlag != selected)
		{
		itsIsSelectedFlag = selected;
		itsTree->BroadcastSelectionChange(this, itsIsSelectedFlag);
		}
}

/******************************************************************************
 ForceVisible

	Uncollapses primary parents to make us visible.  If we have no parents,
	we simply show ourselves.

	*** Client is responsible for rebuilding tree afterwards.

 ******************************************************************************/

void
CBClass::ForceVisible()
{
	CBClass* c = this;
	CBClass* parent;
	while (c->GetParent(1, &parent) &&
		   (!parent->IsVisible() || parent->IsCollapsed()))
		{
		parent->SetCollapsed(kJFalse);
		c = parent;
		}
}

/******************************************************************************
 GetFileName

 ******************************************************************************/

JBoolean
CBClass::GetFileName
	(
	JString* fileName
	)
	const
{
	if (itsFileID != JFAID::kInvalidID)
		{
		*fileName = ((itsTree->GetProjectDoc())->GetAllFileList())->
					GetFileName(itsFileID);
		return kJTrue;
		}
	else
		{
		fileName->Clear();
		return kJFalse;		// ghost
		}
}

/******************************************************************************
 AddParent

 ******************************************************************************/

void
CBClass::AddParent
	(
	const InheritType type,
	const JCharacter* name
	)
{
	ParentInfo pInfo(jnew JString(name), nullptr, type);
	assert( pInfo.name != nullptr );
	itsParentInfo->AppendElement(pInfo);
}

/******************************************************************************
 ClearConnections (private)

	Called by CBTree before FindParents() iteration.

 ******************************************************************************/

void
CBClass::ClearConnections()
{
	const JSize parentCount = GetParentCount();
	for (JIndex i=1; i<=parentCount; i++)
		{
		ParentInfo pInfo = itsParentInfo->GetElement(i);
		pInfo.parent     = nullptr;
		itsParentInfo->SetElement(i, pInfo);
		}

	itsHasPrimaryChildrenFlag   = kJFalse;
	itsHasSecondaryChildrenFlag = kJFalse;
}

/******************************************************************************
 FindParents

	Search the class tree for our parents and update itsParentInfo.

	Returns kJTrue if it managed to fill in another nullptr parent.

 ******************************************************************************/

JBoolean
CBClass::FindParents
	(
	const JBoolean okToCreateGhost
	)
{
	JBoolean foundAnotherParent = kJFalse;

	const JSize parentCount = GetParentCount();
	for (JIndex i=1; i<=parentCount; i++)
		{
		ParentInfo pInfo             = itsParentInfo->GetElement(i);
		const JBoolean parentWasNull = JConvertToBoolean( pInfo.parent == nullptr );

		if (parentWasNull &&
			!FindParent(&pInfo, okToCreateGhost) && okToCreateGhost)
			{
			pInfo.parent = NewGhost(*(pInfo.name), itsTree);
			}

		if (parentWasNull && pInfo.parent != nullptr)
			{
			foundAnotherParent = kJTrue;
			(pInfo.parent)->AddChild(this, JI2B(i==1));
			itsParentInfo->SetElement(i, pInfo);
			}
		}

	return foundAnotherParent;
}

/******************************************************************************
 FindParent (private)

	Search the tree for the given parent.  First, we check if we find
	an exact match.  Otherwise, we have to work our way up our namespace
	to see if any of the classes that contain us (or a parent thereof)
	contains our parent.

	Example: JTextEditor::TextChanged inherits from JBroadcaster::Message.

 ******************************************************************************/

JBoolean
CBClass::FindParent
	(
	ParentInfo*		pInfo,
	const JBoolean	okToSearchGhosts
	)
{
	// check for exact match that isn't a ghost

	if (itsTree->FindClass(*(pInfo->name), &(pInfo->parent)) &&
		pInfo->parent != this &&
		!(pInfo->parent)->IsGhost())
		{
		return kJTrue;
		}

	// try all possible namespaces to look for existing parent

	const JCharacter* namespaceOp = GetNamespaceOperator();

	JString nameSpace = itsFullName;
	JString prefixStr, testName;

	JIndex opIndex;
	while (nameSpace.LocateLastSubstring(namespaceOp, &opIndex) && opIndex > 1)
		{
		nameSpace.RemoveSubstring(opIndex, nameSpace.GetLength());

		// check if parent exists in namespace

		testName  = nameSpace;
		testName += namespaceOp;
		testName += *(pInfo->name);
		if (itsTree->FindClass(testName, &(pInfo->parent)) &&
			pInfo->parent != this)
			{
			*(pInfo->name) = testName;
			return kJTrue;
			}

		// if namespace exists as a class, check its ancestors

		const CBClass* nsClass;
		if (itsTree->FindClass(nameSpace, &nsClass) &&
			itsTree->FindParent(*(pInfo->name), nsClass, &(pInfo->parent), &prefixStr) &&
			pInfo->parent != this)
			{
			(pInfo->name)->Prepend(prefixStr);
			return kJTrue;
			}
		}

	// check for any exact match

	return JConvertToBoolean(okToSearchGhosts &&
							 itsTree->FindClass(*(pInfo->name), &(pInfo->parent)) &&
							 pInfo->parent != this);
}

/******************************************************************************
 NewGhost (virtual protected)

	Creates a new ghost using the appropriate derived class.  The file name
	is not passed in because it can be empty.

 ******************************************************************************/

CBClass*
CBClass::NewGhost
	(
	const JCharacter*	name,
	CBTree*				tree
	)
{
	assert_msg( 0, "The programmer forgot to override CBClass::NewGhost()" );
	return nullptr;
}

/******************************************************************************
 GetNamespaceOperator (virtual protected)

	Returns the languages's namespace operator.

 ******************************************************************************/

const JCharacter*
CBClass::GetNamespaceOperator()
	const
{
	assert_msg( 0, "The programmer forgot to override CBClass::GetNamespaceOperator()" );
	return nullptr;
}

/******************************************************************************
 IsInherited (virtual)

	Returns kJTrue if the specified function will be inherited by derived
	classes.

 ******************************************************************************/

JBoolean
CBClass::IsInherited
	(
	const JIndex		index,
	const InheritType	inherit,
	FnAccessLevel*		access
	)
	const
{
	assert_msg( 0, "The programmer forgot to override CBClass::IsInherited()" );
	return kJFalse;
}

/******************************************************************************
 GetParent

	This function will return kJFalse if the parent is not part of the tree.

 ******************************************************************************/

JBoolean
CBClass::GetParent
	(
	const JIndex	index,
	const CBClass**	parent
	)
	const
{
	return GetParent(index, const_cast<CBClass**>(parent));
}

JBoolean
CBClass::GetParent
	(
	const JIndex	index,
	CBClass**		parent
	)
	const
{
	if (itsParentInfo->IndexValid(index))
		{
		ParentInfo pInfo = itsParentInfo->GetElement(index);
		*parent          = pInfo.parent;
		return JConvertToBoolean( *parent != nullptr );
		}
	else
		{
		*parent = nullptr;
		return kJFalse;
		}
}

/******************************************************************************
 IsAncestor

	Returns kJTrue if we are an ancestor of the given class.

	We are an ancestor of child if we are one of its parents
	or an ancestor of one of its parents.

 ******************************************************************************/

JBoolean
CBClass::IsAncestor
	(
	const CBClass* child
	)
	const
{
	const JIndex parentCount = child->GetParentCount();
	for (JIndex i=1; i<=parentCount; i++)
		{
		const CBClass* p;
		if (child->GetParent(i, &p) && (this == p || IsAncestor(p)))
			{
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 AddChild (private)

	We sort the function names so others can perform an incremental search.

 ******************************************************************************/

void
CBClass::AddChild
	(
	CBClass*		child,
	const JBoolean	primary
	)
{
	if (primary)
		{
		itsHasPrimaryChildrenFlag = kJTrue;
		}
	else
		{
		itsHasSecondaryChildrenFlag = kJTrue;
		}
}

/******************************************************************************
 AddFunction

	We sort the function names so others can perform an incremental search.

 ******************************************************************************/

void
CBClass::AddFunction
	(
	const JCharacter*	name,
	const FnAccessLevel	access,
	const JBoolean		implemented
	)
{
	FunctionInfo fInfo(jnew JString(name), access, implemented);
	assert( fInfo.name != nullptr );

	itsFunctionInfo->InsertSorted(fInfo);
	if (!implemented)
		{
		itsIsAbstractFlag = kJTrue;
		}
}

/******************************************************************************
 Implements

	Returns kJTrue if this class implements the specified function.

 ******************************************************************************/

JBoolean
CBClass::Implements
	(
	const JCharacter*	name,
	const JBoolean		caseSensitive
	)
	const
{
	JString s = name;
	FunctionInfo fInfo;
	fInfo.name = &s;
	JIndex i;
	if (!itsFunctionInfo->SearchSorted(fInfo, JListT::kFirstMatch, &i))
		{
		return kJFalse;
		}
	else if (!caseSensitive)
		{
		return kJTrue;
		}
	else
		{
		// check each name that gives a case-insensitive match
		// to see if it is a case-sensitive match

		const JSize fnCount = itsFunctionInfo->GetElementCount();
		for( ; i<=fnCount; i++)
			{
			const FunctionInfo info = itsFunctionInfo->GetElement(i);
			if (JString::Compare(name, *(info.name), kJFalse) != 0)
				{
				break;
				}
			if (JString::Compare(name, *(info.name), kJTrue) == 0)
				{
				return kJTrue;
				}
			}

		return kJFalse;
		}
}

/******************************************************************************
 View... (virtual)

	These are not pure virtual because CBTree needs to be able to construct
	a CBClass as a search target.

 ******************************************************************************/

void
CBClass::ViewSource()
	const
{
	assert_msg( 0, "The programmer forgot to override CBClass::ViewSource()" );
}

void
CBClass::ViewHeader()
	const
{
	assert_msg( 0, "The programmer forgot to override CBClass::ViewHeader()" );
}

JBoolean
CBClass::ViewDefinition
	(
	const JCharacter*	fnName,
	const JBoolean		caseSensitive,
	const JBoolean		reportNotFound
	)
	const
{
	assert_msg( 0, "The programmer forgot to override CBClass::ViewDefinition()" );
	return kJFalse;
}

JBoolean
CBClass::ViewDeclaration
	(
	const JCharacter*	fnName,
	const JBoolean		caseSensitive,
	const JBoolean		reportNotFound
	)
	const
{
	assert_msg( 0, "The programmer forgot to override CBClass::ViewDeclaration()" );
	return kJFalse;
}

/******************************************************************************
 ViewInheritedDefinition (protected)

	Search all ancestors for the given function.  Returns kJTrue if the
	function was found.

 ******************************************************************************/

JBoolean
CBClass::ViewInheritedDefinition
	(
	const JCharacter*	fnName,
	const JBoolean		caseSensitive,
	const JBoolean		reportNotFound
	)
	const
{
	if (Implements(fnName, caseSensitive))
		{
		return ViewDefinition(fnName, caseSensitive, reportNotFound);
		}

	JBoolean found = kJFalse;

	const JSize parentCount = GetParentCount();
	for (JIndex i=1; i<=parentCount; i++)
		{
		const CBClass* parent;
		const JBoolean ok = GetParent(i, &parent);
		assert( ok );

		if (parent->ViewInheritedDefinition(fnName, caseSensitive, reportNotFound))
			{
			found = kJTrue;
			}
		}

	return found;
}

/******************************************************************************
 ViewInheritedDeclaration (protected)

	Search all ancestors for the given function.  Returns kJTrue if the
	function was found.

 ******************************************************************************/

JBoolean
CBClass::ViewInheritedDeclaration
	(
	const JCharacter*	fnName,
	const JBoolean		caseSensitive,
	const JBoolean		reportNotFound
	)
	const
{
	if (Implements(fnName, caseSensitive))
		{
		return ViewDeclaration(fnName, caseSensitive, reportNotFound);
		}

	JBoolean found = kJFalse;

	const JSize parentCount = GetParentCount();
	for (JIndex i=1; i<=parentCount; i++)
		{
		const CBClass* parent;
		const JBoolean ok = GetParent(i, &parent);
		assert( ok );

		if (parent->ViewInheritedDeclaration(fnName, caseSensitive, reportNotFound))
			{
			found = kJTrue;
			}
		}

	return found;
}

/******************************************************************************
 FindParentsAfterRead

	Called by CBTree after reading in classes.

 ******************************************************************************/

void
CBClass::FindParentsAfterRead()
{
	const JSize count = itsParentInfo->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		ParentInfo pInfo    = itsParentInfo->GetElement(i);
		pInfo.parent        = itsTree->IndexToClassAfterRead(pInfo.indexFromFile);
		pInfo.indexFromFile = 0;	// make sure we never use it again
		itsParentInfo->SetElement(i, pInfo);
		}
}

/******************************************************************************
 NeedToDrawLink (private)

 ******************************************************************************/

inline JBoolean
CBClass::NeedToDrawLink
	(
	const JPoint&	pt1,
	const JPoint&	pt2,
	const JRect&	visRect
	)
	const
{
	return JNegate(
		(pt1.y < visRect.top    && pt2.y < visRect.top) ||
		(pt1.y > visRect.bottom && pt2.y > visRect.bottom));
}

/******************************************************************************
 Link points (private)

 ******************************************************************************/

inline JPoint
CBClass::GetLinkFromPt()
	const
{
	return JPoint(itsFrame.right, itsFrame.ycenter());
}

inline JPoint
CBClass::GetLinkToPt()
	const
{
	return JPoint(itsFrame.left, itsFrame.ycenter());
}

/******************************************************************************
 NeedDrawName (private)

	Most classes don't need GetDrawName(), and Draw() goes faster if
	we don't have to copy strings.

 ******************************************************************************/

inline JBoolean
CBClass::NeedDrawName()
	const
{
	return JI2B(itsIsTemplateFlag ||
				itsDeclType == kStructType || itsDeclType == kEnumType);
}

/******************************************************************************
 GetDrawName (private)

 ******************************************************************************/

JString
CBClass::GetDrawName()
	const
{
	JString drawName = itsFullName;
	if (itsDeclType == kStructType)
		{
		drawName.Prepend(kStructNamePrefix);
		}
	else if (itsDeclType == kEnumType)
		{
		drawName.Prepend(kEnumNamePrefix);
		}

	if (itsIsTemplateFlag)
		{
		drawName.Prepend(kTemplateNamePrefix);
		}

	return drawName;
}

/******************************************************************************
 Draw

 ******************************************************************************/

void
CBClass::Draw
	(
	JPainter&		p,
	const JRect&	rect
	)
	const
{
	// only draw box if visible

	JRect irect;
	if (JIntersection(itsFrame, rect, &irect))
		{
		if (itsIsSelectedFlag && itsDeclType == kGhostType)
			{
			p.SetPenColor(JColorManager::GetBlueColor());
			}
		else if (itsIsSelectedFlag)
			{
			p.SetPenColor(JColorManager::GetCyanColor());
			}
		else if (itsDeclType == kGhostType)
			{
			p.SetPenColor(JColorManager::GetGrayColor(80));
			}
		else
			{
			p.SetPenColor(JColorManager::GetWhiteColor());
			}
		p.SetFilling(kJTrue);
		p.Rect(itsFrame);

		// draw black frame

		p.SetFilling(kJFalse);
		p.SetPenColor(JColorManager::GetBlackColor());
		p.Rect(itsFrame);
		}

	// draw dashed line if collapsed and has children

	if (itsCollapsedFlag && itsHasPrimaryChildrenFlag)
		{
		const JPoint pt = GetLinkFromPt();
		if (rect.top <= pt.y && pt.y <= rect.bottom)
			{
			// remember to update kCollapsedLinkLength

			p.SetPenColor(JColorManager::GetBlackColor());
			p.Line(pt.x   ,pt.y, pt.x+2 ,pt.y);
			p.Line(pt.x+6 ,pt.y, pt.x+8 ,pt.y);
			p.Line(pt.x+12,pt.y, pt.x+14,pt.y);
			}
		}

	// primary inheritance

	CBClass* parent;
	if (GetParent(1, &parent))
		{
		const JPoint linkToPt   = GetLinkToPt();
		const JPoint linkFromPt = parent->GetLinkFromPt();
		if (NeedToDrawLink(linkFromPt, linkToPt, rect))
			{
			const InheritType type = GetParentType(1);
			if (type == kInheritPublic)
				{
				p.SetPenColor(JColorManager::GetBlackColor());
				}
			else if (type == kInheritProtected)
				{
				p.SetPenColor(JColorManager::GetYellowColor());
				}
			else
				{
				assert( type == kInheritPrivate );
				p.SetPenColor(JColorManager::GetRedColor());
				}

			const JCoordinate x = (linkFromPt.x + linkToPt.x)/2;
			p.Line(linkToPt.x, linkToPt.y, x, linkToPt.y);
			p.SetPenColor(JColorManager::GetBlackColor());
			p.LineTo(x, linkFromPt.y);
			p.LineTo(linkFromPt);
			}
		}
}

/******************************************************************************
 DrawMILinks

	We draw the secondary inheritance links separately so they can be
	drawn either above or below everything else.

 ******************************************************************************/

void
CBClass::DrawMILinks
	(
	JPainter&		p,
	const JRect&	rect
	)
	const
{
	const JSize parentCount = GetParentCount();
	if (parentCount < 2)
		{
		return;
		}

	const JPoint linkToPt = GetLinkToPt();

	for (JIndex i=2; i<=parentCount; i++)
		{
		CBClass* parent;
		const JBoolean ok = GetParent(i, &parent);
		assert( ok );

		JCoordinate deltaLinkFromX = 0;
		while (!parent->IsVisible())
			{
			deltaLinkFromX    = kCollapsedLinkLength;
			const JBoolean ok = parent->GetParent(1, &parent);
			assert( ok );
			}

		const JPoint linkFromPt = parent->GetLinkFromPt() + JPoint(deltaLinkFromX,0);
		if (NeedToDrawLink(linkFromPt, linkToPt, rect))
			{
			const InheritType type = GetParentType(i);
			if (type == kInheritPublic)
				{
				p.SetPenColor(JColorManager::GetGreenColor());
				}
			else if (type == kInheritProtected)
				{
				p.SetPenColor(JColorManager::GetYellowColor());
				}
			else
				{
				assert( type == kInheritPrivate );
				p.SetPenColor(JColorManager::GetRedColor());
				}

			p.Line(linkFromPt, linkToPt);
			}
		}
}

/******************************************************************************
 DrawText

	We draw the text separately so it can be drawn above the MI links.

 ******************************************************************************/

void
CBClass::DrawText
	(
	JPainter&		p,
	const JRect&	rect
	)
	const
{
	// only draw text if visible

	JRect irect;
	if (JIntersection(itsFrame, rect, &irect))
		{
		JFontStyle style = kConcreteLabelStyle;
		AdjustNameStyle(p.GetColormap(), &style);
		p.SetFontStyle(style);

		if (NeedDrawName())
			{
			const JString drawName = GetDrawName();
			p.String(itsFrame, drawName,
					 JPainter::kHAlignCenter, JPainter::kVAlignCenter);
			}
		else
			{
			p.String(itsFrame, itsFullName,
					 JPainter::kHAlignCenter, JPainter::kVAlignCenter);
			}
		}
}

/******************************************************************************
 AdjustNameStyle (virtual protected)

 ******************************************************************************/

void
CBClass::AdjustNameStyle
	(
	JFontStyle* style
	)
	const
{
	if (itsIsAbstractFlag)
		{
		*style = kAbstractLabelStyle;
		}

	if (itsIsSelectedFlag && itsDeclType == kGhostType)
		{
		style->color = JColorManager::GetWhiteColor();
		}
	else if (itsDeclType == kGhostType)
		{
		style->color = GetGhostNameColor();
		}
}

/******************************************************************************
 CalcFrameHeight (static private)

 ******************************************************************************/

inline JCoordinate
CBClass::CalcFrameHeight
	(
	JFontManager*	fontManager,
	const JSize		fontSize
	)
{
	JFont font = JFontManager::GetDefaultFont();
	font.SetSize(fontSize);
	font.SetStyle(kConcreteLabelStyle);
	return 2*kVMarginWidth + font.GetLineHeight(fontManager);
}

/******************************************************************************
 GetTotalWidth

	Return the width (in pixels) that this class takes up in the tree.

 ******************************************************************************/

JCoordinate
CBClass::GetTotalWidth()
	const
{
	const JCoordinate w = itsFrame.width();
	return (HasParents() ? w + kLinkWidth : w);
}

/******************************************************************************
 GetTotalHeight

	Return the height (in pixels) that this class takes up in the tree.

 ******************************************************************************/

JCoordinate
CBClass::GetTotalHeight()
	const
{
	return 2*kVMarginWidth + itsFrame.height();
}

// static

JCoordinate
CBClass::GetTotalHeight
	(
	CBTree*			tree,
	JFontManager*	fontManager
	)
{
	return 2*kVMarginWidth + CalcFrameHeight(fontManager, tree->GetFontSize());
}

/******************************************************************************
 CalcFrame

 ******************************************************************************/

void
CBClass::CalcFrame()
{
	JCoordinate x = itsHCoord;
	if (HasParents())
		{
		x += kLinkWidth;
		}

	JFontManager* fontManager = GetTree()->GetFontManager();
	const JSize fontSize      = itsTree->GetFontSize();

	JCoordinate w = 10;
	if (!CBInUpdateThread())
		{
		const JString name = GetDrawName();

		JFontStyle style = kConcreteLabelStyle;
		style.bold       = kJTrue;	// if not bold, extra space doesn't hurt

		JFont font = JFontManager::GetDefaultFont();
		font.SetSize(fontSize);
		font.SetStyle(style);

		w = JMax(kMinFrameWidth, 2*kHMarginWidth + font.GetStringWidth(fontManager, name));
		}

	const JCoordinate h = CalcFrameHeight(fontManager, fontSize);

	itsFrame.Set(itsVCoord, x, itsVCoord+h, x+w);
}

/******************************************************************************
 CompareFunctionNames (static private)

 ******************************************************************************/

JListT::CompareResult
CBClass::CompareFunctionNames
	(
	const FunctionInfo& i1,
	const FunctionInfo& i2
	)
{
	return JCompareStringsCaseInsensitive(i1.name, i2.name);
}

/******************************************************************************
 Global functions for CBClass class

 ******************************************************************************/

/******************************************************************************
 Global functions for CBClass::DeclareType

 ******************************************************************************/

std::istream&
operator>>
	(
	std::istream&				input,
	CBClass::DeclareType&	type
	)
{
	long temp;
	input >> temp;
	type = (CBClass::DeclareType) temp;
	assert( type == CBClass::kClassType ||
			type == CBClass::kStructType ||
			type == CBClass::kGhostType ||
			type == CBClass::kEnumType );
	return input;
}

std::ostream&
operator<<
	(
	std::ostream&					output,
	const CBClass::DeclareType	type
	)
{
	output << (long) type;
	return output;
}

/******************************************************************************
 Global functions for CBClass::InheritType

 ******************************************************************************/

std::istream&
operator>>
	(
	std::istream&				input,
	CBClass::InheritType&	type
	)
{
	long temp;
	input >> temp;
	type = (CBClass::InheritType) temp;
	assert( type == CBClass::kInheritPublic    ||
			type == CBClass::kInheritProtected ||
			type == CBClass::kInheritPrivate   ||
			type == CBClass::kInheritJavaDefault);
	return input;
}

std::ostream&
operator<<
	(
	std::ostream&					output,
	const CBClass::InheritType	type
	)
{
	output << (long) type;
	return output;
}

/******************************************************************************
 Global functions for CBClass::FnAccessLevel

 ******************************************************************************/

std::istream&
operator>>
	(
	std::istream&				input,
	CBClass::FnAccessLevel&	access
	)
{
	long temp;
	input >> temp;
	access = (CBClass::FnAccessLevel) temp;
	assert( access == CBClass::kPublicAccess          ||
			access == CBClass::kProtectedAccess       ||
			access == CBClass::kPrivateAccess         ||
			access == CBClass::kJavaDefaultAccess     ||
			access == CBClass::kQtSignalAccess        ||
			access == CBClass::kQtPublicSlotAccess    ||
			access == CBClass::kQtProtectedSlotAccess ||
			access == CBClass::kQtPrivateSlotAccess );
	return input;
}

std::ostream&
operator<<
	(
	std::ostream&						output,
	const CBClass::FnAccessLevel	access
	)
{
	output << (long) access;
	return output;
}
