/******************************************************************************
 CBClass.h

	Interface for the CBClass Class

	Copyright (C) 1995-97 John Lindal.

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
class CBTree;

class CBClass : virtual public JBroadcaster
{
	friend class CBTree;

public:

	// Do not change these values once they are assigned
	// because they are stored in files.

	enum DeclareType
	{
		kClassType = 0,
		kStructUnused,
		kGhostType,		// inferred, but not parsed
		kEnumUnused
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
		kUnusedQtSignalAccess,
		kUnusedQtPublicSlotAccess,
		kUnusedQtProtectedSlotAccess,
		kUnusedQtPrivateSlotAccess
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
	JBoolean		IsGhost() const;

	void			AddParent(const InheritType type, const JString& name);
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

	JBoolean		Implements(const JString& name, const JBoolean caseSensitive) const;

	void	Draw(JPainter& p, const JRect& rect) const;
	void	DrawMILinks(JPainter& p, const JRect& rect) const;
	void	DrawText(JPainter& p, const JRect& rect) const;

	const JRect&		GetFrame() const;
	JBoolean			Contains(const JPoint& pt) const;
	JCoordinate			GetTotalWidth() const;
	JCoordinate			GetTotalHeight() const;
	static JCoordinate	GetTotalHeight(CBTree* tree, JFontManager* fontManager);
	void				GetCoords(JCoordinate* x, JCoordinate* y) const;
	void				SetCoords(const JCoordinate x, const JCoordinate y);

	JBoolean	IsVisible() const;
	void		SetVisible(const JBoolean visible);
	void		ForceVisible();

	JBoolean	IsSelected() const;
	void		SetSelected(const JBoolean selected);
	void		ToggleSelected();

	JBoolean	IsCollapsed() const;
	void		SetCollapsed(const JBoolean collapse);

	virtual void	StreamOut(std::ostream& output) const;

	static JColorID	GetGhostNameColor();
	static void		SetGhostNameColor(const JColorID color);

	// called by CBTree::ReloadSetup

	void	CalcFrame();

protected:

	CBClass(const JString& fullName, const DeclareType declType,
			const JFAID_t fileID, CBTree* tree,
			const JUtf8Byte* namespaceOperator);
	CBClass(std::istream& input, const JFileVersion vers, CBTree* tree,
			const JUtf8Byte* namespaceOperator);
	CBClass(const JString& name);	// search target

	virtual CBClass*	NewGhost(const JString& name, CBTree* tree);
	const JUtf8Byte*	GetNamespaceOperator() const;
	JString				RemoveNamespace(const JString& fullName);

	virtual void	AdjustNameStyle(JFontStyle* style) const;

private:

	struct ParentInfo
	{
		JString*	name;
		CBClass*	parent;			// can be nullptr; not owned
		InheritType	inheritance;
		JIndex		indexFromFile;	// used while reading from file

		ParentInfo()
			:
			name(nullptr),
			parent(nullptr),
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

		void CleanOut();
	};

private:

	const JUtf8Byte* const	itsNamespaceOperator;	// must be first, to use when construction itsName

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

	static JColorID	theGhostNameColor;

private:

	void		CBClassX(CBTree* tree);
	JBoolean	FindParent(ParentInfo* pInfo, const JBoolean okToSearchGhosts);
	void		AddChild(CBClass* child, const JBoolean primary);

	static JCoordinate	CalcFrameHeight(JFontManager* fontManager,
										const JSize fontSize);

	JBoolean	NeedDrawName() const;
	JString		GetDrawName() const;

	JBoolean	NeedToDrawLink(const JPoint& pt1, const JPoint& pt2,
							   const JRect& visRect) const;
	JPoint		GetLinkFromPt() const;
	JPoint		GetLinkToPt() const;

	// called by CBTree

	void	ClearConnections();
	void	FindParentsAfterRead();

	// not allowed

	CBClass(const CBClass& source);
	CBClass& operator=(const CBClass& source);
};

std::istream& operator>>(std::istream& input, CBClass::DeclareType& type);
std::ostream& operator<<(std::ostream& output, const CBClass::DeclareType type);

std::istream& operator>>(std::istream& input, CBClass::InheritType& type);
std::ostream& operator<<(std::ostream& output, const CBClass::InheritType type);

std::istream& operator>>(std::istream& input, CBClass::FnAccessLevel& access);
std::ostream& operator<<(std::ostream& output, const CBClass::FnAccessLevel access);


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
 GetNamespaceOperator (protected)

	Returns the languages's namespace operator.

 ******************************************************************************/

inline const JUtf8Byte*
CBClass::GetNamespaceOperator()
	const
{
	return itsNamespaceOperator;
}

/******************************************************************************
 Ghost name color (static)

 ******************************************************************************/

inline JColorID
CBClass::GetGhostNameColor()
{
	return theGhostNameColor;
}

inline void
CBClass::SetGhostNameColor
	(
	const JColorID color
	)
{
	theGhostNameColor = color;
}

#endif
