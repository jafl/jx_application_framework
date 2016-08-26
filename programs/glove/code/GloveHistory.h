/******************************************************************************
 GloveHistory.h

	Interface for the GloveHistory class

	Copyright © 1997 by Glenn W. Bach. All rights reserved.

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

	virtual void		Receive(JBroadcaster* sender, const Message& message);
	virtual JCoordinate	GetPrintHeaderHeight(JPagePrinter& p) const;
	virtual void		DrawPrintHeader(JPagePrinter& p, const JCoordinate headerHeight);

private:

	JSize		itsTabCharCount;


	// owned by the menu bar

	JXXFontMenu*	itsFontMenu;	// NULL if no fonts are available
	JXFontSizeMenu*	itsSizeMenu;	// NULL if fonts other than Courier are available

private:

	void	AdjustFont();
	void	AdjustTabWidth();

	static JOrderedSetT::CompareResult
		CompareFontNames(JString * const &, JString * const &);

	// not allowed

	GloveHistory(const GloveHistory& source);
	const GloveHistory& operator=(const GloveHistory& source);

public:

	// JBroadcaster messages

	static const JCharacter* kFontWillChange;

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
