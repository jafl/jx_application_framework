/******************************************************************************
 JUserInputFunction.h

	Interface for the JUserInputFunction class

	Copyright (C) 1996-98 by John Lindal.

 ******************************************************************************/

#ifndef _H_JUserInputFunction
#define _H_JUserInputFunction

#include <JFunction.h>
#include <JTextEditor.h>
#include <JXStyledText.h>

class JFontManager;
class JExprEditor;
class JExprRectList;

class JUserInputFunction : public JFunction, public JTextEditor
{
public:

	JUserInputFunction(JExprEditor* editor, const JString& text = JString::empty);
	JUserInputFunction(const JUserInputFunction& source);

	virtual ~JUserInputFunction();

	void	Activate();
	void	Deactivate();

	JBoolean	IsEmpty() const;
	void		Clear();

	JBoolean	IsGreek() const;
	void		SetGreek(const JBoolean greek);

	JBoolean	HandleMouseDown(const JPoint& pt, const JBoolean extendSelection,
								const JExprRectList& rectList,
								const JExprRenderer& renderer);
	JBoolean	HandleMouseDrag(const JPoint& pt, const JExprRectList& rectList,
								const JExprRenderer& renderer);
	JBoolean	HandleMouseUp();
	JBoolean	HandleKeyPress(const JUtf8Character& key, JBoolean* needParse,
							   JBoolean* needRender);
	JBoolean	Parse(const JUtf8Character& c, JFunction** f,
					  JUserInputFunction** newUIF, JBoolean* needRender);

	virtual JBoolean	Evaluate(JFloat* result) const override;
	virtual JBoolean	Evaluate(JComplex* result) const override;
	virtual void		Print(std::ostream& output) const override;
	virtual JFunction*	Copy() const override;
	virtual JIndex		Layout(const JExprRenderer& renderer,
							   const JPoint& upperLeft, const JSize fontSize,
							   JExprRectList* rectList) override;
	virtual void		Render(const JExprRenderer& renderer,
							   const JExprRectList& rectList) const override;

	static const JString&	GetEmptyString();

	virtual JBoolean	TEHasSearchText() const override;

protected:

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
	virtual void		TEUpdateClipboard(const JString& text, const JRunArray<JFont>& style) const override;
	virtual JBoolean	TEGetClipboard(JString* text, JRunArray<JFont>* style) const override;
	virtual JBoolean	TEBeginDND() override;
	virtual void		TEPasteDropData() override;
	virtual void		TECaretShouldBlink(const JBoolean blink) override;

protected:

	class StyledText : public JXStyledText
	{
		public:

		StyledText(JFontManager* fontManager)
			:
			JXStyledText(kJFalse, kJFalse, fontManager)
		{ };

		protected:

		virtual void	AdjustStylesBeforeBroadcast(
							const JString& text, JRunArray<JFont>* styles,
							JStyledText::TextRange* recalcRange,
							JStyledText::TextRange* redrawRange,
							const JBoolean deletion) override;
	};

private:

	JExprEditor*	itsEditor;
	JCoordinate		itsWidth;
	JCoordinate		itsHeight;
	JBoolean		itsGreekFlag;

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

/******************************************************************************
 Greek

 ******************************************************************************/

inline JBoolean
JUserInputFunction::IsGreek()
	const
{
	return itsGreekFlag;
}

inline void
JUserInputFunction::SetGreek
	(
	const JBoolean greek
	)
{
	itsGreekFlag = greek;
}

#endif
