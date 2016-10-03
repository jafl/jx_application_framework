/******************************************************************************
 GNBTreeWidgetBase.h

	Interface for the GNBTreeWidgetBase class

	Copyright (C) 1997 by Glenn W. Bach.  All rights reserved.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#ifndef _H_GNBTreeWidgetBase
#define _H_GNBTreeWidgetBase

#include <JXNamedTreeListWidget.h>

#include <JXCursor.h>

#include <JPoint.h>
#include <JFileArray.h>

class JXMenuBar;
class JTreeList;
class JTree;
class JXTextMenu;
class JNamedTreeNode;
class JXImage;
class JXTreeTableInput;
class GNBBaseNode;
class GNBNoteDir;
class GNBTreeDir;

class GNBTreeWidgetBase : public JXNamedTreeListWidget
{
public:

	GNBTreeWidgetBase(const JIndex prefsIndex, GNBTreeDir* dir,
		JTree* tree, JNamedTreeList* treeList,
		JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
		const HSizingOption hSizing, const VSizingOption vSizing,
		const JCoordinate x, const JCoordinate y,
		const JCoordinate w, const JCoordinate h);
	virtual ~GNBTreeWidgetBase();

	virtual void HandleKeyPress(const int key,
								const JXKeyModifiers&   modifiers);
	void		 Save();
	void		 SelectedFlushed();

	virtual JBoolean	WantsInputFieldKey(const int key, const JXKeyModifiers& modifiers) const;

protected:

	enum SelectionType
	{
		kNoneSelected = 1,
		kToDosSelected, //kSinglesSelected,
		kNotesSelected, //kParentsSelected,
		kItemsSelected,
		kCategoriesSelected
	};

protected:

	virtual void Draw(JXWindowPainter& p, const JRect& rect);
	virtual void Receive(JBroadcaster* sender, const Message& message);
	virtual void HandleMouseDown(const JPoint& pt, const JXMouseButton button,
								const JSize clickCount,
								const JXButtonStates& buttonStates,
								const JXKeyModifiers& modifiers);
	virtual void HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
								const JXKeyModifiers& modifiers);
	virtual void HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								const JXButtonStates& buttonStates,
								const JXKeyModifiers& modifiers);

	virtual void	HandleDNDResponse(const JXContainer* target,
									  const JBoolean dropAccepted, const Atom action);
	virtual void 	HandleDNDHere(const JPoint& pt, const JXWidget* source);
	virtual void 	HandleDNDLeave();
	virtual void 	HandleDNDDrop(	const JPoint& pt, const JArray<Atom>& typeList,
									const Atom 	action, const Time time,
									const JXWidget* source);
	virtual Atom	 GetDNDAction(const JXContainer* target,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers);
	virtual void	 GetDNDAskActions(const JXButtonStates& buttonStates,
									  const JXKeyModifiers& modifiers,
									  JArray<Atom>* askActionList,
									  JPtrArray<JString>* askDescriptionList);
	virtual JBoolean GetImage(const JIndex index, const JXImage** image) const;
	virtual JBoolean WillAcceptDrop(const JArray<Atom>& typeList,
									Atom* action,const Time time,
									const JXWidget*		source);
	virtual JBoolean ExtractInputData(const JPoint& cell);

	virtual void 	 DoubleClickIcon(const JIndex index, const JXKeyModifiers& modifiers);

	void			 BroadcastDataChanged();
	SelectionType	 GetSelectionType();

	void			 ReadSetup();
	virtual JBoolean IsSelectable(const JPoint& cell,
								  const JBoolean forExtend) const;

	void	HandleReturn();
	void	PlaceNewNode(JNamedTreeNode* parent, GNBBaseNode* child, JTreeNode* jsibling,
						 const JBoolean after, const JBoolean startEditing = kJFalse);

	void	ExportSelected();
	void	PrintSelected();
	void	Print();

	void	MarkSelectedAsDone(const JBoolean done = kJTrue);

	GNBTreeDir*	GetDir();

private:

	enum DragType
	{
		kDraggingNotes = 1,
		kDraggingFoundNotes,
		kDraggingMail
	};

private:

	GNBTreeDir*		itsDir;
	JTree* 			itsTree;				// We don't own this.
	JPoint			itsDownPt;
	JBoolean		itsDownInCell;
	JIndex			itsCurrentDndHereIndex;
	Atom 			itsMessageXAtom;
	Atom 			itsNoteXAtom;

	JXImage*			itsNoteIcon;
	JXImage*			itsCategoryIcon;
	JXImage*			itsSelectedCategoryIcon;
	JXImage*			itsToDoDoneIcon;
	JXImage*			itsToDoNotDoneIcon;
	JXImage*			itsToDoNoteDoneIcon;
	JXImage*			itsToDoNoteNotDoneIcon;

	JBoolean			itsOutsideRoot;
	JBoolean			itsWaitingToEdit;
	JCursorIndex 		itsDNDCursor;

	JXTreeTableInput*	itsStringInputField;

	DragType			itsDragType;

	SelectionType		itsCurrentSelectionType;
	JSize				itsSelectionDepth;
	JBoolean			itsDNDIsValid;

	JIndex				itsPrefsIndex;
	JBoolean			itsIgnoreSelChangesFlag;

private:

	void		GetSelectedNodes(JPtrArray<JTreeNode>* nodes);
	void		ArrangeDroppedNodes(JPtrArray<JTreeNode>& nodes, const JIndex index, const JBoolean external = kJFalse);
	void		CleanSelection();
	JIndex		ClosestSelection(const JPoint& pt, const JIndex index, const JXWidget* source);
	void		AdjustSelectionType();
	GNBNoteDir* MergeAppended();
	void		MarkItemDone(const JPoint& cell, const JBoolean done);

	// not allowed

	GNBTreeWidgetBase(const GNBTreeWidgetBase& source);
	const GNBTreeWidgetBase& operator=(const GNBTreeWidgetBase& source);

public:

	static const JCharacter* kDataChanged;

	class DataChanged: public JBroadcaster::Message
		{
		public:

			DataChanged()
				:
				JBroadcaster::Message(kDataChanged)
				{ };
		};
};

/******************************************************************************
 GetSekectionType

 ******************************************************************************/

inline GNBTreeWidgetBase::SelectionType
GNBTreeWidgetBase::GetSelectionType()
{
	return itsCurrentSelectionType;
}

/******************************************************************************


 ******************************************************************************/

inline GNBTreeDir*
GNBTreeWidgetBase::GetDir()
{
	return itsDir;
}


#endif
