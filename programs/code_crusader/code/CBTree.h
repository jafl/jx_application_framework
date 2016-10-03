/******************************************************************************
 CBTree.h

	Interface for the CBTree Class

	Copyright (C) 1995-99 John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBTree
#define _H_CBTree

#include <JContainer.h>
#include "CBTextFileType.h"
#include "CBPrefsManager.h"		// need definition of FileTypesChanged
#include <JPtrArray.h>
#include <JRect.h>
#include <JFAID.h>
#include <jTime.h>

class JString;
class JPainter;
class JFontManager;
class JProgressDisplay;
class CBClass;
class CBDirList;
class CBTreeDirector;
class CBProjectDocument;

class CBTree;
typedef CBClass* (CBClassStreamInFn)(istream& input, const JFileVersion vers, CBTree* tree);

class CBTree : public JContainer
{
	friend class CBClass;

public:

	CBTree(CBClassStreamInFn* streamInFn,
		   CBTreeDirector* director, const CBTextFileType fileType,
		   const JSize marginWidth);
	CBTree(istream& projInput, const JFileVersion projVers,
		   istream* setInput, const JFileVersion setVers,
		   istream* symInput, const JFileVersion symVers,
		   CBClassStreamInFn* streamInFn,
		   CBTreeDirector* director, const CBTextFileType fileType,
		   const JSize marginWidth, CBDirList* dirList);

	virtual ~CBTree();

	void		NextUpdateMustReparseAll();
	JBoolean	NeedsReparseAll() const;

	JBoolean	FindClass(const JCharacter* fullName, CBClass** theClass) const;
	JBoolean	FindClass(const JCharacter* fullName, const CBClass** theClass) const;
	JBoolean	IsUniqueClassName(const JCharacter* name, const CBClass** theClass) const;
	JBoolean	ClosestVisibleMatch(const JCharacter* prefixStr, CBClass** theClass) const;
	JBoolean	FindParent(const JCharacter* parentName, const CBClass* container,
						   CBClass** parent, JString* nameSpace) const;

	virtual void	StreamOut(ostream& projOutput, ostream* setOutput,
							  ostream* symOutput, const CBDirList* dirList) const;

	JBoolean	HasSelection() const;
	void		DeselectAll();
	JBoolean	GetSelectedClasses(JPtrArray<CBClass>* classList) const;
	JBoolean	GetSelectionCoverage(JRect* coverage, JSize* count) const;
	void		SelectClasses(const JCharacter* name, const JBoolean deselectAll = kJTrue);
	void		SelectImplementors(const JCharacter* fnName, const JBoolean caseSensitive,
								   const JBoolean deselectAll = kJTrue);
	void		SelectParents();
	void		SelectDescendants();
	void		CollapseExpandSelectedClasses(const JBoolean collapse);
	void		ExpandAllClasses();

	void		ViewSelectedSources() const;
	void		ViewSelectedHeaders() const;
	void		ViewSelectedFunctionLists() const;
	void		CopySelectedClassNames() const;
	void		DeriveFromSelected() const;

	JBoolean	WillShowLoneClasses() const;
	void		ShowLoneClasses(const JBoolean visible);
	void		ToggleShowLoneClasses();

	JBoolean	WillShowLoneStructs() const;
	void		ShowLoneStructs(const JBoolean visible);
	void		ToggleShowLoneStructs();

	JBoolean	WillShowEnums() const;
	void		ShowEnums(const JBoolean visible);
	void		ToggleShowEnums();

	JBoolean	WillAutoMinimizeMILinks() const;
	void		ShouldAutoMinimizeMILinks(const JBoolean autoMinimize);
	JBoolean	NeedsMinimizeMILinks() const;		// when auto-minimize is off or cancelled
	void		ForceMinimizeMILinks();

	void		GetMenuInfo(JBoolean* hasSelection,
							JBoolean* canCollapse, JBoolean* canExpand) const;
	JBoolean	GetClass(const JPoint& pt, CBClass** theClass) const;
	JBoolean	HitSameClass(const JPoint& pt1, const JPoint& pt2, CBClass** theClass) const;

	void		Draw(JPainter& p, const JRect& rect) const;

	JBoolean	WillDrawMILinksOnTop() const;
	void		ShouldDrawMILinksOnTop(const JBoolean drawOnTop);

	const JFontManager*	GetFontManager() const;
	JSize				GetFontSize() const;
	void				SetFontSize(const JSize size);
	JCoordinate			GetLineHeight() const;
	void				GetBounds(JCoordinate* width, JCoordinate* height) const;

	const JPtrArray<JString>&	GetParseSuffixes() const;
	const JPtrArray<CBClass>&	GetClasses() const;
	const JPtrArray<CBClass>&	GetVisibleClasses() const;

	CBProjectDocument*	GetProjectDoc() const;
	CBTreeDirector*		GetTreeDirector() const;
	CBTextFileType		GetFileType() const;

	// for loading updated symbols

	virtual void	ReloadSetup(istream& input, const JFileVersion vers);

	// called by CBTreeDirector

	void	FileTypesChanged(const CBPrefsManager::FileTypesChanged& info);

	virtual void		PrepareForUpdate(const JBoolean reparseAll);
	virtual JBoolean	UpdateFinished(const JArray<JFAID_t>& deadFileList);

	// called by CBFileListTable

	void	FileChanged(const JCharacter* fileName,
						const CBTextFileType fileType, const JFAID_t id);

protected:

	struct RootGeom
	{
		CBClass*	root;	// not owned
		JSize		top;
		JSize		h;

		RootGeom()
			:
			root(NULL), top(0), h(0)
		{ };

		RootGeom(CBClass* r, const JSize t, const JSize height)
			:
			root(r), top(t), h(height)
		{ };
	};

protected:

	JPtrArray<CBClass>*	GetClasses();
	JPtrArray<CBClass>*	GetVisibleClasses();

	void	PlaceAll(JArray<RootGeom>* rootGeom = NULL);

	virtual void	ParseFile(const JCharacter* fileName, const JFAID_t id) = 0;

private:

	struct RootConn
	{
		JSize	dy;
		JIndex	otherRoot;

		RootConn()
			:
			dy(0), otherRoot(0)
		{ };

		RootConn(const JSize deltaY, const JIndex i)
			:
			dy(deltaY), otherRoot(i)
		{ };
	};

	struct RootMIInfo
	{
		CBClass*			root;		// not owned
		JSize				h;
		JArray<RootConn>*	connList;

		RootMIInfo()
			:
			root(NULL), h(0), connList(NULL)
		{ };

		RootMIInfo(CBClass* r, const JSize height, JArray<RootConn>* list)
			:
			root(r), h(height), connList(list)
		{ };
	};

	struct RootSubset
	{
		JArray<JBoolean>*	content;
		JArray<JIndex>*		order;
		JSize				linkLength;

		RootSubset()
			:
			content(NULL), order(NULL), linkLength(0)
		{ };

		RootSubset(JArray<JBoolean>* c, JArray<JIndex>* o, const JSize l)
			:
			content(c), order(o), linkLength(l)
		{ };
	};

private:

	CBTreeDirector*			itsDirector;				// not owned

	JPtrArray<CBClass>*		itsClassesByFull;			// owns objects
	JPtrArray<CBClass>*		itsVisibleByGeom;			// doesn't own objects
	JPtrArray<CBClass>*		itsVisibleByName;			// doesn't own objects

	const CBTextFileType	itsFileType;
	JPtrArray<JString>*		itsSuffixList;
	JPtrArray<JString>*		itsCollapsedList;			// NULL unless updating files
	JBoolean				itsReparseAllFlag;			// kJTrue => flush all on next update
	JBoolean				itsChangedDuringParseFlag;	// only used while parsing
	JBoolean				itsBeganEmptyFlag;			// kJTrue => ignore RemoveFile()

	JSize		itsFontSize;
	JCoordinate	itsWidth;
	JCoordinate	itsHeight;
	const JSize	itsMarginWidth;
	JBoolean	itsBroadcastClassSelFlag;

	JBoolean	itsShowLoneClassesFlag;
	JBoolean	itsShowLoneStructsFlag;
	JBoolean	itsShowEnumsFlag;
	JBoolean	itsDrawMILinksOnTopFlag;

	JBoolean	itsMinimizeMILinksFlag;
	JBoolean	itsNeedsMinimizeMILinksFlag;		// kJTrue => links not currently minimized

	CBClassStreamInFn*	itsStreamInFn;

private:

	void	CBTreeX(CBTreeDirector* director, CBClassStreamInFn* streamInFn);

	void	RemoveFile(const JFAID_t id);

	void	RebuildTree();
	void	RecalcVisible(const JBoolean forceRebuildVisible = kJFalse,
						  const JBoolean forcePlaceAll = kJFalse);
	void	PlaceClass(CBClass* theClass, const JCoordinate x, JCoordinate* y,
					   JCoordinate* maxWidth);

	void	ForceVisible(CBClass* theClass);

	void		SaveCollapsedClasses(JPtrArray<JString>* list) const;
	JBoolean	RestoreCollapsedClasses(const JPtrArray<JString>& list);

	void		MinimizeMILinks();
	JBoolean	ArrangeRoots(const JArray<RootMIInfo>& rootList,
							 JArray<JIndex>* rootOrder, JProgressDisplay& pg) const;
	void		CleanList(JArray<RootSubset>* list) const;
	void		FindMIClasses(CBClass* theClass, JArray<JBoolean>* marked,
							  const JArray<RootGeom>& rootGeom,
							  JArray<RootMIInfo>* rootList) const;
	void		FindRoots(CBClass* theClass, const JArray<RootGeom>& rootGeom,
						  JArray<RootMIInfo>* rootInfoList) const;
	JBoolean	FindRoot(CBClass* root, const JArray<RootMIInfo>& rootInfoList,
						 JIndex* index) const;

	void	CollectAncestors(CBClass* cbClass, JPtrArray<CBClass>* list) const;

	static JOrderedSetT::CompareResult
		CompareClassFullNames(CBClass* const & c1, CBClass* const & c2);
	static JOrderedSetT::CompareResult
		CompareClassNames(CBClass* const & c1, CBClass* const & c2);

	static JOrderedSetT::CompareResult
		CompareRGClassPtrs(const RootGeom& i1, const RootGeom& i2);
	static JOrderedSetT::CompareResult
		CompareRSContent(const RootSubset& i1, const RootSubset& i2);

	// called by CBClass

	void		AddClass(CBClass* newClass);

	CBClass*	IndexToClassAfterRead(const JIndex index) const;
	JIndex		ClassToIndexForWrite(const CBClass* theClass) const;

	void		BroadcastSelectionChange(CBClass* theClass, const JBoolean isSelected);

	// not allowed

	CBTree(const CBTree& source);
	CBTree& operator=(const CBTree& source);

public:

	// JBroadcaster messages

	static const JCharacter* kChanged;
	static const JCharacter* kBoundsChanged;
	static const JCharacter* kNeedsRefresh;
	static const JCharacter* kFontSizeChanged;

	static const JCharacter* kPrepareForParse;
	static const JCharacter* kParseFinished;

	static const JCharacter* kClassSelected;
	static const JCharacter* kClassDeselected;
	static const JCharacter* kAllClassesDeselected;

	class Changed : public JBroadcaster::Message
		{
		public:

			Changed()
				:
				JBroadcaster::Message(kChanged)
				{ };
		};

	class BoundsChanged : public JBroadcaster::Message
		{
		public:

			BoundsChanged()
				:
				JBroadcaster::Message(kBoundsChanged)
				{ };
		};

	class NeedsRefresh : public JBroadcaster::Message
		{
		public:

			NeedsRefresh()
				:
				JBroadcaster::Message(kNeedsRefresh)
				{ };
		};

	class FontSizeChanged : public JBroadcaster::Message
		{
		public:

			FontSizeChanged(const JSize origSize, const JSize newSize,
							const JFloat vScaleFactor)
				:
				JBroadcaster::Message(kFontSizeChanged),
				itsOrigSize(origSize),
				itsNewSize(newSize),
				itsVertScaleFactor(vScaleFactor)
				{ };

			JSize
			GetOrigSize() const
			{
				return itsOrigSize;
			};

			JSize
			GetNewSize() const
			{
				return itsNewSize;
			};

			JFloat
			GetVertScaleFactor() const
			{
				return itsVertScaleFactor;
			};

		private:

			JSize	itsOrigSize, itsNewSize;
			JFloat	itsVertScaleFactor;
		};

	class PrepareForParse : public JBroadcaster::Message
		{
		public:

			PrepareForParse()
				:
				JBroadcaster::Message(kPrepareForParse)
				{ };
		};

	class ParseFinished : public JBroadcaster::Message
		{
		public:

			ParseFinished(const JBoolean changed)
				:
				JBroadcaster::Message(kParseFinished),
				itsChangedFlag(changed)
				{ };

			JBoolean
			Changed()
				const
			{
				return itsChangedFlag;
			};

		private:

			JBoolean	itsChangedFlag;
		};

	class ClassSelected : public JBroadcaster::Message
		{
		public:

			ClassSelected(CBClass* theClass)
				:
				JBroadcaster::Message(kClassSelected),
				itsClass(theClass)
				{ };

			CBClass*
			GetClass()
				const
			{
				return itsClass;
			};

		private:

			CBClass*	itsClass;
		};

	class ClassDeselected : public JBroadcaster::Message
		{
		public:

			ClassDeselected(CBClass* theClass)
				:
				JBroadcaster::Message(kClassDeselected),
				itsClass(theClass)
				{ };

			CBClass*
			GetClass()
				const
			{
				return itsClass;
			};

		private:

			CBClass*	itsClass;
		};

	class AllClassesDeselected : public JBroadcaster::Message
		{
		public:

			AllClassesDeselected()
				:
				JBroadcaster::Message(kAllClassesDeselected)
				{ };
		};
};


/******************************************************************************
 GetClasses

	We do not guarantee any particular ordering of the classes.
	If nothing else, the list may not be sorted.

 ******************************************************************************/

