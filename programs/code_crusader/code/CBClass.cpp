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

	Derived classes can override:

		StreamOut
			Write out extra data after calling CBClass::StreamOut().

		AdjustNameStyle
			Adjust the font style used to draw the class name.

	BASE CLASS = virtual JBroadcaster

	Copyright © 1995-97 John Lindal.

 ******************************************************************************/

#include "CBClass.h"
#include "CBTree.h"
#include "CBProjectDocument.h"
#include "CBFileListTable.h"
#include "cbGlobals.h"
#include <JPainter.h>
#include <JXColorManager.h>
#include <JFontManager.h>
#include <JStringIterator.h>
#include <JMinMax.h>
#include <jGlobals.h>
#include <jAssert.h>

// class data

JColorID CBClass::theGhostNameColor;

static JFontStyle kConcreteLabelStyle;
static JFontStyle kAbstractLabelStyle(false, true, 0, false, 0);

#ifndef CODE_CRUSADER_UNIT_TEST
const JCoordinate kHMarginWidth        = 3;
const JSize       kMinFrameWidth       = 50;
#endif
const JCoordinate kVMarginWidth        = 2;
const JCoordinate kLinkWidth           = 15;
const JCoordinate kCollapsedLinkLength = 14;

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBClass::CBClass
	(
	const JString&		fullName,
	const DeclareType	declType,
	const JFAID_t		fileID,
	CBTree*				tree,
	const JUtf8Byte*	namespaceOperator
	)
	:
	itsNamespaceOperator(namespaceOperator),
	itsFullName(fullName),
	itsName(RemoveNamespace(fullName)),
	itsFileID(fileID)
{
	CBClassX(tree);

	itsDeclType       = declType;
	itsIsAbstractFlag = false;
	itsIsTemplateFlag = false;

	SetCoords(0,0);
	itsTree->AddClass(this);
}

CBClass::CBClass
	(
	std::istream&		input,
	const JFileVersion	vers,
	CBTree*				tree,
	const JUtf8Byte*	namespaceOperator
	)
	:
	itsNamespaceOperator(namespaceOperator)
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
		itsName = RemoveNamespace(itsFullName);
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

	input >> JBoolFromString(itsIsAbstractFlag);

	itsIsTemplateFlag = false;
	if (vers >= 49)
		{
		input >> JBoolFromString(itsIsTemplateFlag);
		}

	if (vers >= 10)
		{
		input >> JBoolFromString(itsVisibleFlag)
			  >> JBoolFromString(itsCollapsedFlag)
			  >> JBoolFromString(itsIsSelectedFlag);
		if (vers == 10)
			{
			itsCollapsedFlag = false;	// itsWasVisibleFlag was true
			}
		}

	if (vers >= 11)
		{
		input >> JBoolFromString(itsHasPrimaryChildrenFlag)
			  >> JBoolFromString(itsHasSecondaryChildrenFlag);
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

	if (vers < 88)
		{
		JSize fnCount;
		input >> fnCount;

		JString name;
		JInteger access;
		bool pureVirtual;
		for (i=1; i<=fnCount; i++)
			{
			input >> name >> access >> JBoolFromString(pureVirtual);
			}
		}
}

// search target

CBClass::CBClass
	(
	const JString& name
	)
	:
	itsNamespaceOperator(""),
	itsFullName(name),
	itsName(name),
	itsTree(nullptr),
	itsParentInfo(nullptr)
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

	itsHasPrimaryChildrenFlag   = false;
	itsHasSecondaryChildrenFlag = false;

	itsVisibleFlag    = true;
	itsCollapsedFlag  = false;
	itsIsSelectedFlag = false;

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
		for (auto pInfo : *itsParentInfo)
			{
			pInfo.CleanOut();
			}
		jdelete itsParentInfo;
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
	output << ' ' << JBoolToString(itsIsAbstractFlag)
				  << JBoolToString(itsIsTemplateFlag)
				  << JBoolToString(itsVisibleFlag)
				  << JBoolToString(itsCollapsedFlag)
				  << JBoolToString(itsIsSelectedFlag)
				  << JBoolToString(itsHasPrimaryChildrenFlag)
				  << JBoolToString(itsHasSecondaryChildrenFlag);

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

	output << ' ';
}

/******************************************************************************
 SetSelected

	Sets its state and tells itsTree to broadcast.

 ******************************************************************************/

