/******************************************************************************
 JXDNDManager.h

	Copyright (C) 1997-99 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXDNDManager
#define _H_JXDNDManager

#include "jx-af/jx/JXSelectionManager.h"
#include "jx-af/jx/JXButtonStates.h"
#include "jx-af/jx/JXKeyModifiers.h"
#include "jx-af/jx/JXCursor.h"
#include <jx-af/jcore/JRect.h>

class JXDisplay;
class JXContainer;
class JXWidget;
class JXButtonStates;
class JXKeyModifiers;
class JXDNDChooseDropActionDialog;

class JXDNDManager : virtual public JBroadcaster
{
public:

	class TargetFinder;

public:

	JXDNDManager(JXDisplay* display);

	~JXDNDManager();

	Atom			GetDNDSelectionName() const;
	Atom			GetCurrentDNDVersion() const;
	JCursorIndex	GetDefaultDNDCursor(const bool dropAccepted = false,
										const Atom action = None) const;
	JCursorIndex	GetDefaultDNDCopyCursor() const;
	JCursorIndex	GetDefaultDNDMoveCursor() const;
	JCursorIndex	GetDefaultDNDLinkCursor() const;
	JCursorIndex	GetDefaultDNDAskCursor() const;
	JCursorIndex	GetDNDCursor(const bool dropAccepted, const Atom action,
								 const JCursorIndex* cursor) const;

	Atom	GetDNDActionCopyXAtom() const;
	Atom	GetDNDActionMoveXAtom() const;
	Atom	GetDNDActionLinkXAtom() const;
	Atom	GetDNDActionAskXAtom() const;
	Atom	GetDNDActionPrivateXAtom() const;
	Atom	GetDNDActionDirectSaveXAtom() const;

	bool	GetAskActions(JArray<Atom>* actionList,
							  JPtrArray<JString>* descriptionList) const;
	bool	ChooseDropAction(const JArray<Atom>& actionList,
								 const JPtrArray<JString>& descriptionList,
								 Atom* action);

	bool	IsDragging() const;
	Window		GetDraggerWindow() const;
	bool	TargetWillAcceptDrop() const;

	Atom	GetDNDDirectSave0XAtom() const;

	// dragging files

	JCursorIndex	GetDNDFileCursor(const bool dropAccepted = false,
									 const Atom action = None) const;
	JCursorIndex	GetDNDCopyFileCursor() const;
	JCursorIndex	GetDNDMoveFileCursor() const;
	JCursorIndex	GetDNDLinkFileCursor() const;
	JCursorIndex	GetDNDAskFileCursor() const;

	JCursorIndex	GetDNDDirectoryCursor(const bool dropAccepted = false,
										  const Atom action = None) const;
	JCursorIndex	GetDNDCopyDirectoryCursor() const;
	JCursorIndex	GetDNDMoveDirectoryCursor() const;
	JCursorIndex	GetDNDLinkDirectoryCursor() const;
	JCursorIndex	GetDNDAskDirectoryCursor() const;

	JCursorIndex	GetDNDFileAndDirectoryCursor(const bool dropAccepted = false,
												 const Atom action = None) const;
	JCursorIndex	GetDNDCopyFileAndDirectoryCursor() const;
	JCursorIndex	GetDNDMoveFileAndDirectoryCursor() const;
	JCursorIndex	GetDNDLinkFileAndDirectoryCursor() const;
	JCursorIndex	GetDNDAskFileAndDirectoryCursor() const;

	// utility functions for target acquisition

	bool	IsDNDAware(const Window xWindow, Window* proxy, JSize* vers) const;

	// called by JXWidget

	bool	BeginDND(JXWidget* widget, const JPoint& pt,
						 const JXButtonStates& buttonStates,
						 const JXKeyModifiers& modifiers,
						 JXSelectionData* data,
						 TargetFinder* targetFinder);
	void		HandleDND(const JPoint& pt,
						  const JXButtonStates& buttonStates,
						  const JXKeyModifiers& modifiers,
						  const JXMouseButton scrollButton);
	void		FinishDND();

	// called by JXWindow

	void		EnableDND(const Window xWindow) const;
	bool	CancelDND();

	// called by JXDisplay

	bool	HandleClientMessage(const XClientMessageEvent& clientMessage);
	bool	HandleDestroyNotify(const XDestroyWindowEvent& xEvent);

	// called by JXSelectionManager

	bool	IsLastFakePasteTime(const Time time) const;

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;
	void	ReceiveWithFeedback(JBroadcaster* sender, Message* message) override;
	void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	enum
	{
		kCopyCursorIndex = 0,	// values required by GetDNDCursor()
		kMoveCursorIndex = 1,
		kLinkCursorIndex = 2,
		kAskCursorIndex  = 3,

		kCopyFileCursorIndex,
		kMoveFileCursorIndex,
		kLinkFileCursorIndex,
		kAskFileCursorIndex,

		kCopyDirCursorIndex,
		kMoveDirCursorIndex,
		kLinkDirCursorIndex,
		kAskDirCursorIndex,

		kCopyMixedCursorIndex,
		kMoveMixedCursorIndex,
		kLinkMixedCursorIndex,
		kAskMixedCursorIndex,

		kDefDNDCursorCount
	};

public:		// kAtomCount required at global scope

	enum
	{
		kDNDSelectionAtomIndex,

		kDNDProxyAtomIndex,
		kDNDAwareAtomIndex,
		kDNDTypeListAtomIndex,

		kDNDEnterAtomIndex,
		kDNDHereAtomIndex,
		kDNDStatusAtomIndex,
		kDNDLeaveAtomIndex,
		kDNDDropAtomIndex,
		kDNDFinishedAtomIndex,

		kDNDActionCopyAtomIndex,
		kDNDActionMoveAtomIndex,
		kDNDActionLinkAtomIndex,
		kDNDActionAskAtomIndex,
		kDNDActionPrivateAtomIndex,
		kDNDActionDirectSaveAtomIndex,

		kDNDActionListAtomIndex,
		kDNDActionDescriptionAtomIndex,

		kDNDDirectSave0AtomIndex,

		kAtomCount
	};

private:

	JXDisplay*			itsDisplay;					// not owned
	bool				itsIsDraggingFlag;			// true until FinishDND() is called
	JXWidget*			itsDragger;					// widget initiating drag; not owned
	Window				itsDraggerWindow;			// window of itsDragger; not owned
	JArray<Atom>*		itsDraggerTypeList;			// data types supported by itsDragger
	JArray<Atom>*		itsDraggerAskActionList;	// actions accepted by itsDragger
	JPtrArray<JString>*	itsDraggerAskDescripList;	// description of each action
	TargetFinder*		itsTargetFinder;			// nullptr if use default method; not owned

	Atom			itsDNDVersion;			// version being used
	Window			itsMouseWindow;			// window that mouse is in; not owned
	bool			itsMouseWindowIsAware;	// true if itsMouseWindow has XdndAware
	JXContainer*	itsMouseContainer;		// widget that mouse is in; nullptr if inter-app; not owned
	Window			itsMsgWindow;			// window that receives messages (not itsMouseWindow if proxy)

	bool	itsWillAcceptDropFlag;	// true if target will accept drop
	bool	itsWaitForStatusFlag;	// true if waiting for XdndStatus message
	bool	itsSendHereMsgFlag;		// true if need another XdndPosition with itsPrevHandleDNDPt
	bool	itsReceivedStatusFlag;	// true if received any XdndStatus from current target
	bool	itsUseMouseRectFlag;	// true if use itsMouseRectR
	JRect	itsMouseRectR;			// don't send another XdndPosition while inside here

	JPoint	itsPrevMousePt;			// last XdndPosition coordinates (local coords of target)
	Atom	itsPrevHereAction;		// last XdndPosition action
	Atom	itsPrevStatusAction;	// last XdndStatus action

	JPoint			itsPrevHandleDNDPt;		// last values sent to HandleDND()
	Atom			itsPrevHandleDNDAction;
	JXMouseButton	itsPrevHandleDNDScrollButton;
	JXKeyModifiers	itsPrevHandleDNDModifiers;

	JXDNDChooseDropActionDialog*	itsChooseDropActionDialog;
	Atom*							itsUserDropAction;		// nullptr unless waiting for GetDropActionDialog

	bool	itsSentFakePasteFlag;		// true if times are valid
	Time	itsFakeButtonPressTime;
	Time	itsFakeButtonReleaseTime;

	JCursorIndex	itsDefDNDCursor [ kDefDNDCursorCount ];

	Atom	itsAtoms[ kAtomCount ];

private:

	void	InitCursors();

	void	SendDNDEnter(const Window xWindow, const Window msgWindow, JXContainer* widget,
						 const bool isAware, const Atom vers);
	void	SendDNDHere(const JPoint& pt, const Atom action,
						const JXMouseButton scrollButton, const JXKeyModifiers& modifiers);
	void	SendDNDLeave(const bool sendPasteClick = false);
	void	SendDNDDrop();
	void	PrepareForDrop(const JXContainer* target);
	void	SendDNDStatus(const bool willAcceptDrop, const Atom action);
	void	SendDNDFinished();

	void	AnnounceTypeList(const Window xWindow, const JArray<Atom>& list) const;
	void	AnnounceAskActions(const JXButtonStates& buttonStates,
							   const JXKeyModifiers& modifiers) const;
	bool	DraggerCanProvideText() const;

	void	HandleDNDEnter(const XClientMessageEvent& clientMessage);
	void	HandleDNDHere(const XClientMessageEvent& clientMessage);
	void	HandleDNDLeave(const XClientMessageEvent& clientMessage);
	void	HandleDNDLeave1();
	void	HandleDNDDrop(const XClientMessageEvent& clientMessage);
	void	HandleDNDFinished();
	void	HandleDNDStatus(const XClientMessageEvent& clientMessage);

	bool	FindTarget(const JXContainer* coordOwner, const JPoint& pt,
					   Window* xWindow, Window* msgWindow,
					   JXContainer** target, Atom* vers) const;

	void	InvokeDNDScroll(const XClientMessageEvent& clientMessage, const JPoint& pt);

	void		FinishDND1();
	bool		WaitForLastStatusMsg();
	static Bool	GetNextStatusEvent(Display* display, XEvent* event, char* arg);

	long	PackPoint(const JPoint& pt) const;
	JPoint	UnpackPoint(const long data) const;
	JRect	UnpackRect(const long pt, const long area) const;

	// not allowed

	JXDNDManager(const JXDNDManager&) = delete;
	JXDNDManager& operator=(const JXDNDManager&) = delete;

public:

	class TargetFinder
	{
		public:

			TargetFinder(JXDisplay* display);

			virtual	~TargetFinder();

			JXDisplay*			GetDisplay() const;
			JXSelectionManager*	GetSelectionManager() const;
			JXDNDManager*		GetDNDManager() const;

			virtual bool	FindTarget(const JXContainer* coordOwner,
										   const JPoint& pt,
										   Window* xWindow, Window* msgWindow,
										   JXContainer** target, Atom* vers) = 0;

		private:

			JXDisplay*	itsDisplay;		// not owned
	};
};


/******************************************************************************
 GetDNDSelectionName

 ******************************************************************************/

