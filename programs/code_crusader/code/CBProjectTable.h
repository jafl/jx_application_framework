/******************************************************************************
 CBProjectTable.h

	Copyright © 1999 by John Lindal.  All rights reserved.

 ******************************************************************************/

#ifndef _H_CBProjectTable
#define _H_CBProjectTable

#include <JXNamedTreeListWidget.h>
#include "CBRelPathCSF.h"		// need definition of PathType

class JXImage;
class JXMenuBar;
class JXTextMenu;
class JXImageButton;
class JXGetStringDialog;
class CBProjectDocument;
class CBProjectNode;
class CBGroupNode;

class CBProjectTable : public JXNamedTreeListWidget
{
public:

	enum	// selection depth
	{
		kEitherDepth = 0,
		kGroupDepth  = 1,
		kFileDepth   = 2
	};

	enum SelType
	{
		kNoSelection,
		kFileSelection,
		kGroupSelection
	};

	// Do not change these values once they are assigned because
	// they are stored in the prefs file.

	enum DropFileAction
	{
		kAskPathType     = 0,
		kAbsolutePath    = CBRelPathCSF::kAbsolutePath,
		kProjectRelative = CBRelPathCSF::kProjectRelative,
		kHomeRelative    = CBRelPathCSF::kHomeRelative
	};

	// action to take after file path/name has been edited

	enum InputAction
	{
		kRelink,
		kRename
	};

public:

	CBProjectTable(CBProjectDocument* doc, JXMenuBar* menuBar,
				   JNamedTreeList* treeList,
				   JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	virtual ~CBProjectTable();

	JBoolean	NewGroup(CBGroupNode** returnNode = NULL);
	void		AddDirectoryTree();
	void		AddDirectoryTree(const JCharacter* fullPath,
								 const CBRelPathCSF::PathType pathType);
	void		AddFiles();
	JBoolean	AddFiles(const JPtrArray<JString>& fullNameList,
						 const CBRelPathCSF::PathType pathType,
						 const JBoolean updateProject = kJTrue,
						 const JBoolean silent = kJFalse);

	JBoolean	HasSelection() const;
	JBoolean	GetSelectionType(SelType* type, JBoolean* single, JIndex* index) const;
	void		SelectAll();
	void		ClearSelection();
	void		SelectFileNodes(const JPtrArray<JTreeNode>& nodeList);
	void		OpenSelection();
	void		OpenComplementFiles();
	void		RemoveSelection();
	void		PlainDiffSelection();
	void		VCSDiffSelection();
	void		ShowSelectedFileLocations();
	void		GetSelectedFileNames(JPtrArray<JString>* list) const;
	void		EditFilePath();
	void		EditSubprojectConfig();

	CBProjectNode*			GetProjectNode(const JIndex index);
	const CBProjectNode*	GetProjectNode(const JIndex index) const;

	void	ReadSetup(istream& projInput, const JFileVersion projVers,
					  istream* setInput, const JFileVersion setVers);
	void	WriteSetup(ostream& projOutput, ostream* setOutput) const;

	static DropFileAction	GetDropFileAction();
	static void				SetDropFileAction(const DropFileAction action);

	virtual void		HandleKeyPress(const int key, const JXKeyModifiers& modifiers);
	virtual JBoolean	IsSelectable(const JPoint& cell,
									 const JBoolean forExtend) const;

	void	SetInputAction(const InputAction action);

protected:

	JSize		GetDepth(const JIndex index) const;

	virtual JBoolean	GetImage(const JIndex index, const JXImage** image) const;
	virtual void		AdjustToTree();

	virtual void	Draw(JXWindowPainter& p, const JRect& rect);

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers);

	virtual void	DNDInit(const JPoint& pt, const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers);
	virtual void	GetSelectionData(JXSelectionData* data,
									 const JCharacter* id);
	virtual Atom	GetDNDAction(const JXContainer* target,
								 const JXButtonStates& buttonStates,
								 const JXKeyModifiers& modifiers);
	virtual void	HandleDNDResponse(const JXContainer* target,
									  const JBoolean dropAccepted, const Atom action);

