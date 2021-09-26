/******************************************************************************
 JXTEBase.h

	Interface for the JXTEBase class

	Copyright (C) 1996-99 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXTEBase
#define _H_JXTEBase

#include "jx-af/jx/JXScrollableWidget.h"
#include <jx-af/jcore/JTextEditor.h>
#include <jx-af/jcore/JError.h>

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

		bool			hadSelection;
		JCharacterRange	range;
	};

public:

	virtual ~JXTEBase();

	virtual void	Activate() override;
	virtual void	Deactivate() override;

	virtual void	Suspend() override;
	virtual void	Resume() override;

	virtual void	HandleKeyPress(const JUtf8Character& c, const int keySym,
								   const JXKeyModifiers& modifiers) override;
	virtual void	HandleShortcut(const int key, const JXKeyModifiers& modifiers) override;

	JXTextMenu*		AppendEditMenu(JXMenuBar* menuBar);	// no extras
	JXTextMenu*		AppendEditMenu(JXMenuBar* menuBar,
								   const bool showCheckSpellingCmds,
								   const bool allowCheckSpelling,
								   const bool showAdjustMarginsCmds,
								   const bool allowAdjustMargins,
								   const bool showCleanWhitespaceCmds,
								   const bool allowCleanWhitespace,
								   const bool showToggleReadOnly,
								   const bool allowToggleReadOnly);
	JXTextMenu*		ShareEditMenu(JXTEBase* te);		// no extras
	JXTextMenu*		ShareEditMenu(JXTEBase* te,
								  const bool allowCheckSpelling,
								  const bool allowAdjustMargins,
								  const bool allowCleanWhitespace,
								  const bool allowToggleReadOnly);
	void			ShareEditMenu(JXTextMenu* menu);	// no extras
	void			ShareEditMenu(JXTextMenu* menu,
								  const bool allowCheckSpelling,
								  const bool allowAdjustMargins,
								  const bool allowCleanWhitespace,
								  const bool allowToggleReadOnly);
	bool	HasEditMenu() const;
	bool	GetEditMenu(JXTextMenu** menu) const;
	bool	EditMenuIndexToCmd(const JIndex index, CmdIndex* cmd) const;
	bool	EditMenuCmdToIndex(const CmdIndex cmd, JIndex* index) const;

	static JXTextMenu*	StaticAppendEditMenu(JXMenuBar* menuBar,
											 const bool showCheckSpellingCmds,
											 const bool allowCheckSpelling,
											 const bool showAdjustMarginsCmds,
											 const bool allowAdjustMargins,
											 const bool showCleanWhitespaceCmds,
											 const bool allowCleanWhitespace,
											 const bool showToggleReadOnly,
											 const bool allowToggleReadOnly);

	JXTextMenu*	AppendSearchMenu(JXMenuBar* menuBar);
	JXTextMenu*	AppendSearchReplaceMenu(JXMenuBar* menuBar);
	JXTextMenu*	ShareSearchMenu(JXTEBase* te);		// uses whichever one is available
	void		ShareSearchMenu(JXTextMenu* menu);
	void		ShareSearchReplaceMenu(JXTextMenu* menu);
	bool		HasSearchMenu() const;
	bool		GetSearchMenu(JXTextMenu** menu) const;
	bool		SearchMenuIndexToCmd(const JIndex index, CmdIndex* cmd) const;
	bool		SearchMenuCmdToIndex(const CmdIndex cmd, JIndex* index) const;
	bool		HasSearchReplaceMenu() const;
	bool		GetSearchReplaceMenu(JXTextMenu** menu) const;
	bool		SearchReplaceMenuIndexToCmd(const JIndex index, CmdIndex* cmd) const;
	bool		SearchReplaceMenuCmdToIndex(const CmdIndex cmd, JIndex* index) const;

	bool	SearchForward(const bool reportNotFound = true);
	bool	SearchBackward();
	bool	EnterSearchSelection();
	bool	EnterReplaceSelection();
	bool	SearchSelectionForward();
	bool	SearchSelectionBackward();
	bool	SearchClipboardForward();
	bool	SearchClipboardBackward();
	bool	ReplaceSelection();
	bool	ReplaceAndSearchForward();
	bool	ReplaceAndSearchBackward();
	bool	ReplaceAll(const bool restrictToSelection);

	virtual bool	TEHasSearchText() const override;

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

	static bool	CaretWillFollowScroll();
	static void	CaretShouldFollowScroll(const bool follow);

	static bool	WillUseWindowsHomeEnd();
	static void	ShouldUseWindowsHomeEnd(const bool use);

	static bool	MiddleButtonWillPaste();
	static void	MiddleButtonShouldPaste(const bool paste);

	void	AskForLine();
	void	MoveCaretToEdge(const JUtf8Character& c);

	JCoordinate	GetMinBoundsWidth() const;
	JCoordinate	GetMinBoundsHeight() const;

	DisplayState	SaveDisplayState() const;
	void			RestoreDisplayState(const DisplayState& state);

protected:

	JXTEBase(const Type type, JStyledText* text, const bool ownsText,
			 const bool breakCROnly,
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

	virtual bool	HitSamePart(const JPoint& pt1, const JPoint& pt2) const override;

	virtual bool	TEBeginDND() override;
	virtual void	DNDFinish(const bool isDrop, const JXContainer* target) override;
	virtual Atom	GetDNDAction(const JXContainer* target,
								 const JXButtonStates& buttonStates,
								 const JXKeyModifiers& modifiers) override;
	virtual void	GetDNDAskActions(const JXButtonStates& buttonStates,
									 const JXKeyModifiers& modifiers,
									 JArray<Atom>* askActionList,
									 JPtrArray<JString>* askDescriptionList) override;
	virtual void	GetSelectionData(JXSelectionData* data,
									 const JString& id) override;

	virtual bool	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
								   const JPoint& pt, const Time time,
								   const JXWidget* source) override;
	virtual void	HandleDNDEnter() override;
	virtual void	HandleDNDHere(const JPoint& pt, const JXWidget* source) override;
	virtual void	HandleDNDLeave() override;
	virtual void	HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
								  const Atom action, const Time time,
								  const JXWidget* source) override;
	virtual void	TEPasteDropData() override;

	virtual bool	TEXWillAcceptDrop(const JArray<Atom>& typeList,
										  const Atom action, const Time time,
										  const JXWidget* source);
	virtual bool	TEXConvertDropData(const JArray<Atom>& typeList,
										   const Atom action, const Time time,
										   JString* text, JRunArray<JFont>* style);

	virtual void	HandleFocusEvent() override;
	virtual void	HandleUnfocusEvent() override;
	virtual void	AdjustCursor(const JPoint& pt, const JXKeyModifiers& modifiers) override;

	virtual void	HandleWindowFocusEvent() override;
	virtual void	HandleWindowUnfocusEvent() override;

	virtual void	BoundsMoved(const JCoordinate dx, const JCoordinate dy) override;
	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;

	virtual void	TERefresh() override;
	virtual void	TERefreshRect(const JRect& rect) override;
	virtual void	TERedraw() override;
	virtual void	TESetGUIBounds(const JCoordinate w, const JCoordinate h,
								   const JCoordinate changeY) override;
	virtual bool	TEWidthIsBeyondDisplayCapacity(const JSize width) const override;
	virtual bool	TEScrollToRect(const JRect& rect,
								   const bool centerInDisplay) override;
	virtual bool	TEScrollForDrag(const JPoint& pt) override;
	virtual bool	TEScrollForDND(const JPoint& pt) override;
	virtual void	TESetVertScrollStep(const JCoordinate vStep) override;

	virtual void	TECaretShouldBlink(const bool blink) override;

	virtual void	TEUpdateClipboard(const JString& text, const JRunArray<JFont>& style) const override;
	virtual bool	TEGetClipboard(JString* text, JRunArray<JFont>* style) const override;

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
	bool			itsWillPasteCustomFlag;		// true => call TEXConvertDropData() in TEPasteDropData()

	static PartialWordModifier	thePWMod;		// which modifier to use for partial word movement

	JXTEBlinkCaretTask*	itsBlinkTask;
	JXGoToLineDialog*	itsGoToLineDialog;

	static bool		theWindowsHomeEndFlag;	// true => use Windows/Motif Home/End action
	static bool		theScrollCaretFlag;		// true => caret follows when text scrolls
	static bool		theMiddleButtonPasteFlag;

	// edit menu

	JXTextMenu*	itsEditMenu;				// can be nullptr
	bool		itsCanCheckSpellingFlag;	// true => menu items usable
	bool		itsCanAdjustMarginsFlag;	// true => menu items usable
	bool		itsCanCleanWhitespaceFlag;	// true => menu items usable
	bool		itsCanToggleReadOnlyFlag;	// true => menu items usable

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

	bool	GetAvailDataTypes(const JArray<Atom>& typeList,
							  bool* canGetStyledText,
							  bool* canGetText, Atom* textType) const;
	JError	GetSelectionData(const Atom selectionName, const Time time,
							 JString* text, JRunArray<JFont>* style) const;
	JError	GetSelectionData(const Atom selectionName,
							 const JArray<Atom>& typeList, const Time time,
							 JString* text, JRunArray<JFont>* style) const;

	void	RemapWindowsHomeEnd(int* key, JXKeyModifiers* modifiers) const;

	void	PrivateActivate();
	void	PrivateDeactivate();

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

inline bool
JXTEBase::HasEditMenu()
	const
{
	return itsEditMenu != nullptr;
}

inline bool
JXTEBase::GetEditMenu
	(
	JXTextMenu** menu
	)
	const
{
	*menu = itsEditMenu;
	return itsEditMenu != nullptr;
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
	return AppendEditMenu(menuBar, false, false, false, false, false, false, false, false);
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
	return ShareEditMenu(te, false, false, false, false);
}

inline void
JXTEBase::ShareEditMenu
	(
	JXTextMenu* menu
	)
{
	return ShareEditMenu(menu, false, false, false, false);
}

/******************************************************************************
 Search menu

 ******************************************************************************/

