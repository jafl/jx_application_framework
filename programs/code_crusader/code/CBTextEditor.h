/******************************************************************************
 CBTextEditor.h

	Copyright © 1996-2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBTextEditor
#define _H_CBTextEditor

#include <JXTEBase.h>
#include <JXStyledText.h>
#include <JSTStyler.h>
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

	CBTextEditor(CBTextDocument* document, const JString& fileName,
				 JXMenuBar* menuBar, CBTELineIndexInput* lineInput,
				 CBTEColIndexInput* colInput, const bool pasteStyledText,
				 JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	virtual ~CBTextEditor();

	CBTextDocument*	GetDocument();

	void	OpenSelection();
	void	ScrollForDefinition(const CBLanguage lang);

	virtual void	HandleKeyPress(const JUtf8Character& c,
								   const int keySym, const JXKeyModifiers& modifiers) override;
	virtual void	HandleShortcut(const int key, const JXKeyModifiers& modifiers) override;

	JSize		GetTabCharCount() const;
	void		SetTabCharCount(const JSize charCount);

	void		SetFont(const JString& name, const JSize size,
						const JSize tabCharCount);
	void		SetFont(const JString& name, const JSize size,
						const JSize tabCharCount, const bool breakCROnly);
	void		SetWritable(const bool writable);

	JCoordinate	CalcTabWidth(const JFont& font, const JSize tabCharCount) const;

	bool	WillBalanceWhileTyping() const;
	void		ShouldBalanceWhileTyping(const bool balance);

	bool	WillScrollToBalance() const;
	void		ShouldScrollToBalance(const bool scroll);

	bool	WillBeepWhenTypeUnbalanced() const;
	void		ShouldBeepWhenTypeUnbalanced(const bool beep);

	bool	TabIsSmart() const;
	void		TabShouldBeSmart(const bool smart);

	bool	CBAllowsDragAndDrop() const;
	void		CBShouldAllowDragAndDrop(const bool allow);

	bool	GetRightMarginWidth(JSize* width) const;
	void		SetRightMarginWidth(const bool show, const JSize width);
	void		SetRightMarginColor(const JColorID color);

	const JString&	GetScriptPath() const;
	void			SetScriptPath(const JString& path);

	void	RecalcStyles();

	// preferences

	void	ReadSetup(std::istream& input);
	void	WriteSetup(std::ostream& output) const;

	// called by CBTextDocument

	void	FileTypeChanged(const CBTextFileType type);
	void	UpdateWritable(const JString& name);

	// called by CBPSPrinter

	void	SetFontBeforePrintPS(const JSize fontSize);
	void	ResetFontAfterPrintPS();

	// called by CB*KeyHandler

	CBTELineIndexInput*	GetLineInput();

protected:

	virtual void	Draw(JXWindowPainter& p, const JRect& rect) override;

	virtual void	AdjustCursor(const JPoint& pt, const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers) override;

	virtual JCoordinate	GetPrintHeaderHeight(JPagePrinter& p) const override;
	virtual void		DrawPrintHeader(JPagePrinter& p, const JCoordinate footerHeight) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

protected:

	class StyledText : public JXStyledText
	{
		public:

		StyledText(CBTextDocument* doc, JFontManager* fontManager,
				   const bool pasteStyledText);

		virtual ~StyledText();

		protected:

		virtual void	AdjustStylesBeforeBroadcast(
							const JString& text, JRunArray<JFont>* styles,
							JStyledText::TextRange* recalcRange,
							JStyledText::TextRange* redrawRange,
							const bool deletion) override;

		private:

		CBTextDocument*					itsDoc;
		JArray<JSTStyler::TokenData>*	itsTokenStartList;	// nullptr if styling is turned off
	};

private:

	CBTextDocument*			itsDoc;					// this owns us
	JSize					itsTabCharCount;
	JSize					itsLastClickCount;
	JXKeyModifiers			itsLastModifiers;

	JIndex					itsExecScriptCmdIndex;	// index of "Run script" on Edit menu
	JIndex					itsFirstSearchMenuItem;	// index of first item added to Search menu
	CBFunctionMenu*			itsFnMenu;				// nullptr if not source
	JXStringCompletionMenu*	itsCompletionMenu;		// nullptr if doesn't have completer
	JXTextMenu*				itsContextMenu;			// nullptr until first used
	JString					itsScriptPath;			// can be empty

	// owned by their enclosures

	CBTELineIndexInput*	itsLineInput;
	CBTEColIndexInput*	itsColInput;

	// balance while typing

	bool	itsBalanceWhileTypingFlag;
	bool	itsScrollToBalanceFlag;
	bool	itsBeepWhenTypeUnbalancedFlag;

	// right margin

	bool	itsDrawRightMarginFlag;
	JSize		itsRightMarginWidth;
	JColorID	itsRightMarginColor;		// saved by CBPrefsManager

	// other options

	bool	itsAllowDNDFlag;			// buffered since Meta turns it on
	bool	itsSmartTabFlag;
	bool	itsSavedBreakCROnlyFlag;	// used during printing

private:

	void	UpdateTabHandling();

	void		UpdateCustomEditMenuItems();
	bool	HandleCustomEditMenuItems(const JIndex index);

	void		UpdateCustomSearchMenuItems();
	bool	HandleCustomSearchMenuItems(const JIndex index);

	void	PlaceBookmark();

	void	FindSelectedSymbol(const JXMouseButton button, const bool useContext);
	void	DisplayManPage();

	JIndex	GetLineIndex(const JStyledText::TextIndex& startIndex) const;

	void	PrivateSetTabCharCount(const JSize charCount);

	bool	IsNonstdError(JString* fileName,
							  JStyledText::TextRange* fileNameRange,
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
	GetText()->SetCRMTabCharCount(itsTabCharCount);
}

inline JSize
CBTextEditor::GetTabCharCount()
	const
{
	return itsTabCharCount;
}

/******************************************************************************
 CalcTabWidth

 ******************************************************************************/

