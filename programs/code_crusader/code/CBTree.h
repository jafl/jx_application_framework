/******************************************************************************
 CBTree.h

	Interface for the CBTree Class

	Copyright © 1995-99 John Lindal.

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
typedef CBClass* (CBClassStreamInFn)(std::istream& input, const JFileVersion vers, CBTree* tree);

class CBTree : public JContainer
{
	friend class CBClass;

public:

	CBTree(CBClassStreamInFn* streamInFn,
		   CBTreeDirector* director, const CBTextFileType fileType,
		   const JSize marginWidth);
	CBTree(std::istream& projInput, const JFileVersion projVers,
		   std::istream* setInput, const JFileVersion setVers,
		   std::istream* symInput, const JFileVersion symVers,
		   CBClassStreamInFn* streamInFn,
		   CBTreeDirector* director, const CBTextFileType fileType,
		   const JSize marginWidth, CBDirList* dirList);

	virtual ~CBTree();

	void		NextUpdateMustReparseAll();
	bool	NeedsReparseAll() const;
	void		RebuildLayout();

	bool	FindClass(const JString& fullName, CBClass** theClass) const;
	bool	FindClass(const JString& fullName, const CBClass** theClass) const;
	bool	IsUniqueClassName(const JString& name, const CBClass** theClass) const;
	bool	ClosestVisibleMatch(const JString& prefixStr, CBClass** theClass) const;
	bool	FindParent(const JString& parentName, const CBClass* container,
						   CBClass** parent, JString* nameSpace) const;

	virtual void	StreamOut(std::ostream& projOutput, std::ostream* setOutput,
							  std::ostream* symOutput, const CBDirList* dirList) const;

	bool	HasSelection() const;
	void		DeselectAll();
	bool	GetSelectedClasses(JPtrArray<CBClass>* classList) const;
	bool	GetSelectionCoverage(JRect* coverage, JSize* count) const;
	void		SelectClasses(const JString& name, const bool deselectAll = true);
	void		SelectImplementors(const JString& fnName, const bool caseSensitive,
								   const bool deselectAll = true);
	void		SelectParents();
	void		SelectDescendants();
	void		CollapseExpandSelectedClasses(const bool collapse);
	void		ExpandAllClasses();

	void		ViewSelectedSources() const;
	void		ViewSelectedHeaders() const;
	void		ViewSelectedFunctionLists() const;
	void		CopySelectedClassNames() const;
	void		DeriveFromSelected() const;

	bool	WillAutoMinimizeMILinks() const;
	void		ShouldAutoMinimizeMILinks(const bool autoMinimize);
	bool	NeedsMinimizeMILinks() const;		// when auto-minimize is off or cancelled
	void		ForceMinimizeMILinks();

	void		GetMenuInfo(bool* hasSelection,
							bool* canCollapse, bool* canExpand) const;
	bool	GetClass(const JPoint& pt, CBClass** theClass) const;
	bool	HitSameClass(const JPoint& pt1, const JPoint& pt2, CBClass** theClass) const;

	void		Draw(JPainter& p, const JRect& rect) const;

	bool	WillDrawMILinksOnTop() const;
	void		ShouldDrawMILinksOnTop(const bool drawOnTop);

	JFontManager*	GetFontManager() const;
	JSize			GetFontSize() const;
	void			SetFontSize(const JSize size);
	JCoordinate		GetLineHeight() const;
	void			GetBounds(JCoordinate* width, JCoordinate* height) const;

	const JPtrArray<JString>&	GetParseSuffixes() const;
	const JPtrArray<CBClass>&	GetClasses() const;
	const JPtrArray<CBClass>&	GetVisibleClasses() const;

	CBProjectDocument*	GetProjectDoc() const;
	CBTreeDirector*		GetTreeDirector() const;
	CBTextFileType		GetFileType() const;

	// for loading updated symbols

	virtual void	ReloadSetup(std::istream& input, const JFileVersion vers);

	// called by CBTreeDirector

	void	FileTypesChanged(const CBPrefsManager::FileTypesChanged& info);

	virtual void		PrepareForUpdate(const bool reparseAll);
	virtual bool	UpdateFinished(const JArray<JFAID_t>& deadFileList);

	// called by CBFileListTable

	void	FileChanged(const JString& fileName,
						const CBTextFileType fileType, const JFAID_t id);

protected:

	struct RootGeom
	{
		CBClass*	root;	// not owned
		JSize		top;
		JSize		h;

		RootGeom()
			:
			root(nullptr), top(0), h(0)
		{ };

		RootGeom(CBClass* r, const JSize t, const JSize height)
			:
			root(r), top(t), h(height)
		{ };
	};

protected:

	JPtrArray<CBClass>*	GetClasses();
	JPtrArray<CBClass>*	GetVisibleClasses();

	void	PlaceAll(JArray<RootGeom>* rootGeom = nullptr);

	virtual void	ParseFile(const JString& fileName, const JFAID_t id) = 0;

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
			root(nullptr), h(0), connList(nullptr)
		{ };

		RootMIInfo(CBClass* r, const JSize height, JArray<RootConn>* list)
			:
			root(r), h(height), connList(list)
		{ };
	};

	struct RootSubset
	{
		JArray<bool>*	content;
		JArray<JIndex>*		order;
		JSize				linkLength;

		RootSubset()
			:
			content(nullptr), order(nullptr), linkLength(0)
		{ };

		RootSubset(JArray<bool>* c, JArray<JIndex>* o, const JSize l)
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
	JPtrArray<JString>*		itsCollapsedList;			// nullptr unless updating files
	bool				itsReparseAllFlag;			// true => flush all on next update
	bool				itsChangedDuringParseFlag;	// only used while parsing
	bool				itsBeganEmptyFlag;			// true => ignore RemoveFile()

	JSize		itsFontSize;
	JCoordinate	itsWidth;
	JCoordinate	itsHeight;
	const JSize	itsMarginWidth;
	bool	itsBroadcastClassSelFlag;

	bool	itsDrawMILinksOnTopFlag;

	bool	itsMinimizeMILinksFlag;
	bool	itsNeedsMinimizeMILinksFlag;		// true => links not currently minimized

	CBClassStreamInFn*	itsStreamInFn;

private:

	void	CBTreeX(CBTreeDirector* director, CBClassStreamInFn* streamInFn);

	void	RemoveFile(const JFAID_t id);

	void	RebuildTree();
	void	RecalcVisible(const bool forceRebuildVisible = false,
						  const bool forcePlaceAll = false);
	void	PlaceClass(CBClass* theClass, const JCoordinate x, JCoordinate* y,
					   JCoordinate* maxWidth);

	void		SaveCollapsedClasses(JPtrArray<JString>* list) const;
	bool	RestoreCollapsedClasses(const JPtrArray<JString>& list);

	void		MinimizeMILinks();
	bool	ArrangeRoots(const JArray<RootMIInfo>& rootList,
							 JArray<JIndex>* rootOrder, JProgressDisplay& pg) const;
	void		CleanList(JArray<RootSubset>* list) const;
	void		FindMIClasses(CBClass* theClass, JArray<bool>* marked,
							  const JArray<RootGeom>& rootGeom,
							  JArray<RootMIInfo>* rootList) const;
	void		FindRoots(CBClass* theClass, const JArray<RootGeom>& rootGeom,
						  JArray<RootMIInfo>* rootInfoList) const;
	bool	FindRoot(CBClass* root, const JArray<RootMIInfo>& rootInfoList,
						 JIndex* index) const;

	void	CollectAncestors(CBClass* cbClass, JPtrArray<CBClass>* list) const;

	static JListT::CompareResult
		CompareClassFullNames(CBClass* const & c1, CBClass* const & c2);
	static JListT::CompareResult
		CompareClassNames(CBClass* const & c1, CBClass* const & c2);

	static JListT::CompareResult
		CompareRGClassPtrs(const RootGeom& i1, const RootGeom& i2);
	static JListT::CompareResult
		CompareRSContent(const RootSubset& i1, const RootSubset& i2);

	// called by CBClass

	void		AddClass(CBClass* newClass);

	CBClass*	IndexToClassAfterRead(const JIndex index) const;
	JIndex		ClassToIndexForWrite(const CBClass* theClass) const;

	void		BroadcastSelectionChange(CBClass* theClass, const bool isSelected);

	// not allowed

	CBTree(const CBTree& source);
	CBTree& operator=(const CBTree& source);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kChanged;
	static const JUtf8Byte* kBoundsChanged;
	static const JUtf8Byte* kNeedsRefresh;
	static const JUtf8Byte* kFontSizeChanged;

	static const JUtf8Byte* kPrepareForParse;
	static const JUtf8Byte* kParseFinished;

	static const JUtf8Byte* kClassSelected;
	static const JUtf8Byte* kClassDeselected;
	static const JUtf8Byte* kAllClassesDeselected;

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

			ParseFinished(const bool changed)
				:
				JBroadcaster::Message(kParseFinished),
				itsChangedFlag(changed)
				{ };

			bool
			Changed()
				const
			{
				return itsChangedFlag;
			};

		private:

			bool	itsChangedFlag;
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
 Show enums

 ******************************************************************************/

