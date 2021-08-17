/******************************************************************************
 CMVarTreeWidget.h

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#ifndef _H_CMVarTreeWidget
#define _H_CMVarTreeWidget

#include <JXNamedTreeListWidget.h>
#include "CMMemoryDir.h"	// need defn of DisplayType

class JTree;
class JNamedTreeList;
class JXMenuBar;
class JXTextMenu;
class CMCommandDirector;
class CMVarNode;

class CMVarTreeWidget : public JXNamedTreeListWidget
{
public:

	CMVarTreeWidget(CMCommandDirector* dir,
					const bool mainDisplay, JXMenuBar* menuBar,
					JTree* tree, JNamedTreeList* treeList,
					JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	virtual ~CMVarTreeWidget();

	CMVarNode*	NewExpression(const JString& expr = JString::empty);
	CMVarNode*	DisplayExpression(const JString& expr);
	void		ShowNode(const CMVarNode* node);

	bool	HasSelection() const;
	void	RemoveSelection();

	void	WatchExpression();
	void	WatchLocation();
	void	DisplayAsCString();
	void	Display1DArray();
	void	Plot1DArray();
	void	Display2DArray();
	void	ExamineMemory(const CMMemoryDir::DisplayType type);
	void	ShouldUpdate(const bool update);

	virtual void	HandleKeyPress(const JUtf8Character& c, const int keySym,
								   const JXKeyModifiers& modifiers) override;
	virtual bool	IsEditable(const JPoint& cell) const override;

	void	ReadSetup(std::istream& input, const JFileVersion vers);
	void	WriteSetup(std::ostream& output) const;

protected:

	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;
	virtual JSize	GetMinCellWidth(const JPoint& cell) const override;

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers) override;

	virtual JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h) override;
	virtual bool	ExtractInputData(const JPoint& cell) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	enum DragType
	{
		kInvalidDrag,
		kSelectDrag,
		kOpenNodeDrag
	};

private:

	CMCommandDirector*	itsDir;
	JTree*				itsTree;
	const bool			itsIsMainDisplayFlag;
	bool				itsWaitingForReloadFlag;
	DragType			itsDragType;

	JXTextMenu*	itsEditMenu;			// not owned
	JIndex		itsCopyPathCmdIndex;
	JIndex		itsCopyValueCmdIndex;

	JXTextMenu*	itsBaseMenu;
	JXTextMenu*	itsBasePopupMenu;

	// used while editing

	bool	itsEditingNewNodeFlag;
	JString	itsOrigEditValue;

private:

	void	FlushOldData();

	void	UpdateEditMenu();
	void	HandleEditMenu(const JIndex item);

	void	CopySelectedItems(const bool useFullName, const bool copyValue) const;

	void	UpdateBaseMenu(JXTextMenu* menu);
	void	HandleBaseMenu(const JIndex item);

	// not allowed

	CMVarTreeWidget(const CMVarTreeWidget& source);
	const CMVarTreeWidget& operator=(const CMVarTreeWidget& source);
};

#endif
