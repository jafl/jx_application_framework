/******************************************************************************
 JXDirTable.h

	Copyright © 1996 by Glenn W. Bach.
	Copyright © 1998 by John Lindal.  All rights reserved.

 ******************************************************************************/

#ifndef _H_JXDirTable
#define _H_JXDirTable

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXTable.h>

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

	virtual ~JXDirTable();

	const JString&	GetPath() const;

	JBoolean	HasSelection() const;
	JBoolean	HasSingleSelection() const;
	JBoolean	GetFirstSelection(const JDirEntry** theEntry) const;
	JBoolean	GetSelection(JPtrArray<JDirEntry>* entryList) const;
	JBoolean	SelectSingleEntry(const JIndex index, const JBoolean scroll = kJTrue);
	void		SelectFirstEntry(const JBoolean scroll = kJTrue);
	void		SelectLastEntry(const JBoolean scroll = kJTrue);
	void		SelectAll();
	JBoolean	ClosestMatch(const JCharacter* prefixStr, JIndex* index) const;

	void	ShowHidden(const JBoolean showHidden);
	void	AllowSelectFiles(const JBoolean allowSelectFiles, const JBoolean allowMultiple);
	void	AllowDblClickInactive(const JBoolean allow);
	void	InstallShortcuts();

	JBoolean	GoToSelectedDirectory();

	JBoolean	WillSelectWhenChangePath() const;
	void		ShouldSelectWhenChangePath(const JBoolean select);

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers);
	virtual void	HandleShortcut(const int key, const JXKeyModifiers& modifiers);

	virtual JBoolean	IsSelectable(const JPoint& cell,
									 const JBoolean forExtend) const;

protected:

	JBoolean	ItemIsActive(const JIndex index) const;
	JBoolean	ItemIsFile(const JIndex index) const;

	JBoolean	GetNextSelectable(const JIndex startIndex, const JBoolean forMulti,
								  JIndex* nextIndex) const;
	JBoolean	GetPrevSelectable(const JIndex startIndex, const JBoolean forMulti,
								  JIndex* nextIndex) const;
	JBoolean	ItemIsSelectable(const JIndex index, const JBoolean forMulti) const;

	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers);

	virtual JBoolean	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
									   const Time time, const JXWidget* source);
	virtual void		HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
									  const Atom action, const Time time,
									  const JXWidget* source);

	virtual void	HandleFocusEvent();

	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh);
	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JDirInfo*				itsDirInfo;			// not owned
	JRunArray<JBoolean>*	itsActiveCells;
	JXTimerTask*			itsDirUpdateTask;

	JBoolean	itsIgnoreSelChangesFlag;		// kJTrue while cleaning selection
	JBoolean	itsAllowSelectFilesFlag;
	JBoolean	itsAllowSelectMultipleFlag;		// kJTrue => select multiple files, but not directories
	JBoolean	itsAllowDblClickInactiveFlag;	// kJTrue => broadcast FileDblClicked even if inactive
	JBoolean	itsSelectWhenChangePathFlag;	// kJTrue => select first entry when path changes
	JSize		itsMaxStringWidth;
	JString*	itsKeyBuffer;

	JBoolean			itsReselectFlag;		// kJFalse => select first item in directory
	JPtrArray<JString>*	itsReselectNameList;	// non-empty => reselect after AdjustTableContents()
	JPoint				itsReselectScrollOffset;

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

	// not allowed

	JXDirTable(const JXDirTable& source);
	const JXDirTable& operator=(const JXDirTable& source);

public:

	// JBroadcaster messages

	static const JCharacter* kFileDblClicked;

	class FileDblClicked : public JBroadcaster::Message
		{
		public:

			FileDblClicked(const JDirEntry& entry, const JBoolean active)
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

			JBoolean
			IsActive() const
			{
				return itsActiveFlag;
			};

		private:

			const JDirEntry&	itsDirEntry;
			JBoolean			itsActiveFlag;
		};
};


/******************************************************************************
 AllowDblClickInactive

 ******************************************************************************/

inline void
JXDirTable::AllowDblClickInactive
	(
	const JBoolean allow
	)
{
	itsAllowDblClickInactiveFlag = allow;
}

/******************************************************************************
 Select first item when path changes

 ******************************************************************************/

inline JBoolean
JXDirTable::WillSelectWhenChangePath()
	const
{
	return itsSelectWhenChangePathFlag;
}

inline void
JXDirTable::ShouldSelectWhenChangePath
	(
	const JBoolean select
	)
{
	itsSelectWhenChangePathFlag = select;
}

/******************************************************************************
 ItemIsActive (protected)

 ******************************************************************************/

inline JBoolean
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

inline JBoolean
JXDirTable::ItemIsSelectable
	(
	const JIndex	index,
	const JBoolean	forMulti
	)
	const
{
	return JI2B( ItemIsActive(index) && (!forMulti || ItemIsFile(index)) );
}

#endif
