/******************************************************************************
 CBClass.h

	Interface for the CBClass Class

	Copyright © 1995-97 John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBClass
#define _H_CBClass

#include <JBroadcaster.h>
#include <JPtrArray-JString.h>
#include <JFontStyle.h>
#include <JRect.h>
#include <JFAID.h>
#include <jColor.h>

class JPainter;
class JFontManager;
class JColormap;
class CBTree;

typedef JString (CBClassRemoveNamespace)(const JCharacter* name);

class CBClass : virtual public JBroadcaster
{
	friend class CBTree;

public:

	// Do not change these values once they are assigned
	// because they are stored in files.

	enum DeclareType
	{
		kClassType = 0,
		kStructType,
		kGhostType,		// inferred, but not parsed
		kEnumType
	};

	enum InheritType
	{
		kInheritPublic = 0,
		kInheritProtected,
		kInheritPrivate,
		kInheritJavaDefault
	};

	enum FnAccessLevel
	{
		kPublicAccess = 0,
		kProtectedAccess,
		kPrivateAccess,
		kJavaDefaultAccess,
		kQtSignalAccess,
		kQtPublicSlotAccess,
		kQtProtectedSlotAccess,
		kQtPrivateSlotAccess
	};

	enum ForceVisibleAction
	{
		kDoNothing,
		kShowLoneClasses,
		kShowLoneStructs,
		kShowEnums
	};

public:

	virtual ~CBClass();

	const CBTree*	GetTree() const;

	const JString&	GetFullName() const;
	const JString&	GetName() const;
	JBoolean		GetFileID(JFAID_t* id) const;
	JBoolean		GetFileName(JString* fileName) const;

	virtual void	ViewSource() const;
	virtual void	ViewHeader() const;

	DeclareType		GetDeclareType() const;
	JBoolean		IsAbstract() const;
	void			SetAbstract(const JBoolean abstract = kJTrue);
	JBoolean		IsTemplate() const;
	void			SetTemplate(const JBoolean tmpl = kJTrue);
	JBoolean		IsStruct() const;
	JBoolean		IsEnum() const;
	JBoolean		IsGhost() const;

	void			AddParent(const InheritType type, const JCharacter* name);
	JBoolean		FindParents(const JBoolean okToCreateGhost);
	JBoolean		HasParents() const;
	JSize			GetParentCount() const;
	const JString&	GetParentName(const JIndex index) const;
	InheritType		GetParentType(const JIndex index) const;
	JBoolean		GetParent(const JIndex index, CBClass** parent) const;
	JBoolean		GetParent(const JIndex index, const CBClass** parent) const;
	JBoolean		IsAncestor(const CBClass* child) const;

	JBoolean		HasChildren() const;
	JBoolean		HasPrimaryChildren() const;
	JBoolean		HasSecondaryChildren() const;

	JSize				GetFunctionCount() const;
	const JString&		GetFunctionName(const JIndex index) const;
	FnAccessLevel		GetFnAccessLevel(const JIndex index) const;
	JBoolean			IsImplemented(const JIndex index) const;
	virtual JBoolean	IsInherited(const JIndex index, const InheritType inherit,
									FnAccessLevel* access) const;
	static JBoolean		IsPublic(const FnAccessLevel access);
	static JBoolean		IsProtected(const FnAccessLevel access);
	static JBoolean		IsPrivate(const FnAccessLevel access);
	static JBoolean		IsSignal(const FnAccessLevel access);
	static JBoolean		IsSlot(const FnAccessLevel access);
	void				AddFunction(const JCharacter* name, const FnAccessLevel access,
									const JBoolean implemented);
	JBoolean			Implements(const JCharacter* name, const JBoolean caseSensitive) const;

	virtual JBoolean	ViewDefinition(const JCharacter* fnName,
									   const JBoolean caseSensitive,
									   const JBoolean reportNotFound = kJTrue) const;
	virtual JBoolean	ViewDeclaration(const JCharacter* fnName,
										const JBoolean caseSensitive,
										const JBoolean reportNotFound = kJTrue) const;

	void	Draw(JPainter& p, const JRect& rect) const;
	void	DrawMILinks(JPainter& p, const JRect& rect) const;
	void	DrawText(JPainter& p, const JRect& rect) const;

	const JRect&		GetFrame() const;
	JBoolean			Contains(const JPoint& pt) const;
	JCoordinate			GetTotalWidth() const;
	JCoordinate			GetTotalHeight() const;
	static JCoordinate	GetTotalHeight(CBTree* tree, const JFontManager* fontManager);
	void				GetCoords(JCoordinate* x, JCoordinate* y) const;
	void				SetCoords(const JCoordinate x, const JCoordinate y);

	JBoolean			IsVisible() const;
	void				SetVisible(const JBoolean visible);
	ForceVisibleAction	ForceVisible();

	JBoolean	IsSelected() const;
	void		SetSelected(const JBoolean selected);
	void		ToggleSelected();

	JBoolean	IsCollapsed() const;
	void		SetCollapsed(const JBoolean collapse);

	virtual void	StreamOut(ostream& output) const;

	static JColorIndex	GetGhostNameColor();
	static void			SetGhostNameColor(const JColorIndex color);

	// called by CBTree::ReloadSetup

	void	CalcFrame();

protected:

	CBClass(const JCharacter* fullName, const DeclareType declType,
			const JFAID_t fileID, CBTree* tree,
			CBClassRemoveNamespace* removeNamespaceFn);
	CBClass(istream& input, const JFileVersion vers, CBTree* tree,
			CBClassRemoveNamespace* removeNamespaceFn);
	CBClass(const JCharacter* name);	// search target

	virtual CBClass*			NewGhost(const JCharacter* name, CBTree* tree);
	virtual const JCharacter*	GetNamespaceOperator() const;

	JBoolean	ViewInheritedDefinition(const JCharacter* fnName,
										const JBoolean caseSensitive,
										const JBoolean reportNotFound) const;
	JBoolean	ViewInheritedDeclaration(const JCharacter* fnName,
										 const JBoolean caseSensitive,
										 const JBoolean reportNotFound) const;

	virtual void	AdjustNameStyle(const JColormap* colormap, JFontStyle* style) const;

private:

	struct ParentInfo
	{
		JString*	name;
		CBClass*	parent;			// can be NULL; not owned
		InheritType	inheritance;
		JIndex		indexFromFile;	// used while reading from file

		ParentInfo()
			:
			name(NULL),
			parent(NULL),
			inheritance(kInheritPublic),
			indexFromFile(0)
			{ };

		ParentInfo(JString* n, CBClass* p, const InheritType type)
			:
			name(n),
			parent(p),
			inheritance(type),
			indexFromFile(0)
			{ };
	};

	struct FunctionInfo
	{
		JString*		name;
		FnAccessLevel	access;
		JBoolean		implemented;

		FunctionInfo()
			:
			name(NULL),
			access(kPublicAccess),
			implemented(kJTrue)
			{ };

		FunctionInfo(JString* n, const FnAccessLevel level, const JBoolean impl)
			:
			name(n),
			access(level),
			implemented(impl)
			{ };
	};

private:

	JString		itsFullName;					// namespace + class name
	JString		itsName;						// class name
	DeclareType	itsDeclType;
	JFAID_t		itsFileID;
	JBoolean	itsIsAbstractFlag;				// kJTrue if contains pure virtual functions
	JBoolean	itsIsTemplateFlag;

	JCoordinate	itsHCoord;						// coordinates in class tree
	JCoordinate	itsVCoord;
	JRect		itsFrame;

	JBoolean	itsVisibleFlag;
	JBoolean	itsCollapsedFlag;				// kJTrue => children are invisible
	JBoolean	itsIsSelectedFlag;

	CBTree*				itsTree;				// our owner
	JArray<ParentInfo>*	itsParentInfo;

	JBoolean	itsHasPrimaryChildrenFlag;
	JBoolean	itsHasSecondaryChildrenFlag;

	JArray<FunctionInfo>*	itsFunctionInfo;

	static JColorIndex	itsGhostNameColor;

private:

	void		CBClassX(CBTree* tree);
	JBoolean	FindParent(ParentInfo* pInfo, const JBoolean okToSearchGhosts);
	void		AddChild(CBClass* child, const JBoolean primary);

	static JCoordinate	CalcFrameHeight(const JFontManager* fontManager,
										const JSize fontSize);

	JBoolean	NeedDrawName() const;
	JString		GetDrawName() const;

	JBoolean	NeedToDrawLink(const JPoint& pt1, const JPoint& pt2,
							   const JRect& visRect) const;
	JPoint		GetLinkFromPt() const;
	JPoint		GetLinkToPt() const;

	static JOrderedSetT::CompareResult
		CompareFunctionNames(const FunctionInfo& i1, const FunctionInfo& i2);

	// called by CBTree

	void	ClearConnections();
	void	FindParentsAfterRead();

	// not allowed

	CBClass(const CBClass& source);
	CBClass& operator=(const CBClass& source);
};

istream& operator>>(istream& input, CBClass::DeclareType& type);
ostream& operator<<(ostream& output, const CBClass::DeclareType type);

istream& operator>>(istream& input, CBClass::InheritType& type);
ostream& operator<<(ostream& output, const CBClass::InheritType type);

istream& operator>>(istream& input, CBClass::FnAccessLevel& access);
ostream& operator<<(ostream& output, const CBClass::FnAccessLevel access);


/******************************************************************************
 GetFullName

	Returns the name of the class including the namespace prefix.

 ******************************************************************************/

