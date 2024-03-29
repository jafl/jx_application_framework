/******************************************************************************
 ClipboardWidget.h

	Interface for the ClipboardWidget class

	Written by Glenn Bach - 1998.

 ******************************************************************************/

#ifndef _H_ClipboardWidget
#define _H_ClipboardWidget

#include <jx-af/jx/JXWidget.h>

class JXTextMenu;
class JXMenuBar;

class ClipboardWidget : public JXWidget
{
public:

	ClipboardWidget(const JString& text,
					JXMenuBar* menuBar, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	~ClipboardWidget() override;

	void			SetText(const JString& text);
	const JString&	GetText() const;
	
protected:
	
	void	Draw(JXWindowPainter& p, const JRect& rect) override;
	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;
	
private:

	JString		itsText;
	JXTextMenu*	itsEditMenu;

private:

	void	HandleEditMenu(const JIndex index);
	void	Paste();
};

#endif
