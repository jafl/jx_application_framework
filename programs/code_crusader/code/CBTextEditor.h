/******************************************************************************
 CBTextEditor.h

	Copyright (C) 1996-2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBTextEditor
#define _H_CBTextEditor

#include <JXTEBase.h>
#include <JTEStyler.h>
#include "CBTextFileType.h"

class JXMenu;
class JXStringCompletionMenu;
class CBTextDocument;
class CBTELineIndexInput;
class CBTEColIndexInput;
class CBFunctionMenu;
class CBTEScriptMenu;

class CBTextEditor : public JXTEBase
{
public:

	CBTextEditor(CBTextDocument* document, const JCharacter* fileName,
				 JXMenuBar* menuBar, CBTELineIndexInput* lineInput,
				 CBTEColIndexInput* colInput,
				 JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	virtual ~CBTextEditor();

	CBTextDocument*	GetDocument();

	void	OpenSelection();
	void	ScrollForDefinition(const CBLanguage lang);

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers);
	virtual void	HandleShortcut(const int key, const JXKeyModifiers& modifiers);

	JSize			GetTabCharCount() const;
	void			SetTabCharCount(const JSize charCount);

	void			SetFont(const JCharacter* name, const JSize size,
							const JSize tabCharCount);
	void			SetFont(const JCharacter* name, const JSize size,
							const JSize tabCharCount, const JBoolean breakCROnly);
	void			SetWritable(const JBoolean writable);

	static JCoordinate	CalcTabWidth(const JFont& font, const JSize tabCharCount);

	JBoolean	WillBalanceWhileTyping() const;
	void		ShouldBalanceWhileTyping(const JBoolean balance);

	JBoolean	WillScrollToBalance() const;
	void		ShouldScrollToBalance(const JBoolean scroll);

	JBoolean	WillBeepWhenTypeUnbalanced() const;
	void		ShouldBeepWhenTypeUnbalanced(const JBoolean beep);

	JBoolean	TabIsSmart() const;
	void		TabShouldBeSmart(const JBoolean smart);

	JBoolean	CBAllowsDragAndDrop() const;
	void		CBShouldAllowDragAndDrop(const JBoolean allow);

	JBoolean	GetRightMarginWidth(JSize* width) const;
	void		SetRightMarginWidth(const JBoolean show, const JSize width);
	void		SetRightMarginColor(const JColorIndex color);

	const JString&	GetScriptPath() const;
	void			SetScriptPath(const JCharacter* path);

	void	RecalcStyles();

	// preferences

	void	ReadSetup(std::istream& input);
	void	WriteSetup(std::ostream& output) const;

	// called by CBTextDocument

	void	FileTypeChanged(const CBTextFileType type);
	void	UpdateWritable(const JCharacter* name);

	// called by CBPSPrinter

	void	SetFontBeforePrintPS(const JSize fontSize);
	void	ResetFontAfterPrintPS();

	// called by CB*KeyHandler

	CBTELineIndexInput*	GetLineInput();

protected:

	virtual void	Draw(JXWindowPainter& p, const JRect& rect);

	virtual void	AdjustCursor(const JPoint& pt, const JXKeyModifiers& modifiers);
	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers);

	virtual void	AdjustStylesBeforeRecalc(const JString& buffer, JRunArray<JFont>* styles,
											 JIndexRange* recalcRange, JIndexRange* redrawRange,
											 const JBoolean deletion);

	virtual JCoordinate	GetPrintHeaderHeight(JPagePrinter& p) const;
	virtual void		DrawPrintHeader(JPagePrinter& p, const JCoordinate footerHeight);

	virtual JBoolean	VIsCharacterInWord(const JString& text,
										   const JIndex charIndex) const;

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	CBTextDocument*			itsDoc;					// this owns us
	JSize					itsTabCharCount;
	JSize					itsLastClickCount;
	JXKeyModifiers			itsLastModifiers;

	JIndex					itsExecScriptCmdIndex;	// index of "Run script" on Edit menu
	JIndex					itsFirstSearchMenuItem;	// index of first item added to Search menu
	CBFunctionMenu*			itsFnMenu;				// NULL if not source
	JXStringCompletionMenu*	itsCompletionMenu;		// NULL if doesn't have completer
	JXTextMenu*				itsContextMenu;			// NULL until first used
	JString					itsScriptPath;			// can be empty

	// owned by their enclosures

	CBTELineIndexInput*	itsLineInput;
	CBTEColIndexInput*	itsColInput;

	// keyword styling

	JArray<JTEStyler::TokenData>*	itsTokenStartList;	// NULL if styling is turned off

	// balance while typing

	JBoolean	itsBalanceWhileTypingFlag;
	JBoolean	itsScrollToBalanceFlag;
	JBoolean	itsBeepWhenTypeUnbalancedFlag;

	// right margin

	JBoolean	itsDrawRightMarginFlag;
	JSize		itsRightMarginWidth;
	JColorIndex	itsRightMarginColor;		// saved by CBPrefsManager

	// other options

	JBoolean	itsAllowDNDFlag;			// buffered since Meta turns it on
	JBoolean	itsSmartTabFlag;
	JBoolean	itsSavedBreakCROnlyFlag;	// used during printing

private:

	void	UpdateTabHandling();

	void		UpdateCustomEditMenuItems();
	JBoolean	HandleCustomEditMenuItems(const JIndex index);

	void		UpdateCustomSearchMenuItems();
	JBoolean	HandleCustomSearchMenuItems(const JIndex index);

	void	PlaceBookmark();

	void	FindSelectedSymbol(const JXMouseButton button, const JBoolean useContext);
	void	DisplayManPage();

	JIndex	GetLineIndex(const JIndex startIndex) const;

	void	PrivateSetTabCharCount(const JSize charCount);

	JBoolean	IsNonstdError(JIndex* fileNameStart, JIndex* fileNameEnd,
							  JIndex* lineIndex) const;

	void	ShowBalancingOpenGroup();

	void			CreateContextMenu();
	CBTEScriptMenu*	CreateScriptMenu(JXMenu* parent, const JIndex index);

	void	UpdateContextMenu();
	void	HandleContextMenu(const JIndex index);

	void	SelectWordIfNoSelection();

	// not allowed

	CBTextEditor(const CBTextEditor& source);
	const CBTextEditor& operator=(const CBTextEditor& source);
};


/******************************************************************************
 GetDocument

 ******************************************************************************/