inline Atom
JXDNDManager::GetDNDSelectionName()
	const
{
	return itsAtoms[ kDNDSelectionAtomIndex ];
}

/******************************************************************************
 GetDNDDirectSave0XAtom

 ******************************************************************************/

inline Atom
JXDNDManager::GetDNDDirectSave0XAtom()
	const
{
	return itsAtoms[ kDNDDirectSave0AtomIndex ];
}

/******************************************************************************
 Get actions

 ******************************************************************************/

inline Atom
JXDNDManager::GetDNDActionCopyXAtom()
	const
{
	return itsAtoms[ kDNDActionCopyAtomIndex ];
}

inline Atom
JXDNDManager::GetDNDActionMoveXAtom()
	const
{
	return itsAtoms[ kDNDActionMoveAtomIndex ];
}

inline Atom
JXDNDManager::GetDNDActionLinkXAtom()
	const
{
	return itsAtoms[ kDNDActionLinkAtomIndex ];
}

inline Atom
JXDNDManager::GetDNDActionAskXAtom()
	const
{
	return itsAtoms[ kDNDActionAskAtomIndex ];
}

inline Atom
JXDNDManager::GetDNDActionPrivateXAtom()
	const
{
	return itsAtoms[ kDNDActionPrivateAtomIndex ];
}

