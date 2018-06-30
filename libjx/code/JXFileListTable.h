/******************************************************************************
 JXFileListTable.h

	Interface for the JXFileListTable class

	Copyright (C) 1998-99 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXFileListTable
#define _H_JXFileListTable

#include "JXTable.h"

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

	JBoolean	AddFile(const JString& fullName, JIndex* fullNameIndex = nullptr);
	void		RemoveFile(const JString& fullName);
	void		RemoveFiles(const JPtrArray<JString>& fileList);
	void		RemoveSelectedFiles();
	void		RemoveAllFiles();
	JBoolean	GetFullName(const JIndex rowIndex, JString* fullName) const;
//	JBoolean	GetFullName(const JString& fileName, JString* fullName) const;

	const JPtrArray<JString>&	GetFullNameList() const;
	JIndex		RowIndexToFileIndex(const JIndex rowIndex) const;

	JBoolean	GetFilterRegex(JString* regexStr) const;
	JError		SetFilterRegex(const JString& regexStr);
	void		ClearFilterRegex();

	JBoolean	HasSelection() const;
	JBoolean	GetSelection(JPtrArray<JString>* fileList) const;
	void		SelectSingleEntry(const JIndex index, const JBoolean scroll = kJTrue);
	void		SelectAll();
	void		ClearSelection();

	void		ShowSelectedFileLocations() const;

	JBoolean	GetEditMenuProvider(JXTEBase** te) const;
	void		SetEditMenuProvider(JXTEBase* te);

	JBoolean	WillAcceptFileDrop() const;
	void		ShouldAcceptFileDrop(const JBoolean accept = kJTrue);

	JBoolean	BackspaceWillRemoveSelectedFiles() const;
	void		BackspaceShouldRemoveSelectedFiles(const JBoolean remove = kJTrue);

	virtual void	HandleKeyPress(const JUtf8Character& c, const int keySym,
								   const JXKeyModifiers& modifiers) override;
	void			ClearIncrementalSearchBuffer();

protected:

	JCollection*	GetFullNameDataList() const;
	JString			GetFileName(const JIndex rowIndex) const;

	virtual JBoolean	ResolveFullName(JString* name);

	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers) override;

	virtual void	HandleFocusEvent() override;

	virtual void	GetSelectionData(JXSelectionData* data,
									 const JString& id) override;
	virtual Atom	GetDNDAction(const JXContainer* target,
								 const JXButtonStates& buttonStates,
								 const JXKeyModifiers& modifiers) override;
	virtual void	HandleDNDResponse(const JXContainer* target,
									  const JBoolean dropAccepted, const Atom action) override;

	virtual JBoolean	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
									   const JPoint& pt, const Time time,
									   const JXWidget* source) override;
	virtual void		HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
									  const Atom action, const Time time,
									  const JXWidget* source) override;

	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;
	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

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

	JBoolean			itsAcceptFileDropFlag;	// kJTrue => accept drop of url/url
	JBoolean			itsBSRemoveSelFlag;		// kJTrue => backspace removes selected files
	JSize				itsMaxStringWidth;
	JXImage*			itsFileIcon;
	JString				itsKeyBuffer;

	// dragging

	DragType			itsDragType;
	JPoint				itsMouseDownPt;

	// edit menu (copy, select all)

	JXTEBase*			itsEditMenuProvider;	// can be nullptr; not owned
	JXTextMenu*			itsEditMenu;			// can be nullptr; not owned

private:

	void	RebuildTable(const JBoolean maintainScroll = kJFalse);
	void	FilterFile(const JIndex fileIndex);
	void	AdjustColWidths();

	JBoolean	ClosestMatch(const JString& prefixStr, JIndex* index) const;
//	JBoolean	FileNameToFileIndex(const JString& name, JIndex* index) const;
	JBoolean	MainResolveFullName(const JIndex rowIndex, const JIndex fileIndex,
									JString* name);
	JBoolean	IsInactive(const JString& fullName) const;
	JIndex		UpdateDrawIndex(const JIndex firstIndex, const JString& fileName) const;

	void	UpdateEditMenu();
	void	HandleEditMenu(const JIndex index);
	void	CopySelectedFileNames() const;

	class PrefixMatch : public JElementComparison<VisInfo>
	{
	public:

		PrefixMatch(const JString& prefix, const JPtrArray<JString>& fileList);
		virtual ~PrefixMatch();
		virtual JListT::CompareResult		Compare(const VisInfo&, const VisInfo&) const;
		virtual JElementComparison<VisInfo>*	Copy() const;

	private:

		const JString&				itsPrefix;
		const JPtrArray<JString>&	itsFileList;
	};

	// not allowed

	JXFileListTable(const JXFileListTable& source);
	const JXFileListTable& operator=(const JXFileListTable& source);

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

inline JBoolean
JXFileListTable::WillAcceptFileDrop()
	const
{
	return itsAcceptFileDropFlag;
}

inline void
JXFileListTable::ShouldAcceptFileDrop
	(
	const JBoolean accept
	)
{
	itsAcceptFileDropFlag = accept;
}

/******************************************************************************
 Backspace action

 ******************************************************************************/

inline JBoolean
JXFileListTable::BackspaceWillRemoveSelectedFiles()
	const
{
	return itsBSRemoveSelFlag;
}

inline void
JXFileListTable::BackspaceShouldRemoveSelectedFiles
	(
	const JBoolean remove
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

inline JBoolean
JXFileListTable::GetEditMenuProvider
	(
	JXTEBase** te
	)
	const
{
	*te = itsEditMenuProvider;
	return JI2B( itsEditMenuProvider != nullptr );
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