inline JCoordinate
CBTextEditor::CalcTabWidth
	(
	const JFont&	font,
	const JSize		tabCharCount
	)
	const
{
	return tabCharCount * font.GetCharWidth(GetFontManager(), JUtf8Character(' '));
}

/******************************************************************************
 SetFont

 ******************************************************************************/

inline void
CBTextEditor::SetFont
	(
	const JString&	name,
	const JSize		size,
	const JSize		tabCharCount
	)
{
	SetFont(name, size, tabCharCount, WillBreakCROnly());
}

/******************************************************************************
 Balance while typing

 ******************************************************************************/

inline bool
CBTextEditor::WillBalanceWhileTyping()
	const
{
	return itsBalanceWhileTypingFlag;
}

inline void
CBTextEditor::ShouldBalanceWhileTyping
	(
	const bool balance
	)
{
	itsBalanceWhileTypingFlag = balance;
}

inline bool
CBTextEditor::WillScrollToBalance()
	const
{
	return itsScrollToBalanceFlag;
}

inline void
CBTextEditor::ShouldScrollToBalance
	(
	const bool scroll
	)
{
	itsScrollToBalanceFlag = scroll;
}

inline bool
CBTextEditor::WillBeepWhenTypeUnbalanced()
	const
{
	return itsBeepWhenTypeUnbalancedFlag;
}

inline void
CBTextEditor::ShouldBeepWhenTypeUnbalanced
	(
	const bool beep
	)
{
	itsBeepWhenTypeUnbalancedFlag = beep;
}

/******************************************************************************
 Smart tab

 ******************************************************************************/

inline bool
CBTextEditor::TabIsSmart()
	const
{
	return itsSmartTabFlag;
}

inline void
CBTextEditor::TabShouldBeSmart
	(
	const bool smart
	)
{
	itsSmartTabFlag = smart;
}

/******************************************************************************
 Allow DND

	We buffer the value since Meta toggles it.

 ******************************************************************************/

inline bool
CBTextEditor::CBAllowsDragAndDrop()
	const
{
	return itsAllowDNDFlag;
}

inline void
CBTextEditor::CBShouldAllowDragAndDrop
	(
	const bool allow
	)
{
	itsAllowDNDFlag = allow;
	ShouldAllowDragAndDrop(allow);
}

/******************************************************************************
 Right margin width

 ******************************************************************************/

inline bool
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
	const bool	show,
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
	const JColorID color
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
	const JString& path
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
