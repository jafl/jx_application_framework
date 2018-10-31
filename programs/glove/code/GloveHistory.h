/******************************************************************************
 GloveHistory.h

	Interface for the GloveHistory class

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GloveHistory
#define _H_GloveHistory

#include <JXTEBase.h>

class JString;
class JXXFontMenu;
class JXFontSizeMenu;
class JXPrintPlainTextDialog;
class JXToolBar;

class GloveHistory : public JXTEBase
{
public:

	GloveHistory(JXMenuBar* menuBar, 
				 JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	virtual ~GloveHistory();

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

	GloveHistory(const GloveHistory& source);
	const GloveHistory& operator=(const GloveHistory& source);

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
GloveHistory::GetTabCharCount()
	const
{
	return itsTabCharCount;
}

inline void
GloveHistory::SetTabCharCount
	(
	const JSize charCount
	)
{
	itsTabCharCount = charCount;
	AdjustTabWidth();
}


#endif