	virtual JBoolean	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
									   const JPoint& pt, const Time time,
									   const JXWidget* source);
	virtual void		HandleDNDEnter();
	virtual void		HandleDNDHere(const JPoint& pt, const JXWidget* source);
	virtual void		HandleDNDLeave();
	virtual void		HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
									  const Atom action, const Time time,
									  const JXWidget* source);

	virtual JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h);
	virtual JBoolean		ExtractInputData(const JPoint& cell);
	virtual void			PrepareDeleteXInputField();

	virtual JXInputField*
		CreateTreeListInput(const JPoint& cell, JXContainer* enclosure,
							const HSizingOption hSizing, const VSizingOption vSizing,
							const JCoordinate x, const JCoordinate y,
							const JCoordinate w, const JCoordinate h);

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	enum DragType
	{
		kInvalidDrag,
		kSelectDrag,
		kWaitForDNDDrag,
		kOpenNodeDrag
	};

	enum DNDAction
	{
		kDNDInvalid,
		kDNDFileBombsight,		// line at bottom of itsDNDCell
		kDNDAppendToGroup,		// frame itsDNDCell
		kDNDNewGroup,			// line at bottom of itsDNDCell; table may be empty
		kDNDGroupBombsight		// line at top of itsDNDCell; may be beyond bottom of table
	};

private:

	CBProjectDocument*	itsDoc;				// not owned
	JXTextMenu*			itsEditMenu;		// not owned
	JXTextMenu*			itsContextMenu;		// not owned
	JXImageButton*		itsCSFButton;		// NULL unless editing CBFileNodeBase

	JSize		itsSelDepth;				// depth of items that can be selected
	JBoolean	itsIgnoreSelChangesFlag;	// kJTrue while cleaning selection
	JBoolean	itsMarkWritableFlag;		// kJTrue if mark writable files (e.g. CVS)
	JBoolean	itsLockedSelDepthFlag;		// kJTrue if don't change the selection depth

	JXGetStringDialog*		itsAddFilesFilterDialog;
	JString					itsAddPath;
	CBRelPathCSF::PathType	itsAddPathType;

	// used after editing

	InputAction	itsInputAction;

	// used while dragging

	DragType	itsDragType;
	JPoint		itsStartPt;

	// used during DND

	DNDAction	itsDNDAction;
	JPoint		itsDNDCell;
	JString*	itsDNDBuffer;

	static DropFileAction	itsDropFileAction;

private:

	void	CleanSelection();
	void	UpdateSelDepth();

	void	UpdateEditMenu();
	void	HandleEditMenu(const JIndex index);
	void	CopySelectedNames() const;

	void	CreateContextMenu();
	void	UpdateContextMenu();
	void	HandleContextMenu(const JIndex index);

	void	CopyFileToDNDList(const JTreeNode* node, JPtrArray<JString>* fileList) const;

	void	InsertFileSelectionAfter(JTreeNode* after);
	void	AppendFileSelectionToGroup(JTreeNode* group);
	void	InsertGroupSelectionBefore(JTreeNode* before);
	void	InsertExtDroppedFiles(const JPtrArray<JString>& fileNameList,
								  const CBRelPathCSF::PathType pathType);

	void	AddDirectoryTree(const JCharacter* fullPath, const JCharacter* filterStr,
							 const CBRelPathCSF::PathType pathType);
	void	AddDirectoryTree(const JCharacter* fullPath, const JCharacter* relPath,
							 const JCharacter* filterStr,
							 const CBRelPathCSF::PathType pathType,
							 JBoolean* changed);

	// not allowed

	CBProjectTable(const CBProjectTable& source);
	const CBProjectTable& operator=(const CBProjectTable& source);
};

istream& operator>>(istream& input, CBProjectTable::DropFileAction& action);
ostream& operator<<(ostream& output, const CBProjectTable::DropFileAction action);


/******************************************************************************
 Action when files are dropped from external source (static)

 ******************************************************************************/

inline CBProjectTable::DropFileAction
CBProjectTable::GetDropFileAction()
{
	return itsDropFileAction;
}

inline void
CBProjectTable::SetDropFileAction
	(
	const DropFileAction action
	)
{
	itsDropFileAction = action;
}

/******************************************************************************
 SetInputAction

 ******************************************************************************/

inline void
CBProjectTable::SetInputAction
	(
	const InputAction action
	)
{
	itsInputAction = action;
}

#endif
