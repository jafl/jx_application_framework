/******************************************************************************
 JXTEBase.h

	Interface for the JXTEBase class

	Copyright (C) 1996-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXTEBase
#define _H_JXTEBase

#include <JXScrollableWidget.h>
#include <JTextEditor.h>
#include <JError.h>

class JXMenuBar;
class JXTextMenu;
class JXPSPrinter;
class JXPTPrinter;
class JXGoToLineDialog;
class JXTEBlinkCaretTask;

class JXTEBase : public JXScrollableWidget, public JTextEditor
{
	friend class JXTEBlinkCaretTask;
	friend class JXSpellCheckerDialog;

public:

	// do not change these values because they can be stored in prefs files

	enum PartialWordModifier
	{
		kCtrlMetaPWMod = 0,
		kMod2PWMod,
		kMod3PWMod,
		kMod4PWMod,
		kMod5PWMod
	};

	struct DisplayState
	{
		JXScrollableWidget::DisplayState	scroll;

		JBoolean	hadSelection;
		JIndex		selStart;
		JIndex		selEnd;
	};

public:

	virtual ~JXTEBase();

	virtual void	Activate();
	virtual void	Deactivate();

	virtual void	Suspend();
	virtual void	Resume();

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers);
	virtual void	HandleShortcut(const int key, const JXKeyModifiers& modifiers);

	JXTextMenu*		AppendEditMenu(JXMenuBar* menuBar);	// no extras
	JXTextMenu*		AppendEditMenu(JXMenuBar* menuBar,
								   const JBoolean showCheckSpellingCmds,
								   const JBoolean allowCheckSpelling,
								   const JBoolean showAdjustMarginsCmds,
								   const JBoolean allowAdjustMargins,
								   const JBoolean showCleanWhitespaceCmds,
								   const JBoolean allowCleanWhitespace,
								   const JBoolean showToggleReadOnly,
								   const JBoolean allowToggleReadOnly);
	JXTextMenu*		ShareEditMenu(JXTEBase* te);		// no extras
	JXTextMenu*		ShareEditMenu(JXTEBase* te,
								  const JBoolean allowCheckSpelling,
								  const JBoolean allowAdjustMargins,
								  const JBoolean allowCleanWhitespace,
								  const JBoolean allowToggleReadOnly);
	void			ShareEditMenu(JXTextMenu* menu);	// no extras
	void			ShareEditMenu(JXTextMenu* menu,
								  const JBoolean allowCheckSpelling,
								  const JBoolean allowAdjustMargins,
								  const JBoolean allowCleanWhitespace,
								  const JBoolean allowToggleReadOnly);
	JBoolean		HasEditMenu() const;
	JBoolean		GetEditMenu(JXTextMenu** menu) const;
	JBoolean		EditMenuIndexToCmd(const JIndex index, CmdIndex* cmd) const;
	JBoolean		EditMenuCmdToIndex(const CmdIndex cmd, JIndex* index) const;

	static JXTextMenu*	StaticAppendEditMenu(JXMenuBar* menuBar,
											 const JBoolean showCheckSpellingCmds,
											 const JBoolean allowCheckSpelling,
											 const JBoolean showAdjustMarginsCmds,
											 const JBoolean allowAdjustMargins,
											 const JBoolean showCleanWhitespaceCmds,
											 const JBoolean allowCleanWhitespace,
											 const JBoolean showToggleReadOnly,
											 const JBoolean allowToggleReadOnly);

	JXTextMenu*		AppendSearchMenu(JXMenuBar* menuBar);
	JXTextMenu*		AppendSearchReplaceMenu(JXMenuBar* menuBar);
	JXTextMenu*		ShareSearchMenu(JXTEBase* te);		// uses whichever one is available
	void			ShareSearchMenu(JXTextMenu* menu);
	void			ShareSearchReplaceMenu(JXTextMenu* menu);
	JBoolean		HasSearchMenu() const;
	JBoolean		GetSearchMenu(JXTextMenu** menu) const;
	JBoolean		SearchMenuIndexToCmd(const JIndex index, CmdIndex* cmd) const;
	JBoolean		SearchMenuCmdToIndex(const CmdIndex cmd, JIndex* index) const;
	JBoolean		HasSearchReplaceMenu() const;
	JBoolean		GetSearchReplaceMenu(JXTextMenu** menu) const;
	JBoolean		SearchReplaceMenuIndexToCmd(const JIndex index, CmdIndex* cmd) const;
	JBoolean		SearchReplaceMenuCmdToIndex(const CmdIndex cmd, JIndex* index) const;

	JBoolean	SearchForward(const JBoolean reportNotFound = kJTrue);
	JBoolean	SearchBackward();
	JBoolean	EnterSearchSelection();
	JBoolean	EnterReplaceSelection();
	JBoolean	SearchSelectionForward();
	JBoolean	SearchSelectionBackward();
	JBoolean	SearchClipboardForward();
	JBoolean	SearchClipboardBackward();
	JBoolean	ReplaceSelection();
	JBoolean	ReplaceAndSearchForward();
	JBoolean	ReplaceAndSearchBackward();
	JBoolean	ReplaceAllForward();
	JBoolean	ReplaceAllBackward();
	JBoolean	ReplaceAllInSelection();

	virtual JBoolean	TEHasSearchText() const;

	void			SetPSPrinter(JXPSPrinter* p);
	const JString&	GetPSPrintFileName() const;
	void			SetPSPrintFileName(const JCharacter* fileName);
	void			HandlePSPageSetup();
	void			PrintPS();

	void			SetPTPrinter(JXPTPrinter* p);
	const JString&	GetPTPrintFileName() const;
	void			SetPTPrintFileName(const JCharacter* fileName);
	void			HandlePTPageSetup();
	void			PrintPT();

	static PartialWordModifier	GetPartialWordModifier();
	static void					SetPartialWordModifier(const PartialWordModifier mod);

	static JBoolean	CaretWillFollowScroll();
	static void		CaretShouldFollowScroll(const JBoolean follow);

	static JBoolean	WillUseWindowsHomeEnd();
	static void		ShouldUseWindowsHomeEnd(const JBoolean use);

	static JBoolean	MiddleButtonWillPaste();
	static void		MiddleButtonShouldPaste(const JBoolean paste);

	void	AskForLine();
	void	MoveCaretToEdge(const int key);

	JCoordinate	GetMinBoundsWidth() const;
	JCoordinate	GetMinBoundsHeight() const;

	DisplayState	SaveDisplayState() const;
	void			RestoreDisplayState(const DisplayState& state);

protected:

	JXTEBase(const Type type, const JBoolean breakCROnly,
			 const JBoolean pasteStyledText,
			 JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
			 const HSizingOption hSizing, const VSizingOption vSizing,
			 const JCoordinate x, const JCoordinate y,
			 const JCoordinate w, const JCoordinate h);

	virtual void	Draw(JXWindowPainter& p, const JRect& rect);

	virtual void	HandleMouseEnter();
	virtual void	HandleMouseHere(const JPoint& pt, const JXKeyModifiers& modifiers);
	virtual void	HandleMouseLeave();

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers);

	virtual JBoolean	HitSamePart(const JPoint& pt1, const JPoint& pt2) const;

	virtual void		TEDisplayBusyCursor() const;

	virtual JBoolean	TEBeginDND();
	virtual void		DNDFinish(const JBoolean isDrop, const JXContainer* target);
	virtual Atom		GetDNDAction(const JXContainer* target,
									 const JXButtonStates& buttonStates,
									 const JXKeyModifiers& modifiers);
	virtual void		GetDNDAskActions(const JXButtonStates& buttonStates,
										 const JXKeyModifiers& modifiers,
										 JArray<Atom>* askActionList,
										 JPtrArray<JString>* askDescriptionList);
	virtual void		GetSelectionData(JXSelectionData* data,
										 const JCharacter* id);

	virtual JBoolean	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
									   const JPoint& pt, const Time time,
									   const JXWidget* source);
	virtual void		HandleDNDEnter();
	virtual void		HandleDNDHere(const JPoint& pt, const JXWidget* source);
	virtual void		HandleDNDLeave();
	virtual void		HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
									  const Atom action, const Time time,
									  const JXWidget* source);
	virtual void		TEPasteDropData();

	virtual JBoolean	TEXWillAcceptDrop(const JArray<Atom>& typeList,
										  const Atom action, const Time time,
										  const JXWidget* source);
	virtual JBoolean	TEXConvertDropData(const JArray<Atom>& typeList,
										   const Atom action, const Time time,
										   JString* text, JRunArray<JFont>* style);

	virtual void	HandleFocusEvent();
	virtual void	HandleUnfocusEvent();
	virtual void	AdjustCursor(const JPoint& pt, const JXKeyModifiers& modifiers);

	virtual void	HandleWindowFocusEvent();
	virtual void	HandleWindowUnfocusEvent();

	virtual void	BoundsMoved(const JCoordinate dx, const JCoordinate dy);
	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh);

	virtual void		TERefresh();
	virtual void		TERefreshRect(const JRect& rect);
	virtual void		TERedraw();
	virtual void		TESetGUIBounds(const JCoordinate w, const JCoordinate h,
									   const JCoordinate changeY);
	virtual JBoolean	TEWidthIsBeyondDisplayCapacity(const JSize width) const;
	virtual JBoolean	TEScrollToRect(const JRect& rect,
									   const JBoolean centerInDisplay);
	virtual JBoolean	TEScrollForDrag(const JPoint& pt);
	virtual JBoolean	TEScrollForDND(const JPoint& pt);
	virtual void		TESetVertScrollStep(const JCoordinate vStep);

	virtual void		TECaretShouldBlink(const JBoolean blink);

	virtual void		TEClipboardChanged();
	virtual JBoolean	TEOwnsClipboard() const;
	virtual JBoolean	TEGetExternalClipboard(JString* text, JRunArray<JFont>* style) const;

	virtual JCoordinate	GetPrintFooterHeight(JPagePrinter& p) const;
	virtual void		DrawPrintFooter(JPagePrinter& p, const JCoordinate footerHeight);

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	struct DNDDragInfo
	{
		const JPoint*			pt;
		const JXButtonStates*	buttonStates;
		const JXKeyModifiers*	modifiers;

		DNDDragInfo(const JPoint* p, const JXButtonStates* b,
					const JXKeyModifiers* m)
			:
			pt(p), buttonStates(b), modifiers(m)
			{ };
	};

	struct DNDDropInfo
	{
		const JArray<Atom>* typeList;
		Atom				action;
		Time				time;

		DNDDropInfo(const JArray<Atom>* l, const Atom a, const Time t)
			:
			typeList(l), action(a), time(t)
			{ };
	};

private:

	JCoordinate	itsMinWidth;					// minimum required by JTextEditor
	JCoordinate	itsMinHeight;

	JPoint			itsPrevMousePt;				// used while mouse is on top of us
	JXPSPrinter*	itsPSPrinter;				// not owned; can be NULL
	JString*		itsPSPrintName;				// file name when printing to a file; can be NULL
	JXPTPrinter*	itsPTPrinter;				// not owned; can be NULL
	JString*		itsPTPrintName;				// file name when printing to a file; can be NULL

	Atom			itsStyledText0XAtom;
	JBoolean		itsWillPasteCustomFlag;		// kJTrue => call TEXConvertDropData() in TEPasteDropData()

	static PartialWordModifier	thePWMod;		// which modifier to use for partial word movement

	JXTEBlinkCaretTask*	itsBlinkTask;
	JXGoToLineDialog*	itsGoToLineDialog;

	static JBoolean		theWindowsHomeEndFlag;	// kJTrue => use Windows/Motif Home/End action
	static JBoolean		theScrollCaretFlag;		// kJTrue => caret follows when text scrolls
	static JBoolean		theMiddleButtonPasteFlag;

	// edit menu

	JXTextMenu*		itsEditMenu;				// can be NULL
	JBoolean		itsCanCheckSpellingFlag;	// kJTrue => menu items usable
	JBoolean		itsCanAdjustMarginsFlag;	// kJTrue => menu items usable
	JBoolean		itsCanCleanWhitespaceFlag;	// kJTrue => menu items usable
	JBoolean		itsCanToggleReadOnlyFlag;	// kJTrue => menu items usable

	// search/replace menu

	JXTextMenu*		itsSearchMenu;				// can be NULL; NULL if itsReplaceMenu != NULL
	JXTextMenu*		itsReplaceMenu;				// can be NULL; NULL if itsSearchMenu != NULL

	// used to pass values from HandleMouseDrag() to TEBeginDND()

	DNDDragInfo*	itsDNDDragInfo;		// NULL if not in HandleMouseDrag()

	// used to pass values from HandleDNDDrop() to TEPasteDropData()

	DNDDropInfo*	itsDNDDropInfo;		// NULL if not in HandleDNDDrop()

private:

	void	UpdateEditMenu();
	void	HandleEditMenu(const JIndex item);

	void	UpdateSearchMenu();
	void	HandleSearchMenu(const JIndex item);

	void	UpdateReplaceMenu();
	void	HandleReplaceMenu(const JIndex item);

	void	HandleSearchReplaceCmd(const CmdIndex cmd);

	JBoolean	GetAvailDataTypes(const JArray<Atom>& typeList,
								  JBoolean* canGetStyledText,
								  JBoolean* canGetText, Atom* textType) const;
	JError		GetSelectionData(const Atom selectionName, const Time time,
								 JString* text, JRunArray<JFont>* style) const;
	JError		GetSelectionData(const Atom selectionName,
								 const JArray<Atom>& typeList, const Time time,
								 JString* text, JRunArray<JFont>* style) const;

	JBoolean	OKToPassToJTE(const int key, char* c) const;
	void		RemapWindowsHomeEnd(int* key, JXKeyModifiers* modifiers) const;

	void	PrivateActivate();
	void	PrivateDeactivate();

	// not allowed

	JXTEBase(const JXTEBase& source);
	const JXTEBase& operator=(const JXTEBase& source);

public:

	// JError classes

	static const JCharacter* kNoData;
	static const JCharacter* kDataNotCompatible;

	class NoData : public JError
		{
		public:

			NoData()
				:
				JError(kNoData)
			{ };
		};

	class DataNotCompatible : public JError
		{
		public:

			DataNotCompatible(const Atom type = None, JXDisplay* d = NULL);
		};
};

std::istream& operator>>(std::istream& input, JXTEBase::PartialWordModifier& pwMod);
std::ostream& operator<<(std::ostream& output, const JXTEBase::PartialWordModifier pwMod);


/******************************************************************************
 Edit menu

 ******************************************************************************/

