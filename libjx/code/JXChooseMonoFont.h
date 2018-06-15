/******************************************************************************
 JXChooseMonoFont.h

	Interface for the JXChooseMonoFont class

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXChooseMonoFont
#define _H_JXChooseMonoFont

#include "JXWidgetSet.h"

class JXXFontMenu;
class JXFontSizeMenu;

class JXChooseMonoFont : public JXWidgetSet
{
public:

	JXChooseMonoFont(JXContainer* enclosure,
					 const HSizingOption hSizing, const VSizingOption vSizing,
					 const JCoordinate x, const JCoordinate y,
					 const JCoordinate w, const JCoordinate h);

	virtual ~JXChooseMonoFont();

	void	GetFont(JString* name, JSize* size) const;
	void	SetFont(const JString& name, const JSize size);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXXFontMenu*		itsFontMenu;
	JXFontSizeMenu*		itsSizeMenu;

private:

	void	PrependOtherMonospaceFonts(JXXFontMenu* menu);
	void	UpdateMenus(const JBoolean updateSize);

	static JListT::CompareResult
		CompareFontNames(JString * const &, JString * const &);

	// not allowed

	JXChooseMonoFont(const JXChooseMonoFont& source);
	const JXChooseMonoFont& operator=(const JXChooseMonoFont& source);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kFontChanged;

	class FontChanged : public JBroadcaster::Message
		{
		public:

			FontChanged()
				:
				JBroadcaster::Message(kFontChanged)
				{ };

		// call broadcaster's GetFont() to get fontName
		};
};

#endif
