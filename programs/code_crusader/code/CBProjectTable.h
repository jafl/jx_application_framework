/******************************************************************************
 CBProjectTable.h

	Copyright © 1999 by John Lindal.

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

	bool	NewGroup(CBGroupNode** returnNode = nullptr);
	void		AddDirectoryTree();
	void		AddDirectoryTree(const JString& fullPath,
								 const CBRelPathCSF::PathType pathType);
	void		AddFiles();
	bool	AddFiles(const JPtrArray<JString>& fullNameList,
						 const CBRelPathCSF::PathType pathType,
						 const bool updateProject = true,
						 const bool silent = false);

	bool	HasSelection() const;
	bool	GetSelectionType(SelType* type, bool* single, JIndex* index) const;
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

	void	ReadSetup(std::istream& projInput, const JFileVersion projVers,
					  std::istream* setInput, const JFileVersion setVers);
	void	WriteSetup(std::ostream& projOutput, std::ostream* setOutput) const;

	static DropFileAction	GetDropFileAction();
	static void				SetDropFileAction(const DropFileAction action);

	virtual void		HandleKeyPress(const JUtf8Character& c,
									   const int keySym, const JXKeyModifiers& modifiers) override;
	virtual bool	IsSelectable(const JPoint& cell,
									 const bool forExtend) const override;

	void	SetInputAction(const InputAction action);

protected:

	JSize		GetDepth(const JIndex index) const;

	virtual bool	GetImage(const JIndex index, const JXImage** image) const override;
	virtual void		AdjustToTree() override;

	virtual void	Draw(JXWindowPainter& p, const JRect& rect) override;

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers) override;

	virtual void	DNDInit(const JPoint& pt, const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;
	virtual void	GetSelectionData(JXSelectionData* data,
									 const JString& id) override;
	virtual Atom	GetDNDAction(const JXContainer* target,
								 const JXButtonStates& buttonStates,
								 const JXKeyModifiers& modifiers) override;
	virtual void	HandleDNDResponse(const JXContainer* target,
									  const bool dropAccepted, const Atom action) override;

	virtual bool	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
									   const JPoint& pt, const Time time,
									   const JXWidget* source) override;
	virtual void		HandleDNDEnter() override;
	virtual void		HandleDNDHere(const JPoint& pt, const JXWidget* source) override;
	virtual void		HandleDNDLeave() override;
	virtual void		HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
									  const Atom action, const Time time,
									  const JXWidget* source) override;

	virtual JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h) override;
	virtual bool		ExtractInputData(const JPoint& cell) override;
	virtual void			PrepareDeleteXInputField() override;

	virtual JXInputField*
		CreateTreeListInput(const JPoint& cell, JXContainer* enclosure,
							const HSizingOption hSizing, const VSizingOption vSizing,
							const JCoordinate x, const JCoordinate y,
							const JCoordinate w, const JCoordinate h) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

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
	JXImageButton*		itsCSFButton;		// nullptr unless editing CBFileNodeBase

	JSize		itsSelDepth;				// depth of items that can be selected
	bool	itsIgnoreSelChangesFlag;	// true while cleaning selection
	bool	itsMarkWritableFlag;		// true if mark writable files (e.g. CVS)
	bool	itsLockedSelDepthFlag;		// true if don't change the selection depth

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

	void	AddDirectoryTree(const JString& fullPath, const JString& filterStr,
							 const CBRelPathCSF::PathType pathType);
	void	AddDirectoryTree(const JString& fullPath, const JString& relPath,
							 const JString& filterStr,
							 const CBRelPathCSF::PathType pathType,
							 bool* changed);

	// not allowed

	CBProjectTable(const CBProjectTable& source);
	const CBProjectTable& operator=(const CBProjectTable& source);
};

std::istream& operator>>(std::istream& input, CBProjectTable::DropFileAction& action);
std::ostream& operator<<(std::ostream& output, const CBProjectTable::DropFileAction action);


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
