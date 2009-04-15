/******************************************************************************
 ClipboardWidget.h

	Interface for the ClipboardWidget class

	Written by Glenn Bach - 1998.

 ******************************************************************************/

#ifndef _H_ClipboardWidget
#define _H_ClipboardWidget

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWidget.h>
#include <JString.h>

class JXTextMenu;
class JXMenuBar;

class ClipboardWidget : public JXWidget
{
public:

	ClipboardWidget(const JCharacter* text, 
					JXMenuBar* menuBar, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	virtual ~ClipboardWidget();

	void			SetText(const JCharacter* text);
	const JString&	GetText() const;
	
protected:
	
	virtual void		Draw(JXWindowPainter& p, const JRect& rect);
	virtual void		DrawBorder(JXWindowPainter& p, const JRect& frame);
	virtual void		Receive(JBroadcaster* sender, const Message& message);
	
private:

	JString		itsText;
	JXTextMenu*	itsEditMenu;

private:

	void	HandleEditMenu(const JIndex index);
	void	Paste();

	// not allowed

	ClipboardWidget(const ClipboardWidget& source);
	const ClipboardWidget& operator=(const ClipboardWidget& source);
};

#endif