inline const JPtrArray<CBClass>&
CBTree::GetClasses()
	const
{
	return *itsClassesByFull;
}

inline JPtrArray<CBClass>*
CBTree::GetClasses()
{
	return itsClassesByFull;
}

/******************************************************************************
 GetVisibleClasses

	We do not guarantee any particular ordering of the classes.
	If nothing else, the list may not be sorted.

 ******************************************************************************/

inline const JPtrArray<CBClass>&
CBTree::GetVisibleClasses()
	const
{
	return *itsVisibleByName;
}

inline JPtrArray<CBClass>*
CBTree::GetVisibleClasses()
{
	return itsVisibleByName;
}

/******************************************************************************
 GetTreeDirector

 ******************************************************************************/

inline CBTreeDirector*
CBTree::GetTreeDirector()
	const
{
	return itsDirector;
}

/******************************************************************************
 GetFileType

 ******************************************************************************/

inline CBTextFileType
CBTree::GetFileType()
	const
{
	return itsFileType;
}

/******************************************************************************
 GetFontSize

 ******************************************************************************/

inline JSize
CBTree::GetFontSize()
	const
{
	return itsFontSize;
}

/******************************************************************************
 GetBounds

 ******************************************************************************/

inline void
CBTree::GetBounds
	(
	JCoordinate* width,
	JCoordinate* height
	)
	const
{
	*width  = itsWidth;
	*height = itsHeight;
}

