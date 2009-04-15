/******************************************************************************
 JXExprWidget.h

	Interface for the JXExprWidget class

	Copyright © 2000 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXExprEditor
#define _H_JXExprEditor

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXScrollableWidget.h>
#include <JExprEditor.h>

class JString;
class JVariableList;
class JPagePrinter;

class JXExprWidget : public JXScrollableWidget, public JExprEditor
{
public:

	JXExprWidget(const JVariableList* varList, 
				 JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	virtual ~JXExprWidget();

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers);
	void			Print(JPagePrinter& p, const JRect& rect);

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

	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh);

	virtual void	Draw(JXWindowPainter& p, const JRect& rect);

	virtual void	HandleFocusEvent();
	virtual void	HandleUnfocusEvent();

	virtual JBoolean	OKToUnfocus();
	virtual void		Receive(JBroadcaster* sender, const Message& message);

private:

private:

	void	JXExprEditorX();

	void	PrivateDraw(JPainter& p, const JRect& rect);
	void	GetDrawingOffset(JPoint* delta) const;
	JPoint	BoundsToRenderer(const JPoint& pt) const;
	JPoint	RendererToBounds(const JPoint& pt) const;

	// not allowed

	JXExprWidget(const JXExprWidget& source);
	const JXExprWidget& operator=(const JXExprWidget& source);
};

#endif
