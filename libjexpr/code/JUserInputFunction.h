/******************************************************************************
 JUserInputFunction.h

	Interface for the JUserInputFunction class

	Copyright (C) 1996-98 by John Lindal.

 ******************************************************************************/

#ifndef _H_JUserInputFunction
#define _H_JUserInputFunction

#include <JFunction.h>
#include <JTextEditor.h>

class JFontManager;
class JVariableList;
class JExprRectList;

class JUserInputFunction : public JFunction, public JTextEditor
{
public:

	JUserInputFunction(const JVariableList* varList, JFontManager* fontManager,
					   const JCharacter* text = NULL);
	JUserInputFunction(const JUserInputFunction& source);

	virtual ~JUserInputFunction();

	void	Activate();
	void	Deactivate();

	JBoolean	IsEmpty() const;
	void		Clear();

	JBoolean	HandleMouseDown(const JPoint& pt, const JBoolean extendSelection,
								const JExprRectList& rectList,
								const JExprRenderer& renderer);
	JBoolean	HandleMouseDrag(const JPoint& pt, const JExprRectList& rectList,
								const JExprRenderer& renderer);
	JBoolean	HandleMouseUp();
	JBoolean	HandleKeyPress(const JCharacter key, JBoolean* needParse,
							   JBoolean* needRender);
	JBoolean	Parse(JFunction** f, JUserInputFunction** newUIF,
					  JBoolean* needRender);

	virtual JBoolean	Evaluate(JFloat* result) const;
	virtual JBoolean	Evaluate(JComplex* result) const;
	virtual void		Print(std::ostream& output) const;
	virtual JFunction*	Copy() const;
	virtual JBoolean	SameAs(const JFunction& theFunction) const;
	virtual JIndex		PrepareToRender(const JExprRenderer& renderer,
										const JPoint& upperLeft, const JSize fontSize,
										JExprRectList* rectList);
	virtual void		Render(const JExprRenderer& renderer,
							   const JExprRectList& rectList) const;
	virtual void		BuildNodeList(JExprNodeList* nodeList, const JIndex myNode);

	static const JCharacter*	GetEmptyString();

	virtual JBoolean	TEHasSearchText() const;

	// called by other input objects

	static JString	GetParseableText(const JTextEditor& te);
	static void		SetParseableText(JTextEditor* te, const JCharacter* text);

protected:

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
	virtual void		TEClipboardChanged();
	virtual JBoolean	TEGetExternalClipboard(JString* text, JRunArray<JFont>* style) const;
	virtual void		TEDisplayBusyCursor() const;
	virtual JBoolean	TEBeginDND();
	virtual void		TEPasteDropData();
	virtual void		TECaretShouldBlink(const JBoolean blink);

private:

	const JVariableList*	itsVarList;

	JCoordinate	itsWidth;
	JCoordinate	itsHeight;

	mutable JBoolean	itsNeedRedrawFlag;
	mutable JBoolean	itsNeedRenderFlag;

private:

	// not allowed

	const JUserInputFunction& operator=(const JUserInputFunction& source);
};


/******************************************************************************
 Activate

 ******************************************************************************/

inline void
JUserInputFunction::Activate()
{
	TEActivate();
}

/******************************************************************************
 Deactivate

 ******************************************************************************/

inline void
JUserInputFunction::Deactivate()
{
	TEDeactivate();
}

#endif
