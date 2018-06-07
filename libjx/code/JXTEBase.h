/******************************************************************************
 JXTEBase.h

	Interface for the JXTEBase class

	Copyright (C) 1996-99 by John Lindal.

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

		JBoolean		hadSelection;
		JCharacterRange	range;
	};

public:

	virtual ~JXTEBase();

	virtual void	Activate() override;
	virtual void	Deactivate() override;

	virtual void	Suspend() override;
	virtual void	Resume() override;

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers) override;
	virtual void	HandleShortcut(const int key, const JXKeyModifiers& modifiers) override;

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
	JBoolean	ReplaceAll(const JBoolean restrictToSelection);

	virtual JBoolean	TEHasSearchText() const override;

	void			SetPSPrinter(JXPSPrinter* p);
	const JString&	GetPSPrintFileName() const;
	void			SetPSPrintFileName(const JString& fileName);
	void			HandlePSPageSetup();
	void			PrintPS();

	void			SetPTPrinter(JXPTPrinter* p);
	const JString&	GetPTPrintFileName() const;
	void			SetPTPrintFileName(const JString& fileName);
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

	JXTEBase(const Type type, JStyledText* text, const JBoolean ownsText,
			 const JBoolean breakCROnly,
			 JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
			 const HSizingOption hSizing, const VSizingOption vSizing,
			 const JCoordinate x, const JCoordinate y,
			 const JCoordinate w, const JCoordinate h);

	virtual void	Draw(JXWindowPainter& p, const JRect& rect) override;

	virtual void	HandleMouseEnter() override;
	virtual void	HandleMouseHere(const JPoint& pt, const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseLeave() override;

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers) override;

	virtual JBoolean	HitSamePart(const JPoint& pt1, const JPoint& pt2) const override;

	virtual JBoolean	TEBeginDND() override;
	virtual void		DNDFinish(const JBoolean isDrop, const JXContainer* target) override;
	virtual Atom		GetDNDAction(const JXContainer* target,
									 const JXButtonStates& buttonStates,
									 const JXKeyModifiers& modifiers) override;
	virtual void		GetDNDAskActions(const JXButtonStates& buttonStates,
										 const JXKeyModifiers& modifiers,
										 JArray<Atom>* askActionList,
										 JPtrArray<JString>* askDescriptionList) override;
	virtual void		GetSelectionData(JXSelectionData* data,
										 const JString& id) override;

	virtual JBoolean	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
									   const JPoint& pt, const Time time,
									   const JXWidget* source) override;
	virtual void		HandleDNDEnter() override;
	virtual void		HandleDNDHere(const JPoint& pt, const JXWidget* source) override;
	virtual void		HandleDNDLeave() override;
	virtual void		HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
									  const Atom action, const Time time,
									  const JXWidget* source) override;
	virtual void		TEPasteDropData() override;

	virtual JBoolean	TEXWillAcceptDrop(const JArray<Atom>& typeList,
										  const Atom action, const Time time,
										  const JXWidget* source);
	virtual JBoolean	TEXConvertDropData(const JArray<Atom>& typeList,
										   const Atom action, const Time time,
										   JString* text, JRunArray<JFont>* style);

	virtual void	HandleFocusEvent() override;
	virtual void	HandleUnfocusEvent() override;
	virtual void	AdjustCursor(const JPoint& pt, const JXKeyModifiers& modifiers) override;

	virtual void	HandleWindowFocusEvent() override;
	virtual void	HandleWindowUnfocusEvent() override;

	virtual void	BoundsMoved(const JCoordinate dx, const JCoordinate dy) override;
	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;

	virtual void		TERefresh() override;
	virtual void		TERefreshRect(const JRect& rect) override;
	virtual void		TERedraw() override;
	virtual void		TESetGUIBounds(const JCoordinate w, const JCoordinate h,
									   const JCoordinate changeY) override;
	virtual JBoolean	TEWidthIsBeyondDisplayCapacity(const JSize width) const override;
	virtual JBoolean	TEScrollToRect(const JRect& rect,
									   const JBoolean centerInDisplay) override;
	virtual JBoolean	TEScrollForDrag(const JPoint& pt) override;
	virtual JBoolean	TEScrollForDND(const JPoint& pt) override;
	virtual void		TESetVertScrollStep(const JCoordinate vStep) override;

	virtual void		TECaretShouldBlink(const JBoolean blink) override;

	virtual void		TEUpdateClipboard(const JString& text, const JRunArray<JFont>& style) const override;
	virtual JBoolean	TEGetClipboard(JString* text, JRunArray<JFont>* style) const override;

	virtual JCoordinate	GetPrintFooterHeight(JPagePrinter& p) const override;
	virtual void		DrawPrintFooter(JPagePrinter& p, const JCoordinate footerHeight) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

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
	JXPSPrinter*	itsPSPrinter;				// not owned; can be nullptr
	JString*		itsPSPrintName;				// file name when printing to a file; can be nullptr
	JXPTPrinter*	itsPTPrinter;				// not owned; can be nullptr
	JString*		itsPTPrintName;				// file name when printing to a file; can be nullptr

	Atom			itsStyledText0XAtom;
	JBoolean		itsWillPasteCustomFlag;		// kJTrue => call TEXConvertDropData() in TEPasteDropData()

	static PartialWordModifier	thePWMod;		// which modifier to use for partial word movement

	JXTEBlinkCaretTask*	itsBlinkTask;
	JXGoToLineDialog*	itsGoToLineDialog;

	static JBoolean		theWindowsHomeEndFlag;	// kJTrue => use Windows/Motif Home/End action
	static JBoolean		theScrollCaretFlag;		// kJTrue => caret follows when text scrolls
	static JBoolean		theMiddleButtonPasteFlag;

	// edit menu

	JXTextMenu*		itsEditMenu;				// can be nullptr
	JBoolean		itsCanCheckSpellingFlag;	// kJTrue => menu items usable
	JBoolean		itsCanAdjustMarginsFlag;	// kJTrue => menu items usable
	JBoolean		itsCanCleanWhitespaceFlag;	// kJTrue => menu items usable
	JBoolean		itsCanToggleReadOnlyFlag;	// kJTrue => menu items usable

	// search/replace menu

	JXTextMenu*		itsSearchMenu;				// can be nullptr; nullptr if itsReplaceMenu != nullptr
	JXTextMenu*		itsReplaceMenu;				// can be nullptr; nullptr if itsSearchMenu != nullptr

	// used to pass values from HandleMouseDrag() to TEBeginDND()

	DNDDragInfo*	itsDNDDragInfo;		// nullptr if not in HandleMouseDrag()

	// used to pass values from HandleDNDDrop() to TEPasteDropData()

	DNDDropInfo*	itsDNDDropInfo;		// nullptr if not in HandleDNDDrop()

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

	static const JUtf8Byte* kNoData;
	static const JUtf8Byte* kDataNotCompatible;

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

			DataNotCompatible(const Atom type = None, JXDisplay* d = nullptr);
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
	return JConvertToBoolean( itsEditMenu != nullptr );
}

inline JBoolean
JXTEBase::GetEditMenu
	(
	JXTextMenu** menu
	)
	const
{
	*menu = itsEditMenu;
	return JConvertToBoolean( itsEditMenu != nullptr );
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
	return JConvertToBoolean( itsSearchMenu != nullptr );
}

inline JBoolean
JXTEBase::GetSearchMenu
	(
	JXTextMenu** menu
	)
	const
{
	*menu = itsSearchMenu;
	return JConvertToBoolean( itsSearchMenu != nullptr );
}

/******************************************************************************
 Search/Replace menu

 ******************************************************************************/

inline JBoolean
JXTEBase::HasSearchReplaceMenu()
	const
{
	return JConvertToBoolean( itsReplaceMenu != nullptr );
}

inline JBoolean
JXTEBase::GetSearchReplaceMenu
	(
	JXTextMenu** menu
	)
	const
{
	*menu = itsReplaceMenu;
	return JConvertToBoolean( itsReplaceMenu != nullptr );
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
