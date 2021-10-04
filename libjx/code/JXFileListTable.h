/******************************************************************************
 JXFileListTable.h

	Interface for the JXFileListTable class

	Copyright (C) 1998-99 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXFileListTable
#define _H_JXFileListTable

#include "jx-af/jx/JXTable.h"

class JError;
class JString;
class JRegex;
class JXImage;
class JXTEBase;
class JXTextMenu;

class JXFileListTable : public JXTable
{
public:

	JXFileListTable(JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	virtual ~JXFileListTable();

	bool	AddFile(const JString& fullName, JIndex* fullNameIndex = nullptr);
	void	RemoveFile(const JString& fullName);
	void	RemoveFiles(const JPtrArray<JString>& fileList);
	void	RemoveSelectedFiles();
	void	RemoveAllFiles();
	bool	GetFullName(const JIndex rowIndex, JString* fullName) const;
//	bool	GetFullName(const JString& fileName, JString* fullName) const;

	const JPtrArray<JString>&	GetFullNameList() const;
	JIndex						RowIndexToFileIndex(const JIndex rowIndex) const;

	bool	GetFilterRegex(JString* regexStr) const;
	JError	SetFilterRegex(const JString& regexStr);
	void	ClearFilterRegex();

	bool	HasSelection() const;
	bool	GetSelection(JPtrArray<JString>* fileList) const;
	void	SelectSingleEntry(const JIndex index, const bool scroll = true);
	void	SelectAll();
	void	ClearSelection();

	void	ShowSelectedFileLocations() const;

	bool	GetEditMenuProvider(JXTEBase** te) const;
	void	SetEditMenuProvider(JXTEBase* te);

	bool	WillAcceptFileDrop() const;
	void	ShouldAcceptFileDrop(const bool accept = true);

	bool	BackspaceWillRemoveSelectedFiles() const;
	void	BackspaceShouldRemoveSelectedFiles(const bool remove = true);

	void	HandleKeyPress(const JUtf8Character& c, const int keySym,
								   const JXKeyModifiers& modifiers) override;
	void			ClearIncrementalSearchBuffer();

protected:

	JCollection*	GetFullNameDataList() const;
	JString			GetFileName(const JIndex rowIndex) const;

	virtual bool	ResolveFullName(JString* name);

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

	void	HandleFocusEvent() override;

	void	GetSelectionData(JXSelectionData* data,
									 const JString& id) override;
	Atom	GetDNDAction(const JXContainer* target,
								 const JXButtonStates& buttonStates,
								 const JXKeyModifiers& modifiers) override;
	void	HandleDNDResponse(const JXContainer* target,
									  const bool dropAccepted, const Atom action) override;

	bool	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
								   const JPoint& pt, const Time time,
								   const JXWidget* source) override;
	void	HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
								  const Atom action, const Time time,
								  const JXWidget* source) override;

	void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;
	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	enum DragType
	{
		kInvalidDrag,
		kWaitForDND
	};

	struct VisInfo
	{
		JIndex fileIndex;		// index into itsFileList
		JIndex nameIndex;		// index of first character of file name
		JIndex drawIndex;		// index of first character to draw
	};

private:

	JPtrArray<JString>*	itsFileList;			// full name of each file
	JArray<VisInfo>*	itsVisibleList;			// info about each visible item
	JRegex*				itsRegex;				// can be nullptr

	bool				itsAcceptFileDropFlag;	// true => accept drop of url/url
	bool				itsBSRemoveSelFlag;		// true => backspace removes selected files
	JSize				itsMaxStringWidth;
	JXImage*			itsFileIcon;			// not owned
	JString				itsKeyBuffer;

	// dragging

	DragType			itsDragType;
	JPoint				itsMouseDownPt;

	// edit menu (copy, select all)

	JXTEBase*			itsEditMenuProvider;	// can be nullptr; not owned
	JXTextMenu*			itsEditMenu;			// can be nullptr; not owned

private:

	void	RebuildTable(const bool maintainScroll = false);
	void	FilterFile(const JIndex fileIndex);
	void	AdjustColWidths();

	bool	ClosestMatch(const JString& prefixStr, JIndex* index) const;
//	bool	FileNameToFileIndex(const JString& name, JIndex* index) const;
	bool	MainResolveFullName(const JIndex rowIndex, const JIndex fileIndex,
								JString* name);
	bool	IsInactive(const JString& fullName) const;
	JIndex	UpdateDrawIndex(const JIndex firstIndex, const JString& fileName) const;

	void	UpdateEditMenu();
	void	HandleEditMenu(const JIndex index);
	void	CopySelectedFileNames() const;

	class PrefixMatch : public JElementComparison<VisInfo>
	{
	public:

		PrefixMatch(const JString& prefix, const JPtrArray<JString>& fileList);
		virtual ~PrefixMatch();
		virtual JListT::CompareResult			Compare(const VisInfo&, const VisInfo&) const;
		virtual JElementComparison<VisInfo>*	Copy() const;

	private:

		const JString&				itsPrefix;
		const JPtrArray<JString>&	itsFileList;
	};

public:

	// JBroadcaster messages

	static const JUtf8Byte* kProcessSelection;

	class ProcessSelection : public JBroadcaster::Message
		{
		public:

			ProcessSelection()
				:
				JBroadcaster::Message(kProcessSelection)
			{ };
		};
};


/******************************************************************************
 Accepting file drops

 ******************************************************************************/

inline bool
JXFileListTable::WillAcceptFileDrop()
	const
{
	return itsAcceptFileDropFlag;
}

inline void
JXFileListTable::ShouldAcceptFileDrop
	(
	const bool accept
	)
{
	itsAcceptFileDropFlag = accept;
}

/******************************************************************************
 Backspace action

 ******************************************************************************/

inline bool
JXFileListTable::BackspaceWillRemoveSelectedFiles()
	const
{
	return itsBSRemoveSelFlag;
}

inline void
JXFileListTable::BackspaceShouldRemoveSelectedFiles
	(
	const bool remove
	)
{
	itsBSRemoveSelFlag = remove;
}

/******************************************************************************
 GetFullNameList

	This is guaranteed to be sorted using JCompareStringsCaseSensitive().
	If you passed in relative paths, however, these may still be there.

 ******************************************************************************/

inline const JPtrArray<JString>&
JXFileListTable::GetFullNameList()
	const
{
	return *itsFileList;
}

/******************************************************************************
 GetFullNameDataList (protected)

	Derived classes can listen to this object and keep an array of extra
	data in sync with our information.

 ******************************************************************************/

inline JCollection*
JXFileListTable::GetFullNameDataList()
	const
{
	return itsFileList;
}

/******************************************************************************
 GetEditMenuProvider

 ******************************************************************************/

inline bool
JXFileListTable::GetEditMenuProvider
	(
	JXTEBase** te
	)
	const
{
	*te = itsEditMenuProvider;
	return itsEditMenuProvider != nullptr;
}

/******************************************************************************
 RowIndexToFileIndex

	Get the index into GetFullNameList() for the file in the specified row.

 ******************************************************************************/

inline JIndex
JXFileListTable::RowIndexToFileIndex
	(
	const JIndex rowIndex
	)
	const
{
	return (itsVisibleList->GetElement(rowIndex)).fileIndex;
}

#endif