inline const JString&
CBClass::GetFullName()
	const
{
	return itsFullName;
}

/******************************************************************************
 GetName

	Returns the name of the class without the namespace prefix.

 ******************************************************************************/

inline const JString&
CBClass::GetName()
	const
{
	return itsName;
}

/******************************************************************************
 GetDeclareType (protected)

 ******************************************************************************/

inline CBClass::DeclareType
CBClass::GetDeclareType()
	const
{
	return itsDeclType;
}

inline JBoolean
CBClass::IsAbstract()
	const
{
	return itsIsAbstractFlag;
}

inline JBoolean
CBClass::IsStruct()
	const
{
	return JConvertToBoolean( itsDeclType == kStructType );
}

inline JBoolean
CBClass::IsEnum()
	const
{
	return JConvertToBoolean( itsDeclType == kEnumType );
}

inline JBoolean
CBClass::IsGhost()
	const
{
	return JConvertToBoolean( itsDeclType == kGhostType );
}

/******************************************************************************
 SetAbstract

 ******************************************************************************/

inline void
CBClass::SetAbstract
	(
	const JBoolean abstract
	)
{
	itsIsAbstractFlag = abstract;
}

/******************************************************************************
 Template

 ******************************************************************************/

inline JBoolean
CBClass::IsTemplate()
	const
{
	return itsIsTemplateFlag;
}

