/******************************************************************************
 GMessageTable.h

	Interface for the GMessageTable class

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GMessageTable
#define _H_GMessageTable

#include <JXTable.h>
#include <GMessageHeader.h>

class GMessageTableDir;
class GMessageHeaderList;
class GMMailboxData;

class JString;
class JXImage;
class JXTextMenu;
class JXMenuBar;
class JXTimerTask;
class JXFSDirMenu;
class JXScrollbar;

class GMessageTable : public JXTable
{
public:

	GMessageTable(GMessageTableDir* dir,
				JXMenuBar* menuBar,
				JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	virtual ~GMessageTable();

	void	SetData(GMMailboxData* data);
	void	SelectHeader(GMessageHeader* header);
	void	UnSelectHeader(GMessageHeader* header);
	void	Reply(GMessageHeader* header,
					GMessageHeader::ReplyStatus status = GMessageHeader::kReplied,
					const JBoolean useReplyTo = kJTrue,
					const JBoolean useCC = kJFalse,
					const JBoolean fillTo = kJTrue,
					const JBoolean insertText = kJFalse,
					const JBoolean quoteText = kJFalse);
	void	ReplySender(GMessageHeader* header);
	void	ReplyAll(GMessageHeader* header);
	void	Forward(GMessageHeader* header);
	void	Redirect(GMessageHeader* header);

	virtual void HandleKeyPress(const int key,
								const JXKeyModifiers& modifiers);

	void	AbortMessageDND();
	void	ScrollToFirstNew();

	virtual void	HandleShortcut(const int key,					// must call inherited
								   const JXKeyModifiers& modifiers) override;
protected:

	virtual void Draw(JXWindowPainter& p, const JRect& rect);
	virtual void Receive(JBroadcaster* sender, const Message& message);
	virtual void TableDrawCell(JPainter& p, const JPoint& cell,
								const JRect& rect);

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers) override;

	virtual void	GetSelectionData(JXSelectionData* data,
									 const JCharacter* id) override;
	virtual Atom	GetDNDAction(const JXContainer* target,
								 const JXButtonStates& buttonStates,
								 const JXKeyModifiers& modifiers) override;
	virtual void	GetDNDAskActions(const JXButtonStates& buttonStates,
									  const JXKeyModifiers& modifiers,
									  JArray<Atom>* askActionList,
									  JPtrArray<JString>* askDescriptionList) override;

	virtual JBoolean	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
									   const JPoint& pt, const Time time,
									   const JXWidget* source) override;

	virtual void	HandleDNDEnter() override;
	virtual void	HandleDNDHere(const JPoint& pt, const JXWidget* source) override;
	virtual void	HandleDNDLeave() override;
	virtual void	HandleDNDDrop(	const JPoint& pt, const JArray<Atom>& typeList,
									const Atom	action, const Time time,
									const JXWidget* source) override;

private:

	GMessageTableDir*	itsDir;				// We don't own this.
	JSize				itsLineHeight;
	GMMailboxData*		itsData;
	GMessageHeaderList* itsList;
	JXImage*			itsMailIcon;
	JXImage*			itsCheckIcon;
	JXImage*			itsClipIcon;
	JIndex				itsLastSelectedIndex;
	JIndex				itsDNDDropIndex;	// 0 => not doing DND
	Atom				itsMessageXAtom;
	JBoolean			itsDownInCell;
	JPoint				itsDownPt;
	JXTextMenu*			itsMessageMenu;
	JXTimerTask*		itsUnlockTask;
	JBoolean			itsIsWaitingForDeselect;

	JXImage*			itsReplyIcon;
	JXImage*			itsReplySenderIcon;
	JXImage*			itsReplyAllIcon;
	JXImage*			itsForwardIcon;
	JXImage*			itsRedirectIcon;

	JXFSDirMenu*		itsTransferMenu;
	JXFSDirMenu*		itsCopyMenu;
	JXTextMenu*			itsPopupMenu;

	JBoolean			itsHasLockedMBox;

private:

	void	UpdateMessageMenu();
	void	HandleMessageMenu(const JIndex index);
	void	HandleMessageTransfer(const JCharacter* dest, const JBoolean move);

	void	HandleDelete();
	void	HandleUndelete();
	void	HandleMarkRead();
	void	HandleMarkNew();
	void	HandleReplies(const JIndex index);

	void	HandlePopupMenu(const JIndex index);

	void	AddToSelection(const JIndex index);
	void	ExtendSelection(const JIndex index);
	void	ElementSelected(const JIndex index);
	void	SyncWithList();

	// not allowed

	GMessageTable(const GMessageTable& source);
	const GMessageTable& operator=(const GMessageTable& source);

};

#endif
