/******************************************************************************
 SyGFileTreeTable.h

	Copyright © 1999 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_SyGFileTreeTable
#define _H_SyGFileTreeTable

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXNamedTreeListWidget.h>
#include "SyGFileTreeNode.h"
#include "SyGColUtils.h"
#include <JXCursor.h>

class JProcess;
class JXMenuBar;
class JXTextMenu;
class JXFSDirMenu;
class JXToolBar;
class JXRadioGroupDialog;
class JXGetStringDialog;
class JXTimerTask;
class JXWindowIcon;
class SyGTreeSet;
class SyGFileTreeList;
class SyGFileTree;
class SyGTrashButton;
class SyGBeginEditingTask;
class SyGNewGitRemoteDialog;

class SyGFileTreeTable : public JXNamedTreeListWidget
{
	friend class SyGBeginEditingTask;

public:

	SyGFileTreeTable(JXMenuBar* menuBar,
					SyGFileTree* fileTree, SyGFileTreeList* treeList,
					SyGTreeSet* treeSet, SyGTrashButton* trashButton,
					JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	virtual ~SyGFileTreeTable();

	SyGFileTree*		GetFileTree() const;
	SyGFileTreeList*	GetFileTreeList() const;

	void	CreateNewDirectory();
	void	CreateNewTextFile();
	void	DuplicateSelected();
	void	DeleteSelected();
	void	MakeLinks();
	void	FindOriginals();

	void	ViewManPage();
	void	OpenTerminal();
	void	RunCommand();
	void	FormatDisk();

	void	UpdateDisplay(const JBoolean force = kJFalse);
	void	SetWindowIcon();
	void	GoUp(const JBoolean sameWindow);
	void	GoTo(const JString& path, const JBoolean sameWindow);

	JBoolean	SelectName(const JCharacter* name, const SyGFileTreeNode* parent,
						   JPoint* cell,
						   const JBoolean updateContent = kJTrue,
						   const JBoolean updateView = kJTrue);
	JBoolean	SelectName(const JPtrArray<JString>& pathList,
						   const JCharacter* name, JPoint* cell,
						   const JBoolean clearSelection = kJTrue,
						   const JBoolean updateContent = kJTrue);

	void	LoadToolBarDefaults(JXToolBar* toolBar);

	void	LoadPrefs();
	void	LoadPrefs(istream& is, const JFileVersion vers);
	void	SavePrefs(ostream& os);
	void	SavePrefsAsDefault();

	void	RestoreDirState(istream& is, const JFileVersion vers);
	void	SaveDirState(ostream& os);

	JString		GetColTitle(const JIndex index) const;
	JCoordinate	GetBufferWidth(const JIndex index) const;
	JBoolean	IsCurrentCol(const JIndex index) const;
	void		SetCurrentColIndex(const JIndex index);
	void		SetCurrentColType(const GFMColType type);

	virtual JBoolean	IsEditable(const JPoint& cell) const;
	virtual void		HandleKeyPress(const int key, const JXKeyModifiers& modifiers);
	virtual void		HandleShortcut(const int key, const JXKeyModifiers& modifiers);

	static Atom	GetDNDAction(const JXWidget* source, const JCharacter* sourcePath,
							 const JXContainer* target, const JXKeyModifiers& modifiers);

protected:

	virtual void		AdjustToTree();
	virtual JSize		GetMinCellWidth(const JPoint& cell) const;
	virtual void		TableDrawCell(JPainter &p, const JPoint& cell, const JRect& rect);
	virtual JBoolean	GetImage(const JIndex index, const JXImage** image) const;

	virtual void	HandleMouseHere(const JPoint& pt, const JXKeyModifiers& modifiers);
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
									   const JPoint& pt, const Time time,
									   const JXWidget* source);
	virtual void		HandleDNDEnter();
	virtual void		HandleDNDHere(const JPoint& pt, const JXWidget* source);
	virtual void		HandleDNDLeave();
	virtual void		HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
									  const Atom action, const Time time,
									  const JXWidget* source);

	virtual void		GetSelectionData(JXSelectionData* data,
										 const JCharacter* id);
	virtual Atom		GetDNDAction(const JXContainer* target,
									  const JXButtonStates& buttonStates,
									  const JXKeyModifiers& modifiers);
	virtual void		GetDNDAskActions(const JXButtonStates& buttonStates,
									  const JXKeyModifiers& modifiers,
									  JArray<Atom>* askActionList,
									  JPtrArray<JString>* askDescriptionList);
	virtual void		HandleDNDResponse(const JXContainer* target,
									  const JBoolean dropAccepted, const Atom action);

	virtual JXInputField*
		CreateTreeListInput(const JPoint& cell, JXContainer* enclosure,
							const HSizingOption hSizing, const VSizingOption vSizing,
							const JCoordinate x, const JCoordinate y,
							const JCoordinate w, const JCoordinate h);

	virtual JBoolean	ExtractInputData(const JPoint& cell);

	virtual void	Receive(JBroadcaster* sender, const Message& message);
	virtual void	ReceiveWithFeedback(JBroadcaster* sender, Message* message);

private:

	SyGFileTree*		itsFileTree;
	SyGFileTreeList*	itsFileTreeList;	// owned by base class
	SyGTreeSet*			itsTreeSet;
	SyGTrashButton*		itsTrashButton;
	JXTimerTask*		itsUpdateTask;
	JFSFileTreeNodeBase*itsUpdateNode;

	JBoolean			itsVisibleCols[5];
	GFMColType			itsCurrentColType;
	JColorIndex			itsAltRowColor;
	JCoordinate			itsPermCharWidth;
	JBoolean			itsIgnoreExecPermFlag;

	JXMenuBar*			itsMenuBar;
	JXTextMenu*			itsFileMenu;
	JXFSDirMenu*		itsRecentFilesMenu;
	JXTextMenu*			itsEditMenu;				// not owned
	JIndex				itsCopyPathCmdIndex;
	JXTextMenu*			itsGitMenu;
	JXTextMenu*			itsGitLocalBranchMenu;
	JXTextMenu*			itsGitPullSourceMenu;
	JXTextMenu*			itsGitPushDestMenu;
	JXTextMenu*			itsGitMergeBranchMenu;
	JXTextMenu*			itsGitRemoteBranchMenu;
	JXTextMenu*			itsGitRemoveBranchMenu;
	JXTextMenu*			itsGitRemoveRemoteMenu;
	JXTextMenu*			itsGitPruneRemoteMenu;
	JXTextMenu* 		itsViewMenu;
	JXTextMenu*			itsShortcutMenu;

	JXTextMenu*			itsContextMenu;

	JXRadioGroupDialog*	itsChooseDiskFormatDialog;	// NULL unless asking user
	JProcess*			itsFormatProcess;			// NULL unless formatting

	JXGetStringDialog*	itsCreateGitBranchDialog;	// NULL unless creating branch
	JXGetStringDialog*	itsFetchGitBranchDialog;	// NULL unless creating branch
	JString				itsFetchGitBranch;
	JXGetStringDialog*	itsCommitGitBranchDialog;	// NULL unless committing branch
	JProcess*			itsGitProcess;				// NULL unless waiting for git

	SyGNewGitRemoteDialog*	itsAddGitRemoteDialog;

	// Drag-and-Drop

	JXWindowIcon*	itsIconWidget;			// not owned
	JIndex			itsWindowIconType;

	enum CursorType
	{
		kDNDFileCursor,
		kDNDDirCursor,
		kDNDMixedCursor
	};

	CursorType	itsDNDCursorType;

	// used during dragging

	JPoint		itsStartPt;
	JPoint		itsPrevPt;
	JSize		itsLastClickCount;
	JBoolean	itsWaitingForDragFlag;
	JBoolean	itsClearIfNotDNDFlag;

	// delayed editing

	JBoolean				itsWaitingToEditFlag;
	SyGBeginEditingTask*	itsEditTask;	// NULL unless waiting to edit
	JPoint					itsEditCell;
	SyGFileTreeNode*		itsSortNode;	// sort when mouse released

	// set by GetGitBranches()

	JString	itsCurrentGitBranch;

private:

	void	UpdateInfo();
	void	UpdateMenus();

	JString	GetCellString(const JPoint& cell) const;
	void	StartDragRect(const JPoint& pt, const JXMouseButton button,
						  const JXKeyModifiers& modifiers);

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	OpenSelection(const JBoolean alternate, const JBoolean alwaysRunCmd,
						  const JBoolean iconifyAfter, const JBoolean closeAfter);

	void	MakeLinkToFile(const JString& src, const SyGFileTreeNode* parentNode,
						   const JBoolean allowRelative);
	JString	GetCommandPath() const;

	void	ChangeExecPermission(const JBoolean canExec);

	void	UpdateEditMenu();
	void	HandleEditMenu(const JIndex item);

	void	CopySelectedFileNames(const JBoolean useFullPath) const;

	void	InitGitBranchMenus();
	void	UpdateGitMenus(const JBoolean shortcut);
	void	UpdateGitLocalBranchMenu();
	void	HandleGitMenu(const JIndex index);

	JBoolean	GetGitBranches(const JCharacter* cmd,
							   JPtrArray<JString>* branchList, JIndex* currentIndex,
							   JPtrArray<JString>* repoList);
	void		CreateGitBranch(const JCharacter* branchName);
	void		CommitGitBranch(const JCharacter* msg);
	void		RevertGitBranch();

	void	SwitchToGitBranch(const JString& branch);
	void	MergeFromGitBranch(const JString& branch);
	void	FetchRemoteGitBranch1(const JString& branch);
	void	FetchRemoteGitBranch2(const JString& name);
	void	PullBranch(const JString& repo);
	void	PushBranch(const JString& repo);
	void	RemoveGitBranch(const JString& branch);
	void	AddGitRemote(const JString& repoURL, const JString& name);
	void	RemoveGitRemote(const JString& repo);
	void	PruneRemoteGitBranches(const JString& name);

	void	UpdateViewMenu();
	void	HandleViewMenu(const JIndex index);

	void	InsertFMTreeCol(const GFMColType type);
	void	RemoveFMTreeCol(const GFMColType type);

	void	SetPreferences(const JBoolean prefs[]);
	void	GetPreferences(JBoolean prefs[]);
	void	TogglePref(const JIndex i);

	void	UpdateShortcutMenu();
	void	HandleShortcutMenu(const JIndex index);

	void	UpdateContextMenu();
	void	HandleContextMenu(const JIndex index);

	void		ChooseDNDCursors();
	JBoolean	GetTrueDropAction(Atom* action);

	JIndex		GetNearestDirIndex(const JIndex index, const JBoolean requireWritable);
	JBoolean	WarnForDelete() const;

	// not allowed

	SyGFileTreeTable(const SyGFileTreeTable& source);
	const SyGFileTreeTable& operator=(const SyGFileTreeTable& source);
};


/******************************************************************************
 GetFileTree

 ******************************************************************************/

inline SyGFileTree*
SyGFileTreeTable::GetFileTree()
	const
{
	return itsFileTree;
}

/******************************************************************************
 GetFileTreeList

 ******************************************************************************/

inline SyGFileTreeList*
SyGFileTreeTable::GetFileTreeList()
	const
{
	return itsFileTreeList;
}

#endif