inline JBoolean
JXTEBase::HasEditMenu()
	const
{
	return JConvertToBoolean( itsEditMenu != NULL );
}

inline JBoolean
JXTEBase::GetEditMenu
	(
	JXTextMenu** menu
	)
	const
{
	*menu = itsEditMenu;
	return JConvertToBoolean( itsEditMenu != NULL );
}

/******************************************************************************
 AppendEditMenu

 ******************************************************************************/

inline JXTextMenu*
JXTEBase::AppendEditMenu
	(
	JXMenuBar* menuBar
	)
{
	return AppendEditMenu(menuBar, kJFalse, kJFalse, kJFalse, kJFalse, kJFalse, kJFalse, kJFalse, kJFalse);
}

/******************************************************************************
 ShareEditMenu

 ******************************************************************************/

inline JXTextMenu*
JXTEBase::ShareEditMenu
	(
	JXTEBase* te
	)
{
	return ShareEditMenu(te, kJFalse, kJFalse, kJFalse, kJFalse);
}

inline void
JXTEBase::ShareEditMenu
	(
	JXTextMenu* menu
	)
{
	return ShareEditMenu(menu, kJFalse, kJFalse, kJFalse, kJFalse);
}

/******************************************************************************
 Search menu

 ******************************************************************************/

inline JBoolean
JXTEBase::HasSearchMenu()
	const
{
	return JConvertToBoolean( itsSearchMenu != NULL );
}