inline Atom
JXDNDManager::GetDNDActionDirectSaveXAtom()
	const
{
	return itsAtoms[ kDNDActionDirectSaveAtomIndex ];
}

/******************************************************************************
 IsDragging

	Returns true while dragging.

 ******************************************************************************/

inline bool
JXDNDManager::IsDragging()
	const
{
	return itsDragger != nullptr && itsIsDraggingFlag;
}

/******************************************************************************
 GetDraggerWindow

	This should only be called while dragging.

 ******************************************************************************/

inline Window
JXDNDManager::GetDraggerWindow()
	const
{
	return itsDraggerWindow;
}

/******************************************************************************
 TargetWillAcceptDrop

	This should only be called from JXWidget::HandleDNDHere().

 ******************************************************************************/

inline bool
JXDNDManager::TargetWillAcceptDrop()
	const
{
	return itsDragger != nullptr && itsWillAcceptDropFlag;
}

/******************************************************************************
 IsLastFakePasteTime

 ******************************************************************************/

inline bool
JXDNDManager::IsLastFakePasteTime
	(
	const Time time
	)
	const
{
	return itsSentFakePasteFlag &&
				 (time == itsFakeButtonPressTime ||
				  time == itsFakeButtonReleaseTime);
}

/******************************************************************************
 Default cursors

 ******************************************************************************/