void
CBClass::SetSelected
	(
	const bool selected
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
		parent->SetCollapsed(false);
		c = parent;
		}
}

/******************************************************************************
 GetFileName

 ******************************************************************************/

#ifndef CODE_CRUSADER_UNIT_TEST

bool
CBClass::GetFileName
	(
	JString* fileName
	)
	const
{
	if (itsFileID != JFAID::kInvalidID)
		{
		*fileName = itsTree->GetProjectDoc()->GetAllFileList()->
					GetFileName(itsFileID);
		return true;
		}
	else
		{
		fileName->Clear();
		return false;		// ghost
		}
}

#endif

/******************************************************************************
 AddParent

 ******************************************************************************/

void
CBClass::AddParent
	(
	const InheritType	type,
	const JString&		name
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

	itsHasPrimaryChildrenFlag   = false;
	itsHasSecondaryChildrenFlag = false;
}

/******************************************************************************
 FindParents

	Search the class tree for our parents and update itsParentInfo.

	Returns true if it managed to fill in another nullptr parent.

 ******************************************************************************/

bool
CBClass::FindParents
	(
	const bool okToCreateGhost
	)
{
	bool foundAnotherParent = false;

	const JSize parentCount = GetParentCount();
	for (JIndex i=1; i<=parentCount; i++)
		{
		ParentInfo pInfo             = itsParentInfo->GetElement(i);
		const bool parentWasNull = pInfo.parent == nullptr;

		if (parentWasNull &&
			!FindParent(&pInfo, okToCreateGhost) && okToCreateGhost)
			{
			pInfo.parent = NewGhost(*(pInfo.name), itsTree);
			}

		if (parentWasNull && pInfo.parent != nullptr)
			{
			foundAnotherParent = true;
			(pInfo.parent)->AddChild(this, i==1);
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

bool
CBClass::FindParent
	(
	ParentInfo*		pInfo,
	const bool	okToSearchGhosts
	)
{
	// check for exact match that isn't a ghost

	if (itsTree->FindClass(*(pInfo->name), &(pInfo->parent)) &&
		pInfo->parent != this &&
		!(pInfo->parent)->IsGhost())
		{
		return true;
		}

	// try all possible namespaces to look for existing parent

	const JUtf8Byte* namespaceOp = GetNamespaceOperator();

	JString nameSpace = itsFullName;
	JString prefixStr, testName;

	JStringIterator iter(&nameSpace, kJIteratorStartAtEnd);
	while (iter.Prev(namespaceOp) && !iter.AtBeginning())
		{
		iter.RemoveAllNext();

		// check if parent exists in namespace

		testName  = nameSpace;
		testName += namespaceOp;
		testName += *(pInfo->name);
		if (itsTree->FindClass(testName, &pInfo->parent) &&
			pInfo->parent != this)
			{
			*(pInfo->name) = testName;
			return true;
			}

		// if namespace exists as a class, check its ancestors

		const CBClass* nsClass;
		if (itsTree->FindClass(nameSpace, &nsClass) &&
			itsTree->FindParent(*pInfo->name, nsClass, &pInfo->parent, &prefixStr) &&
			pInfo->parent != this)
			{
			(pInfo->name)->Prepend(prefixStr);
			return true;
			}
		}

	// check for any exact match

	return okToSearchGhosts &&
				itsTree->FindClass(*pInfo->name, &pInfo->parent) &&
				pInfo->parent != this;
}

/******************************************************************************
 NewGhost (virtual protected)

	Creates a new ghost using the appropriate derived class.  The file name
	is not passed in because it can be empty.

 ******************************************************************************/

CBClass*
CBClass::NewGhost
	(
	const JString&	name,
	CBTree*			tree
	)
{
	assert_msg( 0, "The programmer forgot to override CBClass::NewGhost()" );
	return nullptr;
}

/******************************************************************************
 RemoveNamespace (protected)

	Extracts the name of the class without the namespace prefix.

 ******************************************************************************/

JString
CBClass::RemoveNamespace
	(
	const JString& fullName
	)
{
	JString name = fullName;

	JStringIterator iter(&name, kJIteratorStartAtEnd);
	if (iter.Prev(GetNamespaceOperator()))
		{
		iter.Next(GetNamespaceOperator());
		iter.RemoveAllPrev();
		}

	return name;
}

/******************************************************************************
 GetParent

	This function will return false if the parent is not part of the tree.

 ******************************************************************************/

bool
CBClass::GetParent
	(
	const JIndex	index,
	const CBClass**	parent
	)
	const
{
	return GetParent(index, const_cast<CBClass**>(parent));
}

bool
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
		return *parent != nullptr;
		}
	else
		{
		*parent = nullptr;
		return false;
		}
}

/******************************************************************************
 IsAncestor

	Returns true if we are an ancestor of the given class.

	We are an ancestor of child if we are one of its parents
	or an ancestor of one of its parents.

 ******************************************************************************/

bool
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
			return true;
			}
		}

	return false;
}