inline void
CBClass::SetTemplate
	(
	const JBoolean tmpl
	)
{
	itsIsTemplateFlag = tmpl;
	CalcFrame();
}

/******************************************************************************
 GetFileID

 ******************************************************************************/

inline JBoolean
CBClass::GetFileID
	(
	JFAID_t* id
	)
	const
{
	*id = itsFileID;
	return JI2B( itsFileID != JFAID::kInvalidID );
}

/******************************************************************************
 HasParents

 ******************************************************************************/

inline JBoolean
CBClass::HasParents()
	const
{
	return !itsParentInfo->IsEmpty();
}

/******************************************************************************
 GetParentCount

 ******************************************************************************/

inline JSize
CBClass::GetParentCount()
	const
{
	return itsParentInfo->GetElementCount();
}

/******************************************************************************
 GetParentName

 ******************************************************************************/

inline const JString&
CBClass::GetParentName
	(
	const JIndex index
	)
	const
{
	return *((itsParentInfo->GetElement(index)).name);
}

/******************************************************************************
 GetParentType

 ******************************************************************************/

inline CBClass::InheritType
CBClass::GetParentType
	(
	const JIndex index
	)
	const
{
	return (itsParentInfo->GetElement(index)).inheritance;
}

/******************************************************************************
 Children

 ******************************************************************************/

inline JBoolean
CBClass::HasChildren()
	const
{
	return JConvertToBoolean(
			itsHasPrimaryChildrenFlag || itsHasSecondaryChildrenFlag );
}

inline JBoolean
CBClass::HasPrimaryChildren()
	const
{
	return itsHasPrimaryChildrenFlag;
}

inline JBoolean
CBClass::HasSecondaryChildren()
	const
{
	return itsHasSecondaryChildrenFlag;
}

