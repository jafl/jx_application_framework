/******************************************************************************
 JXDirTable.h

	Copyright (C) 1996 by Glenn W. Bach.
	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXDirTable
#define _H_JXDirTable

#include "jx-af/jx/JXTable.h"

class JString;
class JDirInfo;
class JDirEntry;
class JXImage;
class JXTimerTask;

class JXDirTable : public JXTable
{
public:

	JXDirTable(JDirInfo* data,
			   JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
			   const HSizingOption hSizing, const VSizingOption vSizing,
			   const JCoordinate x, const JCoordinate y,
			   const JCoordinate w, const JCoordinate h);

	~JXDirTable() override;

	const JString&	GetPath() const;

	bool	HasSelection() const;
	bool	HasSingleSelection() const;
	bool	GetFirstSelection(const JDirEntry** theEntry) const;
	bool	GetSelection(JPtrArray<JDirEntry>* entryList) const;
	bool	SelectSingleEntry(const JIndex index, const bool scroll = true);
	void	SelectFirstEntry(const bool scroll = true);
	void	SelectLastEntry(const bool scroll = true);
	void	SelectAll();
	bool	ClosestMatch(const JString& prefixStr, JIndex* index) const;

	void	ShowHidden(const bool showHidden);
	void	AllowSelectFiles(const bool allowSelectFiles, const bool allowMultiple);
	void	AllowDblClickInactive(const bool allow);
	void	InstallShortcuts();

	bool	GoToSelectedDirectory();

	bool	WillSelectWhenChangePath() const;
	void	ShouldSelectWhenChangePath(const bool select);

	void	HandleKeyPress(const JUtf8Character& c, const int keySym,
						   const JXKeyModifiers& modifiers) override;
	void	HandleShortcut(const int key, const JXKeyModifiers& modifiers) override;

	bool	IsSelectable(const JPoint& cell,
						 const bool forExtend) const override;

protected:

	bool	ItemIsActive(const JIndex index) const;
	bool	ItemIsFile(const JIndex index) const;

	bool	GetNextSelectable(const JIndex startIndex, const bool forMulti,
						  JIndex* nextIndex) const;
	bool	GetPrevSelectable(const JIndex startIndex, const bool forMulti,
						  JIndex* nextIndex) const;
	bool	ItemIsSelectable(const JIndex index, const bool forMulti) const;

	void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
							const JSize clickCount,
							const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;
	void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;
	void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
						  const JXButtonStates& buttonStates,
						  const JXKeyModifiers& modifiers) override;

	bool	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
						   const JPoint& pt, const Time time,
						   const JXWidget* source) override;
	void		HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
							  const Atom action, const Time time,
							  const JXWidget* source) override;

	void	HandleFocusEvent() override;

	void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;
	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JDirInfo*		itsDirInfo;			// not owned
	JArray<bool>*	itsActiveCells;
	JXTimerTask*	itsDirUpdateTask;

	bool	itsIgnoreSelChangesFlag;		// true while cleaning selection
	bool	itsAllowSelectFilesFlag;
	bool	itsAllowSelectMultipleFlag;		// true => select multiple files, but not directories
	bool	itsAllowDblClickInactiveFlag;	// true => broadcast FileDblClicked even if inactive
	bool	itsSelectWhenChangePathFlag;	// true => select first entry when path changes
	JSize	itsMaxStringWidth;
	JString	itsKeyBuffer;

	bool				itsReselectFlag;		// false => select first item in directory
	JPtrArray<JString>*	itsReselectNameList;	// non-empty => reselect after AdjustTableContents()
	JPoint				itsReselectScrollOffset;

	// not owned

	JXImage*	itsFileIcon;
	JXImage*	itsFolderIcon;
	JXImage*	itsReadOnlyFolderIcon;
	JXImage*	itsLockedFolderIcon;
	JXImage*	itsExecIcon;
	JXImage*	itsUnknownIcon;

private:

	void	HandleDoubleClick(const JIndex index);

	void	AdjustTableContents();
	void	RememberSelections();
	void	CleanSelection();
	void	AdjustColWidths();

	void	UpdateDisplay();

public:

	// JBroadcaster messages

	static const JUtf8Byte* kFileDblClicked;

	class FileDblClicked : public JBroadcaster::Message
		{
		public:

			FileDblClicked(const JDirEntry& entry, const bool active)
				:
				JBroadcaster::Message(kFileDblClicked),
				itsDirEntry(entry),
				itsActiveFlag(active)
			{ };

			const JDirEntry&
			GetDirEntry() const
			{
				return itsDirEntry;
			};

			bool
			IsActive() const
			{
				return itsActiveFlag;
			};

		private:

			const JDirEntry&	itsDirEntry;
			bool			itsActiveFlag;
		};
};


/******************************************************************************
 AllowDblClickInactive

 ******************************************************************************/

inline void
JXDirTable::AllowDblClickInactive
	(
	const bool allow
	)
{
	itsAllowDblClickInactiveFlag = allow;
}

/******************************************************************************
 Select first item when path changes

 ******************************************************************************/

inline bool
JXDirTable::WillSelectWhenChangePath()
	const
{
	return itsSelectWhenChangePathFlag;
}

inline void
JXDirTable::ShouldSelectWhenChangePath
	(
	const bool select
	)
{
	itsSelectWhenChangePathFlag = select;
}

/******************************************************************************
 ItemIsActive (protected)

 ******************************************************************************/

inline bool
JXDirTable::ItemIsActive
	(
	const JIndex index
	)
	const
{
	return itsActiveCells->GetElement(index);
}

/******************************************************************************
 ItemIsSelectable (protected)

 ******************************************************************************/

inline bool
JXDirTable::ItemIsSelectable
	(
	const JIndex	index,
	const bool	forMulti
	)
	const
{
	return ItemIsActive(index) && (!forMulti || ItemIsFile(index));
}

#endif