/******************************************************************************
 AddChild (private)

	We sort the function names so others can perform an incremental search.

 ******************************************************************************/

void
CBClass::AddChild
	(
	CBClass*		child,
	const bool	primary
	)
{
	if (primary)
		{
		itsHasPrimaryChildrenFlag = true;
		}
	else
		{
		itsHasSecondaryChildrenFlag = true;
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

inline bool
CBClass::NeedToDrawLink
	(
	const JPoint&	pt1,
	const JPoint&	pt2,
	const JRect&	visRect
	)
	const
{
	return !((pt1.y < visRect.top    && pt2.y < visRect.top) ||
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

inline bool
CBClass::NeedDrawName()
	const
{
	return itsIsTemplateFlag;
}

/******************************************************************************
 GetDrawName (private)

 ******************************************************************************/

JString
CBClass::GetDrawName()
	const
{
	JString drawName = itsFullName;
	if (itsIsTemplateFlag)
		{
		drawName.Append(JGetString("TemplateNameSuffix::CBClass"));
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
		p.SetFilling(true);
		p.Rect(itsFrame);

		// draw black frame

		p.SetFilling(false);
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
		const bool ok = GetParent(i, &parent);
		assert( ok );

		JCoordinate deltaLinkFromX = 0;
		while (!parent->IsVisible())
			{
			deltaLinkFromX    = kCollapsedLinkLength;
			const bool ok = parent->GetParent(1, &parent);
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
		AdjustNameStyle(&style);
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
#ifndef CODE_CRUSADER_UNIT_TEST

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
		style.bold       = true;	// if not bold, extra space doesn't hurt

		JFont font = JFontManager::GetDefaultFont();
		font.SetSize(fontSize);
		font.SetStyle(style);

		w = JMax(kMinFrameWidth, 2*kHMarginWidth + font.GetStringWidth(fontManager, name));
		}

	const JCoordinate h = CalcFrameHeight(fontManager, fontSize);

	itsFrame.Set(itsVCoord, x, itsVCoord+h, x+w);

#endif
}

/******************************************************************************
 CBClass::ParentInfo

 ******************************************************************************/

void
CBClass::ParentInfo::CleanOut()
{
	jdelete name;
	name = nullptr;
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
	std::istream&			input,
	CBClass::DeclareType&	type
	)
{
	long temp;
	input >> temp;
	type = (CBClass::DeclareType) temp;
	assert( type == CBClass::kClassType ||
			type == CBClass::kStructUnused ||
			type == CBClass::kGhostType ||
			type == CBClass::kEnumUnused );
	return input;
}

std::ostream&
operator<<
	(
	std::ostream&				output,
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
	std::istream&			input,
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
	std::ostream&				output,
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
	std::istream&			input,
	CBClass::FnAccessLevel&	access
	)
{
	long temp;
	input >> temp;

	if (temp == CBClass::kUnusedQtSignalAccess ||
		temp == CBClass::kUnusedQtPublicSlotAccess)
		{
		temp = CBClass::kPublicAccess;
		}
	else if (temp == CBClass::kUnusedQtProtectedSlotAccess)
		{
		temp = CBClass::kProtectedAccess;
		}
	else if (temp == CBClass::kUnusedQtPrivateSlotAccess)
		{
		temp = CBClass::kPrivateAccess;
		}

	access = (CBClass::FnAccessLevel) temp;
	assert( access == CBClass::kPublicAccess     ||
			access == CBClass::kProtectedAccess  ||
			access == CBClass::kPrivateAccess    ||
			access == CBClass::kJavaDefaultAccess);
	return input;
}

std::ostream&
operator<<
	(
	std::ostream&					output,
	const CBClass::FnAccessLevel	access
	)
{
	output << (long) access;
	return output;
}