inline CBTextDocument*
CBTextEditor::GetDocument()
{
	return itsDoc;
}

/******************************************************************************
 Tab width (# of characters)

 ******************************************************************************/

inline void
CBTextEditor::PrivateSetTabCharCount
	(
	const JSize charCount
	)
{
	itsTabCharCount = charCount;
	SetCRMTabCharCount(itsTabCharCount);
}

inline JSize
CBTextEditor::GetTabCharCount()
	const
{
	return itsTabCharCount;
}

/******************************************************************************
 CalcTabWidth (static)

 ******************************************************************************/

inline JCoordinate
CBTextEditor::CalcTabWidth
	(
	const JFont&	font,
	const JSize		tabCharCount
	)
{
	return tabCharCount * font.GetCharWidth(' ');
}

/******************************************************************************
 SetFont

 ******************************************************************************/

inline void
CBTextEditor::SetFont
	(
	const JCharacter*	name,
	const JSize			size,
	const JSize			tabCharCount
	)
{
	SetFont(name, size, tabCharCount, WillBreakCROnly());
}

/******************************************************************************
 Balance while typing

 ******************************************************************************/

inline JBoolean
CBTextEditor::WillBalanceWhileTyping()
	const
{
	return itsBalanceWhileTypingFlag;
}

inline void
CBTextEditor::ShouldBalanceWhileTyping
	(
	const JBoolean balance
	)
{
	itsBalanceWhileTypingFlag = balance;
}

inline JBoolean
CBTextEditor::WillScrollToBalance()
	const
{
	return itsScrollToBalanceFlag;
}

inline void
CBTextEditor::ShouldScrollToBalance
	(
	const JBoolean scroll
	)
{
	itsScrollToBalanceFlag = scroll;
}

inline JBoolean
CBTextEditor::WillBeepWhenTypeUnbalanced()
	const
{
	return itsBeepWhenTypeUnbalancedFlag;
}

inline void
CBTextEditor::ShouldBeepWhenTypeUnbalanced
	(
	const JBoolean beep
	)
{
	itsBeepWhenTypeUnbalancedFlag = beep;
}

/******************************************************************************
 Smart tab

 ******************************************************************************/

inline JBoolean
CBTextEditor::TabIsSmart()
	const
{
	return itsSmartTabFlag;
}

inline void
CBTextEditor::TabShouldBeSmart
	(
	const JBoolean smart
	)
{
	itsSmartTabFlag = smart;
}

/******************************************************************************
 Allow DND

	We buffer the value since Meta toggles it.

 ******************************************************************************/

inline JBoolean
CBTextEditor::CBAllowsDragAndDrop()
	const
{
	return itsAllowDNDFlag;
}

inline void
CBTextEditor::CBShouldAllowDragAndDrop
	(
	const JBoolean allow
	)
{
	itsAllowDNDFlag = allow;
	ShouldAllowDragAndDrop(allow);
}

/******************************************************************************
 Right margin width

 ******************************************************************************/

inline JBoolean
CBTextEditor::GetRightMarginWidth
	(
	JSize* width
	)
	const
{
	*width = itsRightMarginWidth;
	return itsDrawRightMarginFlag;
}

inline void
CBTextEditor::SetRightMarginWidth
	(
	const JBoolean	show,
	const JSize		width
	)
{
	itsRightMarginWidth = width;

	if (show != itsDrawRightMarginFlag)
		{
		itsDrawRightMarginFlag = show;
		Refresh();
		}
}

inline void
CBTextEditor::SetRightMarginColor
	(
	const JColorIndex color
	)
{
	if (color != itsRightMarginColor)
		{
		itsRightMarginColor = color;
		SetWhitespaceColor(color);
		if (itsDrawRightMarginFlag)
			{
			Refresh();
			}
		}
}

/******************************************************************************
 Script path

 ******************************************************************************/

inline const JString&
CBTextEditor::GetScriptPath()
	const
{
	return itsScriptPath;
}

inline void
CBTextEditor::SetScriptPath
	(
	const JCharacter* path
	)
{
	itsScriptPath = path;
}

/******************************************************************************
 GetLineInput

 ******************************************************************************/

inline CBTELineIndexInput*
CBTextEditor::GetLineInput()
{
	return itsLineInput;
}

#endif
