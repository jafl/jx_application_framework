/******************************************************************************
 GLHistory.h

	Interface for the GLHistory class

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GLHistory
#define _H_GLHistory

#include <JXTEBase.h>

class JString;
class JXXFontMenu;
class JXFontSizeMenu;
class JXPrintPlainTextDialog;
class JXToolBar;

class GLHistory : public JXTEBase
{
public:

	GLHistory(JXMenuBar* menuBar, 
				 JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	virtual ~GLHistory();

	JSize	GetTabCharCount() const;
	void	SetTabCharCount(const JSize charCount);

	void	LoadDefaultToolBar(JXToolBar* toolBar);

protected:

	virtual void		Receive(JBroadcaster* sender, const Message& message) override;
	virtual JCoordinate	GetPrintHeaderHeight(JPagePrinter& p) const override;
	virtual void		DrawPrintHeader(JPagePrinter& p, const JCoordinate headerHeight) override;

private:

	JSize		itsTabCharCount;


	// owned by the menu bar

	JXXFontMenu*	itsFontMenu;	// nullptr if no fonts are available
	JXFontSizeMenu*	itsSizeMenu;	// nullptr if fonts other than Courier are available

private:

	void	AdjustFont();
	void	AdjustTabWidth();

	static JListT::CompareResult
		CompareFontNames(JString * const &, JString * const &);

	// not allowed

	GLHistory(const GLHistory& source);
	const GLHistory& operator=(const GLHistory& source);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kFontWillChange;

	class FontWillChange : public JBroadcaster::Message
	{
	public:

		FontWillChange()
			:
			JBroadcaster::Message(kFontWillChange)
			{ };
	};
};


/******************************************************************************
 Tab width (# of characters)

 ******************************************************************************/

inline JSize
GLHistory::GetTabCharCount()
	const
{
	return itsTabCharCount;
}

inline void
GLHistory::SetTabCharCount
	(
	const JSize charCount
	)
{
	itsTabCharCount = charCount;
	AdjustTabWidth();
}


#endif