/******************************************************************************
 GetFunctionCount

 ******************************************************************************/

inline JSize
CBClass::GetFunctionCount()
	const
{
	return itsFunctionInfo->GetElementCount();
}

/******************************************************************************
 GetFunctionName

 ******************************************************************************/

inline const JString&
CBClass::GetFunctionName
	(
	const JIndex index
	)
	const
{
	return *((itsFunctionInfo->GetElement(index)).name);
}

/******************************************************************************
 GetFnAccessLevel

 ******************************************************************************/

inline CBClass::FnAccessLevel
CBClass::GetFnAccessLevel
	(
	const JIndex index
	)
	const
{
	return (itsFunctionInfo->GetElement(index)).access;
}

/******************************************************************************
 FnAccess types (static)

 ******************************************************************************/

inline JBoolean
CBClass::IsPublic
	(
	const FnAccessLevel access
	)
{
	return JI2B(access == kPublicAccess || access == kQtPublicSlotAccess);
}

inline JBoolean
CBClass::IsProtected
	(
	const FnAccessLevel access
	)
{
	return JI2B(access == kProtectedAccess || access == kQtProtectedSlotAccess);
}

inline JBoolean
CBClass::IsPrivate
	(
	const FnAccessLevel access
	)
{
	return JI2B(access == kPrivateAccess || access == kQtPrivateSlotAccess);
}

inline JBoolean
CBClass::IsSignal
	(
	const FnAccessLevel access
	)
{
	return JI2B(access == kQtSignalAccess);
}

inline JBoolean
CBClass::IsSlot
	(
	const FnAccessLevel access
	)
{
	return JI2B(access == kQtPublicSlotAccess ||
				access == kQtProtectedSlotAccess ||
				access == kQtPrivateSlotAccess);
}

/******************************************************************************
 IsImplemented

 ******************************************************************************/

inline JBoolean
CBClass::IsImplemented
	(
	const JIndex index
	)
	const
{
	return (itsFunctionInfo->GetElement(index)).implemented;
}

/******************************************************************************
 GetCoords

 ******************************************************************************/

inline void
CBClass::GetCoords
	(
	JCoordinate* x,
	JCoordinate* y
	)
	const
{
	*x = itsHCoord;
	*y = itsVCoord;
}

/******************************************************************************
 SetCoords

 ******************************************************************************/

inline void
CBClass::SetCoords
	(
	const JCoordinate x,
	const JCoordinate y
	)
{
	itsHCoord = x;
	itsVCoord = y;
	CalcFrame();
}

/******************************************************************************
 GetFrame

 ******************************************************************************/

inline const JRect&
CBClass::GetFrame()
	const
{
	return itsFrame;
}

/******************************************************************************
 Contains

 ******************************************************************************/

inline JBoolean
CBClass::Contains
	(
	const JPoint& pt
	)
	const
{
	return itsFrame.Contains(pt);
}

/******************************************************************************
 Visible

 ******************************************************************************/

inline JBoolean
CBClass::IsVisible()
	const
{
	return itsVisibleFlag;
}

inline void
CBClass::SetVisible
	(
	const JBoolean visible
	)
{
	itsVisibleFlag = visible;
}

/******************************************************************************
 Selection

 ******************************************************************************/

inline JBoolean
CBClass::IsSelected()
	const
{
	return itsIsSelectedFlag;
}

inline void
CBClass::ToggleSelected()
{
	SetSelected(!itsIsSelectedFlag);
}

/******************************************************************************
 Collapsed

 ******************************************************************************/

inline JBoolean
CBClass::IsCollapsed()
	const
{
	return itsCollapsedFlag;
}

inline void
CBClass::SetCollapsed
	(
	const JBoolean collapse
	)
{
	itsCollapsedFlag = collapse;
}

/******************************************************************************
 GetTree

 ******************************************************************************/

inline const CBTree*
CBClass::GetTree()
	const
{
	return itsTree;
}

/******************************************************************************
 Ghost name color (static)

 ******************************************************************************/

inline JColorIndex
CBClass::GetGhostNameColor()
{
	return itsGhostNameColor;
}

inline void
CBClass::SetGhostNameColor
	(
	const JColorIndex color
	)
{
	itsGhostNameColor = color;
}

#endif