inline JBoolean
JXTEBase::GetSearchMenu
	(
	JXTextMenu** menu
	)
	const
{
	*menu = itsSearchMenu;
	return JConvertToBoolean( itsSearchMenu != NULL );
}

/******************************************************************************
 Search/Replace menu

 ******************************************************************************/

inline JBoolean
JXTEBase::HasSearchReplaceMenu()
	const
{
	return JConvertToBoolean( itsReplaceMenu != NULL );
}

inline JBoolean
JXTEBase::GetSearchReplaceMenu
	(
	JXTextMenu** menu
	)
	const
{
	*menu = itsReplaceMenu;
	return JConvertToBoolean( itsReplaceMenu != NULL );
}

/******************************************************************************
 Search for selection

 ******************************************************************************/

inline JBoolean
JXTEBase::SearchSelectionForward()
{
	return JConvertToBoolean( EnterSearchSelection() && SearchForward() );
}

inline JBoolean
JXTEBase::SearchSelectionBackward()
{
	return JConvertToBoolean( EnterSearchSelection() && SearchBackward() );
}

/******************************************************************************
 Replace and search again

 ******************************************************************************/

inline JBoolean
JXTEBase::ReplaceAndSearchForward()
{
	return JI2B(ReplaceSelection() && SearchForward());
}