/******************************************************************************
 Show lone classes

 ******************************************************************************/

inline JBoolean
CBTree::WillShowLoneClasses()
	const
{
	return itsShowLoneClassesFlag;
}

inline void
CBTree::ToggleShowLoneClasses()
{
	ShowLoneClasses(!itsShowLoneClassesFlag);
}

/******************************************************************************
 Show lone structs

 ******************************************************************************/

inline JBoolean
CBTree::WillShowLoneStructs()
	const
{
	return itsShowLoneStructsFlag;
}

inline void
CBTree::ToggleShowLoneStructs()
{
	ShowLoneStructs(!itsShowLoneStructsFlag);
}

/******************************************************************************
 Show enums

 ******************************************************************************/

inline JBoolean
CBTree::WillShowEnums()
	const
{
	return itsShowEnumsFlag;
}

inline void
CBTree::ToggleShowEnums()
{
	ShowEnums(!itsShowEnumsFlag);
}

/******************************************************************************
 Show enums

 ******************************************************************************/

inline JBoolean
CBTree::WillAutoMinimizeMILinks()
	const
{
	return itsMinimizeMILinksFlag;
}

inline void
CBTree::ShouldAutoMinimizeMILinks
	(
	const JBoolean autoMinimize
	)
{
	if (autoMinimize != itsMinimizeMILinksFlag)
		{
		itsMinimizeMILinksFlag = autoMinimize;
		if (itsMinimizeMILinksFlag && itsNeedsMinimizeMILinksFlag)
			{
			RecalcVisible(kJTrue);
			}
		}
}