inline JCursorIndex
JXDNDManager::GetDefaultDNDCursor
	(
	const bool	dropAccepted,
	const Atom		action
	)
	const
{
	return GetDNDCursor(dropAccepted, action, itsDefDNDCursor + kCopyCursorIndex);
}

inline JCursorIndex
JXDNDManager::GetDefaultDNDCopyCursor()
	const
{
	return itsDefDNDCursor [ kCopyCursorIndex ];
}

inline JCursorIndex
JXDNDManager::GetDefaultDNDMoveCursor()
	const
{
	return itsDefDNDCursor [ kMoveCursorIndex ];
}

inline JCursorIndex
JXDNDManager::GetDefaultDNDLinkCursor()
	const
{
	return itsDefDNDCursor [ kLinkCursorIndex ];
}

inline JCursorIndex
JXDNDManager::GetDefaultDNDAskCursor()
	const
{
	return itsDefDNDCursor [ kAskCursorIndex ];
}

/******************************************************************************
 Dragging file cursors

 ******************************************************************************/

inline JCursorIndex
JXDNDManager::GetDNDFileCursor
	(
	const bool	dropAccepted,
	const Atom		action
	)
	const
{
	return GetDNDCursor(dropAccepted, action, itsDefDNDCursor + kCopyFileCursorIndex);
}

inline JCursorIndex
JXDNDManager::GetDNDCopyFileCursor()
	const
{
	return itsDefDNDCursor [ kCopyFileCursorIndex ];
}

inline JCursorIndex
JXDNDManager::GetDNDMoveFileCursor()
	const
{
	return itsDefDNDCursor [ kMoveFileCursorIndex ];
}

inline JCursorIndex
JXDNDManager::GetDNDLinkFileCursor()
	const
{
	return itsDefDNDCursor [ kLinkFileCursorIndex ];
}

inline JCursorIndex
JXDNDManager::GetDNDAskFileCursor()
	const
{
	return itsDefDNDCursor [ kAskFileCursorIndex ];
}

/******************************************************************************
 Dragging directory cursors

 ******************************************************************************/

inline JCursorIndex
JXDNDManager::GetDNDDirectoryCursor
	(
	const bool	dropAccepted,
	const Atom		action
	)
	const
{
	return GetDNDCursor(dropAccepted, action, itsDefDNDCursor + kCopyDirCursorIndex);
}

inline JCursorIndex
JXDNDManager::GetDNDCopyDirectoryCursor()
	const
{
	return itsDefDNDCursor [ kCopyDirCursorIndex ];
}

inline JCursorIndex
JXDNDManager::GetDNDMoveDirectoryCursor()
	const
{
	return itsDefDNDCursor [ kMoveDirCursorIndex ];
}

inline JCursorIndex
JXDNDManager::GetDNDLinkDirectoryCursor()
	const
{
	return itsDefDNDCursor [ kLinkDirCursorIndex ];
}

inline JCursorIndex
JXDNDManager::GetDNDAskDirectoryCursor()
	const
{
	return itsDefDNDCursor [ kAskDirCursorIndex ];
}

/******************************************************************************
 Dragging file and directory cursors

 ******************************************************************************/

inline JCursorIndex
JXDNDManager::GetDNDFileAndDirectoryCursor
	(
	const bool	dropAccepted,
	const Atom		action
	)
	const
{
	return GetDNDCursor(dropAccepted, action, itsDefDNDCursor + kCopyMixedCursorIndex);
}

inline JCursorIndex
JXDNDManager::GetDNDCopyFileAndDirectoryCursor()
	const
{
	return itsDefDNDCursor [ kCopyMixedCursorIndex ];
}

inline JCursorIndex
JXDNDManager::GetDNDMoveFileAndDirectoryCursor()
	const
{
	return itsDefDNDCursor [ kMoveMixedCursorIndex ];
}

inline JCursorIndex
JXDNDManager::GetDNDLinkFileAndDirectoryCursor()
	const
{
	return itsDefDNDCursor [ kLinkMixedCursorIndex ];
}

inline JCursorIndex
JXDNDManager::GetDNDAskFileAndDirectoryCursor()
	const
{
	return itsDefDNDCursor [ kAskMixedCursorIndex ];
}

/******************************************************************************
 GetDisplay

 ******************************************************************************/

inline JXDisplay*
JXDNDManager::TargetFinder::GetDisplay()
	const
{
	return itsDisplay;
}

#endif
