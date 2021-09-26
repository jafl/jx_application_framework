/******************************************************************************
 JUserInputFunction.h

	Interface for the JUserInputFunction class

	Copyright (C) 1996-98 by John Lindal.

 ******************************************************************************/

#ifndef _H_JUserInputFunction
#define _H_JUserInputFunction

#include "jx-af/jexpr/JFunction.h"
#include <jx-af/jcore/JTextEditor.h>

class JFontManager;
class JExprEditor;
class JExprRectList;

class JUserInputFunction : public JFunction, public JTextEditor
{
public:

	static const JUtf8Character kSwitchFontCharacter;

public:

	JUserInputFunction(JExprEditor* editor, const JString& text = JString::empty);
	JUserInputFunction(const JUserInputFunction& source);

	virtual ~JUserInputFunction();

	void	Activate();
	void	Deactivate();

	bool	IsEmpty() const;
	void	Clear();

	bool	IsGreek() const;
	void	SetGreek(const bool greek);

	bool	HandleMouseDown(const JPoint& pt, const bool extendSelection,
							const JExprRectList& rectList,
							const JExprRenderer& renderer);
	bool	HandleMouseDrag(const JPoint& pt, const JExprRectList& rectList,
							const JExprRenderer& renderer);
	bool	HandleMouseUp();
	bool	HandleKeyPress(const JUtf8Character& key, bool* needParse,
						   bool* needRender);
	bool	Parse(const JUtf8Character& c, JFunction** f,
				  JUserInputFunction** newUIF, bool* needRender);

	virtual bool		Evaluate(JFloat* result) const override;
	virtual bool		Evaluate(JComplex* result) const override;
	virtual void		Print(std::ostream& output) const override;
	virtual JFunction*	Copy() const override;
	virtual JIndex		Layout(const JExprRenderer& renderer,
							   const JPoint& upperLeft, const JSize fontSize,
							   JExprRectList* rectList) override;
	virtual void		Render(const JExprRenderer& renderer,
							   const JExprRectList& rectList) const override;

	static const JString&	GetEmptyString();

	virtual bool	TEHasSearchText() const override;

	static JUtf8Character	ConvertToGreek(const JUtf8Character& c);
	static JString			ConvertToGreek(const JString& s);

	static void	AdjustStylesBeforeBroadcast(
						const JString& text, JRunArray<JFont>* styles,
						JStyledText::TextRange* recalcRange,
						JStyledText::TextRange* redrawRange,
						const bool deletion);

protected:

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
	virtual void	TEUpdateClipboard(const JString& text, const JRunArray<JFont>& style) const override;
	virtual bool	TEGetClipboard(JString* text, JRunArray<JFont>* style) const override;
	virtual bool	TEBeginDND() override;
	virtual void	TEPasteDropData() override;
	virtual void	TECaretShouldBlink(const bool blink) override;

private:

	JExprEditor*	itsEditor;
	JCoordinate		itsWidth;
	JCoordinate		itsHeight;
	bool			itsGreekFlag;

	mutable bool	itsNeedRedrawFlag;
	mutable bool	itsNeedRenderFlag;
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

inline bool
JUserInputFunction::IsGreek()
	const
{
	return itsGreekFlag;
}

inline void
JUserInputFunction::SetGreek
	(
	const bool greek
	)
{
	itsGreekFlag = greek;
}

#endif