inline JBoolean
CBTree::NeedsMinimizeMILinks()
	const
{
	return itsNeedsMinimizeMILinksFlag;
}

/******************************************************************************
 Draw MI links on top of text

 ******************************************************************************/

inline JBoolean
CBTree::WillDrawMILinksOnTop()
	const
{
	return itsDrawMILinksOnTopFlag;
}

inline void
CBTree::ShouldDrawMILinksOnTop
	(
	const JBoolean drawOnTop
	)
{
	if (drawOnTop != itsDrawMILinksOnTopFlag)
		{
		itsDrawMILinksOnTopFlag = drawOnTop;
		Broadcast(NeedsRefresh());
		}
}

/******************************************************************************
 GetParseSuffixes

 ******************************************************************************/

inline const JPtrArray<JString>&
CBTree::GetParseSuffixes()
	const
{
	return *itsSuffixList;
}

/******************************************************************************
 NextUpdateMustReparseAll

 ******************************************************************************/

inline void
CBTree::NextUpdateMustReparseAll()
{
	itsReparseAllFlag = kJTrue;
}

/******************************************************************************
 NeedsReparseAll

 ******************************************************************************/

inline JBoolean
CBTree::NeedsReparseAll()
	const
{
	return itsReparseAllFlag;
}

#endif