inline bool
CBTree::WillAutoMinimizeMILinks()
	const
{
	return itsMinimizeMILinksFlag;
}

inline void
CBTree::ShouldAutoMinimizeMILinks
	(
	const bool autoMinimize
	)
{
	if (autoMinimize != itsMinimizeMILinksFlag)
		{
		itsMinimizeMILinksFlag = autoMinimize;
		if (itsMinimizeMILinksFlag && itsNeedsMinimizeMILinksFlag)
			{
			RecalcVisible(true);
			}
		}
}

inline bool
CBTree::NeedsMinimizeMILinks()
	const
{
	return itsNeedsMinimizeMILinksFlag;
}

/******************************************************************************
 Draw MI links on top of text

 ******************************************************************************/

inline bool
CBTree::WillDrawMILinksOnTop()
	const
{
	return itsDrawMILinksOnTopFlag;
}

inline void
CBTree::ShouldDrawMILinksOnTop
	(
	const bool drawOnTop
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
	itsReparseAllFlag = true;
}

/******************************************************************************
 NeedsReparseAll

 ******************************************************************************/

inline bool
CBTree::NeedsReparseAll()
	const
{
	return itsReparseAllFlag;
}

/******************************************************************************
 RebuildLayout

 ******************************************************************************/

inline void
CBTree::RebuildLayout()
{
	const JSize size = itsFontSize;
	itsFontSize      = 0;
	SetFontSize(size);
}

#endif
