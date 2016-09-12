/******************************************************************************
 CMVarTreeWidget.h

	Copyright © 2001 by John Lindal.  All rights reserved.

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
					const JBoolean mainDisplay, JXMenuBar* menuBar,
					JTree* tree, JNamedTreeList* treeList,
					JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	virtual ~CMVarTreeWidget();

	CMVarNode*	NewExpression(const JCharacter* expr = NULL);
	CMVarNode*	DisplayExpression(const JCharacter* expr);
	void		ShowNode(const CMVarNode* node);

	JBoolean	HasSelection() const;
	void		RemoveSelection();

	void	WatchExpression();
	void	WatchLocation();
	void	DisplayAsCString();
	void	Display1DArray();
	void	Plot1DArray();
	void	Display2DArray();
	void	ExamineMemory(const CMMemoryDir::DisplayType type);
	void	ShouldUpdate(const JBoolean update);

	virtual void		HandleKeyPress(const int key, const JXKeyModifiers& modifiers);
	virtual JBoolean	IsEditable(const JPoint& cell) const;

	void	ReadSetup(istream& input, const JFileVersion vers);
	void	WriteSetup(ostream& output) const;

protected:

	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);
	virtual JSize	GetMinCellWidth(const JPoint& cell) const;

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers);

	virtual JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h);
	virtual JBoolean		ExtractInputData(const JPoint& cell);

	virtual void	Receive(JBroadcaster* sender, const Message& message);
	virtual void	ReceiveGoingAway(JBroadcaster* sender);

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
	const JBoolean		itsIsMainDisplayFlag;
	JBoolean			itsWaitingForReloadFlag;
	DragType			itsDragType;

	JXTextMenu*	itsEditMenu;			// not owned
	JIndex		itsCopyPathCmdIndex;
	JIndex		itsCopyValueCmdIndex;

	JXTextMenu*	itsBaseMenu;
	JXTextMenu*	itsBasePopupMenu;

	// used while editing

	JBoolean	itsEditingNewNodeFlag;
	JString		itsOrigEditValue;

private:

	void	FlushOldData();

	void	UpdateEditMenu();
	void	HandleEditMenu(const JIndex item);

	void	CopySelectedItems(const JBoolean useFullName,
							  const JBoolean copyValue) const;

	void	UpdateBaseMenu(JXTextMenu* menu);
	void	HandleBaseMenu(const JIndex item);

	// not allowed

	CMVarTreeWidget(const CMVarTreeWidget& source);
	const CMVarTreeWidget& operator=(const CMVarTreeWidget& source);
};

#endif
