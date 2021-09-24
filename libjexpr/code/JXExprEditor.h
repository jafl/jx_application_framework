/******************************************************************************
 JXExprEditor.h

	Interface for the JXExprEditor class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXExprEditor
#define _H_JXExprEditor

#include <JXScrollableWidget.h>
#include <JExprEditor.h>
#include <JXStyledText.h>

class JXTextMenu;
class JXMenuBar;
class JXEPSPrinter;

class JXExprEditor : public JXScrollableWidget, public JExprEditor
{
public:

	JXExprEditor(const JVariableList* varList, JXMenuBar* menuBar,
				 JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	JXExprEditor(const JVariableList* varList, JXMenuBar* menuBar,
				 JXTextMenu* editMenu,
				 JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	JXExprEditor(const JVariableList* varList, JXExprEditor* menuProvider,
				 JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	virtual ~JXExprEditor();

	JXTextMenu*		GetEditMenu() const;
	bool		EditMenuIndexToCmd(const JIndex index, CmdIndex* cmd) const;
	bool		EditMenuCmdToIndex(const CmdIndex cmd, JIndex* index) const;

	virtual void	HandleKeyPress(const JUtf8Character& c, const int keySym,
								   const JXKeyModifiers& modifiers) override;

	virtual JStyledText*	BuildStyledText() override;

protected:

	virtual void		EIPRefresh() override;
	virtual void		EIPRedraw() override;
	virtual void		EIPBoundsChanged() override;
	virtual bool	EIPScrollToRect(const JRect& r) override;
	virtual bool	EIPScrollForDrag(const JPoint& pt) override;
	virtual void		EIPAdjustNeedTab(const bool needTab) override;

	virtual void		EIPClipboardChanged() override;
	virtual bool	EIPOwnsClipboard() override;
	virtual bool	EIPGetExternalClipboard(JString* text) override;

	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;

	virtual void	Draw(JXWindowPainter& p, const JRect& rect) override;

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers) override;

	virtual void	HandleFocusEvent() override;
	virtual void	HandleUnfocusEvent() override;

	virtual bool	OKToUnfocus() override;
	virtual void		AdjustCursor(const JPoint& pt, const JXKeyModifiers& modifiers) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

	JXEPSPrinter&		GetEPSPrinter();
	const JXEPSPrinter&	GetEPSPrinter() const;

protected:

	class StyledText : public JXStyledText
	{
		public:

		StyledText(JFontManager* fontManager)
			:
			JXStyledText(false, false, fontManager)
		{ };

		protected:

		virtual void	AdjustStylesBeforeBroadcast(
							const JString& text, JRunArray<JFont>* styles,
							JStyledText::TextRange* recalcRange,
							JStyledText::TextRange* redrawRange,
							const bool deletion) override;
	};

private:

	JXEPSPrinter*	itsEPSPrinter;

	// owned by the menu bar

	JXTextMenu*	itsEditMenu;
	JXTextMenu*	itsMathMenu;
	JXTextMenu*	itsFunctionMenu;
	JXTextMenu*	itsFontMenu;

private:

	void	JXExprEditorX();
	void	CreateMenus(JXMenuBar* menuBar, JXTextMenu* editMenu);

	void	UpdateEditMenu();
	void	HandleEditMenu(const JIndex item);

	void	UpdateMathMenu();
	void	HandleMathMenu(const JIndex item);

	void	UpdateFunctionMenu();
	void	HandleFunctionMenu(const JIndex item);

	void	UpdateFontMenu();
	void	HandleFontMenu(const JIndex item);

	void	GetDrawingOffset(JPoint* delta) const;
	JPoint	BoundsToRenderer(const JPoint& pt) const;
	JPoint	RendererToBounds(const JPoint& pt) const;
};


/******************************************************************************
 GetEditMenu

 ******************************************************************************/

inline JXTextMenu*
JXExprEditor::GetEditMenu()
	const
{
	return itsEditMenu;
}

/******************************************************************************
 GetEPSPrinter

 ******************************************************************************/

inline JXEPSPrinter&
JXExprEditor::GetEPSPrinter()
{
	return *itsEPSPrinter;
}

inline const JXEPSPrinter&
JXExprEditor::GetEPSPrinter()
	const
{
	return *itsEPSPrinter;
}

#endif