inline JBoolean
JXTEBase::ReplaceAndSearchBackward()
{
	return JI2B(ReplaceSelection() && SearchBackward());
}

/******************************************************************************
 Get min bounds width/height

	This is useful because we keep JXWidget::bounds large enough to
	fill the aperture.

 ******************************************************************************/

inline JCoordinate
JXTEBase::GetMinBoundsWidth()
	const
{
	return itsMinWidth;
}

inline JCoordinate
JXTEBase::GetMinBoundsHeight()
	const
{
	return itsMinHeight;
}

/******************************************************************************
 Partial word movement (static)

 ******************************************************************************/

inline JXTEBase::PartialWordModifier
JXTEBase::GetPartialWordModifier()
{
	return thePWMod;
}

inline void
JXTEBase::SetPartialWordModifier
	(
	const PartialWordModifier mod
	)
{
	thePWMod = mod;
}

/******************************************************************************
 Caret follows scrolling (static)

 ******************************************************************************/

inline JBoolean
JXTEBase::CaretWillFollowScroll()
{
	return theScrollCaretFlag;
}

inline void
JXTEBase::CaretShouldFollowScroll
	(
	const JBoolean follow
	)
{
	theScrollCaretFlag = follow;
}

/******************************************************************************
 Windows Home/End (static)

 ******************************************************************************/

inline JBoolean
JXTEBase::WillUseWindowsHomeEnd()
{
	return theWindowsHomeEndFlag;
}

inline void
JXTEBase::ShouldUseWindowsHomeEnd
	(
	const JBoolean use
	)
{
	theWindowsHomeEndFlag = use;
}

/******************************************************************************
 Middle button pastes clipboard (static)

 ******************************************************************************/

inline JBoolean
JXTEBase::MiddleButtonWillPaste()
{
	return theMiddleButtonPasteFlag;
}

inline void
JXTEBase::MiddleButtonShouldPaste
	(
	const JBoolean paste
	)
{
	theMiddleButtonPasteFlag = paste;
}

#endif
