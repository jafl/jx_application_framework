/******************************************************************************
 JXExprEditor.h

	Interface for the JXExprEditor class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXExprEditor
#define _H_JXExprEditor

#include <JXScrollableWidget.h>
#include <JExprEditor.h>

class JString;
class JVariableList;
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
	JBoolean		EditMenuIndexToCmd(const JIndex index, CmdIndex* cmd) const;
	JBoolean		EditMenuCmdToIndex(const CmdIndex cmd, JIndex* index) const;

	virtual void	EvaluateSelection() const;

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers) override;

protected:

	virtual const JCharacter*	GetMultiplicationString() const;

	virtual void		EIPRefresh();
	virtual void		EIPRedraw();
	virtual void		EIPBoundsChanged();
	virtual JBoolean	EIPScrollToRect(const JRect& r);
	virtual JBoolean	EIPScrollForDrag(const JPoint& pt);
	virtual void		EIPAdjustNeedTab(const JBoolean needTab);

	virtual void		EIPClipboardChanged();
	virtual JBoolean	EIPOwnsClipboard();
	virtual JBoolean	EIPGetExternalClipboard(JString* text);

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

	virtual JBoolean	OKToUnfocus() override;
	virtual void		AdjustCursor(const JPoint& pt, const JXKeyModifiers& modifiers) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

	JXEPSPrinter&		GetEPSPrinter();
	const JXEPSPrinter&	GetEPSPrinter() const;

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

	// not allowed

	JXExprEditor(const JXExprEditor& source);
	const JXExprEditor& operator=(const JXExprEditor& source);
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