inline bool
JXTEBase::HasSearchMenu()
	const
{
	return itsSearchMenu != nullptr;
}

inline bool
JXTEBase::GetSearchMenu
	(
	JXTextMenu** menu
	)
	const
{
	*menu = itsSearchMenu;
	return itsSearchMenu != nullptr;
}

/******************************************************************************
 Search/Replace menu

 ******************************************************************************/

inline bool
JXTEBase::HasSearchReplaceMenu()
	const
{
	return itsReplaceMenu != nullptr;
}

inline bool
JXTEBase::GetSearchReplaceMenu
	(
	JXTextMenu** menu
	)
	const
{
	*menu = itsReplaceMenu;
	return itsReplaceMenu != nullptr;
}

/******************************************************************************
 Search for selection

 ******************************************************************************/

inline bool
JXTEBase::SearchSelectionForward()
{
	return EnterSearchSelection() && SearchForward();
}

inline bool
JXTEBase::SearchSelectionBackward()
{
	return EnterSearchSelection() && SearchBackward();
}

/******************************************************************************
 Replace and search again

 ******************************************************************************/

inline bool
JXTEBase::ReplaceAndSearchForward()
{
	return ReplaceSelection() && SearchForward();
}

inline bool
JXTEBase::ReplaceAndSearchBackward()
{
	return ReplaceSelection() && SearchBackward();
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

inline bool
JXTEBase::CaretWillFollowScroll()
{
	return theScrollCaretFlag;
}

inline void
JXTEBase::CaretShouldFollowScroll
	(
	const bool follow
	)
{
	theScrollCaretFlag = follow;
}

/******************************************************************************
 Windows Home/End (static)

 ******************************************************************************/

inline bool
JXTEBase::WillUseWindowsHomeEnd()
{
	return theWindowsHomeEndFlag;
}

inline void
JXTEBase::ShouldUseWindowsHomeEnd
	(
	const bool use
	)
{
	theWindowsHomeEndFlag = use;
}

/******************************************************************************
 Middle button pastes clipboard (static)

 ******************************************************************************/

inline bool
JXTEBase::MiddleButtonWillPaste()
{
	return theMiddleButtonPasteFlag;
}

inline void
JXTEBase::MiddleButtonShouldPaste
	(
	const bool paste
	)
{
	theMiddleButtonPasteFlag = paste;
}

#endif
