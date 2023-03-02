/******************************************************************************
 JXExprEditor.h

	Interface for the JXExprEditor class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXExprEditor
#define _H_JXExprEditor

#include <jx-af/jx/JXScrollableWidget.h>
#include "JExprEditor.h"
#include <jx-af/jx/JXStyledText.h>

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

	~JXExprEditor() override;

	JXTextMenu*	GetEditMenu() const;
	bool		EditMenuIndexToCmd(const JIndex index, CmdIndex* cmd) const;
	bool		EditMenuCmdToIndex(const CmdIndex cmd, JIndex* index) const;

	void	HandleKeyPress(const JUtf8Character& c, const int keySym,
						   const JXKeyModifiers& modifiers) override;

	JStyledText*	BuildStyledText() override;

protected:

	void	EIPRefresh() override;
	void	EIPRedraw() override;
	void	EIPBoundsChanged() override;
	bool	EIPScrollToRect(const JRect& r) override;
	bool	EIPScrollForDrag(const JPoint& pt) override;
	void	EIPAdjustNeedTab(const bool needTab) override;

	void	EIPClipboardChanged() override;
	bool	EIPOwnsClipboard() override;
	bool	EIPGetExternalClipboard(JString* text) override;

	void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;

	void	Draw(JXWindowPainter& p, const JRect& rect) override;

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
							const JSize clickCount,
							const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;
	void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;
	void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
						  const JXButtonStates& buttonStates,
						  const JXKeyModifiers& modifiers) override;

	void	HandleFocusEvent() override;
	void	HandleUnfocusEvent() override;

	bool	OKToUnfocus() override;
	void	AdjustCursor(const JPoint& pt, const JXKeyModifiers& modifiers) override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

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

		void	AdjustStylesBeforeBroadcast(
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
